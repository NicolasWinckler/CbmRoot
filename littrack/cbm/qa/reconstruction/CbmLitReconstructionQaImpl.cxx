/**
 * \file CbmLitReconstructionQaImpl.cxx
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 *
 * Implementation of global track reconstruction QA.
 **/
#include "qa/reconstruction/CbmLitReconstructionQaImpl.h"
#include "qa/reconstruction/CbmLitQaPrintGenerator.h"
#include "qa/reconstruction/CbmLitQaHTMLGenerator.h"
#include "qa/reconstruction/CbmLitReconstructionQaChecker.h"
#include "qa/CbmLitHistManager.h"
#include "qa/reconstruction/CbmLitQaHistCreator.h"
#include "qa/reconstruction/CbmLitQaDraw.h"
#include "qa/reconstruction/CbmLitQaPTreeCreator.h"

#include "base/CbmLitEnvironment.h"
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
#include "TH3F.h"
#include "TFile.h"
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
   fOutputDir(""),

   fPrimVertex(NULL),
   fKFFitter(NULL),

   fElectronId(NULL),
   fHM(NULL)
{
}

CbmLitReconstructionQaImpl::~CbmLitReconstructionQaImpl()
{
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
   H1("hEventNo")->Fill(0.5);
   std::cout << "Event: " << H1("hEventNo")->GetEntries() << std::endl;

   FillNofCrossedStationsHistos();
   FillRichRingNofHits();
   ProcessHits();
   ProcessGlobalTracks();
   ProcessMcTracks();
   PionSuppression3D();
   StsTracksQa();
   IncreaseCounters();

   CbmLitQaPTreeCreator* ptc = new CbmLitQaPTreeCreator(fHM);
   ptc->fIsElectronSetup = fIsElectronSetup;
   ptc->fIsMvd = fIsMvd;
   ptc->fIsSts = fIsSts;
   ptc->fIsRich = fIsRich;
   ptc->fIsTrd = fIsTrd;
   ptc->fIsMuch = fIsMuch;
   ptc->fIsTof = fIsTof;
   ptc->fOutputDir = fOutputDir;
   ptc->fMinAngle = fMinAngle;
   ptc->fMaxAngle = fMaxAngle;
   ptc->fNofBinsAngle = fNofBinsAngle;
   boost::property_tree::ptree pt = ptc->PrintPTree();
   CbmLitQaPrintGenerator::PrintEventStatistics(std::cout, &pt);
}

void CbmLitReconstructionQaImpl::Finish()
{
   CalculateEfficiencyHistos();
   Draw();
   WriteToFile();
}

TH1F* CbmLitReconstructionQaImpl::H1(
      const string& name)
{
   return fHM->H1(name);
}

TH2F* CbmLitReconstructionQaImpl::H2(
      const string& name)
{
   return fHM->H2(name);
}

TH3F* CbmLitReconstructionQaImpl::H3(
      const string& name)
{
   return fHM->H3(name);
}

TH1* CbmLitReconstructionQaImpl::H(
      const string& name)
{
   return fHM->H(name);
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
      default: return false;
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
         H1("hMvdNofHitsInStation")->Fill(hit->GetStationNr());
      }
   }
   if (fIsSts) {
      for (Int_t i = 0; i < fStsHits->GetEntriesFast(); i++) {
         CbmHit* hit = static_cast<CbmHit*>(fStsHits->At(i));
         H1("hStsNofHitsInStation")->Fill(hit->GetStationNr());
      }
   }
   if (fIsTrd) {
      for (Int_t i = 0; i < fTrdHits->GetEntriesFast(); i++) {
         CbmBaseHit* hit = static_cast<CbmBaseHit*>(fTrdHits->At(i));
         H1("hTrdNofHitsInStation")->Fill(hit->GetPlaneId());
      }
   }
   if (fIsMuch) {
      for (Int_t i = 0; i < fMuchPixelHits->GetEntriesFast(); i++) {
         CbmBaseHit* hit = static_cast<CbmBaseHit*>(fMuchPixelHits->At(i));
         H1("hMuchNofHitsInStation")->Fill(hit->GetPlaneId());
      }
      for (Int_t i = 0; i < fMuchStrawHits->GetEntriesFast(); i++) {
         CbmBaseHit* hit = static_cast<CbmBaseHit*>(fMuchStrawHits->At(i));
         H1("hMuchNofHitsInStation")->Fill(hit->GetPlaneId());
      }
   }
   if (fIsTof) {
      for (Int_t i = 0; i < fTofHits->GetEntriesFast(); i++) {
         CbmBaseHit* hit = static_cast<CbmBaseHit*>(fTofHits->At(i));
         H1("hTofNofHitsInStation")->Fill(hit->GetPlaneId());
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
   fMcStsRichNoMatchingMap.clear();
   fMcStsRichTrdMap.clear();
   fMcStsRichTrdTofMap.clear();

   ProcessRichRings();

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
            H1("hStsGhostNh")->Fill(nofHits);

            // calculate number of ghost after RICH matching
            if (isRichOk){
               CbmRichRing* ring = static_cast<CbmRichRing*>(fRichRings->At(richId));
               if (NULL != ring){
                  if (ring->GetDistance() < 1.) H1("hStsGhostRichMatchingNh")->Fill(nofHits);
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
               H1("hRecGhostNh")->Fill(nofHits);
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
               H1("hRecGhostNh")->Fill(nofHits);
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
            H1("hRichGhostNh")->Fill(nofHits);

            // calculate number of ghost after STS matching and electron identification
            CbmRichRing* ring = static_cast<CbmRichRing*>(fRichRings->At(richId));
            if (NULL != ring){
               if (ring->GetDistance() < 1.) H1("hRichGhostStsMatchingNh")->Fill(nofHits);

               Double_t momentumMc = 0.;
               if (stsTrackMatch!=NULL){
                  CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(stsTrackMatch->GetMCTrackId());
                  if (mcTrack != NULL) momentumMc = mcTrack->GetP();
                }
                if (ring->GetDistance() < 1. && fElectronId->IsRichElectron(globalTrack, momentumMc))
                   H1("hRichGhostElIdNh")->Fill(nofHits);
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
      Bool_t richNoMatching = isRichOk;

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
    		 // fill in ProcessRichRings() procedure, because
    		 // loop over all found rings needed
			//  fMcRichMap.insert(pair<Int_t, Int_t>(richMCId, iTrack));
		  }
    	  // select STS+RICH tracks
    	  if (sts && rich) {
    		  fMcStsRichMap.insert(pair<Int_t, Int_t>(stsMCId, iTrack));
    	  }
        // select STS+RICH tracks no matching
         if (sts && richNoMatching) {
            fMcStsRichNoMatchingMap.insert(pair<Int_t, Int_t>(stsMCId, iTrack));
         }
    	  // select STS+RICH+TRD tracks
    	  if (sts && rich && trd) {
			  fMcStsRichTrdMap.insert(pair<Int_t, Int_t>(stsMCId, iTrack));
		  }
    	  if (fIsTrd) { // select STS+RICH+TRD+TOF tracks
			  if (sts && rich && trd && tof) {
				  fMcStsRichTrdTofMap.insert(pair<Int_t, Int_t>(stsMCId, iTrack));
			  }
    	  } else { // select STS+RICH+TOF tracks
    		  if (sts && rich && tof) {
				  fMcStsRichTrdTofMap.insert(pair<Int_t, Int_t>(stsMCId, iTrack));
			  }
    	  }
      }
   }
}

void CbmLitReconstructionQaImpl::ProcessRichRings()
{
   if (!fIsRich) return;
   Int_t nofRings = fRichRings->GetEntriesFast();
   for(Int_t iRing = 0; iRing < nofRings; iRing++) {
	  CbmRichRing* ring = static_cast<CbmRichRing*>(fRichRings->At(iRing));
	  if (ring == NULL) { continue; }
	  CbmRichRingMatch* richRingMatch = static_cast<CbmRichRingMatch*>(fRichRingMatches->At(iRing));
	  if (richRingMatch == NULL){ continue;}
	  Bool_t isRichOk = CheckRingQuality(richRingMatch);
	  Int_t richMCId = -1;
	  if (isRichOk) { richMCId = richRingMatch->GetMCTrackID(); }
	  if (isRichOk && -1 != richMCId) {
         fMcRichMap.insert(std::pair<Int_t, Int_t>(richMCId, iRing));
	  }
   }// iRing
}

void CbmLitReconstructionQaImpl::ProcessMvd(
   Int_t stsId)
{
   CbmStsTrack* track = static_cast<CbmStsTrack*>(fStsTracks->At(stsId));
   if (NULL == track) return;
   Int_t nofHits = track->GetNMvdHits();
   H1("hMvdTrackHits_All")->Fill(nofHits);

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
   H1("hMvdTrackHits_True")->Fill(nofTrueHits);
   H1("hMvdTrackHits_Fake")->Fill(nofFakeHits);
   if (nofHits != 0) {
      H1("hMvdTrackHits_TrueOverAll")->Fill(Float_t(nofTrueHits) / Float_t(nofHits));
      H1("hMvdTrackHits_FakeOverAll")->Fill(Float_t(nofFakeHits) / Float_t(nofHits));
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
      H1("hStsTrackHits_All")->Fill(nofHits);
      H1("hStsTrackHits_True")->Fill(nofTrue);
      H1("hStsTrackHits_Fake")->Fill(nofFake + nofWrong);
      H1("hStsTrackHits_TrueOverAll")->Fill(quali);
      H1("hStsTrackHits_FakeOverAll")->Fill(fakequali);
   }
   if(detId == kTRD) {
      H1("hTrdTrackHits_All")->Fill(nofHits);
      H1("hTrdTrackHits_True")->Fill(nofTrue);
      H1("hTrdTrackHits_Fake")->Fill(nofFake + nofWrong);
      H1("hTrdTrackHits_TrueOverAll")->Fill(quali);
      H1("hTrdTrackHits_FakeOverAll")->Fill(fakequali);
   }
   if(detId == kMUCH) {
      H1("hMuchTrackHits_All")->Fill(nofHits);
      H1("hMuchTrackHits_True")->Fill(nofTrue);
      H1("hMuchTrackHits_Fake")->Fill(nofFake + nofWrong);
      H1("hMuchTrackHits_TrueOverAll")->Fill(quali);
      H1("hMuchTrackHits_FakeOverAll")->Fill(fakequali);
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

   H1("hRichRingHits_All")->Fill(nofHits);
   H1("hRichRingHits_True")->Fill(nofTrue);
   H1("hRichRingHits_Fake")->Fill(nofFake + nofWrong);
   H1("hRichRingHits_TrueOverAll")->Fill(quali);
   H1("hRichRingHits_FakeOverAll")->Fill(fakequali);

   if (quali < fQuotaRich) { return false; }
   return true;
}

void CbmLitReconstructionQaImpl::FillMcHistoForDetAcc(
      Double_t p,
      Double_t y,
      Double_t pt)
{
   H3("hStsDetAcc3D_El_Mc")->Fill(p, y, pt);
   H3("hStsRichDetAcc3D_El_Mc")->Fill(p, y, pt);
   H3("hStsTrdDetAcc3D_El_Mc")->Fill(p, y, pt);
   H3("hStsTofDetAcc3D_El_Mc")->Fill(p, y, pt);
   H3("hStsRichTrdDetAcc3D_El_Mc")->Fill(p, y, pt);
   H3("hStsRichTrdTofDetAcc3D_El_Mc")->Fill(p, y, pt);
   H3("hStsTrdTofDetAcc3D_El_Mc")->Fill(p, y, pt);
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
      Double_t mcP = mcTrack->GetP();
      Double_t mcY = mcTrack->GetRapidity();
      Double_t mcPt = mcTrack->GetPt();

      Bool_t isPrimElectron = (mcTrack->GetMotherId() == -1 && std::abs(mcTrack->GetPdgCode()) == 11);

      if (isPrimElectron) FillMcHistoForDetAcc(mcP, mcY, mcPt);

      // Check accepted tracks cutting on minimal number of MC points
      // acceptance: STS tracks only
      if (isStsOk) {
         // p-y-pt dependence histograms
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, "hSts3D");
         // number of points dependence histograms
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, "hStsNp", nofPointsSts);
         // polar angle dependence histograms
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, "hStsAngle", angle);

         if (isPrimElectron) H3("hStsDetAcc3D_El_Acc")->Fill(mcP, mcY, mcPt);

         FillMCMomVsAngle(mcTrack);
      }
      // acceptance: STS+TRD(MUCH)
      if (isStsOk && isRecOk) {
         // p-y-pt dependence histograms
    	 // STS normalized to STS+TRD(MUCH)
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, "hSts3DNormHalfGlobal");
    	 // STS+TRD(MUCH)
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, "hHalfGlobal3D");
         // STS+TRD: Electron identification
         FillGlobalElIdHistos3D(mcTrack, iMCTrack, fMcHalfGlobalMap, "hStsTrd3DElId_ElId", "trd");

         if (isPrimElectron) H3("hStsTrdDetAcc3D_El_Acc")->Fill(mcP, mcY, mcPt);
      }
      // acceptance: STS+TRD(MUCH)+TOF
      if (isStsOk && isRecOk && isTofOk) {
         // p-y-pt dependence histograms
    	 // STS normalized to STS+TRD(MUCH)+TOF
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, "hSts3DNormGlobal");
         // STS+TRD(MUCH) normalized to STS+TRD(MUCH)+TOF
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, "hHalfGlobal3DNormGlobal");
    	 // STS+TRD(MUCH)+TOF
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcGlobalMap, "hGlobal3D");

         // STS+TRD: Electron identification
         FillGlobalElIdHistos3D(mcTrack, iMCTrack, fMcHalfGlobalMap, "hStsTrd3DElIdNormStsTrdTof_ElId", "trd");
         // STS+TRD+TOF: Electron identification
         FillGlobalElIdHistos3D(mcTrack, iMCTrack, fMcGlobalMap, "hStsTrdTof3DElId_ElId", "trd+tof");

         if (isPrimElectron) H3("hStsTrdTofDetAcc3D_El_Acc")->Fill(mcP, mcY, mcPt);
      }

      // acceptance: STS+TOF
      if (isStsOk && isTofOk) {
         if (isPrimElectron) H3("hStsTofDetAcc3D_El_Acc")->Fill(mcP, mcY, mcPt);
      }

      // acceptance: STS as 100% + local TRD(MUCH) track cutting on number of points
      if ( isStsReconstructed && isStsOk && isRecOk ) {
         // p-y-pt dependence histograms
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, "hRec3D");
         // number of points dependence histograms
         Int_t np = fIsElectronSetup ? nofPointsTrd : nofPointsMuch;
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, "hRecNp", np);
         // polar angle dependence histograms
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, "hRecAngle", angle);
      }

      if (fIsTrd || fIsMuch) {
         if (isHalfGlobalReconstructed && isStsOk && isRecOk && isTofOk) {
            // p-y-pt dependence histograms
            FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcGlobalMap, "hTof3D");
            // polar angle dependence histograms
            FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcGlobalMap, "hTofAngle", angle);
         }
      } else { // for STS+TOF setup
         if (isStsOk && isTofOk) {
            // p-y-pt dependence histograms
            FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcGlobalMap, "hTof3D");
            // polar angle dependence histograms
            FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcGlobalMap, "hTofAngle", angle);
         }
      }

      // acceptance: RICH
      if (isRichOk) {
    	  // p-y-pt dependence histograms
    	  FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcRichMap, "hRich3D");
    	  // number of hits dependence histograms
    	  FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcRichMap, "hRichNh", fNofHitsInRingMap[iMCTrack]);
      }
      // acceptance: STS+RICH
      if (isStsOk && isRichOk) {
    	  // STS
          FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsMap, "hSts3DNormStsRich");
    	  // STS+RICH
    	  FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsRichMap, "hStsRich3D");

    	  //STS+RICH no matching
    	  FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsRichNoMatchingMap, "hStsRichNoMatching3D");

        // STS+RICH: Electron identification
        FillGlobalElIdHistos3D(mcTrack, iMCTrack, fMcStsRichMap, "hStsRich3DElId_ElId", "rich");

        if (isPrimElectron) H3("hStsRichDetAcc3D_El_Acc")->Fill(mcP, mcY, mcPt);
      }
      // acceptance: STS+RICH+TRD
      if (isStsOk && isRichOk && isTrdOk) {
    	 // STS
         FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsMap, "hSts3DNormStsRichTrd");
         // STS+RICH
         FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsRichMap, "hStsRich3DNormStsRichTrd");
         // STS+RICH+TRD
         FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsRichTrdMap, "hStsRichTrd3D");

         // STS+RICH: Electron identification
         FillGlobalElIdHistos3D(mcTrack, iMCTrack, fMcStsRichMap, "hStsRich3DElIdNormStsRichTrd_ElId", "rich");
         // STS+RICH+TRD: Electron identification
         FillGlobalElIdHistos3D(mcTrack, iMCTrack, fMcStsRichTrdMap, "hStsRichTrd3DElId_ElId", "rich+trd");

         if (isPrimElectron) H3("hStsRichTrdDetAcc3D_El_Acc")->Fill(mcP, mcY, mcPt);
      }
      // acceptance: STS+RICH+TRD+TOF
      if (isStsOk && isRichOk && isTrdOk && isTofOk) {
    	 // STS normalized to STS+RICH+TRD+TOF
         FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsMap, "hSts3DNormStsRichTrdTof");
       	 // STS+RICH normalized to STS+RICH+TRD+TOF
         FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsRichMap, "hStsRich3DNormStsRichTrdTof");
         // STS+RICH+TRD normalized to STS+RICH+TRD+TOF
         FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsRichTrdMap, "hStsRichTrd3DNormStsRichTrdTof");
         // STS+RICH+TRD+TOF normalized to STS+RICH+TRD+TOF
         FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsRichTrdTofMap, "hStsRichTrdTof3D");

         // STS+RICH: Electron identification
          FillGlobalElIdHistos3D(mcTrack, iMCTrack, fMcStsRichMap, "hStsRich3DElIdNormStsRichTrdTof_ElId", "rich");
          // STS+RICH+TRD: Electron identification
          FillGlobalElIdHistos3D(mcTrack, iMCTrack, fMcStsRichTrdMap, "hStsRichTrd3DElIdNormStsRichTrdTof_ElId", "rich+trd");
          // STS+RICH+TRD+TOF: Electron identification
          FillGlobalElIdHistos3D(mcTrack, iMCTrack, fMcStsRichTrdTofMap, "hStsRichTrdTof3DElId_ElId", "rich+trd+tof");

          if (isPrimElectron) H3("hStsRichTrdTofDetAcc3D_El_Acc")->Fill(mcP, mcY, mcPt);
      }
   } // Loop over MCTracks
}

void CbmLitReconstructionQaImpl::FillGlobalReconstructionHistos(
   const CbmMCTrack* mcTrack,
   Int_t mcId,
   const multimap<Int_t, Int_t>& mcMap,
   const string& hist,
   Double_t par)
{
   // Get MC track properties (vertex, momentum, primary/secondary, pdg, etc...)
   TVector3 vertex;
   mcTrack->GetStartVertex(vertex);
   Bool_t isPrim = mcTrack->GetMotherId() == -1;
   Double_t p = mcTrack->GetP();
   Double_t y = mcTrack->GetRapidity();
   Double_t pt = mcTrack->GetPt();
   Bool_t isRef = isPrim && p > fRefMomentum;
   Bool_t isMuon = std::abs(mcTrack->GetPdgCode()) == 13;
   Bool_t isElectron = std::abs(mcTrack->GetPdgCode()) == 11;

   if (par != -1.){
      // Fill histograms for accepted tracks
      H1(hist+"_All_Acc")->Fill(par);
      if (isPrim) { H1(hist+"_Prim_Acc")->Fill(par); }
      if (isRef) { H1(hist+"_Ref_Acc")->Fill(par); }
      if (!isPrim) { H1(hist+"_Sec_Acc")->Fill(par); }
      if (isPrim && isMuon) { H1(hist+"_Mu_Acc")->Fill(par); }
      if (isPrim && isElectron) { H1(hist+"_El_Acc")->Fill(par); }

      // Fill histograms for reconstructed tracks which are accepted
      if (mcMap.find(mcId) != mcMap.end() ) {
         H1(hist+"_All_Rec")->Fill(par);
         if (isPrim) { H1(hist+"_Prim_Rec")->Fill(par); }
         if (isRef) { H1(hist+"_Ref_Rec")->Fill(par); }
         if (!isPrim) { H1(hist+"_Sec_Rec")->Fill(par); }
         if (isPrim && isMuon) { H1(hist+"_Mu_Rec")->Fill(par); }
         if (isPrim && isElectron) { H1(hist+"_El_Rec")->Fill(par); }
      }
   }else {
      // Fill histograms for accepted tracks
      H3(hist+"_All_Acc")->Fill(p, y, pt);
      if (isPrim) { H3(hist+"_Prim_Acc")->Fill(p, y, pt); }
      if (isRef) { H3(hist+"_Ref_Acc")->Fill(p, y, pt); }
      if (!isPrim) { H3(hist+"_Sec_Acc")->Fill(p, y, pt); }
      if (isPrim && isMuon) { H3(hist+"_Mu_Acc")->Fill(p, y, pt); }
      if (isPrim && isElectron) { H3(hist+"_El_Acc")->Fill(p, y, pt); }

      // Fill histograms for reconstructed tracks which are accepted
      if (mcMap.find(mcId) != mcMap.end() ) {
         H3(hist+"_All_Rec")->Fill(p, y, pt);
         if (isPrim) { H3(hist+"_Prim_Rec")->Fill(p, y, pt); }
         if (isRef) { H3(hist+"_Ref_Rec")->Fill(p, y, pt); }
         if (!isPrim) { H3(hist+"_Sec_Rec")->Fill(p, y, pt); }
         if (isPrim && isMuon) { H3(hist+"_Mu_Rec")->Fill(p, y, pt); }
         if (isPrim && isElectron) { H3(hist+"_El_Rec")->Fill(p, y, pt); }
      }
   }
}

void CbmLitReconstructionQaImpl::FillGlobalElIdHistos3D(
   const CbmMCTrack* mcTrack,
   Int_t mcId,
   const multimap<Int_t, Int_t>& mcMap,
   const string& hist,
   const string& opt)
{
   if (!fIsElectronSetup) return;

   Bool_t isMCPrim = mcTrack->GetMotherId() == -1;
   Bool_t isMCElectron = std::abs(mcTrack->GetPdgCode()) == 11;
   Double_t p = mcTrack->GetP();
   Double_t y = mcTrack->GetRapidity();
   Double_t pt = mcTrack->GetPt();

   if (isMCPrim && isMCElectron) H3(hist+"_Acc")->Fill(p, y, pt);

   Bool_t isRichId = (opt.find("rich") != std::string::npos);
   Bool_t isTrdId = (opt.find("trd") != std::string::npos);
   Bool_t isTofId = (opt.find("tof") != std::string::npos);

   // Fill histograms for identified tracks which are accepted
   multimap<Int_t, Int_t>::const_iterator it = mcMap.find(mcId);
   if (it != mcMap.end() ) {
      // Make electron identification
      Int_t globalId = (*it).second;
      CbmGlobalTrack* track = static_cast<CbmGlobalTrack*>(fGlobalTracks->At(globalId));

      Bool_t isRichElectron = isRichId ? fElectronId->IsRichElectron(track, mcTrack->GetP()) : true;
      Bool_t isTrdElectron = isTrdId ? fElectronId->IsTrdElectron(track, mcTrack->GetP()) : true;
      Bool_t isTofElectron = isTofId ? fElectronId->IsTofElectron(track, mcTrack->GetP()) : true;

      Bool_t isElectron = isRichElectron && isTrdElectron && isTofElectron;

      if (isMCPrim && isMCElectron && isElectron) H3(hist+"_Rec")->Fill(p, y, pt);
   }
}

void CbmLitReconstructionQaImpl::FillGlobalReconstructionHistosRich(
   const CbmMCTrack* mcTrack,
   Int_t mcId,
   const multimap<Int_t, Int_t>& mcMap,
   const string& hist,
   Double_t par)
{
   // Get MC track properties (vertex, momentum, primary/secondary, pdg, etc...)
   TVector3 vertex;
   mcTrack->GetStartVertex(vertex);
   Bool_t isPrim = mcTrack->GetMotherId() == -1;
   Double_t p = mcTrack->GetP();
   Double_t y = mcTrack->GetRapidity();
   Double_t pt = mcTrack->GetPt();
   Int_t nofHitsInRing = fNofHitsInRingMap[mcId];
   Bool_t isRef = isPrim && p > fRefMomentum && nofHitsInRing >= fRefMinNofHitsRich;
   Bool_t isPion = std::abs(mcTrack->GetPdgCode()) == 211;
   Bool_t isElectron = std::abs(mcTrack->GetPdgCode()) == 11;

   if (par != -1.){
      // Fill histograms for accepted tracks and rings
      H1(hist+"_All_Acc")->Fill(par);
      if (isRef) { H1(hist+"_AllRef_Acc")->Fill(par); }
      if (isElectron && isPrim) { H1(hist+"_El_Acc")->Fill(par); }
      if (isElectron && isRef) { H1(hist+"_ElRef_Acc")->Fill(par); }
      if (isPion) { H1(hist+"_Pi_Acc")->Fill(par); }
      if (isPion && isRef) { H1(hist+"_PiRef_Acc")->Fill(par); }

      // Fill histograms for reconstructed rings and tracks which are accepted
      if (mcMap.find(mcId) != mcMap.end() ) {
         H1(hist+"_All_Rec")->Fill(par);
         if (isRef) { H1(hist+"_AllRef_Rec")->Fill(par); }
         if (isElectron && isPrim) { H1(hist+"_El_Rec")->Fill(par); }
         if (isElectron && isRef) { H1(hist+"_ElRef_Rec")->Fill(par); }
         if (isPion) { H1(hist+"_Pi_Rec")->Fill(par); }
         if (isPion && isRef) { H1(hist+"_PiRef_Rec")->Fill(par); }
      }
   } else {
      // Fill histograms for accepted tracks and rings
      H3(hist+"_All_Acc")->Fill(p, y, pt);
      if (isRef) { H3(hist+"_AllRef_Acc")->Fill(p, y, pt); }
      if (isElectron && isPrim) { H3(hist+"_El_Acc")->Fill(p, y, pt); }
      if (isElectron && isRef) { H3(hist+"_ElRef_Acc")->Fill(p, y, pt); }
      if (isPion) { H3(hist+"_Pi_Acc")->Fill(p, y, pt); }
      if (isPion && isRef) { H3(hist+"_PiRef_Acc")->Fill(p, y, pt); }

      // Fill histograms for reconstructed rings and tracks which are accepted
      if (mcMap.find(mcId) != mcMap.end() ) {
         H3(hist+"_All_Rec")->Fill(p, y, pt);
         if (isRef) { H3(hist+"_AllRef_Rec")->Fill(p, y, pt); }
         if (isElectron && isPrim) { H3(hist+"_El_Rec")->Fill(p, y, pt); }
         if (isElectron && isRef) { H3(hist+"_ElRef_Rec")->Fill(p, y, pt); }
         if (isPion) { H3(hist+"_Pi_Rec")->Fill(p, y, pt); }
         if (isPion && isRef) { H3(hist+"_PiRef_Rec")->Fill(p, y, pt); }
      }
   }
}

void CbmLitReconstructionQaImpl::PionSuppression3D()
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
      Double_t p = mcTrack->GetP();
      Double_t y = mcTrack->GetRapidity();
      Double_t pt = mcTrack->GetPt();

      Bool_t isPion = (pdg == 211);

      if (!isPion) continue;

      if (isTrdOk) H3("hStsTrdMomElId_PiSupp_Acc")->Fill(p, y, pt);
      if (isTrdOk && isTofOk) H3("hStsTrdMomElIdNormStsTrdTof_PiSupp_Acc")->Fill(p, y, pt);
      if (isTrdOk && isTofOk) H3("hStsTrdTofMomElId_PiSupp_Acc")->Fill(p, y, pt);
      if (isRichOk) H3("hStsRichMomElId_PiSupp_Acc")->Fill(p, y, pt);
      if (isRichOk && isTrdOk) H3("hStsRichMomElIdNormStsRichTrd_PiSupp_Acc")->Fill(p, y, pt);
      if (isRichOk && isTrdOk) H3("hStsRichTrdMomElId_PiSupp_Acc")->Fill(p, y, pt);
      if (isRichOk && isTrdOk && isTofOk) H3("hStsRichMomElIdNormStsRichTrdTof_PiSupp_Acc")->Fill(p, y, pt);
      if (isRichOk && isTrdOk && isTofOk) H3("hStsRichTrdMomElIdNormStsRichTrdTof_PiSupp_Acc")->Fill(p, y, pt);
      if (isRichOk && isTrdOk && isTofOk) H3("hStsRichTrdTofMomElId_PiSupp_Acc")->Fill(p, y, pt);

//      H3("hStsTrdMomElId_PiSupp_Acc")->Fill(p, y, pt);
//      H3("hStsTrdMomElIdNormStsTrdTof_PiSupp_Acc")->Fill(p, y, pt);
//      H3("hStsTrdTofMomElId_PiSupp_Acc")->Fill(p, y, pt);
//      H3("hStsRichMomElId_PiSupp_Acc")->Fill(p, y, pt);
//      H3("hStsRichMomElIdNormStsRichTrd_PiSupp_Acc")->Fill(p, y, pt);
//      H3("hStsRichTrdMomElId_PiSupp_Acc")->Fill(p, y, pt);
//      H3("hStsRichMomElIdNormStsRichTrdTof_PiSupp_Acc")->Fill(p, y, pt);
//      H3("hStsRichTrdMomElIdNormStsRichTrdTof_PiSupp_Acc")->Fill(p, y, pt);
//      H3("hStsRichTrdTofMomElId_PiSupp_Acc")->Fill(p, y, pt);

      Bool_t isRichElectron = fElectronId->IsRichElectron(gTrack, mcTrack->GetP());
      Bool_t isTrdElectron = fElectronId->IsTrdElectron(gTrack, mcTrack->GetP());
      Bool_t isTofElectron = fElectronId->IsTofElectron(gTrack, mcTrack->GetP());

      if (isTrdElectron) H3("hStsTrdMomElId_PiSupp_Rec")->Fill(p, y, pt);
      if (isTrdElectron) H3("hStsTrdMomElIdNormStsTrdTof_PiSupp_Rec")->Fill(p, y, pt);
      if (isTrdElectron && isTofElectron) H3("hStsTrdTofMomElId_PiSupp_Rec")->Fill(p, y, pt);
      if (isRichElectron) H3("hStsRichMomElId_PiSupp_Rec")->Fill(p, y, pt);
      if (isRichElectron) H3("hStsRichMomElIdNormStsRichTrd_PiSupp_Rec")->Fill(p, y, pt);
      if (isRichElectron && isTrdElectron) H3("hStsRichTrdMomElId_PiSupp_Rec")->Fill(p, y, pt);
      if (isRichElectron) H3("hStsRichMomElIdNormStsRichTrdTof_PiSupp_Rec")->Fill(p, y, pt);
      if (isRichElectron && isTrdElectron) H3("hStsRichTrdMomElIdNormStsRichTrdTof_PiSupp_Rec")->Fill(p, y, pt);
      if (isRichElectron && isTrdElectron && isTofElectron) H3("hStsRichTrdTofMomElId_PiSupp_Rec")->Fill(p, y, pt);
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
      H1("hStsChiprim")->Fill(chiPrimary);

      FairTrackParam vtxTrack;
      fKFFitter->FitToVertex(stsTrack, fPrimVertex, &vtxTrack);
      TVector3 momRec;
      vtxTrack.Momentum(momRec);

      Double_t dpp = 100. * (momMC.Mag() - momRec.Mag()) / momMC.Mag();
      H2("hStsMomresVsMom")->Fill(momMC.Mag(), dpp);
   }

   //Track length in TOF detector
   Int_t nofGlobalTracks = fGlobalTracks->GetEntriesFast();
   for (Int_t iG = 0; iG < nofGlobalTracks; iG++){
      CbmGlobalTrack* gTrack  = (CbmGlobalTrack*) fGlobalTracks->At(iG);
      if(!gTrack) continue;

      Int_t tofIndex = gTrack->GetTofHitIndex();
      if (tofIndex < 0) continue;
      CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofIndex);
      if (NULL == tofHit) continue;

      Double_t trackLength = gTrack->GetLength();
      Int_t stsInd = gTrack->GetStsTrackIndex();
      if (stsInd < 0) continue;
      CbmTrackMatch* stsMatch  = (CbmTrackMatch*) fStsMatches->At(stsInd);
      if (stsMatch == NULL) continue;
      Int_t stsMCTrackId = stsMatch->GetMCTrackId();
      if (stsMCTrackId < 0) continue;
      CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMCTracks->At(stsMCTrackId);
      if (mcTrack1 == NULL) continue;

      Int_t nofTofPoints = fTofPoints->GetEntriesFast();
      for (Int_t iTof = 0; iTof < nofTofPoints; iTof++){
         CbmTofPoint* tofPoint  = (CbmTofPoint*) fTofPoints->At(iTof);
         if(NULL == tofPoint) continue;
         if (tofPoint->GetTrackID() == stsMCTrackId){

            Double_t dll = 100. * (tofPoint->GetLength() - trackLength) / tofPoint->GetLength();
            H1("hTrackLength")->Fill(dll);
            break;
         }
      }
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

   H2("hMCMomVsAngle_All")->Fill(p, angle);
   if (isPrim) { H2("hMCMomVsAngle_Prim")->Fill(p, angle); }
   if (isRef) { H2("hMCMomVsAngle_Ref")->Fill(p, angle); }
   if (!isPrim) { H2("hMCMomVsAngle_Sec")->Fill(p, angle); }
   if (isPrim && isMuon) { H2("hMCMomVsAngle_Mu")->Fill(p, angle); }
   if (isPrim && isElectron) { H2("hMCMomVsAngle_El")->Fill(p, angle); }
}

void CbmLitReconstructionQaImpl::CreateHistos(
		TFile* file)
{
   CbmLitQaHistCreator* hc = new CbmLitQaHistCreator();
   hc->fMinMom = fMinMom;
   hc->fMaxMom = fMaxMom;
   hc->fNofBinsMom = fNofBinsMom;
   hc->fMinPt = fMinPt;
   hc->fMaxPt = fMaxPt;
   hc->fNofBinsPt = fNofBinsPt;
   hc->fMinY = fMinY;
   hc->fMaxY = fMaxY;
   hc->fNofBinsY = fNofBinsY;
   hc->fMinAngle = fMinAngle;
   hc->fMaxAngle = fMaxAngle;
   hc->fNofBinsAngle = fNofBinsAngle;

   if (file != NULL){
      fHM = hc->ReadFromFile(file);
   } else {
      fHM = hc->Create();
   }
}

void CbmLitReconstructionQaImpl::CreateProjections3D(
   const string& hist,
   LitQaNameOption opt)
{
   const vector<string>& type = CbmLitQaHistNames::GetTypes(opt);
   const vector<string>& cat = CbmLitQaHistNames::GetCategories(opt);

   for (Int_t i = 0; i < cat.size(); i++) {
      for (Int_t j = 0; j < type.size(); j++){
         TH1D* px = (TH1D*)(H3(hist+"_"+cat[i]+"_"+type[j])->ProjectionX());
         TH1D* py = (TH1D*)(H3(hist+"_"+cat[i]+"_"+type[j])->ProjectionY());
         TH1D* pz = (TH1D*)(H3(hist+"_"+cat[i]+"_"+type[j])->ProjectionZ());
         px->SetName(string(hist+"px_"+cat[i]+"_"+type[j]).c_str());
         py->SetName(string(hist+"py_"+cat[i]+"_"+type[j]).c_str());
         pz->SetName(string(hist+"pz_"+cat[i]+"_"+type[j]).c_str());
         fHM->Add(px->GetName(), px);
         fHM->Add(py->GetName(), py);
         fHM->Add(pz->GetName(), pz);
      }
   }
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

void CbmLitReconstructionQaImpl::DivideHistos(
   const string& hist,
   LitQaNameOption opt,
   Bool_t addProjections)
{
   const vector<string>& type = CbmLitQaHistNames::GetTypes(opt);
   const vector<string>& cat = CbmLitQaHistNames::GetCategories(opt);

   if (addProjections) CreateProjections3D(hist, opt);

   for (Int_t i = 0; i < cat.size(); i++) {
      Double_t c = 100.;
      Int_t acc = 0;
      Int_t rec = 1;

      if (opt == kElid && i == 1){
         acc = 1;
         rec = 0;
         c = 1.;
      }
      DivideHistos(H(hist+"_"+cat[i]+"_"+type[rec]), H(hist+"_"+cat[i]+"_"+type[acc]),
            H(hist+"_"+cat[i]+"_"+type[2]), c);

      if (addProjections){
         DivideHistos(H(hist+"px_"+cat[i]+"_"+type[rec]), H(hist+"px_"+cat[i]+"_"+type[acc]),
               H(hist+"px_"+cat[i]+"_"+type[2]), c);
         DivideHistos(H(hist+"py_"+cat[i]+"_"+type[rec]), H(hist+"py_"+cat[i]+"_"+type[acc]),
               H(hist+"py_"+cat[i]+"_"+type[2]), c);
         DivideHistos(H(hist+"pz_"+cat[i]+"_"+type[rec]), H(hist+"pz_"+cat[i]+"_"+type[acc]),
               H(hist+"pz_"+cat[i]+"_"+type[2]), c);
      }
   }
}

void CbmLitReconstructionQaImpl::CalculateEfficiencyHistos()
{
   Double_t nofEvents = (Double_t)H1("hEventNo")->GetEntries();

   // Divide histograms for efficiency calculation
   // tracking performance
   DivideHistos("hSts3D", kTracking, true);
   DivideHistos("hSts3DNormHalfGlobal", kTracking, true);
   DivideHistos("hSts3DNormGlobal", kTracking, true);
   DivideHistos("hStsNp", kTracking);
   DivideHistos("hStsAngle", kTracking);
   DivideHistos("hHalfGlobal3D", kTracking, true);
   DivideHistos("hHalfGlobal3DNormGlobal", kTracking, true);
   DivideHistos("hGlobal3D", kTracking, true);
   DivideHistos("hRec3D", kTracking, true);
   DivideHistos("hRecNp", kTracking);
   DivideHistos("hRecAngle", kTracking);
   DivideHistos("hTof3D", kTracking, true);
   DivideHistos("hTofAngle", kTracking);

   // RICH performance
   DivideHistos("hRich3D", kRich, true);
   DivideHistos("hRichNh", kRich);
   DivideHistos("hSts3DNormStsRich", kRich, true);
   DivideHistos("hStsRich3D", kRich, true);
   DivideHistos("hStsRichNoMatching3D", kRich, true);
   DivideHistos("hSts3DNormStsRichTrd", kRich, true);
   DivideHistos("hStsRich3DNormStsRichTrd", kRich, true);
   DivideHistos("hStsRichTrd3D", kRich, true);
   DivideHistos("hSts3DNormStsRichTrdTof", kRich, true);
   DivideHistos("hStsRich3DNormStsRichTrdTof", kRich, true);
   DivideHistos("hStsRichTrd3DNormStsRichTrdTof", kRich, true);
   DivideHistos("hStsRichTrdTof3D", kRich, true);

   //fhMCMomVsAngle[i]->Scale(1./nofEvents);

   // electron identification and pion suppression
   DivideHistos("hStsTrd3DElId", kElid, true);
   DivideHistos("hStsTrd3DElIdNormStsTrdTof", kElid, true);
   DivideHistos("hStsTrdTof3DElId", kElid, true);
   DivideHistos("hStsRich3DElId", kElid, true);
   DivideHistos("hStsRich3DElIdNormStsRichTrd", kElid, true);
   DivideHistos("hStsRichTrd3DElId", kElid, true);
   DivideHistos("hStsRich3DElIdNormStsRichTrdTof", kElid, true);
   DivideHistos("hStsRichTrd3DElIdNormStsRichTrdTof", kElid, true);
   DivideHistos("hStsRichTrdTof3DElId", kElid, true);

   DivideHistos("hStsDetAcc3D", kDetAcc, true);
   DivideHistos("hStsRichDetAcc3D", kDetAcc, true);
   DivideHistos("hStsTrdDetAcc3D", kDetAcc, true);
   DivideHistos("hStsTofDetAcc3D", kDetAcc, true);
   DivideHistos("hStsRichTrdDetAcc3D", kDetAcc, true);
   DivideHistos("hStsRichTrdTofDetAcc3D", kDetAcc, true);
   DivideHistos("hStsTrdTofDetAcc3D", kDetAcc, true);

   H1("hMvdNofHitsInStation")->Scale(1./nofEvents);
   H1("hStsNofHitsInStation")->Scale(1./nofEvents);
   H1("hTrdNofHitsInStation")->Scale(1./nofEvents);
   H1("hMuchNofHitsInStation")->Scale(1./nofEvents);
   H1("hTofNofHitsInStation")->Scale(1./nofEvents);
}

void CbmLitReconstructionQaImpl::WriteToFile()
{
   fHM->WriteToFile();
}

void CbmLitReconstructionQaImpl::IncreaseCounters()
{
   H1("hNofGlobalTracks")->Fill(fGlobalTracks->GetEntriesFast());

   if (fIsMvd) {
      H1("hNofMvdHits")->Fill(fMvdHits->GetEntriesFast());
      H1("hNofMvdPoints")->Fill(fMvdPoints->GetEntriesFast());
   }
   if (fIsSts) {
      H1("hNofStsTracks")->Fill(fStsMatches->GetEntriesFast());
	   H1("hNofStsHits")->Fill(fStsHits->GetEntriesFast());
	   H1("hNofStsPoints")->Fill(fStsPoints->GetEntriesFast());
      if (NULL != fStsDigis) H1("hNofStsDigis")->Fill(fStsDigis->GetEntriesFast());
      if (NULL != fStsClusters) H1("hNofStsClusters")->Fill(fStsClusters->GetEntriesFast());
   }
   if (fIsRich) {
      H1("hNofRichRings")->Fill(fRichRings->GetEntriesFast());
      H1("hNofRichHits")->Fill(fRichHits->GetEntriesFast());
      H1("hNofRichPoints")->Fill(fRichPoints->GetEntriesFast());
      H1("hNofRichProjections")->Fill(fRichProjections->GetEntriesFast());
   }
   if (fIsTrd) {
      H1("hNofTrdTracks")->Fill(fTrdMatches->GetEntriesFast());
	   H1("hNofTrdHits")->Fill(fTrdHits->GetEntriesFast());
	   H1("hNofTrdPoints")->Fill(fTrdPoints->GetEntriesFast());
      if (NULL != fTrdDigis) H1("hNofTrdDigis")->Fill(fTrdDigis->GetEntriesFast());
      if (NULL != fTrdClusters) H1("hNofTrdClusters")->Fill(fTrdClusters->GetEntriesFast());
   }
   if (fIsMuch) {
      H1("hNofMuchTracks")->Fill(fMuchMatches->GetEntriesFast());
	   H1("hNofMuchPixelHits")->Fill(fMuchPixelHits->GetEntriesFast());
	   H1("hNofMuchStrawHits")->Fill(fMuchStrawHits->GetEntriesFast());
	   H1("hNofMuchPoints")->Fill(fMuchPoints->GetEntriesFast());
      if (NULL != fMuchDigis) H1("hNofMuchDigis")->Fill(fMuchDigis->GetEntriesFast());
      if (NULL != fMuchClusters) H1("hNofMuchClusters")->Fill(fMuchClusters->GetEntriesFast());
   }
   if (fIsTof) {
      H1("hNofTofHits")->Fill(fTofHits->GetEntriesFast());
      H1("hNofTofPoints")->Fill(fTofPoints->GetEntriesFast());
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

void CbmLitReconstructionQaImpl::Draw()
{
   CbmLitQaDraw* drawQa = new CbmLitQaDraw(fHM);
   drawQa->fIsElectronSetup = fIsElectronSetup;
   drawQa->fIsMvd = fIsMvd;
   drawQa->fIsSts = fIsSts;
   drawQa->fIsRich = fIsRich;
   drawQa->fIsTrd = fIsTrd;
   drawQa->fIsMuch = fIsMuch;
   drawQa->fIsTof = fIsTof;
   drawQa->fOutputDir = fOutputDir;
   drawQa->SetRebinForDraw(1);
   drawQa->Draw();

   CbmLitQaPTreeCreator* ptc = new CbmLitQaPTreeCreator(fHM);
   ptc->fIsElectronSetup = fIsElectronSetup;
   ptc->fIsMvd = fIsMvd;
   ptc->fIsSts = fIsSts;
   ptc->fIsRich = fIsRich;
   ptc->fIsTrd = fIsTrd;
   ptc->fIsMuch = fIsMuch;
   ptc->fIsTof = fIsTof;
   ptc->fOutputDir = fOutputDir;
   ptc->fMinAngle = fMinAngle;
   ptc->fMaxAngle = fMaxAngle;
   ptc->fNofBinsAngle = fNofBinsAngle;
   boost::property_tree::ptree qa = ptc->PrintPTree();

   CbmLitQaPrintGenerator::PrintFinalStatistics(cout, &qa);

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

void CbmLitReconstructionQaImpl::DrawHistosFromFile(
      const std::string& fileName)
{
	TFile* file = new TFile(fileName.c_str());
	CreateHistos(file);
	CalculateEfficiencyHistos();
	Draw();
}
