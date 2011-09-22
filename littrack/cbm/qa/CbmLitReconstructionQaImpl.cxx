/**
 * \file CbmLitReconstructionQaImpl.cxx
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 *
 * Implementation of global track reconstruction QA.
 **/
#include "qa/CbmLitReconstructionQaImpl.h"
#include "qa/CbmLitQaPrintGenerator.h"
#include "qa/CbmLitQaHTMLGenerator.h"
#include "qa/CbmLitReconstructionQaChecker.h"

#include "base/CbmLitEnvironment.h"
#include "utils/CbmLitDrawHist.h"
#include "utils/CbmLitUtils.h"
#include "elid/CbmLitGlobalElectronId.h"

#include "CbmGlobalTrack.h"
#include "CbmTrackMatch.h"
#include "CbmTofHit.h"
#include "CbmTofPoint.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "FairGeoNode.h"
#include "CbmDetectorList.h"
#include "CbmHit.h"
#include "CbmStsTrack.h"
#include "CbmMvdPoint.h"
#include "CbmMvdHitMatch.h"
#include "CbmRichRingMatch.h"
#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "CbmVertex.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmStsDetectorId.h"
#include "CbmTrdDetectorId.h"
#include "CbmMuchGeoScheme.h"
#include "CbmGeoStsPar.h"
#include "CbmStsPoint.h"
#include "CbmTrdPoint.h"
#include "CbmMuchPoint.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TList.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPad.h"
#include "TFile.h"
#include "TProfile.h"
#include "TLine.h"
#include "TSystem.h"

#include <iostream>
#include <map>
#include <cmath>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

///
/// \brief Type of the histogram.
///
enum LitQaHistoType {
   kAcc = 0, ///< Accepted tracks.
   kRec = 1, ///< Reconstructed tracks.
   kEff = 2, ///< Efficiency.
   kHistoTypeEnd = 3 ///< End marker for convenience.
};

///
/// \brief Track categories.
///
enum LitQaTrackCategory {
   kAll = 0, ///< All tracks.
   kRef = 1, ///< Reference tracks.
   kPrim = 2, ///< Primary tracks.
   kSec = 3, ///< Secondary tracks.
   kMu = 4, ///< Primary muon tracks.
   kEl = 5, ///< Primary electron tracks.
   kTrackCategoryEnd = 6 ///< End marker for convenience.
};

///
/// \brief RICH ring categories.
///
enum LitQaRichCategory {
   kRichAll = 0, ///< All rings.
   kRichAllRef = 1, ///< All reference rings.
   kRichEl = 2, ///< Electron rings.
   kRichElRef = 3, ///< Electron reference rings.
   kRichPi = 4, ///< Pion rings.
   kRichPiRef = 5, ///< Pion reference rings.
   kRichCategoryEnd = 6 ///< End marker for convenience.
};

///
/// \brief Type of hits in the track.
///
enum LitQaHitType {
   kAllHits = 0, ///< All hits in track.
   kTrueHits = 1, ///< True hits in track.
   kFakeHits = 2, ///< Fake hits in track.
   kTrueOverAll = 3, ///< True/All fraction of hits.
   kFakeOverAll = 4, ///< Fake/All fraction of hits.
   kHitTypeEnd = 5 ///< End marker for convenience.
};

///
/// \brief Electron identification.
///
enum LitQaElectronId {
   kElectronId = 0, ///< Electron identification.
   kPionSupp = 1, ///< Pion suppresion.
   kElectronIdEnd = 3 ///< End marker for convenience.
};

///
/// \brief Detector acceptance.
///
enum LitQaDetAcc {
   kDetAccMc = 0, ///< Generated tracks/particles.
   kDetAccAcc = 1, ///< Accepted tracks/particles in the detector.
   kDetAccEff = 2, ///< Acceptance efficiency.
   kDetAccEnd = 3 ///< End marker for convenience.
};


CbmLitReconstructionQaImpl::CbmLitReconstructionQaImpl():
   FairTask("LitReconstructionQA", 1),

   fMinNofPointsSts(4),
   fMinNofPointsTrd(8),
   fMinNofPointsMuch(10),
   fMinNofPointsTof(1),
   fQuota(0.7),
   fUseConsecutivePointsInSts(false),
   fMinNofHitsRich(7),
   fQuotaRich(0.6),

   fMinNofHitsTrd(8),
   fMinNofHitsMuch(10),

   fIsElectronSetup(false),
   fIsMvd(false),
   fIsSts(false),
   fIsRich(false),
   fIsTrd(false),
   fIsMuch(false),
   fIsTof(false),

   fRefMomentum(1.),
   fRefMinNofHitsRich(15),

   fMinMom(0.),
   fMaxMom(25.),
   fNofBinsMom(25),
   fMinAngle(0.),
   fMaxAngle(30.),
   fNofBinsAngle(3),

   fMcStsMap(),
   fMcHalfGlobalMap(),
   fMcGlobalMap(),

   fMCTracks(NULL),
   fGlobalTracks(NULL),

   fMvdHits(NULL),
   fMvdPoints(NULL),
   fMvdHitMatches(NULL),
   fMvdDigis(NULL),
   fMvdClusters(NULL),

   fStsTracks(NULL),
   fStsMatches(NULL),
   fStsPoints(NULL),
   fStsDigis(NULL),
   fStsClusters(NULL),
   fStsHits(NULL),

   fRichHits(NULL),
   fRichRings(NULL),
   fRichProjections(NULL),
   fRichRingMatches(NULL),
   fRichPoints(NULL),

   fMuchPixelHits(NULL),
   fMuchStrawHits(NULL),
   fMuchMatches(NULL),
   fMuchPoints(NULL),
   fMuchDigis(NULL),
   fMuchClusters(NULL),

   fTrdMatches(NULL),
   fTrdHits(NULL),
   fTrdPoints(NULL),
   fTrdDigis(NULL),
   fTrdClusters(NULL),

   fTofPoints(NULL),
   fTofHits(NULL),

   fNofCategories(6),
   fNofTypes(3),

   fhStsMom(),
   fhStsMomNormHalfGlobal(),
   fhStsMomNormGlobal(),
   fhStsNp(),
   fhStsAngle(),
   fhHalfGlobalMom(),
   fhHalfGlobalMomNormGlobal(),
   fhGlobalMom(),
   fhRecMom(),
   fhRecNp(),
   fhRecAngle(),
   fhTofMom(),
   fhTofAngle(),

   fhStsGhostNh(NULL),
   fhRecGhostNh(NULL),
   fhRichGhostNh(NULL),
   fhRichGhostStsMatchingNh(NULL),
   fhRichGhostElIdNh(NULL),
   fhStsGhostRichMatchingNh(NULL),

   fhRichMom(),
   fhRichNh(),

   fhStsMomNormStsRich(),
   fhStsRichMom(),

   fhStsMomNormStsRichTrd(),
   fhStsRichMomNormStsRichTrd(),
   fhStsRichTrdMom(),

   fhStsMomNormStsRichTrdTof(),
   fhStsRichMomNormStsRichTrdTof(),
   fhStsRichTrdMomNormStsRichTrdTof(),
   fhStsRichTrdTofMom(),

   fhTrdNofHitsInStation(NULL),
   fhMuchNofHitsInStation(NULL),
   fhTofNofHitsInStation(NULL),

   fhMvdTrackHits(),
   fhStsTrackHits(),
   fhTrdTrackHits(),
   fhMuchTrackHits(),

   fHistoList(NULL),

   fhNofGlobalTracks(NULL),
   fhNofStsTracks(NULL),
   fhNofTrdTracks(NULL),
   fhNofMuchTracks(NULL),
   fhNofRichRings(NULL),
   fhNofRichProjections(NULL),

   fhNofMvdHits(NULL),
   fhNofStsHits(NULL),
   fhNofRichHits(NULL),
   fhNofTrdHits(NULL),
   fhNofMuchPixelHits(NULL),
   fhNofMuchStrawHits(NULL),
   fhNofTofHits(NULL),

   fhNofMvdPoints(NULL),
   fhNofStsPoints(NULL),
   fhNofRichPoints(NULL),
   fhNofTrdPoints(NULL),
   fhNofMuchPoints(NULL),
   fhNofTofPoints(NULL),

   fhNofMvdDigis(NULL),
   fhNofStsDigis(NULL),
   fhNofMuchDigis(NULL),
   fhNofTrdDigis(NULL),

   fhNofMvdClusters(NULL),
   fhNofStsClusters(NULL),
   fhNofMuchClusters(NULL),
   fhNofTrdClusters(NULL),

   fhStsChiprim(NULL),
   fhStsMomresVsMom(NULL),

   fhStsTrdMomElId(),
   fhStsTrdMomElIdNormStsTrdTof(),
   fhStsTrdTofMomElId(),
   fhStsRichMomElId(),
   fhStsRichMomElIdNormStsRichTrd(),
   fhStsRichTrdMomElId(),
   fhStsRichMomElIdNormStsRichTrdTof(),
   fhStsRichTrdMomElIdNormStsRichTrdTof(),
   fhStsRichTrdTofMomElId(),

   fhStsDetAccEl(),
   fhStsRichDetAccEl(),
   fhStsTrdDetAccEl(),
   fhStsTofDetAccEl(),
   fhStsRichTrdDetAccEl(),
   fhStsRichTrdTofDetAccEl(),
   fhStsTrdTofDetAccEl(),

   fhMCMomVsAngle(),

   fhEventNo(NULL),

   fOutputDir(""),

   fPrimVertex(NULL),
   fKFFitter(NULL),

   fElectronId(NULL)
{
}

CbmLitReconstructionQaImpl::~CbmLitReconstructionQaImpl()
{
   fHistoList->Delete();
   delete fHistoList;
}

InitStatus CbmLitReconstructionQaImpl::Init()
{
   DetermineSetup();
   ReadDataBranches();
   CreateHistos(NULL);

   if (fIsElectronSetup) {
      fElectronId = new CbmLitGlobalElectronId();
      fElectronId->Init();
   }

   return kSUCCESS;
}

void CbmLitReconstructionQaImpl::Exec(
   Option_t* opt)
{
   // Increase event counter
   fhEventNo->Fill(0.5);
   std::cout << "Event: " << fhEventNo->GetEntries() << std::endl;

   FillNofCrossedStationsHistos();
   FillRichRingNofHits();
   ProcessHits();
   ProcessGlobalTracks();
   ProcessMcTracks();
   PionSuppression();
   StsTracksQa();
   IncreaseCounters();

   boost::property_tree::ptree pt = PrintPTree();
   CbmLitQaPrintGenerator::PrintEventStatistics(std::cout, &pt);
}

void CbmLitReconstructionQaImpl::Finish()
{
   CalculateEfficiencyHistos();
   WriteToFile();
   boost::property_tree::ptree pt = PrintPTree();
   CbmLitQaPrintGenerator::PrintFinalStatistics(std::cout, &pt);
   Draw();
}

void CbmLitReconstructionQaImpl::DetermineSetup()
{
   CbmLitEnvironment* env = CbmLitEnvironment::Instance();
   fIsElectronSetup = env->IsElectronSetup();
   fIsMvd = env->IsMvd();
   fIsSts = env->IsSts();
   fIsRich = env->IsRich();
   fIsTrd = env->IsTrd();
   fIsMuch = env->IsMuch();
   fIsTof = env->IsTof();

   if (fIsElectronSetup) { std::cout << "-I- CBM electron setup detected" << std::endl; }
   else { std::cout << "-I- CBM muon setup detected" << std::endl; }
   std::cout << "-I- The following detectors were found in the CBM setup: ";
   if (fIsMvd) { std::cout << "MVD "; }
   if (fIsSts) { std::cout << "STS "; }
   if (fIsRich) { std::cout << "RICH "; }
   if (fIsMuch) { std::cout << "MUCH "; }
   if (fIsTrd) { std::cout << "TRD "; }
   if (fIsTof) { std::cout << "TOF "; }
   std::cout << std::endl;
}

void CbmLitReconstructionQaImpl::SetDetectorPresence(
		   DetectorId detId,
		   bool isDet)
{
	switch(detId) {
		case kMVD: fIsMvd = isDet; break;
		case kSTS: fIsSts = isDet; break;
		case kRICH: fIsRich = isDet; break;
		case kTRD: fIsTrd = isDet; break;
		case kMUCH: fIsMuch = isDet; break;
		case kTOF: fIsTof = isDet; break;
		default: break;
	}
}

bool CbmLitReconstructionQaImpl::GetDetectorPresence(
         DetectorId detId) const
{
   switch(detId) {
      case kMVD: return fIsMvd;
      case kSTS: return fIsSts;
      case kRICH: return fIsRich;
      case kTRD: return fIsTrd;
      case kMUCH: return fIsMuch;
      case kTOF: return fIsTof;
      default: break;
   }
}

void CbmLitReconstructionQaImpl::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("Init","CbmRootManager is not instantiated"); }

   fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
   if (NULL == fMCTracks) { Fatal("Init","No MCTrack array!"); }

   fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
   if (NULL == fGlobalTracks) { Fatal("Init","No GlobalTrack array!"); }

   if (fIsMvd) {
      fMvdHits = (TClonesArray*) ioman->GetObject("MvdHit");
      if (NULL == fMvdHits) { Fatal("Init",": No MvdHit array!"); }
      fMvdPoints = (TClonesArray*) ioman->GetObject("MvdPoint");
      if (NULL == fMvdPoints) { Fatal("Init",": No MvdPoint array!"); }
      fMvdHitMatches = (TClonesArray*) ioman->GetObject("MvdHitMatch");
      if (NULL == fMvdHitMatches) { Fatal("Init",": No MvdHitMatch array!"); }
      fMvdDigis = (TClonesArray*) ioman->GetObject("MvdDigi");
      //if (NULL == fMvdDigis) { Fatal("Init",": No MvdDigi array!"); }
      fMvdClusters = (TClonesArray*) ioman->GetObject("MvdCluster");
      //if (NULL == fMvdClusters) { Fatal("Init",": No MvdCluster array!"); }
   }

   if (fIsSts) {
      fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
      if (NULL == fStsTracks) { Fatal("Init",": No StsTrack array!"); }
      fStsMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
      if (NULL == fStsMatches) { Fatal("Init",": No StsTrackMatch array!"); }
      fStsHits = (TClonesArray*) ioman->GetObject("StsHit");
      if (NULL == fStsHits) { Fatal("Init",": No StsHit array!"); }
      fStsPoints = (TClonesArray*) ioman->GetObject("StsPoint");
      if (NULL == fStsPoints) { Fatal("Init",": No StsPoint array!"); }
      fStsDigis = (TClonesArray*) ioman->GetObject("StsDigi");
      //if (NULL == fStsDigis) { Fatal("Init",": No StsDigi array!"); }
      fStsClusters = (TClonesArray*) ioman->GetObject("StsCluster");
      //if (NULL == fStsClusters) { Fatal("Init",": No StsCluster array!"); }
   }

   if (fIsRich) {
	  fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
	  if (NULL == fRichHits) { Fatal("Init","No RichHit array!"); }
	  fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
	  if (NULL == fRichRings) { Fatal("Init","No RichRing array!"); }
	  fRichProjections = (TClonesArray*) ioman->GetObject("RichProjection");
	  if (NULL == fRichProjections) { Fatal("Init","No RichProjection array!"); }
	  fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
	  if (NULL == fRichRingMatches) { Fatal("Init","No RichRingMatch array!"); }
	  fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
	  if (NULL == fRichPoints) { Fatal("Init","No RichPoint array!"); }
   }

   if (fIsMuch) {
      fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
      fMuchStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
      if (NULL == fMuchPixelHits && NULL == fMuchStrawHits) { Fatal("CbmLitReconstructionQaImpl::Init", "No MuchPixelHit AND MuchStrawHit arrays!"); }
      fMuchMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
      if (NULL == fMuchMatches) { Fatal("Init","No MuchTrackMatch array!"); }
      fMuchPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
      if (NULL == fMuchPoints) { Fatal("Init","No MuchPoint array!"); }
      fMuchDigis = (TClonesArray*) ioman->GetObject("MuchDigi");
//      if (NULL == fMuchDigis) { Fatal("Init","No MuchDigi array!"); }
      fMuchClusters = (TClonesArray*) ioman->GetObject("MuchCluster");
//      if (NULL == fMuchClusters) { Fatal("Init","No MuchCluster array!"); }
   }

   if (fIsTrd) {
      fTrdMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
      if (NULL == fTrdMatches) { Fatal("Init","No TrdTrackMatch array!"); }
      fTrdHits = (TClonesArray*) ioman->GetObject("TrdHit");
      if (NULL == fTrdHits) { Fatal("Init","No TrdHit array!"); }
      fTrdPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
      if (NULL == fTrdPoints) { Fatal("Init","No TrdPoint array!"); }
      fTrdDigis = (TClonesArray*) ioman->GetObject("TrdDigi");
//      if (NULL == fTrdDigis) { Fatal("Init","No TrdDigi array!"); }
      fTrdClusters = (TClonesArray*) ioman->GetObject("TrdCluster");
//      if (NULL == fTrdClusters) { Fatal("Init","No TrdCluster array!"); }
   }

   if (fIsTof) {
      fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
      if (NULL == fTofPoints) { Fatal("Init", "No TofPoint array!"); }
      fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
      if (NULL == fTofHits) { Fatal("Init", "No TofHit array!"); }
   }

   fPrimVertex = (CbmVertex*) ioman->GetObject("PrimaryVertex");
   if (NULL == fPrimVertex) {Fatal("CbmAnaElectronsQa::Init", "No Primary Vertex!");}

   fKFFitter = new CbmStsKFTrackFitter();
   fKFFitter->Init();
}

void CbmLitReconstructionQaImpl::ProcessHits()
{
   if (fIsMvd) {
      for (Int_t i = 0; i < fMvdHits->GetEntriesFast(); i++) {
         CbmHit* hit = static_cast<CbmHit*>(fMvdHits->At(i));
         fhMvdNofHitsInStation->Fill(hit->GetStationNr());
      }
   }
   if (fIsSts) {
      for (Int_t i = 0; i < fStsHits->GetEntriesFast(); i++) {
         CbmHit* hit = static_cast<CbmHit*>(fStsHits->At(i));
         fhStsNofHitsInStation->Fill(hit->GetStationNr());
      }
   }
   if (fIsTrd) {
      for (Int_t i = 0; i < fTrdHits->GetEntriesFast(); i++) {
         CbmBaseHit* hit = static_cast<CbmBaseHit*>(fTrdHits->At(i));
         fhTrdNofHitsInStation->Fill(hit->GetPlaneId());
      }
   }
   if (fIsMuch) {
      for (Int_t i = 0; i < fMuchPixelHits->GetEntriesFast(); i++) {
         CbmBaseHit* hit = static_cast<CbmBaseHit*>(fMuchPixelHits->At(i));
         fhMuchNofHitsInStation->Fill(hit->GetPlaneId());
      }
      for (Int_t i = 0; i < fMuchStrawHits->GetEntriesFast(); i++) {
         CbmBaseHit* hit = static_cast<CbmBaseHit*>(fMuchStrawHits->At(i));
         fhMuchNofHitsInStation->Fill(hit->GetPlaneId());
      }
   }
   if (fIsTof) {
      for (Int_t i = 0; i < fTofHits->GetEntriesFast(); i++) {
         CbmBaseHit* hit = static_cast<CbmBaseHit*>(fTofHits->At(i));
         fhTofNofHitsInStation->Fill(hit->GetPlaneId());
      }
   }
}

void CbmLitReconstructionQaImpl::FillRichRingNofHits()
{
	if (!fIsRich) return;
	fNofHitsInRingMap.clear();
    Int_t nofRichHits = fRichHits->GetEntriesFast();
    for (Int_t iHit=0; iHit < nofRichHits; iHit++) {
        CbmRichHit* hit = static_cast<CbmRichHit*>(fRichHits->At(iHit));
        if (NULL == hit) continue;

        Int_t iPoint = hit->GetRefId();
        if (iPoint < 0) continue;

        FairMCPoint* point = static_cast<FairMCPoint*>(fRichPoints->At(iPoint));
        if (NULL == point) continue;

        Int_t iMCTrack = point->GetTrackID();
        CbmMCTrack* track = static_cast<CbmMCTrack*>(fMCTracks->At(iMCTrack));
        if (NULL == track) continue;

        Int_t iMother = track->GetMotherId();
        if (iMother == -1) continue;

        fNofHitsInRingMap[iMother]++;
    }
}

void CbmLitReconstructionQaImpl::FillNofCrossedStationsHistos()
{
   fNofStsStationsMap.clear();
   fNofTrdStationsMap.clear();
   fNofMuchStationsMap.clear();

   // STS
   FairRunAna* run = FairRunAna::Instance();
   FairRuntimeDb* runDb = run->GetRuntimeDb();
   CbmGeoStsPar* stsGeoPar = (CbmGeoStsPar*) runDb->getContainer("CbmGeoStsPar");
   TObjArray* stsNodes = stsGeoPar->GetGeoSensitiveNodes();
   Int_t nofStsStations = stsNodes->GetEntries();
   std::map<Int_t, Int_t> stsStationNrFromMcId;
   for (Int_t ist = 0; ist < nofStsStations; ist++) {
     FairGeoNode* stsNode = (FairGeoNode*) stsNodes->At(ist);
     std::string stsNodeName(stsNode->GetName());
     std::string stsStationNr = stsNodeName.substr(10, 2); //stsNodeName[10] + stsNodeName[11];
     int stationNr = atoi(stsStationNr.c_str());
     stsStationNrFromMcId[stsNode->getMCid()] = stationNr;
   }

   // std::map<MC track index, set with station numbers>
   std::map<Int_t, std::set<Int_t> > stsStations;
   CbmStsDetectorId stsDetectorId;
   Int_t nofStsPoints = fStsPoints->GetEntriesFast();
   for (Int_t iPoint = 0; iPoint < nofStsPoints; iPoint++) {
      CbmStsPoint* point = static_cast<CbmStsPoint*>(fStsPoints->At(iPoint));
      Int_t mcTrackId = point->GetTrackID();
      Int_t stationId = stsStationNrFromMcId[point->GetDetectorID()];//stsDetectorId.StationNr(point->GetDetectorID());
//      std::cout << stationId << " ";
      stsStations[mcTrackId].insert(stationId);
   }

//   for (std::map<Int_t, std::set<Int_t> >::iterator it = stsStations.begin();
//         it != stsStations.end(); it++) {
//      std::cout << "STS mcid=" << (*it).first << " ns=" << (*it).second.size() << " | ";
//      for (std::set<Int_t>::iterator it2 = (*it).second.begin();
//               it2 != (*it).second.end(); it2++) {
//         std::cout << " " << (*it2);
//      }
//      std::cout << " mc=" << MaxConsecutiveNumbers((*it).second) << std::endl;
//   }
   for (std::map<Int_t, std::set<Int_t> >::iterator it = stsStations.begin();
           it != stsStations.end(); it++) {
      fNofStsStationsMap[(*it).first] = (*it).second.size();
   }

   // Fill map for maximum number of consecutive MC points in STS
   for (std::map<Int_t, std::set<Int_t> >::iterator it = stsStations.begin();
              it != stsStations.end(); it++) {
      fNofStsConsecutiveStationsMap[(*it).first] = MaxConsecutiveNumbers((*it).second);
   }
   // end STS

   // MUCH
   if (fIsMuch) {
      // std::map<MC track index, set with station numbers>
      std::map<Int_t, std::set<Int_t> > muchStations;
      Int_t nofMuchPoints = fMuchPoints->GetEntriesFast();
      for (Int_t iPoint = 0; iPoint < nofMuchPoints; iPoint++) {
         CbmMuchPoint* point = static_cast<CbmMuchPoint*>(fMuchPoints->At(iPoint));
         Int_t mcTrackId = point->GetTrackID();
         Int_t stationId = 10 * CbmMuchGeoScheme::GetStationIndex(point->GetDetectorID())
            + CbmMuchGeoScheme::GetLayerIndex(point->GetDetectorID());
         muchStations[mcTrackId].insert(stationId);
      }

//      for (std::map<Int_t, std::set<Int_t> >::iterator it = muchStations.begin();
//            it != muchStations.end(); it++) {
//         std::cout << "MUCH mcid=" << (*it).first << " ns=" << (*it).second.size() << " | ";
//         for (std::set<Int_t>::iterator it2 = (*it).second.begin();
//                  it2 != (*it).second.end(); it2++) {
//            std::cout << " " << (*it2);
//         }
//         std::cout << std::endl;
//      }
      for (std::map<Int_t, std::set<Int_t> >::iterator it = muchStations.begin();
              it != muchStations.end(); it++) {
         fNofMuchStationsMap[(*it).first] = (*it).second.size();
      }
   }
   // end MUCH

   // TRD
   if (fIsTrd) {
      // std::map<MC track index, set with station numbers>
      std::map<Int_t, std::set<Int_t> > trdStations;
      CbmTrdDetectorId trdDetectorId;
      Int_t nofTrdPoints = fTrdPoints->GetEntriesFast();
      for (Int_t iPoint = 0; iPoint < nofTrdPoints; iPoint++) {
         CbmTrdPoint* point = static_cast<CbmTrdPoint*>(fTrdPoints->At(iPoint));
         Int_t mcTrackId = point->GetTrackID();
         Int_t* detInfo = trdDetectorId.GetDetectorInfo(point->GetDetectorID());
         Int_t stationId = 10 * detInfo[1] + detInfo[2];
         trdStations[mcTrackId].insert(stationId);
      }

//      for (std::map<Int_t, std::set<Int_t> >::iterator it = trdStations.begin();
//            it != trdStations.end(); it++) {
//         std::cout << "TRD mcid=" << (*it).first << " ns=" << (*it).second.size() << " | ";
//         for (std::set<Int_t>::iterator it2 = (*it).second.begin();
//                  it2 != (*it).second.end(); it2++) {
//            std::cout << " " << (*it2);
//         }
//         std::cout << std::endl;
//      }
      for (std::map<Int_t, std::set<Int_t> >::iterator it = trdStations.begin();
              it != trdStations.end(); it++) {
         fNofTrdStationsMap[(*it).first] = (*it).second.size();
      }
   }
   // end TRD
}

//Bool_t CbmLitReconstructionQaImpl::HasStsConsecutivePoints(
//      Int_t mcId)
//{
//   if (!fUseConsecutivePointsInSts) { return true; }
//   else {
//
//   }
//}

Int_t CbmLitReconstructionQaImpl::MaxConsecutiveNumbers(
      const std::set<Int_t>& numbers)
{
   if (numbers.size() == 0) return 0;
   if (numbers.size() == 1) return 1;

   std::vector<Int_t> a(numbers.begin(), numbers.end());

   int maxCnt = 0;
   int cnt = 1;
   for (Int_t i = 0; i < a.size() - 1; i++) {
      if (a[i] == (a[i + 1] - 1)) {
         cnt++;
      } else {
         maxCnt = std::max(cnt, maxCnt);
         cnt = 1;
      }
   }
   maxCnt = std::max(cnt, maxCnt);
   return maxCnt;
}

void CbmLitReconstructionQaImpl::ProcessGlobalTracks()
{
   fMcStsMap.clear();
   fMcHalfGlobalMap.clear();
   fMcGlobalMap.clear();
   fMcRichMap.clear();
   fMcStsRichMap.clear();
   fMcStsRichTrdMap.clear();
   fMcStsRichTrdTofMap.clear();
   Int_t nofGlobalTracks = fGlobalTracks->GetEntriesFast();
   for(Int_t iTrack = 0; iTrack < nofGlobalTracks; iTrack++) {
      CbmGlobalTrack* globalTrack = static_cast<CbmGlobalTrack*>(fGlobalTracks->At(iTrack));
      if (globalTrack == NULL) { continue; }

      // get track segments indices
      Int_t stsId = globalTrack->GetStsTrackIndex();
      Int_t trdId = globalTrack->GetTrdTrackIndex();
      Int_t muchId = globalTrack->GetMuchTrackIndex();
      Int_t tofId = globalTrack->GetTofHitIndex();
      Int_t richId = globalTrack->GetRichRingIndex();

      // check track segments
      Bool_t isStsOk = stsId > -1 && fIsSts;
      Bool_t isTrdOk = trdId > -1 && fIsTrd;
      Bool_t isMuchOk = muchId > -1 && fIsMuch;
      Bool_t isTofOk = tofId > -1 && fIsTof;
      Bool_t isRichOk = richId > -1 && fIsRich;

      // check the quality of track segments
      CbmTrackMatch* stsTrackMatch;
      if (isStsOk) {
         stsTrackMatch = static_cast<CbmTrackMatch*>(fStsMatches->At(stsId));
         isStsOk = CheckTrackQuality(stsTrackMatch, kSTS);
         if (!isStsOk) { // ghost track
            Int_t nofHits = stsTrackMatch->GetNofTrueHits() + stsTrackMatch->GetNofWrongHits() + stsTrackMatch->GetNofFakeHits();
            fhStsGhostNh->Fill(nofHits);

            // calculate number of ghost after RICH matching
            if (isRichOk){
               CbmRichRing* ring = static_cast<CbmRichRing*>(fRichRings->At(richId));
               if (NULL != ring){
                  if (ring->GetDistance() < 1.) fhStsGhostRichMatchingNh->Fill(nofHits);
               }
            }
         } else {
            ProcessMvd(stsId);
         }
      }
      CbmTrackMatch* trdTrackMatch;
      if (isTrdOk) {
         trdTrackMatch = static_cast<CbmTrackMatch*>(fTrdMatches->At(trdId));
         Int_t nofHits = trdTrackMatch->GetNofTrueHits() + trdTrackMatch->GetNofWrongHits() + trdTrackMatch->GetNofFakeHits();
         if (nofHits >= fMinNofHitsTrd) {
            isTrdOk = CheckTrackQuality(trdTrackMatch, kTRD);
            if (!isTrdOk) { // ghost track
               fhRecGhostNh->Fill(nofHits);
            }
         } else {
            isTrdOk = false;
         }
      }
      CbmTrackMatch* muchTrackMatch;
      if (isMuchOk) {
         muchTrackMatch = static_cast<CbmTrackMatch*>(fMuchMatches->At(muchId));
         Int_t nofHits = muchTrackMatch->GetNofTrueHits() + muchTrackMatch->GetNofWrongHits() + muchTrackMatch->GetNofFakeHits();
         if (nofHits >= fMinNofHitsMuch) {
            isMuchOk = CheckTrackQuality(muchTrackMatch, kMUCH);
            if (!isMuchOk) { // ghost track
               fhRecGhostNh->Fill(nofHits);
            }
         } else {
            isMuchOk = false;
         }
      }
      CbmRichRingMatch* richRingMatch;
      if (isRichOk) {
         richRingMatch = static_cast<CbmRichRingMatch*>(fRichRingMatches->At(richId));
         if (richRingMatch == NULL){
        	 std::cout << "-E- no CbmRichRingMatch with index " << richId << endl;
        	 continue;
         }
         Int_t nofHits = richRingMatch->GetNofTrueHits() + richRingMatch->GetNofWrongHits() + richRingMatch->GetNofFakeHits();
         isRichOk = CheckRingQuality(richRingMatch);
         if (!isRichOk) { // ghost ring
            fhRichGhostNh->Fill(nofHits);

            // calculate number of ghost after STS matching and electron identification
            CbmRichRing* ring = static_cast<CbmRichRing*>(fRichRings->At(richId));
            if (NULL != ring){
               if (ring->GetDistance() < 1.) fhRichGhostStsMatchingNh->Fill(nofHits);

               Double_t momentumMc = 0.;
               if (stsTrackMatch!=NULL){
                  CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(stsTrackMatch->GetMCTrackId());
                  if (mcTrack != NULL) momentumMc = mcTrack->GetP();
                }
                if (ring->GetDistance() < 1. && fElectronId->IsRichElectron(globalTrack, momentumMc))
                   fhRichGhostElIdNh->Fill(nofHits);
            }
         }
      }

      // Get MC indices of track segments
      Int_t stsMCId = -1, trdMCId = -1, muchMCId = -1, tofMCId = -1, richMCId = -1;
      if (isStsOk) { stsMCId = stsTrackMatch->GetMCTrackId(); }
      if (isTrdOk) { trdMCId = trdTrackMatch->GetMCTrackId(); }
      if (isMuchOk) { muchMCId = muchTrackMatch->GetMCTrackId(); }
      if (isTofOk) {
         CbmTofHit* tofHit = static_cast<CbmTofHit*>(fTofHits->At(tofId));
         CbmTofPoint* tofPoint = static_cast<CbmTofPoint*>(fTofPoints->At(tofHit->GetRefId()));
         if (tofPoint != NULL) tofMCId = tofPoint->GetTrackID();
      }
      if (isRichOk) { richMCId = richRingMatch->GetMCTrackID(); }

      Bool_t sts = isStsOk && stsMCId != -1;
      Bool_t trd = isTrdOk && stsMCId == trdMCId;
      Bool_t much = isMuchOk && stsMCId == muchMCId;
      Bool_t tof = isTofOk && stsMCId == tofMCId;
      Bool_t rich = isRichOk && stsMCId == richMCId;

      // select STS tracks only
      if (sts) {
         fMcStsMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
      }

      if (fIsMuch && fIsTrd) { // if MUCH and TRD together
         // select STS+MUCH+TRD tracks
         if (sts && (trd && much)) {
            fMcHalfGlobalMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
         }
         // select the longest tracks STS+MUCH+TRD+TOF
         if (sts && (trd && much) && tof) {
            fMcGlobalMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
         }
      } else {
         // select STS+TRD(MUCH) tracks
         if (sts && (trd || much)) {
            fMcHalfGlobalMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
         }
         // select the longest tracks STS+TRD(MUCH)+TOF
         if (fIsTrd || fIsMuch) {
            if (sts && (trd || much) && tof) {
               fMcGlobalMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
            }
         } else { // for STS+TOF setup
            if (sts && tof) {
               fMcGlobalMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
            }
         }
      }
      // RICH: select tracks with RICH
      if (fIsRich) {
    	  // select only RICH
    	  if (isRichOk) {
			  fMcRichMap.insert(std::pair<Int_t, Int_t>(richMCId, iTrack));
		  }
    	  // select STS+RICH tracks
    	  if (sts && rich) {
    		  fMcStsRichMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
    	  }
    	  // select STS+RICH+TRD tracks
    	  if (sts && rich && trd) {
			  fMcStsRichTrdMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
		  }
    	  if (fIsTrd) { // select STS+RICH+TRD+TOF tracks
			  if (sts && rich && trd && tof) {
				  fMcStsRichTrdTofMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
			  }
    	  } else { // select STS+RICH+TOF tracks
    		  if (sts && rich && tof) {
				  fMcStsRichTrdTofMap.insert(std::pair<Int_t, Int_t>(stsMCId, iTrack));
			  }
    	  }
      }
   }
}

void CbmLitReconstructionQaImpl::ProcessMvd(
   Int_t stsId)
{
   CbmStsTrack* track = static_cast<CbmStsTrack*>(fStsTracks->At(stsId));
   if (NULL == track) return;
   Int_t nofHits = track->GetNMvdHits();
   fhMvdTrackHits[kAllHits]->Fill(nofHits);

   CbmTrackMatch* stsTrackMatch = static_cast<CbmTrackMatch*>(fStsMatches->At(stsId));
   if (NULL == stsTrackMatch) return;
   Int_t stsMcTrackId = stsTrackMatch->GetMCTrackId();

   Int_t nofTrueHits = 0, nofFakeHits = 0;
   for (Int_t iHit = 0; iHit < nofHits; iHit++) {
      Int_t hitId = track->GetMvdHitIndex(iHit);
      CbmMvdHitMatch* hitMatch = static_cast<CbmMvdHitMatch*>(fMvdHitMatches->At(hitId));
      if (NULL == hitMatch) continue;
      Int_t pointId = hitMatch->GetPointId();
      CbmMvdPoint* point = static_cast<CbmMvdPoint*>(fMvdPoints->At(pointId));
      if (NULL == point) continue;
      Int_t mcTrackId = point->GetTrackID();
      if (mcTrackId == stsMcTrackId) { // true hit
         nofTrueHits++;
      } else { // fake hit
         nofFakeHits++;
      }
   }
   fhMvdTrackHits[kTrueHits]->Fill(nofTrueHits);
   fhMvdTrackHits[kFakeHits]->Fill(nofFakeHits);
   if (nofHits != 0) {
      fhMvdTrackHits[kTrueOverAll]->Fill(Float_t(nofTrueHits) / Float_t(nofHits));
      fhMvdTrackHits[kFakeOverAll]->Fill(Float_t(nofFakeHits) / Float_t(nofHits));
   }
}

Bool_t CbmLitReconstructionQaImpl::CheckTrackQuality(
   CbmTrackMatch* trackMatch,
   DetectorId detId)
{
   Int_t mcId = trackMatch->GetMCTrackId();
   if(mcId < 0) { return false; }

   Int_t nofTrue = trackMatch->GetNofTrueHits();
   Int_t nofWrong = trackMatch->GetNofWrongHits();
   Int_t nofFake = trackMatch->GetNofFakeHits();
   Int_t nofHits = nofTrue + nofWrong + nofFake;
   Double_t quali = Double_t(nofTrue) / Double_t(nofHits);
   Double_t fakequali = Double_t(nofFake + nofWrong) / Double_t(nofHits);

   if(detId == kSTS) {
      fhStsTrackHits[kAllHits]->Fill(nofHits);
      fhStsTrackHits[kTrueHits]->Fill(nofTrue);
      fhStsTrackHits[kFakeHits]->Fill(nofFake + nofWrong);
      fhStsTrackHits[kTrueOverAll]->Fill(quali);
      fhStsTrackHits[kFakeOverAll]->Fill(fakequali);
   }
   if(detId == kTRD) {
      fhTrdTrackHits[kAllHits]->Fill(nofHits);
      fhTrdTrackHits[kTrueHits]->Fill(nofTrue);
      fhTrdTrackHits[kFakeHits]->Fill(nofFake + nofWrong);
      fhTrdTrackHits[kTrueOverAll]->Fill(quali);
      fhTrdTrackHits[kFakeOverAll]->Fill(fakequali);
   }
   if(detId == kMUCH) {
      fhMuchTrackHits[kAllHits]->Fill(nofHits);
      fhMuchTrackHits[kTrueHits]->Fill(nofTrue);
      fhMuchTrackHits[kFakeHits]->Fill(nofFake + nofWrong);
      fhMuchTrackHits[kTrueOverAll]->Fill(quali);
      fhMuchTrackHits[kFakeOverAll]->Fill(fakequali);
   }

   if (quali < fQuota) { return false; }
   return true;
}

Bool_t CbmLitReconstructionQaImpl::CheckRingQuality(
   CbmRichRingMatch* ringMatch)
{
   Int_t mcId = ringMatch->GetMCTrackID();
   if(mcId < 0) { return false; }

   Int_t nofTrue = ringMatch->GetNofTrueHits();
   Int_t nofWrong = ringMatch->GetNofWrongHits();
   Int_t nofFake = ringMatch->GetNofFakeHits();
   Int_t nofHits = nofTrue + nofWrong + nofFake;
   Double_t quali = Double_t(nofTrue) / Double_t(nofHits);
   Double_t fakequali = Double_t(nofFake + nofWrong) / Double_t(nofHits);

   fhRichRingHits[kAllHits]->Fill(nofHits);
   fhRichRingHits[kTrueHits]->Fill(nofTrue);
   fhRichRingHits[kFakeHits]->Fill(nofFake + nofWrong);
   fhRichRingHits[kTrueOverAll]->Fill(quali);
   fhRichRingHits[kFakeOverAll]->Fill(fakequali);

   if (quali < fQuotaRich) { return false; }
   return true;
}

void CbmLitReconstructionQaImpl::FillMcHistoForDetAcc(Double_t mom)
{
   fhStsDetAccEl[kDetAccMc]->Fill(mom);
   fhStsRichDetAccEl[kDetAccMc]->Fill(mom);
   fhStsTrdDetAccEl[kDetAccMc]->Fill(mom);
   fhStsTofDetAccEl[kDetAccMc]->Fill(mom);
   fhStsRichTrdDetAccEl[kDetAccMc]->Fill(mom);
   fhStsRichTrdTofDetAccEl[kDetAccMc]->Fill(mom);
   fhStsTrdTofDetAccEl[kDetAccMc]->Fill(mom);
}

void CbmLitReconstructionQaImpl::ProcessMcTracks()
{
   Int_t nofMcTracks = fMCTracks->GetEntriesFast();
   for (Int_t iMCTrack = 0; iMCTrack < nofMcTracks; iMCTrack++) {
      CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iMCTrack);
      if (mcTrack == NULL) { continue; }

      // Check accepted tracks cutting on minimal number of MC points
      Int_t nofPointsSts = fNofStsStationsMap[iMCTrack];//mcTrack->GetNPoints(kSTS);
      Int_t nofPointsTrd = fNofTrdStationsMap[iMCTrack];//mcTrack->GetNPoints(kTRD);
      Int_t nofPointsMuch = fNofMuchStationsMap[iMCTrack];//mcTrack->GetNPoints(kMUCH);
      Int_t nofPointsTof = mcTrack->GetNPoints(kTOF);
      Int_t nofHitsRich = fNofHitsInRingMap[iMCTrack];

      // Check local tracks
      Bool_t stsConsecutive = (fUseConsecutivePointsInSts) ?
            fNofStsConsecutiveStationsMap[iMCTrack] >= fMinNofPointsSts : true;
      Bool_t isStsOk = nofPointsSts >= fMinNofPointsSts && fIsSts && stsConsecutive;
      Bool_t isTrdOk = nofPointsTrd >= fMinNofPointsTrd && fIsTrd;
      Bool_t isMuchOk = nofPointsMuch >= fMinNofPointsMuch && fIsMuch;
      Bool_t isTofOk = nofPointsTof >= fMinNofPointsTof && fIsTof;
      Bool_t isRichOk = nofHitsRich >= fMinNofHitsRich && fIsRich;
      Bool_t isRecOk = (fIsMuch && fIsTrd) ? (isTrdOk && isMuchOk) : (isTrdOk || isMuchOk); // MUCH+TRD

      Bool_t isStsReconstructed = fMcStsMap.find(iMCTrack) != fMcStsMap.end();
      Bool_t isHalfGlobalReconstructed = fMcHalfGlobalMap.find(iMCTrack) != fMcHalfGlobalMap.end();

      // calculate polar angle
      TVector3 mom;
      mcTrack->GetMomentum(mom);
      Double_t angle = std::abs(mom.Theta() * 180 / TMath::Pi());

      Bool_t isPrimElectron = (mcTrack->GetMotherId() == -1 && std::abs(mcTrack->GetPdgCode()) == 11);

      if (isPrimElectron) FillMcHistoForDetAcc(mcTrack->GetP());



      // Check accepted tracks cutting on minimal number of MC points
      // acceptance: STS tracks only
      if (isStsOk) {
         // momentum dependence histograms
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, fhStsMom, mcTrack->GetP());
         // number of points dependence histograms
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, fhStsNp, nofPointsSts);
         // polar angle dependence histograms
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, fhStsAngle, angle);

         if (isPrimElectron) fhStsDetAccEl[kDetAccAcc]->Fill(mcTrack->GetP());

         FillMCMomVsAngle(mcTrack);
      }
      // acceptance: STS+TRD(MUCH)
      if (isStsOk && isRecOk) {
         // momentum dependence histograms
    	 // STS normalized to STS+TRD(MUCH)
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, fhStsMomNormHalfGlobal, mcTrack->GetP());
    	 // STS+TRD(MUCH)
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, fhHalfGlobalMom, mcTrack->GetP());

         // STS+TRD: Electron identification
         FillGlobalElIdHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, fhStsTrdMomElId[kElectronId], mcTrack->GetP(), "trd");

         if (isPrimElectron) fhStsTrdDetAccEl[kDetAccAcc]->Fill(mcTrack->GetP());
      }
      // acceptance: STS+TRD(MUCH)+TOF
      if (isStsOk && isRecOk && isTofOk) {
         // momentum dependence histograms
    	 // STS normalized to STS+TRD(MUCH)+TOF
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, fhStsMomNormGlobal, mcTrack->GetP());
         // STS+TRD(MUCH) normalized to STS+TRD(MUCH)+TOF
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, fhHalfGlobalMomNormGlobal, mcTrack->GetP());
    	 // STS+TRD(MUCH)+TOF
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcGlobalMap, fhGlobalMom, mcTrack->GetP());

         // STS+TRD: Electron identification
         FillGlobalElIdHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, fhStsTrdMomElIdNormStsTrdTof[kElectronId], mcTrack->GetP(), "trd");
         // STS+TRD+TOF: Electron identification
         FillGlobalElIdHistos(mcTrack, iMCTrack, fMcGlobalMap, fhStsTrdTofMomElId[kElectronId], mcTrack->GetP(), "trd+tof");

         if (isPrimElectron) fhStsTrdTofDetAccEl[kDetAccAcc]->Fill(mcTrack->GetP());
      }

      // acceptance: STS+TOF
      if (isStsOk && isTofOk) {
         if (isPrimElectron) fhStsTofDetAccEl[kDetAccAcc]->Fill(mcTrack->GetP());
      }

      // acceptance: STS as 100% + local TRD(MUCH) track cutting on number of points
      if ( isStsReconstructed && isStsOk && isRecOk ) {
    	 // std::cout << " isStsReconstructed && isStsOk && isRecOk " << std::endl;
         // momentum dependence histograms
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, fhRecMom, mcTrack->GetP());
         // number of points dependence histograms
         Int_t np = fIsElectronSetup ? nofPointsTrd : nofPointsMuch;
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, fhRecNp, np);
         // polar angle dependence histograms
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, fhRecAngle, angle);
      }

      if (fIsTrd || fIsMuch) {
         if (isHalfGlobalReconstructed && isStsOk && isRecOk && isTofOk) {
            // momentum dependence histograms
            FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcGlobalMap, fhTofMom, mcTrack->GetP());
            // polar angle dependence histograms
            FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcGlobalMap, fhTofAngle, angle);
         }
      } else { // for STS+TOF setup
         if (isStsOk && isTofOk) {
            // momentum dependence histograms
            FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcGlobalMap, fhTofMom, mcTrack->GetP());
            // polar angle dependence histograms
            FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcGlobalMap, fhTofAngle, angle);
         }
      }

      // acceptance: RICH
      if (isRichOk) {
    	  // momentum dependence histograms
    	  FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcRichMap, fhRichMom, mcTrack->GetP());
    	  // number of hits dependence histograms
    	  FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcRichMap, fhRichNh, fNofHitsInRingMap[iMCTrack]);
      }
      // acceptance: STS+RICH
      if (isStsOk && isRichOk) {
    	  // STS
          FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsMap, fhStsMomNormStsRich, mcTrack->GetP());
    	  // STS+RICH
    	  FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsRichMap, fhStsRichMom, mcTrack->GetP());

        // STS+RICH: Electron identification
        FillGlobalElIdHistos(mcTrack, iMCTrack, fMcStsRichMap, fhStsRichMomElId[kElectronId], mcTrack->GetP(), "rich");

        if (isPrimElectron) fhStsRichDetAccEl[kDetAccAcc]->Fill(mcTrack->GetP());
      }
      // acceptance: STS+RICH+TRD
      if (isStsOk && isRichOk && isTrdOk) {
    	 // STS
         FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsMap, fhStsMomNormStsRichTrd, mcTrack->GetP());
         // STS+RICH
         FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsRichMap, fhStsRichMomNormStsRichTrd, mcTrack->GetP());
         // STS+RICH+TRD
         FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsRichTrdMap, fhStsRichTrdMom, mcTrack->GetP());

         // STS+RICH: Electron identification
         FillGlobalElIdHistos(mcTrack, iMCTrack, fMcStsRichMap, fhStsRichMomElIdNormStsRichTrd[kElectronId], mcTrack->GetP(), "rich");
         // STS+RICH+TRD: Electron identification
         FillGlobalElIdHistos(mcTrack, iMCTrack, fMcStsRichTrdMap, fhStsRichTrdMomElId[kElectronId], mcTrack->GetP(), "rich+trd");

         if (isPrimElectron) fhStsRichTrdDetAccEl[kDetAccAcc]->Fill(mcTrack->GetP());
      }
      // acceptance: STS+RICH+TRD+TOF
      if (isStsOk && isRichOk && isTrdOk && isTofOk) {
    	 // STS normalized to STS+RICH+TRD+TOF
         FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsMap, fhStsMomNormStsRichTrdTof, mcTrack->GetP());
       	 // STS+RICH normalized to STS+RICH+TRD+TOF
         FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsRichMap, fhStsRichMomNormStsRichTrdTof, mcTrack->GetP());
         // STS+RICH+TRD normalized to STS+RICH+TRD+TOF
         FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsRichTrdMap, fhStsRichTrdMomNormStsRichTrdTof, mcTrack->GetP());
         // STS+RICH+TRD+TOF normalized to STS+RICH+TRD+TOF
         FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsRichTrdTofMap, fhStsRichTrdTofMom, mcTrack->GetP());

         // STS+RICH: Electron identification
          FillGlobalElIdHistos(mcTrack, iMCTrack, fMcStsRichMap, fhStsRichMomElIdNormStsRichTrdTof[kElectronId], mcTrack->GetP(), "rich");
          // STS+RICH+TRD: Electron identification
          FillGlobalElIdHistos(mcTrack, iMCTrack, fMcStsRichTrdMap, fhStsRichTrdMomElIdNormStsRichTrdTof[kElectronId], mcTrack->GetP(), "rich+trd");
          // STS+RICH+TRD+TOF: Electron identification
          FillGlobalElIdHistos(mcTrack, iMCTrack, fMcStsRichTrdTofMap, fhStsRichTrdTofMomElId[kElectronId], mcTrack->GetP(), "rich+trd+tof");

          if (isPrimElectron) fhStsRichTrdTofDetAccEl[kDetAccAcc]->Fill(mcTrack->GetP());
      }
   } // Loop over MCTracks
}

void CbmLitReconstructionQaImpl::FillGlobalReconstructionHistos(
   const CbmMCTrack* mcTrack,
   Int_t mcId,
   const std::multimap<Int_t, Int_t>& mcMap,
   std::vector<std::vector<TH1F*> >& hist,
   Double_t par)
{
   // Get MC track properties (vertex, momentum, primary/secondary, pdg, etc...)
   TVector3 vertex;
   mcTrack->GetStartVertex(vertex);
   Bool_t isPrim = mcTrack->GetMotherId() == -1;
   Double_t mom = mcTrack->GetP();
   Bool_t isRef = isPrim && mom > fRefMomentum;
   Bool_t isMuon = std::abs(mcTrack->GetPdgCode()) == 13;
   Bool_t isElectron = std::abs(mcTrack->GetPdgCode()) == 11;

   // Fill histograms for accepted tracks
   hist[kAll][kAcc]->Fill(par);
   if (isPrim) { hist[kPrim][kAcc]->Fill(par); }
   if (isRef) { hist[kRef][kAcc]->Fill(par); }
   if (!isPrim) { hist[kSec][kAcc]->Fill(par); }
   if (isPrim && isMuon) { hist[kMu][kAcc]->Fill(par); }
   if (isPrim && isElectron) { hist[kEl][kAcc]->Fill(par); }

   // Fill histograms for reconstructed tracks which are accepted
   if (mcMap.find(mcId) != mcMap.end() ) {
      hist[kAll][kRec]->Fill(par);
      if (isPrim) { hist[kPrim][kRec]->Fill(par); }
      if (isRef) { hist[kRef][kRec]->Fill(par); }
      if (!isPrim) { hist[kSec][kRec]->Fill(par); }
      if (isPrim && isMuon) { hist[kMu][kRec]->Fill(par); }
      if (isPrim && isElectron) { hist[kEl][kRec]->Fill(par); }
   }
}

void CbmLitReconstructionQaImpl::FillGlobalElIdHistos(
   const CbmMCTrack* mcTrack,
   Int_t mcId,
   const std::multimap<Int_t, Int_t>& mcMap,
   std::vector<TH1F*>& hist,
   Double_t par,
   const std::string& opt)
{
   if (!fIsElectronSetup) return;

   Bool_t isMCPrim = mcTrack->GetMotherId() == -1;
   Bool_t isMCElectron = std::abs(mcTrack->GetPdgCode()) == 11;

   if (isMCPrim && isMCElectron) hist[kAcc]->Fill(par);

   Bool_t isRichId = (opt.find("rich") != std::string::npos);
   Bool_t isTrdId = (opt.find("trd") != std::string::npos);
   Bool_t isTofId = (opt.find("tof") != std::string::npos);

   // Fill histograms for identified tracks which are accepted
   std::multimap<Int_t, Int_t>::const_iterator it = mcMap.find(mcId);
   if (it != mcMap.end() ) {
      // Make electron identification
      Int_t globalId = (*it).second;
      CbmGlobalTrack* track = static_cast<CbmGlobalTrack*>(fGlobalTracks->At(globalId));

      Bool_t isRichElectron = isRichId ? fElectronId->IsRichElectron(track, mcTrack->GetP()) : true;
      Bool_t isTrdElectron = isTrdId ? fElectronId->IsTrdElectron(track, mcTrack->GetP()) : true;
      Bool_t isTofElectron = isTofId ? fElectronId->IsTofElectron(track, mcTrack->GetP()) : true;

      Bool_t isElectron = isRichElectron && isTrdElectron && isTofElectron;

      if (isMCPrim && isMCElectron && isElectron) hist[kRec]->Fill(par);
   }
}

void CbmLitReconstructionQaImpl::FillGlobalReconstructionHistosRich(
   const CbmMCTrack* mcTrack,
   Int_t mcId,
   const std::multimap<Int_t, Int_t>& mcMap,
   std::vector<std::vector<TH1F*> >& hist,
   Double_t par)
{
   // Get MC track properties (vertex, momentum, primary/secondary, pdg, etc...)
   TVector3 vertex;
   mcTrack->GetStartVertex(vertex);
   Bool_t isPrim = mcTrack->GetMotherId() == -1;
   Double_t mom = mcTrack->GetP();
   Int_t nofHitsInRing = fNofHitsInRingMap[mcId];
   Bool_t isRef = isPrim && mom > fRefMomentum && nofHitsInRing >= fRefMinNofHitsRich;
   Bool_t isPion = std::abs(mcTrack->GetPdgCode()) == 211;
   Bool_t isElectron = std::abs(mcTrack->GetPdgCode()) == 11;

   // Fill histograms for accepted tracks and rings
   hist[kRichAll][kAcc]->Fill(par);
   if (isRef) { hist[kRichAllRef][kAcc]->Fill(par); }
   if (isElectron && isPrim) { hist[kRichEl][kAcc]->Fill(par); }
   if (isElectron && isRef) { hist[kRichElRef][kAcc]->Fill(par); }
   if (isPion) { hist[kRichPi][kAcc]->Fill(par); }
   if (isPion && isRef) { hist[kRichPiRef][kAcc]->Fill(par); }

   // Fill histograms for reconstructed rings and tracks which are accepted
   if (mcMap.find(mcId) != mcMap.end() ) {
	   hist[kRichAll][kRec]->Fill(par);
	   if (isRef) { hist[kRichAllRef][kRec]->Fill(par); }
	   if (isElectron && isPrim) { hist[kRichEl][kRec]->Fill(par); }
	   if (isElectron && isRef) { hist[kRichElRef][kRec]->Fill(par); }
	   if (isPion) { hist[kRichPi][kRec]->Fill(par); }
	   if (isPion && isRef) { hist[kRichPiRef][kRec]->Fill(par); }
   }
}

void CbmLitReconstructionQaImpl::PionSuppression()
{
   if (!fIsElectronSetup) return;

   Int_t nGlobal = fGlobalTracks->GetEntries();
   for (Int_t iTrack=0; iTrack<nGlobal; iTrack++) {

      CbmGlobalTrack* gTrack = (CbmGlobalTrack*)fGlobalTracks->At(iTrack);

      // get track segments indices
      Int_t stsId = gTrack->GetStsTrackIndex();
      Int_t trdId = gTrack->GetTrdTrackIndex();
      Int_t tofId = gTrack->GetTofHitIndex();
      Int_t richId = gTrack->GetRichRingIndex();

      // check track segments
      Bool_t isStsOk = stsId > -1 && fIsSts;
      Bool_t isTrdOk = trdId > -1 && fIsTrd;
      Bool_t isTofOk = tofId > -1 && fIsTof;
      Bool_t isRichOk = false;//richId > -1 && fIsRich;
      FairTrackParam* proj   = (FairTrackParam*)fRichProjections->At(iTrack);
      if (proj != NULL && proj->GetX()!= 0 &&  proj->GetY()!= 0) isRichOk = true;

      if (!isStsOk) continue;
      CbmStsTrack* stsTrack = (CbmStsTrack*)fStsTracks->At(stsId);
      if (NULL == stsTrack) continue;
      CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*)fStsMatches->At(stsId);
      if (NULL == stsTrackMatch) continue;
      Int_t mcIdSts = stsTrackMatch->GetMCTrackId();
      if (mcIdSts < 0) continue;
      CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(mcIdSts);
      if (NULL == mcTrack) continue;
      Int_t pdg = TMath::Abs(mcTrack->GetPdgCode());
      Int_t motherId = mcTrack->GetMotherId();
      Double_t momentum = mcTrack->GetP();

      Bool_t isPion = (pdg == 211);

      if (!isPion) continue;

      if (isTrdOk) fhStsTrdMomElId[kPionSupp][kAcc]->Fill(momentum);
      if (isTrdOk && isTofOk) fhStsTrdMomElIdNormStsTrdTof[kPionSupp][kAcc]->Fill(momentum);
      if (isTrdOk && isTofOk) fhStsTrdTofMomElId[kPionSupp][kAcc]->Fill(momentum);
      if (isRichOk) fhStsRichMomElId[kPionSupp][kAcc]->Fill(momentum);
      if (isRichOk && isTrdOk) fhStsRichMomElIdNormStsRichTrd[kPionSupp][kAcc]->Fill(momentum);
      if (isRichOk && isTrdOk) fhStsRichTrdMomElId[kPionSupp][kAcc]->Fill(momentum);
      if (isRichOk && isTrdOk && isTofOk) fhStsRichMomElIdNormStsRichTrdTof[kPionSupp][kAcc]->Fill(momentum);
      if (isRichOk && isTrdOk && isTofOk) fhStsRichTrdMomElIdNormStsRichTrdTof[kPionSupp][kAcc]->Fill(momentum);
      if (isRichOk && isTrdOk && isTofOk) fhStsRichTrdTofMomElId[kPionSupp][kAcc]->Fill(momentum);

//      fhStsTrdMomElId[kPionSupp][kAcc]->Fill(momentum);
//      fhStsTrdMomElIdNormStsTrdTof[kPionSupp][kAcc]->Fill(momentum);
//      fhStsTrdTofMomElId[kPionSupp][kAcc]->Fill(momentum);
//      fhStsRichMomElId[kPionSupp][kAcc]->Fill(momentum);
//      fhStsRichMomElIdNormStsRichTrd[kPionSupp][kAcc]->Fill(momentum);
//      fhStsRichTrdMomElId[kPionSupp][kAcc]->Fill(momentum);
//      fhStsRichMomElIdNormStsRichTrdTof[kPionSupp][kAcc]->Fill(momentum);
//      fhStsRichTrdMomElIdNormStsRichTrdTof[kPionSupp][kAcc]->Fill(momentum);
//      fhStsRichTrdTofMomElId[kPionSupp][kAcc]->Fill(momentum);

      Bool_t isRichElectron = fElectronId->IsRichElectron(gTrack, mcTrack->GetP());
      Bool_t isTrdElectron = fElectronId->IsTrdElectron(gTrack, mcTrack->GetP());
      Bool_t isTofElectron = fElectronId->IsTofElectron(gTrack, mcTrack->GetP());

      if (isTrdElectron) fhStsTrdMomElId[kPionSupp][kRec]->Fill(momentum);
      if (isTrdElectron) fhStsTrdMomElIdNormStsTrdTof[kPionSupp][kRec]->Fill(momentum);
      if (isTrdElectron && isTofElectron) fhStsTrdTofMomElId[kPionSupp][kRec]->Fill(momentum);
      if (isRichElectron) fhStsRichMomElId[kPionSupp][kRec]->Fill(momentum);
      if (isRichElectron) fhStsRichMomElIdNormStsRichTrd[kPionSupp][kRec]->Fill(momentum);
      if (isRichElectron && isTrdElectron) fhStsRichTrdMomElId[kPionSupp][kRec]->Fill(momentum);
      if (isRichElectron) fhStsRichMomElIdNormStsRichTrdTof[kPionSupp][kRec]->Fill(momentum);
      if (isRichElectron && isTrdElectron) fhStsRichTrdMomElIdNormStsRichTrdTof[kPionSupp][kRec]->Fill(momentum);
      if (isRichElectron && isTrdElectron && isTofElectron) fhStsRichTrdTofMomElId[kPionSupp][kRec]->Fill(momentum);
   }
}

void CbmLitReconstructionQaImpl::StsTracksQa()
{
   Int_t nSts = fStsTracks->GetEntriesFast();
   for (Int_t i = 0; i < nSts; i++) {
      CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(i);
      if (stsTrack == NULL) continue;
      CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*) fStsMatches->At(i);
      if (stsTrackMatch == NULL) continue;
      Int_t mcIdSts = stsTrackMatch->GetMCTrackId();
      if (mcIdSts < 0) continue;
      CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(mcIdSts);
      if (mcTrack == NULL) continue;
      Int_t motherId = mcTrack->GetMotherId();
      if (motherId != -1) continue;

      TVector3 momMC;
      mcTrack->GetMomentum(momMC);
      //fKFFitter.DoFit(stsTrack,11);
      Double_t chiPrimary = fKFFitter->GetChiToVertex(stsTrack, fPrimVertex);
      fhStsChiprim->Fill(chiPrimary);

      FairTrackParam vtxTrack;
      fKFFitter->FitToVertex(stsTrack, fPrimVertex, &vtxTrack);
      TVector3 momRec;
      vtxTrack.Momentum(momRec);

      Double_t dpp = 100. * (momMC.Mag() - momRec.Mag()) / momMC.Mag();
      fhStsMomresVsMom->Fill(momMC.Mag(), dpp);
   }
}

void CbmLitReconstructionQaImpl::FillMCMomVsAngle(
      const CbmMCTrack* mcTrack)
{
   // Get MC track properties (vertex, momentum, primary/secondary, pdg, etc...)
   TVector3 vertex;
   mcTrack->GetStartVertex(vertex);
   Bool_t isPrim = mcTrack->GetMotherId() == -1;
   Double_t p = mcTrack->GetP();
   Bool_t isRef = isPrim && p > fRefMomentum;
   Bool_t isMuon = std::abs(mcTrack->GetPdgCode()) == 13;
   Bool_t isElectron = std::abs(mcTrack->GetPdgCode()) == 11;
   TVector3 mom;
   mcTrack->GetMomentum(mom);
   Double_t angle = std::abs(mom.Theta() * 180 / TMath::Pi());

   fhMCMomVsAngle[kAll]->Fill(p, angle);
   if (isPrim) { fhMCMomVsAngle[kPrim]->Fill(p, angle); }
   if (isRef) { fhMCMomVsAngle[kRef]->Fill(p, angle); }
   if (!isPrim) { fhMCMomVsAngle[kSec]->Fill(p, angle); }
   if (isPrim && isMuon) { fhMCMomVsAngle[kMu]->Fill(p, angle); }
   if (isPrim && isElectron) { fhMCMomVsAngle[kEl]->Fill(p, angle); }
}

void CbmLitReconstructionQaImpl::CreateEffHisto(
		std::vector<std::vector<TH1F*> >& hist,
		const std::string& name,
		Int_t nofBins,
		Double_t minBin,
		Double_t maxBin,
		const std::string& opt,
		TFile* file)
{
	hist.resize(fNofCategories);
	for (Int_t i = 0; i < fNofCategories; i++) {hist[i].resize(fNofTypes);}

	std::string cat[fNofCategories];
	if (opt == "tracking"){
		cat[kAll] = "All"; cat[kRef] = "Ref"; cat[kPrim] = "Prim";
		cat[kSec] = "Sec"; cat[kMu] = "Muon"; cat[kEl] = "Electron";
	} else if (opt == "rich"){
		cat[kRichAll] = "All"; cat[kRichAllRef] = "AllRef"; cat[kRichEl] = "Electron";
		cat[kRichElRef] = "ElectronRef"; cat[kRichPi] = "Pion"; cat[kRichPiRef] = "PionRef";
	}
	std::string type[] = { "Acc", "Rec", "Eff" };

	for (Int_t i = 0; i < fNofCategories; i++) {
	   for (Int_t j = 0; j < fNofTypes; j++) {
	      std::string histName = name + type[j] + cat[i];
	      if (file == NULL){
			  hist[i][j] = new TH1F(histName.c_str(), histName.c_str(), nofBins, minBin, maxBin);
	      } else {
	    	  hist[i][j] = (TH1F*)file->Get(histName.c_str());
	      }
	      fHistoList->Add(hist[i][j]);
	   }
	}
}

void CbmLitReconstructionQaImpl::CreateEffHistoElId(
      std::vector<std::vector<TH1F*> >& hist,
      const std::string& name,
      Int_t nofBins,
      Double_t minBin,
      Double_t maxBin,
      TFile* file)
{
   hist.resize(2);
   for (Int_t i = 0; i < 2; i++) hist[i].resize(fNofTypes);
   std::string cat[] = {"ElId", "PiSupp"};
   std::string type[] = {"Acc", "Rec", "Eff" };

   for (Int_t i = 0; i < 2; i++){
      for (Int_t j = 0; j < fNofTypes; j++) {
         std::string histName = name + cat[i] +type[j];
         if (file == NULL){
           hist[i][j] = new TH1F(histName.c_str(), histName.c_str(), nofBins, minBin, maxBin);
         } else {
           hist[i][j] = (TH1F*)file->Get(histName.c_str());
         }
         fHistoList->Add(hist[i][j]);
      }
   }
}

void CbmLitReconstructionQaImpl::CreateEffHistoDetAcc(
      std::vector<TH1F*>& hist,
      const std::string& name,
      Int_t nofBins,
      Double_t minBin,
      Double_t maxBin,
      TFile* file)
{
   hist.resize(fNofTypes);
   std::string type[] = {"Mc", "Acc", "Eff" };

   for (Int_t j = 0; j < fNofTypes; j++) {
      std::string histName = name + type[j];
      if (file == NULL){
        hist[j] = new TH1F(histName.c_str(), histName.c_str(), nofBins, minBin, maxBin);
      } else {
        hist[j] = (TH1F*)file->Get(histName.c_str());
      }
      fHistoList->Add(hist[j]);
   }
}

void CbmLitReconstructionQaImpl::CreateHistos(
		TFile* file)
{
   // Histogram list
   fHistoList = new TList();

   // Number of points distributions
   Double_t minNofPoints =  0.;
   Double_t maxNofPoints = 100.;
   Int_t nBinsNofPoints = 100;

   // Reconstruction efficiency histograms

   CreateEffHisto(fhStsMom, "hStsMom", fNofBinsMom, fMinMom, fMaxMom, "tracking", file);
   CreateEffHisto(fhStsMomNormHalfGlobal, "hStsMomNormHalfGlobal", fNofBinsMom, fMinMom, fMaxMom, "tracking", file);
   CreateEffHisto(fhStsMomNormGlobal, "hStsMomNormGlobal", fNofBinsMom, fMinMom, fMaxMom, "tracking", file);
   CreateEffHisto(fhStsNp, "hStsNp", nBinsNofPoints, minNofPoints, maxNofPoints, "tracking", file);
   CreateEffHisto(fhStsAngle, "hStsAngle", fNofBinsAngle, fMinAngle, fMaxAngle, "tracking", file);
   CreateEffHisto(fhHalfGlobalMom, "hHalfGlobalMom", fNofBinsMom, fMinMom, fMaxMom, "tracking", file);
   CreateEffHisto(fhHalfGlobalMomNormGlobal, "hHalfGlobalMomNormGlobal", fNofBinsMom, fMinMom, fMaxMom, "tracking", file);
   CreateEffHisto(fhGlobalMom, "hGlobalMom", fNofBinsMom, fMinMom, fMaxMom, "tracking", file);
   CreateEffHisto(fhRecMom, "hRecMom", fNofBinsMom, fMinMom, fMaxMom, "tracking", file);
   CreateEffHisto(fhRecNp, "hRecNp", nBinsNofPoints, minNofPoints, maxNofPoints, "tracking", file);
   CreateEffHisto(fhRecAngle, "hRecAngle", fNofBinsAngle, fMinAngle, fMaxAngle, "tracking", file);
   CreateEffHisto(fhTofMom, "hTofMom", fNofBinsMom, fMinMom, fMaxMom, "tracking", file);
   CreateEffHisto(fhTofAngle, "hTofAngle", fNofBinsAngle, fMinAngle, fMaxAngle, "tracking", file);

   CreateEffHisto(fhRichMom, "hRichMom", fNofBinsMom, fMinMom, fMaxMom, "rich", file);
   CreateEffHisto(fhRichNh, "hRichNh", nBinsNofPoints, minNofPoints, maxNofPoints, "rich", file);

   CreateEffHisto(fhStsMomNormStsRich, "hStsMomNormStsRich", fNofBinsMom, fMinMom, fMaxMom, "rich", file);
   CreateEffHisto(fhStsRichMom, "hStsRichMom", fNofBinsMom, fMinMom, fMaxMom, "rich", file);

   CreateEffHisto(fhStsMomNormStsRichTrd, "hStsMomNormStsRichTrd", fNofBinsMom, fMinMom, fMaxMom, "rich", file);
   CreateEffHisto(fhStsRichMomNormStsRichTrd, "hStsRichMomNormStsRichTrd", fNofBinsMom, fMinMom, fMaxMom, "rich", file);
   CreateEffHisto(fhStsRichTrdMom, "hStsRichTrdMom", fNofBinsMom, fMinMom, fMaxMom, "rich", file);

   CreateEffHisto(fhStsMomNormStsRichTrdTof, "hStsMomNormStsRichTrdTof", fNofBinsMom, fMinMom, fMaxMom, "rich", file);
   CreateEffHisto(fhStsRichMomNormStsRichTrdTof, "fhStsRichMomNormStsRichTrdTof", fNofBinsMom, fMinMom, fMaxMom, "rich", file);
   CreateEffHisto(fhStsRichTrdMomNormStsRichTrdTof, "fhStsRichTrdMomNormStsRichTrdTof", fNofBinsMom, fMinMom, fMaxMom, "rich", file);
   CreateEffHisto(fhStsRichTrdTofMom, "fhStsRichTrdTofMom", fNofBinsMom, fMinMom, fMaxMom, "rich", file);

   //Electron identification efficiency histograms
   CreateEffHistoElId(fhStsTrdMomElId, "hStsTrdMomElId", fNofBinsMom, fMinMom, fMaxMom, file);
   CreateEffHistoElId(fhStsTrdMomElIdNormStsTrdTof, "hStsTrdMomElIdNormStsTrdTof", fNofBinsMom, fMinMom, fMaxMom, file);
   CreateEffHistoElId(fhStsTrdTofMomElId, "hStsTrdTofMomElId", fNofBinsMom, fMinMom, fMaxMom, file);

   CreateEffHistoElId(fhStsRichMomElId, "fhStsRichMomElId", fNofBinsMom, fMinMom, fMaxMom, file);

   CreateEffHistoElId(fhStsRichMomElIdNormStsRichTrd, "fhStsRichMomElIdNormStsRichTrd", fNofBinsMom, fMinMom, fMaxMom, file);
   CreateEffHistoElId(fhStsRichTrdMomElId, "fhStsRichTrdMomElId", fNofBinsMom, fMinMom, fMaxMom, file);

   CreateEffHistoElId(fhStsRichMomElIdNormStsRichTrdTof, "fhStsRichMomElIdNormStsRichTrdTof", fNofBinsMom, fMinMom, fMaxMom, file);
   CreateEffHistoElId(fhStsRichTrdMomElIdNormStsRichTrdTof, "fhStsRichTrdMomElIdNormStsRichTrdTof", fNofBinsMom, fMinMom, fMaxMom, file);
   CreateEffHistoElId(fhStsRichTrdTofMomElId, "fhStsRichTrdTofMomElId", fNofBinsMom, fMinMom, fMaxMom, file);

   // Detector acceptance histogramm
   CreateEffHistoDetAcc(fhStsDetAccEl, "hStsDetAccEl", fNofBinsMom, fMinMom, fMaxMom, file);
   CreateEffHistoDetAcc(fhStsRichDetAccEl, "hStsRichDetAccEl", fNofBinsMom, fMinMom, fMaxMom, file);
   CreateEffHistoDetAcc(fhStsTrdDetAccEl, "hStsTrdDetAccEl", fNofBinsMom, fMinMom, fMaxMom, file);
   CreateEffHistoDetAcc(fhStsTofDetAccEl, "hStsTofDetAccEl", fNofBinsMom, fMinMom, fMaxMom, file);
   CreateEffHistoDetAcc(fhStsRichTrdDetAccEl, "hStsRichTrdDetAccEl", fNofBinsMom, fMinMom, fMaxMom, file);
   CreateEffHistoDetAcc(fhStsRichTrdTofDetAccEl, "fhStsRichTrdTofDetAccEl", fNofBinsMom, fMinMom, fMaxMom, file);
   CreateEffHistoDetAcc(fhStsTrdTofDetAccEl, "fhStsTrdTofDetAccEl", fNofBinsMom, fMinMom, fMaxMom, file);

   //Create histograms for ghost tracks
   if (file == NULL){
	   fhStsGhostNh = new TH1F("hStsGhostNh", "STS: ghost tracks", nBinsNofPoints, minNofPoints, maxNofPoints);
	   fhRecGhostNh = new TH1F("hRecGhostNh", "TRD(MUCH): ghost tracks", nBinsNofPoints, minNofPoints, maxNofPoints);
	   fhRichGhostNh = new TH1F("hRichGhostNh", "RICH: ghost rings", nBinsNofPoints, minNofPoints, maxNofPoints);
      fhRichGhostStsMatchingNh = new TH1F("fhRichGhostStsMatchingNh", "RICH: ghost rings", nBinsNofPoints, minNofPoints, maxNofPoints);
      fhRichGhostElIdNh = new TH1F("fhRichGhostElIdNh", "RICH: ghost rings", nBinsNofPoints, minNofPoints, maxNofPoints);
      fhStsGhostRichMatchingNh = new TH1F("fhStsGhostRichMatchingNh", "STS: ghost tracks", nBinsNofPoints, minNofPoints, maxNofPoints);
   } else {
	   fhStsGhostNh = (TH1F*)file->Get("hStsGhostNh");
	   fhRecGhostNh = (TH1F*)file->Get("hRecGhostNh");
	   fhRichGhostNh = (TH1F*)file->Get("hRichGhostNh");
      fhRichGhostStsMatchingNh = (TH1F*)file->Get("fhRichGhostStsMatchingNh");
      fhRichGhostElIdNh = (TH1F*)file->Get("fhRichGhostElIdNh");
      fhStsGhostRichMatchingNh = (TH1F*)file->Get("fhStsGhostRichMatchingNh");
   }
   fHistoList->Add(fhStsGhostNh);
   fHistoList->Add(fhRecGhostNh);
   fHistoList->Add(fhRichGhostNh);
   fHistoList->Add(fhRichGhostStsMatchingNh);
   fHistoList->Add(fhRichGhostElIdNh);
   fHistoList->Add(fhStsGhostRichMatchingNh);

   const UInt_t maxNofStations = 30;
   if (file == NULL){
	   fhMvdNofHitsInStation = new TH1F("hMvdNofHitsInStation", "MVD: number of hits", maxNofStations, 0, maxNofStations);
	   fhStsNofHitsInStation = new TH1F("hStsNofHitsInStation", "STS: number of hits", maxNofStations, 0, maxNofStations);
	   fhTrdNofHitsInStation = new TH1F("hTrdNofHitsInStation", "TRD: number of hits", maxNofStations, 0, maxNofStations);
	   fhMuchNofHitsInStation = new TH1F("hMuchNofHitsInStation", "MUCH: number of hits", maxNofStations, 0, maxNofStations);
	   fhTofNofHitsInStation = new TH1F("hTofNofHitsInStation", "TOF: number of hits", maxNofStations, 0, maxNofStations);
   } else {
	   fhMvdNofHitsInStation = (TH1F*)file->Get("hMvdNofHitsInStation");
	   fhStsNofHitsInStation = (TH1F*)file->Get("hStsNofHitsInStation");
	   fhTrdNofHitsInStation = (TH1F*)file->Get("hTrdNofHitsInStation");
	   fhMuchNofHitsInStation = (TH1F*)file->Get("hMuchNofHitsInStation");
	   fhTofNofHitsInStation = (TH1F*)file->Get("hTofNofHitsInStation");
  }
   fHistoList->Add(fhMvdNofHitsInStation);
   fHistoList->Add(fhStsNofHitsInStation);
   fHistoList->Add(fhTrdNofHitsInStation);
   fHistoList->Add(fhMuchNofHitsInStation);
   fHistoList->Add(fhTofNofHitsInStation);

   const UInt_t nofHitsHistos = 5;
   std::string hittype[] = { "All", "True", "Fake", "TrueOverAll", "FakeOverAll" };
   Double_t hitmin[] = {0, 0, 0, -0.1, -0.1};
   Double_t hitmax[] = {20, 20, 20, 1.1, 1.1};
   Int_t hitbins[] = {20, 20, 20, 12, 12};
   Double_t hitmaxrich[] = {50, 50, 50, 1.1, 1.1};
   Int_t hitbinsrich[] = {50, 50, 50, 12, 12};
   fhMvdTrackHits.resize(nofHitsHistos);
   fhStsTrackHits.resize(nofHitsHistos);
   fhTrdTrackHits.resize(nofHitsHistos);
   fhMuchTrackHits.resize(nofHitsHistos);
   fhRichRingHits.resize(nofHitsHistos);
   for(UInt_t i = 0; i < nofHitsHistos; i++) {
      std::string histName1 = "hMvdTrackHits" + hittype[i];
      std::string histName2 = "hStsTrackHits" + hittype[i];
      std::string histName3 = "hTrdTrackHits" + hittype[i];
      std::string histName4 = "hMuchTrackHits" + hittype[i];
      std::string histName5 = "hRichRingHits" + hittype[i];

      if (file == NULL){
		  fhMvdTrackHits[i] = new TH1F(histName1.c_str(), histName1.c_str(), hitbins[i], hitmin[i], hitmax[i]);
		  fhStsTrackHits[i] = new TH1F(histName2.c_str(), histName2.c_str(), hitbins[i], hitmin[i], hitmax[i]);
		  fhTrdTrackHits[i] = new TH1F(histName3.c_str(), histName3.c_str(), hitbins[i], hitmin[i], hitmax[i]);
		  fhMuchTrackHits[i] = new TH1F(histName4.c_str(), histName4.c_str(), hitbins[i], hitmin[i], hitmax[i]);
		  fhRichRingHits[i] = new TH1F(histName5.c_str(), histName5.c_str(), hitbinsrich[i], hitmin[i], hitmaxrich[i]);
      } else {
          fhMvdTrackHits[i] = (TH1F*)file->Get(histName1.c_str());
          fhStsTrackHits[i] = (TH1F*)file->Get(histName2.c_str());
          fhTrdTrackHits[i] = (TH1F*)file->Get(histName3.c_str());
          fhMuchTrackHits[i] = (TH1F*)file->Get(histName4.c_str());
          fhRichRingHits[i] = (TH1F*)file->Get(histName5.c_str());
      }
      fHistoList->Add(fhMvdTrackHits[i]);
      fHistoList->Add(fhStsTrackHits[i]);
      fHistoList->Add(fhTrdTrackHits[i]);
      fHistoList->Add(fhMuchTrackHits[i]);
      fHistoList->Add(fhRichRingHits[i]);
   }

   if (file == NULL){
      Int_t nofBinsC = 1000;
      Double_t maxXC = 50000.;
	   fhNofGlobalTracks = new TH1F("hNofGlobalTracks","hNofGlobalTracks", nofBinsC, 1., maxXC);
	   fhNofStsTracks = new TH1F("hNofStsTracks","hNofStsTracks", nofBinsC, 1., maxXC);
	   fhNofTrdTracks = new TH1F("hNofTrdTracks","hNofTrdTracks", nofBinsC, 1., maxXC);
	   fhNofMuchTracks = new TH1F("hNofMuchTracks","hNofMuchTracks", nofBinsC, 1., maxXC);
	   fhNofRichRings = new TH1F("hNofRichRings","hNofRichRings", nofBinsC, 1., maxXC);
	   fhNofRichProjections = new TH1F("hNofRichProjections","hNofRichProjections", nofBinsC, 1., maxXC);

	   fhNofMvdHits = new TH1F("hNofMvdHits","hNofMvdHits", nofBinsC, 1., maxXC);
	   fhNofStsHits = new TH1F("hNofStsHits","hNofStsHits", nofBinsC, 1., maxXC);
	   fhNofRichHits = new TH1F("hNofRichHits","hNofRichHits", nofBinsC, 1., maxXC);
	   fhNofTrdHits = new TH1F("hNofTrdHits","hNofTrdHits", nofBinsC, 1., maxXC);
	   fhNofMuchPixelHits = new TH1F("hNofMuchPixelHits","hNofMuchPixelHits", nofBinsC, 1., maxXC);
	   fhNofMuchStrawHits = new TH1F("hNofMuchStrawHits","hNofMuchStrawHits", nofBinsC, 1., maxXC);
	   fhNofTofHits = new TH1F("hNofTofHits","hNofTofHits", nofBinsC, 1., maxXC);

	   fhNofMvdPoints = new TH1F("hNofMvdPoints","hNofMvdPoints", nofBinsC, 1., maxXC);
	   fhNofStsPoints = new TH1F("hNofStsPoints","hNofStsPoints", nofBinsC, 1., maxXC);
	   fhNofRichPoints = new TH1F("hNofRichPoints","hNofRichPoints", nofBinsC, 1., maxXC);
	   fhNofTrdPoints = new TH1F("hNofTrdPoints","hNofTrdPoints", nofBinsC, 1., maxXC);
	   fhNofMuchPoints = new TH1F("hNofMuchPoints","hNofMuchPoints", nofBinsC, 1., maxXC);
	   fhNofTofPoints = new TH1F("hNofTofPoints","hNofTofPoints", nofBinsC, 1., maxXC);

	   fhNofMvdDigis = new TH1F("hNofMvdDigis","hNofMvdDigis", nofBinsC, 1., maxXC);
      fhNofStsDigis = new TH1F("hNofStsDigis","hNofStsDigis", nofBinsC, 1., maxXC);
      fhNofMuchDigis = new TH1F("hNofMuchDigis","hNofMuchDigis", nofBinsC, 1., maxXC);
      fhNofTrdDigis = new TH1F("hNofTrdDigis","hNofTrdDigis", nofBinsC, 1., maxXC);

      fhNofMvdClusters = new TH1F("hNofMvdClusters","hNofMvdClusters", nofBinsC, 1., maxXC);
      fhNofStsClusters = new TH1F("hNofStsClusters","hNofStsClusters", nofBinsC, 1., maxXC);
      fhNofMuchClusters = new TH1F("hNofMuchClusters","hNofMuchClusters", nofBinsC, 1., maxXC);
      fhNofTrdClusters = new TH1F("hNofTrdClusters","hNofTrdClusters", nofBinsC, 1., maxXC);

	   fhStsChiprim = new TH1F("fhStsChiprim","fhStsChiprim", 150, 0., 15.);
	   fhStsMomresVsMom = new TH2F("fhStsMomresVsMom","fhStsMomresVsMom", 120, 0., 12., 100, -15., 15.);
   }else {
	   fhNofGlobalTracks = (TH1F*)file->Get("hNofGlobalTracks");
	   fhNofStsTracks = (TH1F*)file->Get("hNofStsTracks");
	   fhNofTrdTracks = (TH1F*)file->Get("hNofTrdTracks");
	   fhNofMuchTracks = (TH1F*)file->Get("hNofMuchTracks");
	   fhNofRichRings = (TH1F*)file->Get("hNofRichRings");
	   fhNofRichProjections = (TH1F*)file->Get("hNofRichProjections");

	   fhNofMvdHits = (TH1F*)file->Get("hNofMvdHits");
	   fhNofStsHits = (TH1F*)file->Get("hNofStsHits");
	   fhNofRichHits = (TH1F*)file->Get("hNofRichHits");
	   fhNofTrdHits = (TH1F*)file->Get("hNofTrdHits");
	   fhNofMuchPixelHits = (TH1F*)file->Get("hNofMuchPixelHits");
	   fhNofMuchStrawHits = (TH1F*)file->Get("hNofMuchStrawHits");
	   fhNofTofHits = (TH1F*)file->Get("hNofTofHits");

	   fhNofMvdPoints = (TH1F*)file->Get("hNofMvdPoints");
      fhNofStsPoints = (TH1F*)file->Get("hNofStsPoints");
      fhNofRichPoints = (TH1F*)file->Get("hNofRichPoints");
      fhNofTrdPoints = (TH1F*)file->Get("hNofTrdPoints");
      fhNofMuchPoints = (TH1F*)file->Get("hNofMuchPoints");
      fhNofTofPoints = (TH1F*)file->Get("hNofTofPoints");

      fhNofMvdDigis = (TH1F*)file->Get("hNofMvdDigis");
      fhNofStsDigis = (TH1F*)file->Get("hNofStsDigis");
      fhNofMuchDigis = (TH1F*)file->Get("hNofMuchDigis");
      fhNofTrdDigis = (TH1F*)file->Get("hNofTrdDigis");

      fhNofMvdClusters = (TH1F*)file->Get("hNofMvdClusters");
      fhNofStsClusters = (TH1F*)file->Get("hNofStsClusters");
      fhNofMuchClusters = (TH1F*)file->Get("hNofMuchClusters");
      fhNofTrdClusters = (TH1F*)file->Get("hNofTrdClusters");

	   fhStsChiprim = (TH1F*)file->Get("fhStsChiprim");
	   fhStsMomresVsMom = (TH2F*)file->Get("fhStsMomresVsMom");
   }
   fHistoList->Add(fhNofGlobalTracks);
   fHistoList->Add(fhNofStsTracks);
   fHistoList->Add(fhNofTrdTracks);
   fHistoList->Add(fhNofMuchTracks);
   fHistoList->Add(fhNofRichRings);
   fHistoList->Add(fhNofRichProjections);

   fHistoList->Add(fhNofMvdHits);
   fHistoList->Add(fhNofStsHits);
   fHistoList->Add(fhNofRichHits);
   fHistoList->Add(fhNofTrdHits);
   fHistoList->Add(fhNofMuchPixelHits);
   fHistoList->Add(fhNofMuchStrawHits);
   fHistoList->Add(fhNofTofHits);

   fHistoList->Add(fhNofMvdPoints);
   fHistoList->Add(fhNofStsPoints);
   fHistoList->Add(fhNofRichPoints);
   fHistoList->Add(fhNofTrdPoints);
   fHistoList->Add(fhNofMuchPoints);
   fHistoList->Add(fhNofTofPoints);

   fHistoList->Add(fhNofMvdDigis);
   fHistoList->Add(fhNofStsDigis);
   fHistoList->Add(fhNofMuchDigis);
   fHistoList->Add(fhNofTrdDigis);

   fHistoList->Add(fhNofMvdDigis);
   fHistoList->Add(fhNofStsClusters);
   fHistoList->Add(fhNofMuchDigis);
   fHistoList->Add(fhNofTrdDigis);

   fHistoList->Add(fhStsChiprim);
   fHistoList->Add(fhStsMomresVsMom);

   // MC momentum vs. polar angle histograms
   fhMCMomVsAngle.resize(fNofCategories);
   std::string cat[fNofCategories];
   cat[kAll] = "All"; cat[kRef] = "Ref"; cat[kPrim] = "Prim";
   cat[kSec] = "Sec"; cat[kMu] = "Muon"; cat[kEl] = "Electron";
   for (Int_t i = 0; i < fNofCategories; i++) {
      std::string histName = "hMCMomVsAngle" + cat[i];
      if (file == NULL){
         fhMCMomVsAngle[i] = new TH2F(histName.c_str(), histName.c_str(), fNofBinsMom, fMinMom, fMaxMom, 10, 0., 35.);
      } else {
         fhMCMomVsAngle[i] = (TH2F*)file->Get(histName.c_str());
      }
      fHistoList->Add(fhMCMomVsAngle[i]);
   }

   // Histogram stores number of events
   if (file == NULL){
	   fhEventNo = new TH1F("hEventNo","hEventNo", 1, 0, 1.);
   } else {
	   fhEventNo = (TH1F*)file->Get("hEventNo");
   }
   fHistoList->Add(fhEventNo);
}

void CbmLitReconstructionQaImpl::DivideHistos(
   TH1* histo1,
   TH1* histo2,
   TH1* histo3,
   Double_t c)
{
   histo1->Sumw2();
   histo2->Sumw2();
   histo3->Divide(histo1, histo2, c, 1., "");
}

void CbmLitReconstructionQaImpl::CalculateEfficiencyHistos()
{
   Double_t nofEvents = (Double_t)fhEventNo->GetEntries();

   // Divide histograms for efficiency calculation
   for (Int_t i = 0; i < fNofCategories; i++) {
      DivideHistos(fhStsMom[i][kRec], fhStsMom[i][kAcc], fhStsMom[i][kEff], 100.);
      DivideHistos(fhStsMomNormHalfGlobal[i][kRec], fhStsMomNormHalfGlobal[i][kAcc], fhStsMomNormHalfGlobal[i][kEff], 100.);
      DivideHistos(fhStsMomNormGlobal[i][kRec], fhStsMomNormGlobal[i][kAcc], fhStsMomNormGlobal[i][kEff], 100.);
      DivideHistos(fhStsNp[i][kRec], fhStsNp[i][kAcc], fhStsNp[i][kEff], 100.);
      DivideHistos(fhStsAngle[i][kRec], fhStsAngle[i][kAcc], fhStsAngle[i][kEff], 100.);
      DivideHistos(fhHalfGlobalMom[i][kRec], fhHalfGlobalMom[i][kAcc], fhHalfGlobalMom[i][kEff], 100.);
      DivideHistos(fhHalfGlobalMomNormGlobal[i][kRec], fhHalfGlobalMomNormGlobal[i][kAcc], fhHalfGlobalMomNormGlobal[i][kEff], 100.);
      DivideHistos(fhGlobalMom[i][kRec], fhGlobalMom[i][kAcc], fhGlobalMom[i][kEff], 100.);
      DivideHistos(fhRecMom[i][kRec], fhRecMom[i][kAcc], fhRecMom[i][kEff], 100.);
      DivideHistos(fhRecNp[i][kRec], fhRecNp[i][kAcc], fhRecNp[i][kEff], 100.);
      DivideHistos(fhRecAngle[i][kRec], fhRecAngle[i][kAcc], fhRecAngle[i][kEff], 100.);
      DivideHistos(fhTofMom[i][kRec], fhTofMom[i][kAcc], fhTofMom[i][kEff], 100.);
      DivideHistos(fhTofAngle[i][kRec], fhTofAngle[i][kAcc], fhTofAngle[i][kEff], 100.);
      DivideHistos(fhRichMom[i][kRec], fhRichMom[i][kAcc], fhRichMom[i][kEff], 100.);
      DivideHistos(fhRichNh[i][kRec], fhRichNh[i][kAcc], fhRichNh[i][kEff], 100.);

      DivideHistos(fhStsMomNormStsRich[i][kRec], fhStsMomNormStsRich[i][kAcc], fhStsMomNormStsRich[i][kEff], 100.);
      DivideHistos(fhStsRichMom[i][kRec], fhStsRichMom[i][kAcc], fhStsRichMom[i][kEff], 100.);

      DivideHistos(fhStsMomNormStsRichTrd[i][kRec], fhStsMomNormStsRichTrd[i][kAcc], fhStsMomNormStsRichTrd[i][kEff], 100.);
      DivideHistos(fhStsRichMomNormStsRichTrd[i][kRec], fhStsRichMomNormStsRichTrd[i][kAcc], fhStsRichMomNormStsRichTrd[i][kEff], 100.);
      DivideHistos(fhStsRichTrdMom[i][kRec], fhStsRichTrdMom[i][kAcc], fhStsRichTrdMom[i][kEff], 100.);

      DivideHistos(fhStsMomNormStsRichTrdTof[i][kRec], fhStsMomNormStsRichTrdTof[i][kAcc], fhStsMomNormStsRichTrdTof[i][kEff], 100.);
      DivideHistos(fhStsRichMomNormStsRichTrdTof[i][kRec], fhStsRichMomNormStsRichTrdTof[i][kAcc], fhStsRichMomNormStsRichTrdTof[i][kEff], 100.);
      DivideHistos(fhStsRichTrdMomNormStsRichTrdTof[i][kRec], fhStsRichTrdMomNormStsRichTrdTof[i][kAcc], fhStsRichTrdMomNormStsRichTrdTof[i][kEff], 100.);
      DivideHistos(fhStsRichTrdTofMom[i][kRec], fhStsRichTrdTofMom[i][kAcc], fhStsRichTrdTofMom[i][kEff], 100.);

      fhMCMomVsAngle[i]->Scale(1./nofEvents);
   }

   for (Int_t i = 0; i < 2; i++) {
      Int_t acc = kAcc;
      Int_t rec = kRec;
      Double_t c = 100.;
      if (i == 1){
         acc = kRec;
         rec = kAcc;
         c = 1.;
      }
      DivideHistos(fhStsTrdMomElId[i][rec], fhStsTrdMomElId[i][acc], fhStsTrdMomElId[i][kEff], c);
      DivideHistos(fhStsTrdMomElIdNormStsTrdTof[i][rec], fhStsTrdMomElIdNormStsTrdTof[i][acc], fhStsTrdMomElIdNormStsTrdTof[i][kEff], c);
      DivideHistos(fhStsTrdTofMomElId[i][rec], fhStsTrdTofMomElId[i][acc], fhStsTrdTofMomElId[i][kEff], c);

      DivideHistos(fhStsRichMomElId[i][rec], fhStsRichMomElId[i][acc], fhStsRichMomElId[i][kEff], c);

      DivideHistos(fhStsRichMomElIdNormStsRichTrd[i][rec], fhStsRichMomElIdNormStsRichTrd[i][acc], fhStsRichMomElIdNormStsRichTrd[i][kEff], c);
      DivideHistos(fhStsRichTrdMomElId[i][rec], fhStsRichTrdMomElId[i][acc], fhStsRichTrdMomElId[i][kEff], c);

      DivideHistos(fhStsRichMomElIdNormStsRichTrdTof[i][rec], fhStsRichMomElIdNormStsRichTrdTof[i][acc], fhStsRichMomElIdNormStsRichTrdTof[i][kEff], c);
      DivideHistos(fhStsRichTrdMomElIdNormStsRichTrdTof[i][rec], fhStsRichTrdMomElIdNormStsRichTrdTof[i][acc], fhStsRichTrdMomElIdNormStsRichTrdTof[i][kEff], c);
      DivideHistos(fhStsRichTrdTofMomElId[i][rec], fhStsRichTrdTofMomElId[i][acc], fhStsRichTrdTofMomElId[i][kEff], c);
   }

   DivideHistos(fhStsDetAccEl[kDetAccAcc], fhStsDetAccEl[kDetAccMc], fhStsDetAccEl[kDetAccEff], 100.);
   DivideHistos(fhStsRichDetAccEl[kDetAccAcc], fhStsRichDetAccEl[kDetAccMc], fhStsRichDetAccEl[kDetAccEff], 100.);
   DivideHistos(fhStsTrdDetAccEl[kDetAccAcc], fhStsTrdDetAccEl[kDetAccMc], fhStsTrdDetAccEl[kDetAccEff], 100.);
   DivideHistos(fhStsTofDetAccEl[kDetAccAcc], fhStsTofDetAccEl[kDetAccMc], fhStsTofDetAccEl[kDetAccEff], 100.);
   DivideHistos(fhStsRichTrdDetAccEl[kDetAccAcc], fhStsRichTrdDetAccEl[kDetAccMc], fhStsRichTrdDetAccEl[kDetAccEff], 100.);
   DivideHistos(fhStsRichTrdTofDetAccEl[kDetAccAcc], fhStsRichTrdTofDetAccEl[kDetAccMc], fhStsRichTrdTofDetAccEl[kDetAccEff], 100.);
   DivideHistos(fhStsTrdTofDetAccEl[kDetAccAcc], fhStsTrdTofDetAccEl[kDetAccMc], fhStsTrdTofDetAccEl[kDetAccEff], 100.);

   fhMvdNofHitsInStation->Scale(1./nofEvents);
   fhStsNofHitsInStation->Scale(1./nofEvents);
   fhTrdNofHitsInStation->Scale(1./nofEvents);
   fhMuchNofHitsInStation->Scale(1./nofEvents);
   fhTofNofHitsInStation->Scale(1./nofEvents);
}

void CbmLitReconstructionQaImpl::WriteToFile()
{
   TIter next(fHistoList);
   while ( TH1* histo = ((TH1*)next()) ) { histo->Write(); }
}

void CbmLitReconstructionQaImpl::IncreaseCounters()
{
   fhNofGlobalTracks->Fill(fGlobalTracks->GetEntriesFast());

   if (fIsMvd) {
      fhNofMvdHits->Fill(fMvdHits->GetEntriesFast());
      fhNofMvdPoints->Fill(fMvdPoints->GetEntriesFast());
   }
   if (fIsSts) {
	   fhNofStsTracks->Fill(fStsMatches->GetEntriesFast());
	   fhNofStsHits->Fill(fStsHits->GetEntriesFast());
      fhNofStsPoints->Fill(fStsPoints->GetEntriesFast());
      if (NULL != fStsDigis) fhNofStsDigis->Fill(fStsDigis->GetEntriesFast());
      if (NULL != fStsClusters) fhNofStsClusters->Fill(fStsClusters->GetEntriesFast());
   }
   if (fIsRich) {
	   fhNofRichRings->Fill(fRichRings->GetEntriesFast());
	   fhNofRichHits->Fill(fRichHits->GetEntriesFast());
      fhNofRichPoints->Fill(fRichPoints->GetEntriesFast());
	   fhNofRichProjections->Fill(fRichProjections->GetEntriesFast());
   }
   if (fIsTrd) {
	   fhNofTrdTracks->Fill(fTrdMatches->GetEntriesFast());
	   fhNofTrdHits->Fill(fTrdHits->GetEntriesFast());
      fhNofTrdPoints->Fill(fTrdPoints->GetEntriesFast());
      if (NULL != fTrdDigis) fhNofTrdDigis->Fill(fTrdDigis->GetEntriesFast());
      if (NULL != fTrdClusters) fhNofTrdClusters->Fill(fTrdClusters->GetEntriesFast());
   }
   if (fIsMuch) {
	   fhNofMuchTracks->Fill(fMuchMatches->GetEntriesFast());
	   fhNofMuchPixelHits->Fill(fMuchPixelHits->GetEntriesFast());
	   fhNofMuchStrawHits->Fill(fMuchStrawHits->GetEntriesFast());
      fhNofMuchPoints->Fill(fMuchPoints->GetEntriesFast());
      if (NULL != fMuchDigis) fhNofMuchDigis->Fill(fMuchDigis->GetEntriesFast());
      if (NULL != fMuchClusters) fhNofMuchClusters->Fill(fMuchClusters->GetEntriesFast());
   }
   if (fIsTof) {
      fhNofTofHits->Fill(fTofHits->GetEntriesFast());
      fhNofTofPoints->Fill(fTofPoints->GetEntriesFast());
   }
}

std::string CbmLitReconstructionQaImpl::RecDetector()
{
   std::string recDet = "";
   if (fIsTrd && !fIsMuch) { recDet = "TRD"; }
   else if (fIsMuch && !fIsTrd) { recDet = "MUCH"; }
   else if (fIsMuch && fIsTrd) { recDet = "MUCH+TRD"; }
   return recDet;
}

boost::property_tree::ptree CbmLitReconstructionQaImpl::PrintPTree()
{
   boost::property_tree::ptree pt;

   pt.put("IsElectronSetup", fIsElectronSetup);
   pt.put("hEventNo", (Int_t)fhEventNo->GetEntries());

   // Print NOF statistics
   NofStatisticsToPTree(&pt,fhNofMvdPoints, fhNofStsPoints, fhNofRichPoints,
         fhNofTrdPoints, fhNofMuchPoints, fhNofMuchPoints, fhNofTofPoints);
   NofStatisticsToPTree(&pt, fhNofMvdDigis, fhNofStsDigis, NULL,
         fhNofTrdDigis, fhNofMuchDigis, NULL, NULL);
   NofStatisticsToPTree(&pt, fhNofMvdClusters, fhNofStsClusters, NULL,
         fhNofTrdClusters, fhNofMuchClusters, NULL, NULL);
   NofStatisticsToPTree(&pt,fhNofMvdHits, fhNofStsHits, fhNofRichHits,
         fhNofTrdHits, fhNofMuchPixelHits, fhNofMuchStrawHits, fhNofTofHits);
   NofStatisticsToPTree(&pt,NULL, fhNofStsTracks, fhNofRichRings,
         fhNofTrdTracks, fhNofMuchTracks, fhNofMuchTracks, NULL);

   pt.put("hNofGlobalTracks", (Int_t)fhNofGlobalTracks->GetMean());
   if (fIsRich) {
      pt.put("hNofRichProjections", (Int_t)fhNofRichProjections->GetMean());
   }

   // print hits histos (nof all, true, fake per track/ring)
   if (fIsMvd) HitsHistosToPTree(&pt, "hMvdTrackHits", fhMvdTrackHits);
   if (fIsSts) HitsHistosToPTree(&pt, "hStsTrackHits", fhStsTrackHits);
   if (fIsTrd) HitsHistosToPTree(&pt, "hTrdTrackHits", fhTrdTrackHits);
   if (fIsMuch) HitsHistosToPTree(&pt, "hMuchTrackHits", fhMuchTrackHits);
   if (fIsRich) HitsHistosToPTree(&pt, "hRichRingHits", fhRichRingHits);

   // Print efficiency without RICH
   EventEfficiencyStatisticsToPTree(&pt, "hStsMom", fhStsMom);
   EventEfficiencyStatisticsToPTree(&pt, "hRecMom", fhRecMom);
   EventEfficiencyStatisticsToPTree(&pt, "hTofMom", fhTofMom);
   EventEfficiencyStatisticsToPTree(&pt, "hStsMomNormHalfGlobal", fhStsMomNormHalfGlobal);
   EventEfficiencyStatisticsToPTree(&pt, "hHalfGlobalMom", fhHalfGlobalMom);
   EventEfficiencyStatisticsToPTree(&pt, "hStsMomNormGlobal", fhStsMomNormGlobal);
   EventEfficiencyStatisticsToPTree(&pt, "hHalfGlobalMomNormGlobal", fhHalfGlobalMomNormGlobal);
   EventEfficiencyStatisticsToPTree(&pt, "hGlobalMom", fhGlobalMom);

   // Print RICH efficiencies
   EventEfficiencyStatisticsRichToPTree(&pt, "hRichMom", fhRichMom);
   EventEfficiencyStatisticsRichToPTree(&pt, "hStsMomNormStsRich", fhStsMomNormStsRich);
   EventEfficiencyStatisticsRichToPTree(&pt, "hStsRichMom", fhStsRichMom);
   EventEfficiencyStatisticsRichToPTree(&pt, "hStsMomNormStsRichTrd", fhStsMomNormStsRichTrd);
   EventEfficiencyStatisticsRichToPTree(&pt, "hStsRichMomNormStsRichTrd", fhStsRichMomNormStsRichTrd);
   EventEfficiencyStatisticsRichToPTree(&pt, "hStsRichTrdMom", fhStsRichTrdMom);
   EventEfficiencyStatisticsRichToPTree(&pt, "hStsMomNormStsRichTrdTof", fhStsMomNormStsRichTrdTof);
   EventEfficiencyStatisticsRichToPTree(&pt, "hStsRichMomNormStsRichTrdTof", fhStsRichMomNormStsRichTrdTof);
   EventEfficiencyStatisticsRichToPTree(&pt, "hStsRichTrdMomNormStsRichTrdTof", fhStsRichTrdMomNormStsRichTrdTof);
   EventEfficiencyStatisticsRichToPTree(&pt, "hStsRichTrdTofMom", fhStsRichTrdTofMom);

   // electron identification
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsTrdMomElId", fhStsTrdMomElId);
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsTrdMomElIdNormStsTrdTof", fhStsTrdMomElIdNormStsTrdTof);
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsTrdTofMomElId", fhStsTrdTofMomElId);
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRichMomElId", fhStsRichMomElId);
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRichMomElIdNormStsRichTrd", fhStsRichMomElIdNormStsRichTrd);
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRichTrdMomElId", fhStsRichTrdMomElId);
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRichMomElIdNormStsRichTrdTof", fhStsRichMomElIdNormStsRichTrdTof);
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRichTrdMomElIdNormStsRichTrdTof", fhStsRichTrdMomElIdNormStsRichTrdTof);
   EventEfficiencyStatisticsElIdToPTree(&pt, "hStsRichTrdTofMomElId", fhStsRichTrdTofMomElId);

   // Detector acceptance efficiency
   if (fIsSts) {
      EventDetAccElStatisticsToPTree(&pt, "hStsDetAccEl", fhStsDetAccEl[kDetAccMc],
         fhStsDetAccEl[kDetAccAcc], fhStsMom[kEl][kRec]);
   }
   if (fIsSts && fIsRich){
      EventDetAccElStatisticsToPTree(&pt, "hStsRichDetAccEl",fhStsRichDetAccEl[kDetAccMc],
         fhStsRichDetAccEl[kDetAccAcc], fhStsRichMom[kRichEl][kRec]);
   }
   if (fIsSts && fIsTrd){
      EventDetAccElStatisticsToPTree(&pt, "hStsTrdDetAccEl", fhStsTrdDetAccEl[kDetAccMc],
         fhStsTrdDetAccEl[kDetAccAcc], fhHalfGlobalMom[kEl][kRec]);
   }
   if (fIsSts && fIsTof){
      EventDetAccElStatisticsToPTree(&pt, "hStsTofDetAccEl", fhStsTofDetAccEl[kDetAccMc],
         fhStsTofDetAccEl[kDetAccAcc], NULL);
   }
   if (fIsSts && fIsRich && fIsTrd){
      EventDetAccElStatisticsToPTree(&pt, "hStsRichTrdDetAccEl", fhStsRichTrdDetAccEl[kDetAccMc],
         fhStsRichTrdDetAccEl[kDetAccAcc], fhStsRichTrdMom[kRichEl][kRec]);
   }
   if (fIsSts && fIsRich && fIsTrd && fIsTof){
      EventDetAccElStatisticsToPTree(&pt, "hStsRichTrdTofDetAccEl", fhStsRichTrdTofDetAccEl[kDetAccMc],
         fhStsRichTrdTofDetAccEl[kDetAccAcc], fhStsRichTrdTofMom[kRichEl][kRec]);
   }
   if (fIsSts && fIsTrd && fIsTof){
      EventDetAccElStatisticsToPTree(&pt, "hStsTrdTofDetAccEl", fhStsTrdTofDetAccEl[kDetAccMc],
         fhStsTrdTofDetAccEl[kDetAccAcc], fhGlobalMom[kEl][kRec]);
   }

   // print ghost statistics
   Double_t nofEvents = (Double_t)fhEventNo->GetEntries();
   pt.put("fhStsGhostNh", fhStsGhostNh->GetEntries() / nofEvents);
   pt.put("fhRecGhostNh", fhRecGhostNh->GetEntries() / nofEvents);
   pt.put("fhRichGhostNh", fhRichGhostNh->GetEntries() / nofEvents);
   pt.put("fhRichGhostStsMatchingNh", fhRichGhostStsMatchingNh->GetEntries() / nofEvents);
   pt.put("fhStsGhostRichMatchingNh", fhStsGhostRichMatchingNh->GetEntries() / nofEvents);
   pt.put("fhRichGhostElIdNh", fhRichGhostElIdNh->GetEntries() / nofEvents);

   // STS Qa
   pt.put("fhStsChiprim.mean", fhStsChiprim->GetMean());
   pt.put("fhStsChiprim.rms", fhStsChiprim->GetRMS());
   pt.put("fhStsMomresVsMom.mean", fhStsMomresVsMom->ProjectionY()->GetMean());
   pt.put("fhStsMomresVsMom.rms", fhStsMomresVsMom->ProjectionY()->GetRMS());

   // Polar angle efficiency
   pt.put("MinAngle", fMinAngle);
   pt.put("MaxAngle", fMaxAngle);
   pt.put("NofBinsAngle", fNofBinsAngle);
   PolarAngleEfficiencyToPTree(&pt, "hStsAngle", fhStsAngle);
   PolarAngleEfficiencyToPTree(&pt, "hRecAngle", fhRecAngle);
   PolarAngleEfficiencyToPTree(&pt, "hTofAngle", fhTofAngle);

   if (fOutputDir != "") write_json(std::string(fOutputDir + "rec_qa.json").c_str(), pt);
   return pt;
}

void CbmLitReconstructionQaImpl::NofStatisticsToPTree(
   boost::property_tree::ptree* pt,
   TH1F* mvd,
   TH1F* sts,
   TH1F* rich,
   TH1F* trd,
   TH1F* muchP,
   TH1F* muchS,
   TH1F* tof)
{
   std::string st;
   if (mvd != NULL && fIsMvd) {
      st = std::string(mvd->GetName());
      pt->put(st, (Int_t)mvd->GetMean());
   }
   if (sts != NULL && fIsSts) {
      st = std::string(sts->GetName());
      pt->put(st, (Int_t)sts->GetMean());
   }
   if (rich != NULL && fIsRich) {
      st = std::string(rich->GetName());
      pt->put(st, (Int_t)rich->GetMean());
   }
   if (trd != NULL && fIsTrd) {
      st = std::string(trd->GetName());
      pt->put(st, (Int_t)trd->GetMean());
   }
   if (muchP != NULL && fIsMuch) {
      st = std::string(muchP->GetName());
      pt->put(st, (Int_t)muchP->GetMean());
   }
   if (muchS != NULL && fIsMuch) {
      st = std::string(muchS->GetName());
      pt->put(st, (Int_t)muchS->GetMean());
   }
   if (tof!= NULL && fIsTof) {
      st = std::string(tof->GetName());
      pt->put(st, (Int_t)tof->GetMean());
   }
}

void CbmLitReconstructionQaImpl::HitsHistosToPTree(
      boost::property_tree::ptree* pt,
      const std::string& name,
      std::vector<TH1F*>& histos)
{
   pt->put(name+".all", histos[0]->GetMean());
   pt->put(name+".true", histos[1]->GetMean());
   pt->put(name+".fake", histos[2]->GetMean());
   pt->put(name+".trueOverAll", histos[3]->GetMean());
   pt->put(name+".fakeOverAll", histos[4]->GetMean());
}


void CbmLitReconstructionQaImpl::EventEfficiencyStatisticsToPTree(
      boost::property_tree::ptree* pt,
      const std::string& name,
      const std::vector<std::vector<TH1F*> >& hist)
{
   Double_t allEff = 0., refEff = 0., primEff = 0., secEff = 0., muEff = 0., elEff = 0.;
   Double_t allRec = hist[kAll][kRec]->GetEntries();
   Double_t allAcc = hist[kAll][kAcc]->GetEntries();
   if (allAcc != 0.) { allEff = 100.*allRec / allAcc; }
   Double_t refRec = hist[kRef][kRec]->GetEntries();
   Double_t refAcc = hist[kRef][kAcc]->GetEntries();
   if (refAcc != 0.) { refEff = 100.*refRec / refAcc; }
   Double_t primRec = hist[kPrim][kRec]->GetEntries();
   Double_t primAcc = hist[kPrim][kAcc]->GetEntries();
   if (primAcc != 0.) { primEff = 100.*primRec / primAcc; }
   Double_t secRec = hist[kSec][kRec]->GetEntries();
   Double_t secAcc = hist[kSec][kAcc]->GetEntries();
   if (secAcc != 0.) { secEff = 100.*secRec / secAcc; }
   Double_t muRec = hist[kMu][kRec]->GetEntries();
   Double_t muAcc = hist[kMu][kAcc]->GetEntries();
   if (muAcc != 0.) { muEff = 100.*muRec / muAcc; }
   Double_t elRec = hist[kEl][kRec]->GetEntries();
   Double_t elAcc = hist[kEl][kAcc]->GetEntries();
   if (elAcc != 0.) { elEff = 100.*elRec / elAcc; }

   Double_t nofEvents = (Double_t)fhEventNo->GetEntries();
   pt->put(name+".all.rec", allRec/nofEvents);
   pt->put(name+".all.acc", allAcc/nofEvents);
   pt->put(name+".all.eff", allEff);
   pt->put(name+".ref.rec", refRec/nofEvents);
   pt->put(name+".ref.acc", refAcc/nofEvents);
   pt->put(name+".ref.eff", refEff);
   pt->put(name+".prim.rec", primRec/nofEvents);
   pt->put(name+".prim.acc", primAcc/nofEvents);
   pt->put(name+".prim.eff", primEff);
   pt->put(name+".sec.rec", secRec/nofEvents);
   pt->put(name+".sec.acc", secAcc/nofEvents);
   pt->put(name+".sec.eff", secEff);
   pt->put(name+".mu.rec", muRec/nofEvents);
   pt->put(name+".mu.acc", muAcc/nofEvents);
   pt->put(name+".mu.eff", muEff);
   pt->put(name+".el.rec", elRec/nofEvents);
   pt->put(name+".el.acc", elAcc/nofEvents);
   pt->put(name+".el.eff", elEff);
}

void CbmLitReconstructionQaImpl::EventEfficiencyStatisticsRichToPTree(
      boost::property_tree::ptree* pt,
      const std::string& name,
      const std::vector<std::vector<TH1F*> >& hist)
{
   Double_t allEff = 0., allRefEff = 0., elEff = 0., elRefEff = 0., piEff = 0., piRefEff = 0.;
   Double_t allRec = hist[kRichAll][kRec]->GetEntries();
   Double_t allAcc = hist[kRichAll][kAcc]->GetEntries();
   if (allAcc != 0.) { allEff = 100.*allRec / allAcc; }
   Double_t allRefRec = hist[kRichAllRef][kRec]->GetEntries();
   Double_t allRefAcc = hist[kRichAllRef][kAcc]->GetEntries();
   if (allRefAcc != 0.) { allRefEff = 100.*allRefRec / allRefAcc; }
   Double_t elRec = hist[kRichEl][kRec]->GetEntries();
   Double_t elAcc = hist[kRichEl][kAcc]->GetEntries();
   if (elAcc != 0.) { elEff = 100.*elRec / elAcc; }
   Double_t elRefRec = hist[kRichElRef][kRec]->GetEntries();
   Double_t elRefAcc = hist[kRichElRef][kAcc]->GetEntries();
   if (elRefAcc != 0.) { elRefEff = 100.*elRefRec / elRefAcc; }
   Double_t piRec = hist[kRichPi][kRec]->GetEntries();
   Double_t piAcc = hist[kRichPi][kAcc]->GetEntries();
   if (piAcc != 0.) { piEff = 100.*piRec / piAcc; }
   Double_t piRefRec = hist[kRichPiRef][kRec]->GetEntries();
   Double_t piRefAcc = hist[kRichPiRef][kAcc]->GetEntries();
   if (piRefAcc != 0.) { piRefEff = 100.*piRefRec / piRefAcc; }

   Double_t nofEvents = (Double_t)fhEventNo->GetEntries();
   pt->put(name+".richAll.rec", allRec/nofEvents);
   pt->put(name+".richAll.acc", allAcc/nofEvents);
   pt->put(name+".richAll.eff", allEff);
   pt->put(name+".richAllRef.rec", allRefRec/nofEvents);
   pt->put(name+".richAllRef.acc", allRefAcc/nofEvents);
   pt->put(name+".richAllRef.eff", allRefEff);
   pt->put(name+".richEl.rec", elRec/nofEvents);
   pt->put(name+".richEl.acc", elAcc/nofEvents);
   pt->put(name+".richEl.eff", elEff);
   pt->put(name+".richElRef.rec", elRefRec/nofEvents);
   pt->put(name+".richElRef.acc", elRefAcc/nofEvents);
   pt->put(name+".richElRef.eff", elRefEff);
   pt->put(name+".richPi.rec", piRec/nofEvents);
   pt->put(name+".richPi.acc", piAcc/nofEvents);
   pt->put(name+".richPi.eff", piEff);
   pt->put(name+".richPiRef.rec", piRefRec/nofEvents);
   pt->put(name+".richPiRef.acc", piRefAcc/nofEvents);
   pt->put(name+".richPiRef.eff", piRefEff);
}

void CbmLitReconstructionQaImpl::EventEfficiencyStatisticsElIdToPTree(
      boost::property_tree::ptree* pt,
      const std::string& name,
      const std::vector<std::vector<TH1F*> >& hist)
{
   Double_t elEff = 0.;
   Double_t elRec = hist[kElectronId][kRec]->GetEntries();
   Double_t elAcc = hist[kElectronId][kAcc]->GetEntries();
   if (elAcc != 0.) { elEff = 100.*elRec / elAcc; }

   Double_t piSupp = 0.;
   Double_t piRec = hist[kPionSupp][kRec]->GetEntries();
   Double_t piAcc = hist[kPionSupp][kAcc]->GetEntries();
   if (piRec != 0.) { piSupp = piAcc / piRec; }

   Double_t nofEvents = (Double_t)fhEventNo->GetEntries();
   pt->put(name+".el.rec", elRec/nofEvents);
   pt->put(name+".el.acc", elAcc/nofEvents);
   pt->put(name+".el.eff", elEff);
   pt->put(name+".pi.rec", piRec/nofEvents);
   pt->put(name+".pi.acc", piAcc/nofEvents);
   pt->put(name+".pi.supp", piSupp);
}

void CbmLitReconstructionQaImpl::EventDetAccElStatisticsToPTree(
      boost::property_tree::ptree* pt,
      const std::string& name,
      TH1F* hmc,
      TH1F* hacc,
      TH1F* hrec
   )
{
   Double_t effAcc = 0., effRec = 0.;

   Double_t mc = hmc->GetEntries();
   Double_t acc = hacc->GetEntries();
   if (mc != 0.) { effAcc = 100.*acc / mc; }

   Double_t rec = 0.;
   if (hrec != NULL){
      rec = hrec->GetEntries();
      if (mc != 0.) { effRec = 100.*rec / mc; }
   }

   Double_t nofEvents = (Double_t)fhEventNo->GetEntries();
   pt->put(name+".detAccAcc.acc", acc/nofEvents);
   pt->put(name+".detAccAcc.mc", mc/nofEvents);
   pt->put(name+".detAccAcc.eff", effAcc);
   pt->put(name+".detAccRec.rec", rec/nofEvents);
   pt->put(name+".detAccRec.mc", mc/nofEvents);
   pt->put(name+".detAccRec.eff", effRec);
}

void CbmLitReconstructionQaImpl::PolarAngleEfficiencyToPTree(
      boost::property_tree::ptree* pt,
      const std::string& name,
      const std::vector<std::vector<TH1F*> >& hist)
{
   Double_t step = (fMaxAngle - fMinAngle) / fNofBinsAngle;
   Double_t eff[fNofCategories][fNofBinsAngle];
   Double_t rec[fNofCategories][fNofBinsAngle];
   Double_t acc[fNofCategories][fNofBinsAngle];
   for(Int_t i = 0; i < fNofBinsAngle; i++) {
      rec[kAll][i] = hist[kAll][kRec]->GetBinContent(i+1);
      acc[kAll][i] = hist[kAll][kAcc]->GetBinContent(i+1);
      if (acc[kAll][i] != 0.) { eff[kAll][i] = 100.*rec[kAll][i] / acc[kAll][i]; }
      rec[kRef][i] = hist[kRef][kRec]->GetBinContent(i+1);
      acc[kRef][i] = hist[kRef][kAcc]->GetBinContent(i+1);
      if (acc[kRef][i] != 0.) { eff[kRef][i] = 100.*rec[kRef][i] / acc[kRef][i]; }
      rec[kPrim][i] = hist[kPrim][kRec]->GetBinContent(i+1);
      acc[kPrim][i] = hist[kPrim][kAcc]->GetBinContent(i+1);
      if (acc[kPrim][i] != 0.) { eff[kPrim][i] = 100.*rec[kPrim][i] / acc[kPrim][i]; }
      rec[kSec][i] = hist[kSec][kRec]->GetBinContent(i+1);
      acc[kSec][i] = hist[kSec][kAcc]->GetBinContent(i+1);
      if (acc[kSec][i] != 0.) { eff[kSec][i] = 100.*rec[kSec][i] / acc[kSec][i]; }
      rec[kEl][i] = hist[kEl][kRec]->GetBinContent(i+1);
      acc[kEl][i] = hist[kEl][kAcc]->GetBinContent(i+1);
      if (acc[kEl][i] != 0.) { eff[kEl][i] = 100.*rec[kEl][i] / acc[kEl][i]; }
      rec[kMu][i] = hist[kMu][kRec]->GetBinContent(i+1);
      acc[kMu][i] = hist[kMu][kAcc]->GetBinContent(i+1);
      if (acc[kMu][i] != 0.) { eff[kMu][i] = 100.*rec[kMu][i] / acc[kMu][i]; }
   }

   Double_t nofEvents = (Double_t)fhEventNo->GetEntries();
   Int_t w = 17;
   std::stringstream ss;
   for (Int_t i = 0; i < fNofBinsAngle; i++) {
      std::stringstream ss1;
      ss1 << i*step;
      std::string angle0= ss1.str();
      std::stringstream ss2;
      ss2 << i*step + step;
      std::string angle1= ss2.str();

      pt->put(name+".all.rec."+angle0 +"_" + angle1, rec[kAll][i]/nofEvents);
      pt->put(name+".all.acc."+angle0 +"_" + angle1, acc[kAll][i]/nofEvents);
      pt->put(name+".all.eff."+angle0 +"_" + angle1, eff[kAll][i]);

      pt->put(name+".ref.rec."+angle0 +"_" + angle1, rec[kRef][i]/nofEvents);
      pt->put(name+".ref.acc."+angle0 +"_" + angle1, acc[kRef][i]/nofEvents);
      pt->put(name+".ref.eff."+angle0 +"_" + angle1, eff[kRef][i]);

      pt->put(name+".prim.rec."+angle0 +"_" + angle1, rec[kPrim][i]/nofEvents);
      pt->put(name+".prim.acc."+angle0 +"_" + angle1, acc[kPrim][i]/nofEvents);
      pt->put(name+".prim.eff."+angle0 +"_" + angle1, eff[kPrim][i]);

      pt->put(name+".sec.rec."+angle0 +"_" + angle1, rec[kSec][i]/nofEvents);
      pt->put(name+".sec.acc."+angle0 +"_" + angle1, acc[kSec][i]/nofEvents);
      pt->put(name+".sec.eff."+angle0 +"_" + angle1, eff[kSec][i]);

      pt->put(name+".el.rec."+angle0 +"_" + angle1, rec[kEl][i]/nofEvents);
      pt->put(name+".el.acc."+angle0 +"_" + angle1, acc[kEl][i]/nofEvents);
      pt->put(name+".el.eff."+angle0 +"_" + angle1, eff[kEl][i]);

      pt->put(name+".mu.rec."+angle0 +"_" + angle1, rec[kMu][i]/nofEvents);
      pt->put(name+".mu.acc."+angle0 +"_" + angle1, acc[kMu][i]/nofEvents);
      pt->put(name+".mu.eff."+angle0 +"_" + angle1, eff[kMu][i]);
   }
}


void CbmLitReconstructionQaImpl::Draw()
{
   SetStyles();
   DrawEfficiencyHistos();
   DrawHitsHistos();
   DrawHitsStationHistos();
   DrawStsTracksQaHistos();
   DrawMcEfficiencyGraph();
   DrawMCMomVsAngle();

   boost::property_tree::ptree qa = PrintPTree();

   boost::property_tree::ptree ideal, check;
   std::string qaIdealFile = std::string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/rec_qa_ideal.json");
   read_json(qaIdealFile.c_str(), ideal);

   CbmLitReconstructionQaChecker qaChecker;
   qaChecker.DoCheck(qa, ideal, check);
   if (fOutputDir != "") write_json(std::string(fOutputDir + "rec_qa_check.json").c_str(), check);

   if (fOutputDir != ""){
      std::ofstream fout(std::string(fOutputDir + "rec_qa.txt").c_str());
      CbmLitQaPrintGenerator::PrintFinalStatistics(fout, &qa);
   }

   if (fOutputDir != ""){
      CbmLitQaHTMLGenerator html;
      html.SetIsElectronSetup(fIsElectronSetup);
      html.SetDetectorPresence(kMVD, fIsMvd);
      html.SetDetectorPresence(kSTS, fIsSts);
      html.SetDetectorPresence(kRICH, fIsRich);
      html.SetDetectorPresence(kTRD, fIsTrd);
      html.SetDetectorPresence(kMUCH, fIsMuch);
      html.SetDetectorPresence(kTOF, fIsTof);

      std::ofstream foutHtml(std::string(fOutputDir + "rec_qa.html").c_str());
      html.Create(foutHtml, &qa, &ideal, &check);
   }
}

void CbmLitReconstructionQaImpl::DrawEfficiencyHistos()
{
   std::string sname("STS");
   std::string rname;
   if (fIsMuch && !fIsTrd) { rname = "MUCH"; }
   else if (fIsTrd && !fIsMuch) { rname = "TRD"; }
   else if (fIsTrd && fIsMuch) { rname = "MUCH+TRD"; }
   std::string hgname(sname + "+" + rname);
   std::string gname = hgname + "+TOF";

   std::string signal = fIsMuch ? "muons" : "electrons";
   Int_t cat = fIsMuch ? kMu : kEl;

   // Draw global tracking efficiency STS+TRD(MUCH)+TOF for all tracks
   DrawEfficiency("rec_qa_global_efficiency_all", &fhStsMomNormGlobal[kAll],
		   &fhHalfGlobalMomNormGlobal[kAll], &fhGlobalMom[kAll], NULL, sname, hgname, gname, "", "");

   // Draw global tracking efficiency STS+TRD(MUCH)+TOF for signal tracks
   DrawEfficiency("rec_qa_global_efficiency_signal", &fhStsMomNormGlobal[cat],
		   &fhHalfGlobalMomNormGlobal[cat], &fhGlobalMom[cat], NULL, sname, hgname, gname, "", "");

   // Draw half global tracking efficiency STS+TRD(MUCH) for all tracks
   DrawEfficiency("rec_qa_half_global_efficiency_all", &fhStsMomNormHalfGlobal[kAll],
		   &fhHalfGlobalMom[cat], NULL, NULL, sname, hgname, "", "", "");

   // Draw half global tracking efficiency STS+TRD(MUCH) for signal tracks
   DrawEfficiency("rec_qa_half_global_efficiency_signal", &fhStsMomNormHalfGlobal[cat],
		   &fhHalfGlobalMom[cat], NULL, NULL, sname, hgname, "", "", "");

   // Draw efficiency for STS
   DrawEfficiency("rec_qa_sts_efficiency", &fhStsMom[kAll],
  		   &fhStsMom[cat], NULL, NULL, "STS: all", "STS: " + signal, "", "", "");

   if (fIsTrd || fIsMuch) {
	   // Draw efficiency for TRD(MUCH)
	   DrawEfficiency("rec_qa_rec_efficiency", &fhRecMom[kAll],
	      &fhRecMom[cat], NULL, NULL, rname + ": all", rname + ": " + signal, "", "", "");
   }

   if (fIsTof) {
	   // Draw efficiency for TOF
	   DrawEfficiency("rec_qa_tof_efficiency", &fhTofMom[kAll],
			&fhTofMom[cat], NULL, NULL, "TOF: all", "TOF: " + signal, "", "", "");
   }

   if (fIsRich) {
	   // Draw efficiency for RICH for electron set
	   DrawEfficiency("rec_qa_rich_efficiency_electrons", &fhRichMom[kRichEl],
	      &fhRichMom[kRichElRef], NULL, NULL, "RICH: electrons", "RICH: electrons ref", "", "", "");

	   // Draw efficiency for STS+RICH for electron set
	   DrawEfficiency("rec_qa_sts_rich_efficiency_electrons", &fhStsMomNormStsRich[kRichEl],
			&fhStsRichMom[kRichEl], NULL, NULL, "STS", "STS+RICH", "", "", "");

	   // Draw efficiency for STS+RICH+TRD for electron set
	   DrawEfficiency("rec_qa_sts_rich_trd_efficiency_electrons", &fhStsMomNormStsRichTrd[kRichEl],
			&fhStsRichMomNormStsRichTrd[kRichEl], &fhStsRichTrdMom[kRichEl], NULL,
			"STS", "STS+RICH", "STS+RICH+TRD", "", "");

	   // Draw efficiency for STS+RICH+TRD+TOF for electron set
	   DrawEfficiency("rec_qa_sts_rich_trd_tof_efficiency_electrons", &fhStsMomNormStsRichTrdTof[kRichEl],
			&fhStsRichMomNormStsRichTrdTof[kRichEl], &fhStsRichTrdMomNormStsRichTrdTof[kRichEl], &fhStsRichTrdTofMom[kRichEl],
			"STS", "STS+RICH", "STS+RICH+TRD", "STS+RICH+TRD+TOF", "");

	   TCanvas* canvas = new TCanvas("rec_qa_sts_rich_trd_tof_momentum_electrons", "rec_qa_sts_rich_trd_tof_momentum_electrons", 600, 500);
	   canvas->SetGrid();
	   DrawHist1D(fhStsMomNormStsRichTrdTof[kRichEl][kRec], fhStsRichMomNormStsRichTrdTof[kRichEl][kRec],
	      fhStsRichTrdMomNormStsRichTrdTof[kRichEl][kRec], fhStsRichTrdTofMom[kRichEl][kRec],
	      "Efficiency", "Momentum [GeV/c]", "Efficiency", "STS", "STS+RICH",
	      "STS+RICH+TRD", "STS+RICH+TRD+TOF", kLitLinearScale, kLitLinearScale, true, 0.3,0.3,0.85,0.6);

      DrawEfficiency("rec_qa_sts_rich_trd_tof_electron_identification", &fhStsRichMomElIdNormStsRichTrdTof[kElectronId],
         &fhStsRichTrdMomElIdNormStsRichTrdTof[kElectronId], &fhStsRichTrdTofMomElId[kElectronId], NULL,
         "RICH", "RICH+TRD", "RICH+TRD+TOF", "", "");

      DrawEfficiency("rec_qa_sts_rich_trd_tof_pion_suppression", &fhStsRichMomElIdNormStsRichTrdTof[kPionSupp],
         &fhStsRichTrdMomElIdNormStsRichTrdTof[kPionSupp], &fhStsRichTrdTofMomElId[kPionSupp], NULL,
         "RICH", "RICH+TRD", "RICH+TRD+TOF", "", "pisupp");
    }

   if (fIsRich) {
      // Draw detector acceptance
      DrawEfficiency("rec_qa_sts_rich_trd_tof_detector_acceptance", &fhStsDetAccEl,
            &fhStsRichDetAccEl, &fhStsRichTrdDetAccEl, &fhStsRichTrdTofDetAccEl,
            "STS", "STS+RICH", "STS+RICH+TRD", "STS+RICH+TRD+TOF", "");

   }
}

void CbmLitReconstructionQaImpl::DrawEfficiency(
		const std::string& canvasName,
		const std::vector<TH1F*>* hist1,
		const std::vector<TH1F*>* hist2,
		const std::vector<TH1F*>* hist3,
		const std::vector<TH1F*>* hist4,
		const std::string& name1,
		const std::string& name2,
		const std::string& name3,
		const std::string& name4,
		const std::string& opt)
{
	TCanvas* canvas = new TCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
	canvas->SetGrid();
	canvas->cd();
	std::string hname1, hname2, hname3, hname4;

	Double_t eff1, eff2, eff3, eff4;
	if (hist1) eff1 = CalcEfficiency((*hist1)[kRec], (*hist1)[kAcc], opt);
	if (hist2) eff2 = CalcEfficiency((*hist2)[kRec], (*hist2)[kAcc], opt);
	if (hist3) eff3 = CalcEfficiency((*hist3)[kRec], (*hist3)[kAcc], opt);
	if (hist4) eff4 = CalcEfficiency((*hist4)[kRec], (*hist4)[kAcc], opt);

	if (hist1) hname1 = name1 + "(" + lit::NumberToString<Double_t>(eff1, 1) + ")";
	if (hist2) hname2 = name2 + "(" + lit::NumberToString<Double_t>(eff2, 1) + ")";
	if (hist3) hname3 = name3 + "(" + lit::NumberToString<Double_t>(eff3, 1) + ")";
	if (hist4) hname4 = name4 + "(" + lit::NumberToString<Double_t>(eff4, 1) + ")";

/*	Double_t nrebin = 10.;
	if (hist1 != NULL) (*hist1)[kEff]->Rebin(nrebin);
	if (hist2 != NULL) (*hist2)[kEff]->Rebin(nrebin);
	if (hist3 != NULL) (*hist3)[kEff]->Rebin(nrebin);
	if (hist4 != NULL) (*hist4)[kEff]->Rebin(nrebin);

	if (hist1 != NULL) (*hist1)[kEff]->Scale(1./nrebin);
	if (hist2 != NULL) (*hist2)[kEff]->Scale(1./nrebin);
	if (hist3 != NULL) (*hist3)[kEff]->Scale(1./nrebin);
	if (hist4 != NULL) (*hist4)[kEff]->Scale(1./nrebin);
*/

	std::string yTitle = "Efficiency [%]";
	if (opt != "pisupp"){
	   (*hist1)[kEff]->SetMinimum(0.);
	   (*hist1)[kEff]->SetMaximum(100.);
	} else {
	   yTitle = "Pion suppression";
	}

	if (hist1 && hist2 && hist3 && hist4) {
		DrawHist1D((*hist1)[kEff], (*hist2)[kEff], (*hist3)[kEff], (*hist4)[kEff],
				yTitle, "Momentum [GeV/c]", yTitle, hname1, hname2, hname3, hname4,
				kLitLinearScale, kLitLinearScale, true, 0.3,0.3,0.85,0.6);
		DrawMeanEfficiencyLines((*hist1)[kEff], eff1, eff2, eff3, eff4);
      if (opt == "pisupp") gPad->SetLogy(true);
	} else if (hist1 && hist2 && hist3 && !hist4) {
	   DrawHist1D((*hist1)[kEff], (*hist2)[kEff], (*hist3)[kEff], NULL,
	        yTitle, "Momentum [GeV/c]", yTitle, hname1, hname2, hname3, "",
	        kLitLinearScale, kLitLinearScale, true, 0.3,0.3,0.85,0.6);
	   DrawMeanEfficiencyLines((*hist1)[kEff], eff1, eff2, eff3);
	   if (opt == "pisupp") gPad->SetLogy(true);
	} else if (hist1 && hist2 && !hist3 && !hist4){
	   DrawHist1D((*hist1)[kEff], (*hist2)[kEff], NULL, NULL,
	        yTitle, "Momentum [GeV/c]", yTitle, hname1, hname2, "", "",
	        kLitLinearScale, kLitLinearScale, true, 0.3,0.3,0.85,0.6);
	   DrawMeanEfficiencyLines((*hist1)[kEff], eff1, eff2);
      if (opt == "pisupp") gPad->SetLogy(true);
	} else if (hist1 && !hist2 && !hist3 && !hist4){
	   DrawHist1D((*hist1)[kEff], NULL, NULL, NULL,
           yTitle, "Momentum [GeV/c]", yTitle, hname1, "", "", "",
           kLitLinearScale, kLitLinearScale, true, 0.3,0.3,0.85,0.6);
	   DrawMeanEfficiencyLines((*hist1)[kEff], eff1);
      if (opt == "pisupp") gPad->SetLogy(true);
   }
	if (fOutputDir != "") lit::SaveCanvasAsImage(canvas, fOutputDir);
}

void CbmLitReconstructionQaImpl::DrawMcEfficiencyGraph()
{
   boost::property_tree::ptree pt = PrintPTree();

   TH1F* hAcc = new TH1F("hMcAccEfficiencyGraph", "hMcAccEfficiencyGraph;Detector;Efficiency [%]", 4, 0, 4);
   hAcc->SetMaximum(100.);
   hAcc->SetMinimum(0.);
   hAcc->SetBinContent(1, pt.get("hStsDetAccEl.detAccAcc.eff", 0.0));
   hAcc->GetXaxis()->SetBinLabel(1, "STS");
   hAcc->SetBinContent(2, pt.get("hStsRichDetAccEl.detAccAcc.eff", 0.0));
   hAcc->GetXaxis()->SetBinLabel(2, "RICH");
   hAcc->SetBinContent(3, pt.get("hStsRichTrdDetAccEl.detAccAcc.eff", 0.0));
   hAcc->GetXaxis()->SetBinLabel(3, "TRD");
   hAcc->SetBinContent(4, pt.get("hStsRichTrdTofDetAccEl.detAccAcc.eff", 0.0));
   hAcc->GetXaxis()->SetBinLabel(4, "TOF");

   TH1F* hRec = new TH1F("hMcRecEfficiencyGraph", "hMcRecEfficiencyGraph", 4, 0, 4);
   hRec->SetBinContent(1, pt.get("hStsDetAccEl.detAccRec.eff", 0.0));
   //hRec->SetBinLabel(1, "STS");
   hRec->SetBinContent(2, pt.get("hStsRichDetAccEl.detAccRec.eff", 0.0));
   //hRec->SetBinLabel(1, "RICH");
   hRec->SetBinContent(3, pt.get("hStsRichTrdDetAccEl.detAccRec.eff", 0.0));
   //hRec->SetBinLabel(1, "TRD");
   hRec->SetBinContent(4, pt.get("hStsRichTrdTofDetAccEl.detAccRec.eff", 0.0));
   //hRec->SetBinLabel(1, "TOF");

   TCanvas* canvas = new TCanvas("mc_acc_rec_graph", "mc_acc_rec_graph", 600, 500);
   DrawHist1D(hAcc, hRec, NULL, NULL, "", "Detector", "Efficiency [%]", "ACC/MC", "REC/MC", "", "",
              kLitLinearScale, kLitLinearScale, true, 0.6,0.1,0.9,0.4);
   gPad->SetGridx(true);
   gPad->SetGridy(true);
}

Double_t CbmLitReconstructionQaImpl::CalcEfficiency(
   TH1* histRec,
   TH1* histAcc,
   const std::string& opt)
{
   if (histAcc->GetEntries() == 0 || histRec->GetEntries() == 0) {
      return 0.;
   } else {
      if (opt != "pisupp") return 100.*Double_t(histRec->GetEntries()) / Double_t(histAcc->GetEntries());
      else return Double_t(histAcc->GetEntries()) / Double_t(histRec->GetEntries());
   }
}

void CbmLitReconstructionQaImpl::DrawMeanEfficiencyLines(
   TH1* h,
   Double_t eff1,
   Double_t eff2,
   Double_t eff3,
   Double_t eff4)
{
   Double_t minY = h->GetXaxis()->GetXmin();
   Double_t maxY = h->GetXaxis()->GetXmax();

   TLine* line1 = new TLine(minY, eff1, maxY, eff1);
   line1->SetLineWidth(1);
   line1->SetLineColor(LIT_COLOR1);
   line1->Draw();

   if (eff2 != -1){
      TLine* line2 = new TLine(minY, eff2, maxY, eff2);
      line2->SetLineWidth(1);
      line2->SetLineColor(LIT_COLOR2);
      line2->Draw();

      if(eff3 != -1){
         TLine* line3 = new TLine(minY, eff3, maxY, eff3);
         line3->SetLineWidth(1);
         line3->SetLineColor(LIT_COLOR3);
         line3->Draw();

         if (eff4 != -1){
            TLine* line4 = new TLine(minY, eff4, maxY, eff4);
            line4->SetLineWidth(1);
            line4->SetLineColor(LIT_COLOR4);
            line4->Draw();
         }
      }
   }
}

void CbmLitReconstructionQaImpl::DrawHitsHistos()
{
   if (fIsMvd) { DrawHitsHistos("rec_qa_mvd_hits", fhMvdTrackHits); }
   if (fIsSts) { DrawHitsHistos("rec_qa_sts_hits", fhStsTrackHits); }
   if (fIsTrd) { DrawHitsHistos("rec_qa_trd_hits", fhTrdTrackHits); }
   if (fIsMuch) { DrawHitsHistos("rec_qa_much_hits", fhMuchTrackHits); }
   if (fIsRich) { DrawHitsHistos("rec_qa_rich_ring_hits", fhRichRingHits); }
}

void CbmLitReconstructionQaImpl::DrawHitsHistos(
   const std::string& canvasName,
   std::vector<TH1F*>& histos)
{
   TCanvas* c = new TCanvas(canvasName.c_str(), canvasName.c_str(), 1200, 600);
   c->Divide(2,1);
   c->SetGrid();

   c->cd(1);
   DrawHist1D(histos[0], histos[1], histos[2], NULL,
              "Number of hits", "Number of hits", "Counter",
              "all: " + lit::NumberToString<Double_t>(histos[0]->GetMean(), 1),
              "true: " + lit::NumberToString<Double_t>(histos[1]->GetMean(), 1),
              "fake: " + lit::NumberToString<Double_t>(histos[2]->GetMean(), 1),  "",
              kLitLinearScale, kLitLogScale, true, 0.25,0.99,0.55,0.75);

   c->cd(2);
   DrawHist1D(histos[3], histos[4], NULL, NULL, "Ratio", "Ratio", "Counter",
              "true/all: " + lit::NumberToString<Double_t>(histos[3]->GetMean()),
              "fake/all: " + lit::NumberToString<Double_t>(histos[4]->GetMean()),
              "", "", kLitLinearScale, kLitLogScale, true, 0.25,0.99,0.55,0.75);

   if (fOutputDir != "") lit::SaveCanvasAsImage(c, fOutputDir);
}

void CbmLitReconstructionQaImpl::DrawHitsStationHisto(
		const std::string& name,
		TH1F* hist)
{
	TCanvas* canvas = new TCanvas(name.c_str(), name.c_str(), 1200, 600);
	DrawHist1D(hist, "Station number", "Number of hits",
			LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
			LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, "HIST TEXT0");
	if (fOutputDir != "") lit::SaveCanvasAsImage(canvas, fOutputDir);
}

void CbmLitReconstructionQaImpl::DrawHitsStationHistos()
{
	if (fIsMvd) {DrawHitsStationHisto("rec_qa_mvd_hits_station", fhMvdNofHitsInStation);}
	if (fIsSts) {DrawHitsStationHisto("rec_qa_sts_hits_station", fhStsNofHitsInStation);}
	if (fIsTrd) {DrawHitsStationHisto("rec_qa_trd_hits_station", fhTrdNofHitsInStation);}
	if (fIsMuch) {DrawHitsStationHisto("rec_qa_much_hits_station", fhMuchNofHitsInStation);}
	if (fIsTof) {DrawHitsStationHisto("rec_qa_tof_hits_station", fhTofNofHitsInStation);}
}

void CbmLitReconstructionQaImpl::DrawStsTracksQaHistos()
{
   TCanvas* canvas1 = new TCanvas("rec_qa_sts_tracks_qa", "rec_qa_sts_tracks_qa", 900, 900);
   canvas1->Divide(2,2);
   canvas1->cd(1);
   fhStsChiprim->Scale(1./fhStsChiprim->Integral());
   DrawHist1D(fhStsChiprim, "#chi^{2}_{vertex}", "Yield",
            kBlack, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
            LIT_MARKER_STYLE1, kLitLinearScale, kLitLogScale, "");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   canvas1->cd(2);
   TH1* projY = (TH1*)fhStsMomresVsMom->ProjectionY()->Clone();
   projY->Scale(1./projY->Integral());
   DrawHist1D(projY, "dP [%]", "Yield", kBlack, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
            LIT_MARKER_STYLE1, kLitLinearScale, kLitLogScale, "");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

//   canvas1->cd(3);
//   DrawHist2D(fhStsMomresVsMom, "P [GeV/c]", "dP [%]", "Counter",
//         kLitLinearScale, kLitLinearScale, kLitLinearScale, "COLZ");
//   gPad->SetGridx(true);
//   gPad->SetGridy(true);

   canvas1->cd(3);
   fhStsMomresVsMom->FitSlicesY();
   TH1* momslice = (TH1*) gDirectory->Get("fhStsMomresVsMom_2");
   //momslice->Rebin(2);
   //momslice->Scale(1./2.);
   DrawHist1D(momslice, "P [GeV/c]", "dP/P, #sigma [%]",
            kBlack, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
            LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, "");
   gPad->SetGridx(true);
   gPad->SetGridy(true);
   if (fOutputDir != "") lit::SaveCanvasAsImage(canvas1, fOutputDir);

   canvas1->cd(4);
   Int_t nBins = fhStsMomresVsMom->GetNbinsX();
   TH1D* momResRms = (TH1D*)fhStsMomresVsMom->ProjectionX()->Clone();
   for (Int_t i = 1; i < nBins; i++){
      TH1* projY = (TH1*)fhStsMomresVsMom->ProjectionY("_py", i, i);
      Double_t rms = projY->GetRMS();
      momResRms->SetBinContent(i, rms);
      momResRms->SetBinError(i, momslice->GetBinError(i));
   }
   DrawHist1D(momResRms, "P [GeV/c]", "dP/P, RMS [%]",
            kBlack, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
            LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, "P");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

}

void CbmLitReconstructionQaImpl::DrawMCMomVsAngle()
{
   TCanvas* canvas1 = new TCanvas("rec_qa_mc_mom_vs_angle", "rec_qa_mc_mom_vs_angle", 1200, 600);
   canvas1->Divide(2,1);
   canvas1->cd(1);
   DrawHist2D(fhMCMomVsAngle[kAll], "Momentum [GeV/c]", "Polar angle [grad]", "Tracks per event",
         kLitLinearScale, kLitLinearScale, kLitLinearScale, "COLZ");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   canvas1->cd(2);
   DrawHist2D(fhMCMomVsAngle[kEl], "Momentum [GeV/c]", "Polar angle [grad]", "Tracks per event",
         kLitLinearScale, kLitLinearScale, kLitLinearScale, "COLZ");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   if (fOutputDir != "") lit::SaveCanvasAsImage(canvas1, fOutputDir);
}
void CbmLitReconstructionQaImpl::DrawHistosFromFile(const std::string& fileName)
{
	TFile* file = new TFile(fileName.c_str());
    //DetermineSetup();
	CreateHistos(file);
	CalculateEfficiencyHistos();
	//WriteToFile();
   boost::property_tree::ptree pt = PrintPTree();
   CbmLitQaPrintGenerator::PrintFinalStatistics(std::cout, &pt);
	Draw();
}
