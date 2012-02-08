/**
 * \file CbmLitTrackingQaCalculator.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 */
#include "qa/tracking/CbmLitTrackingQaCalculator.h"
#include "qa/tracking/CbmLitTrackingQaReport.h"
#include "qa/base/CbmLitResultChecker.h"
#include "qa/base/CbmLitHistManager.h"
#include "qa/tracking/CbmLitTrackingQaHistCreator.h"
#include "qa/tracking/CbmLitTrackingQaDraw.h"
#include "qa/tracking/CbmLitTrackingQaPTreeCreator.h"
#include "qa/mc/CbmLitMCPoint.h"
#include "qa/mc/CbmLitMCTrack.h"
#include "qa/mc/CbmLitMCTrackCreator.h"

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
#include "fitter/CbmRichRingFitterEllipseTau.h"

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

CbmLitTrackingQaCalculator::CbmLitTrackingQaCalculator(
      CbmLitHistManager* histManager):

   fMinNofPointsSts(4),
   fMinNofPointsTrd(8),
   fMinNofPointsMuch(10),
   fMinNofPointsTof(1),
   fQuota(0.7),
   fQuotaRich(0.6),
   fUseConsecutivePointsInSts(false),
   fMinNofHitsRich(7),

   fMinNofHitsTrd(8),
   fMinNofHitsMuch(10),

   fDet(),

   fRefMomentum(1.),
   fRefMinNofHitsRich(15),

   fMcStsMap(),
   fMcHalfGlobalMap(),
   fMcGlobalMap(),

   fMCTracks(NULL),
   fGlobalTracks(NULL),

   fMvdPoints(NULL),
   fMvdHitMatches(NULL),

   fStsTracks(NULL),
   fStsMatches(NULL),
   fStsPoints(NULL),

   fRichHits(NULL),
   fRichRings(NULL),
   fRichProjections(NULL),
   fRichRingMatches(NULL),
   fRichPoints(NULL),

   fMuchMatches(NULL),
   fMuchPoints(NULL),

   fTrdMatches(NULL),
   fTrdPoints(NULL),

   fTofPoints(NULL),
   fTofHits(NULL),

   fPrimVertex(NULL),
   fKFFitter(NULL),

   fElectronId(NULL),

   fHM(histManager)
{
}

CbmLitTrackingQaCalculator::~CbmLitTrackingQaCalculator()
{
}

void CbmLitTrackingQaCalculator::Init()
{
   fDet.DetermineSetup();
   std::cout << fDet.ToString();

   ReadDataBranches();

   if (fDet.GetElectronSetup()) {
      fElectronId = new CbmLitGlobalElectronId();
      fElectronId->Init();
      fRichEllipseFitter = new CbmRichRingFitterEllipseTau();
      fRichEllipseFitter->Init();
   }
   fMCTrackCreator = CbmLitMCTrackCreator::Instance();
}

void CbmLitTrackingQaCalculator::Exec()
{
   // Increase event counter
   fHM->H1F("hEventNo")->Fill(0.5);
   std::cout << "CbmLitTrackingQaCalculator::Exec: event=" << fHM->H1F("hEventNo")->GetEntries() << std::endl;

   fMCTrackCreator->Create();
   FillNofCrossedStationsHistos();
   FillRichRingNofHits();
   ProcessGlobalTracks();
   ProcessMcTracks();
   PionSuppression3D();
   StsTracksQa();
   IncreaseCounters();
}

void CbmLitTrackingQaCalculator::Finish()
{
   CalculateEfficiencyHistos();
   ShrinkEmptyBins();
}

void CbmLitTrackingQaCalculator::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("Init","CbmRootManager is not instantiated"); }

   fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
   if (NULL == fMCTracks) { Fatal("Init","No MCTrack array!"); }

   fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
   if (NULL == fGlobalTracks) { Fatal("Init","No GlobalTrack array!"); }

   if (fDet.GetDet(kMVD)) {
      fMvdPoints = (TClonesArray*) ioman->GetObject("MvdPoint");
      if (NULL == fMvdPoints) { Fatal("Init",": No MvdPoint array!"); }
      fMvdHitMatches = (TClonesArray*) ioman->GetObject("MvdHitMatch");
      if (NULL == fMvdHitMatches) { Fatal("Init",": No MvdHitMatch array!"); }
   }

   if (fDet.GetDet(kSTS)) {
      fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
      if (NULL == fStsTracks) { Fatal("Init",": No StsTrack array!"); }
      fStsMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
      if (NULL == fStsMatches) { Fatal("Init",": No StsTrackMatch array!"); }
      fStsPoints = (TClonesArray*) ioman->GetObject("StsPoint");
      if (NULL == fStsPoints) { Fatal("Init",": No StsPoint array!"); }
   }

   if (fDet.GetDet(kRICH)) {
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

   if (fDet.GetDet(kMUCH)) {
      fMuchMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
      if (NULL == fMuchMatches) { Fatal("Init","No MuchTrackMatch array!"); }
      fMuchPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
      if (NULL == fMuchPoints) { Fatal("Init","No MuchPoint array!"); }
   }

   if (fDet.GetDet(kTRD)) {
      fTrdMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
      if (NULL == fTrdMatches) { Fatal("Init","No TrdTrackMatch array!"); }
      fTrdPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
      if (NULL == fTrdPoints) { Fatal("Init","No TrdPoint array!"); }
   }

   if (fDet.GetDet(kTOF)) {
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

void CbmLitTrackingQaCalculator::FillRichRingNofHits()
{
	if (!fDet.GetDet(kRICH)) return;
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

void CbmLitTrackingQaCalculator::FillNofCrossedStationsHistos()
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
   if (fDet.GetDet(kMUCH)) {
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
   if (fDet.GetDet(kTRD)) {
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

Int_t CbmLitTrackingQaCalculator::MaxConsecutiveNumbers(
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

void CbmLitTrackingQaCalculator::ProcessGlobalTracks()
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
      Bool_t isStsOk = stsId > -1 && fDet.GetDet(kSTS);
      Bool_t isTrdOk = trdId > -1 && fDet.GetDet(kTRD);
      Bool_t isMuchOk = muchId > -1 && fDet.GetDet(kMUCH);
      Bool_t isTofOk = tofId > -1 && fDet.GetDet(kTOF);
      Bool_t isRichOk = richId > -1 && fDet.GetDet(kRICH);

      // check the quality of track segments
      CbmTrackMatch* stsTrackMatch;
      if (isStsOk) {
         stsTrackMatch = static_cast<CbmTrackMatch*>(fStsMatches->At(stsId));
         isStsOk = CheckTrackQuality(stsTrackMatch, kSTS);
         if (!isStsOk) { // ghost track
            Int_t nofHits = stsTrackMatch->GetNofTrueHits() + stsTrackMatch->GetNofWrongHits() + stsTrackMatch->GetNofFakeHits();
            fHM->H1F("hStsGhostNh")->Fill(nofHits);

            // calculate number of ghost after RICH matching
            if (isRichOk){
               CbmRichRing* ring = static_cast<CbmRichRing*>(fRichRings->At(richId));
               if (NULL != ring){
                  if (ring->GetDistance() < 1.) fHM->H1F("hStsGhostRichMatchingNh")->Fill(nofHits);
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
               fHM->H1F("hRecGhostNh")->Fill(nofHits);
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
               fHM->H1F("hRecGhostNh")->Fill(nofHits);
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
            fHM->H1F("hRichGhostNh")->Fill(nofHits);

            // calculate number of ghost after STS matching and electron identification
            CbmRichRing* ring = static_cast<CbmRichRing*>(fRichRings->At(richId));
            if (NULL != ring){
               if (ring->GetDistance() < 1.) fHM->H1F("hRichGhostStsMatchingNh")->Fill(nofHits);

               Double_t momentumMc = 0.;
               if (stsTrackMatch!=NULL){
                  CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(stsTrackMatch->GetMCTrackId());
                  if (mcTrack != NULL) momentumMc = mcTrack->GetP();
                }
                if (ring->GetDistance() < 1. && fElectronId->IsRichElectron(globalTrack, momentumMc))
                   fHM->H1F("hRichGhostElIdNh")->Fill(nofHits);
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

      if (fDet.GetDet(kMUCH) && fDet.GetDet(kTRD)) { // if MUCH and TRD together
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
         if (fDet.GetDet(kTRD) || fDet.GetDet(kMUCH)) {
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
      if (fDet.GetDet(kRICH)) {
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
    	  if (fDet.GetDet(kTRD)) { // select STS+RICH+TRD+TOF tracks
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

void CbmLitTrackingQaCalculator::ProcessRichRings()
{
   if (!fDet.GetDet(kRICH)) return;
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

void CbmLitTrackingQaCalculator::ProcessMvd(
   Int_t stsId)
{
   CbmStsTrack* track = static_cast<CbmStsTrack*>(fStsTracks->At(stsId));
   if (NULL == track) return;
   Int_t nofHits = track->GetNMvdHits();
   fHM->H1F("hMvdTrackHits_All")->Fill(nofHits);

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
   fHM->H1F("hMvdTrackHits_True")->Fill(nofTrueHits);
   fHM->H1F("hMvdTrackHits_Fake")->Fill(nofFakeHits);
   if (nofHits != 0) {
      fHM->H1F("hMvdTrackHits_TrueOverAll")->Fill(Float_t(nofTrueHits) / Float_t(nofHits));
      fHM->H1F("hMvdTrackHits_FakeOverAll")->Fill(Float_t(nofFakeHits) / Float_t(nofHits));
   }
}

Bool_t CbmLitTrackingQaCalculator::CheckTrackQuality(
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
      fHM->H1F("hStsTrackHits_All")->Fill(nofHits);
      fHM->H1F("hStsTrackHits_True")->Fill(nofTrue);
      fHM->H1F("hStsTrackHits_Fake")->Fill(nofFake + nofWrong);
      fHM->H1F("hStsTrackHits_TrueOverAll")->Fill(quali);
      fHM->H1F("hStsTrackHits_FakeOverAll")->Fill(fakequali);
   }
   if(detId == kTRD) {
      fHM->H1F("hTrdTrackHits_All")->Fill(nofHits);
      fHM->H1F("hTrdTrackHits_True")->Fill(nofTrue);
      fHM->H1F("hTrdTrackHits_Fake")->Fill(nofFake + nofWrong);
      fHM->H1F("hTrdTrackHits_TrueOverAll")->Fill(quali);
      fHM->H1F("hTrdTrackHits_FakeOverAll")->Fill(fakequali);
   }
   if(detId == kMUCH) {
      fHM->H1F("hMuchTrackHits_All")->Fill(nofHits);
      fHM->H1F("hMuchTrackHits_True")->Fill(nofTrue);
      fHM->H1F("hMuchTrackHits_Fake")->Fill(nofFake + nofWrong);
      fHM->H1F("hMuchTrackHits_TrueOverAll")->Fill(quali);
      fHM->H1F("hMuchTrackHits_FakeOverAll")->Fill(fakequali);
   }

   return (quali < fQuota) ? false : true;
}

Bool_t CbmLitTrackingQaCalculator::CheckRingQuality(
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

   fHM->H1F("hRichRingHits_All")->Fill(nofHits);
   fHM->H1F("hRichRingHits_True")->Fill(nofTrue);
   fHM->H1F("hRichRingHits_Fake")->Fill(nofFake + nofWrong);
   fHM->H1F("hRichRingHits_TrueOverAll")->Fill(quali);
   fHM->H1F("hRichRingHits_FakeOverAll")->Fill(fakequali);

   return (quali < fQuotaRich) ? false : true;
}

void CbmLitTrackingQaCalculator::FillMcHistoForDetAcc(
      Double_t p,
      Double_t y,
      Double_t pt)
{
   fHM->H3F("hStsDetAcc3D_El_Mc")->Fill(p, y, pt);
   fHM->H3F("hStsRichDetAcc3D_El_Mc")->Fill(p, y, pt);
   fHM->H3F("hStsTrdDetAcc3D_El_Mc")->Fill(p, y, pt);
   fHM->H3F("hStsTofDetAcc3D_El_Mc")->Fill(p, y, pt);
   fHM->H3F("hStsRichTrdDetAcc3D_El_Mc")->Fill(p, y, pt);
   fHM->H3F("hStsRichTrdTofDetAcc3D_El_Mc")->Fill(p, y, pt);
   fHM->H3F("hStsTrdTofDetAcc3D_El_Mc")->Fill(p, y, pt);
}

void CbmLitTrackingQaCalculator::ProcessMcTracks()
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
      Bool_t isStsOk = nofPointsSts >= fMinNofPointsSts && fDet.GetDet(kSTS) && stsConsecutive;
      Bool_t isTrdOk = nofPointsTrd >= fMinNofPointsTrd && fDet.GetDet(kTRD);
      Bool_t isMuchOk = nofPointsMuch >= fMinNofPointsMuch && fDet.GetDet(kMUCH);
      Bool_t isTofOk = nofPointsTof >= fMinNofPointsTof && fDet.GetDet(kTOF);
      Bool_t isRichOk = nofHitsRich >= fMinNofHitsRich && fDet.GetDet(kRICH);
      Bool_t isRecOk = (fDet.GetDet(kMUCH) && fDet.GetDet(kTRD)) ? (isTrdOk && isMuchOk) : (isTrdOk || isMuchOk); // MUCH+TRD

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

         if (isPrimElectron) fHM->H3F("hStsDetAcc3D_El_Acc")->Fill(mcP, mcY, mcPt);

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

         if (isPrimElectron) fHM->H3F("hStsTrdDetAcc3D_El_Acc")->Fill(mcP, mcY, mcPt);
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

         if (isPrimElectron) fHM->H3F("hStsTrdTofDetAcc3D_El_Acc")->Fill(mcP, mcY, mcPt);
      }

      // acceptance: STS+TOF
      if (isStsOk && isTofOk) {
         if (isPrimElectron) fHM->H3F("hStsTofDetAcc3D_El_Acc")->Fill(mcP, mcY, mcPt);
      }

      // acceptance: STS as 100% + local TRD(MUCH) track cutting on number of points
      if ( isStsReconstructed && isStsOk && isRecOk ) {
         // p-y-pt dependence histograms
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, "hRec3D");
         // number of points dependence histograms
         Int_t np = fDet.GetElectronSetup() ? nofPointsTrd : nofPointsMuch;
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, "hRecNp", np);
         // polar angle dependence histograms
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, "hRecAngle", angle);
      }

      if (fDet.GetDet(kTRD) || fDet.GetDet(kMUCH)) {
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

    	   if (fMCTrackCreator->TrackExists(iMCTrack)){
            const CbmLitMCTrack& track = fMCTrackCreator->GetTrack(iMCTrack);
            Int_t nofPointsRich = track.GetNofPoints(kRICH);
            const vector<CbmLitMCPoint>& richPoints = track.GetPoints(kRICH);
            vector<Double_t> xRich, yRich;
            for (Int_t i = 0 ; i < nofPointsRich; i++){
               xRich.push_back(richPoints[i].GetX());
               yRich.push_back(richPoints[i].GetY());
            }
            CbmRichRing* ring = new CbmRichRing();
            fRichEllipseFitter->DoFit(ring, xRich, yRich);
            // B/A dependence histograms
            Double_t boa = ring->GetBaxis()/ring->GetAaxis();
            FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcRichMap, "hRichBoA", boa);
            // radial position dependence histograms
            Double_t radPos = ring->GetRadialPosition();
            FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcRichMap, "hRichRadPos", radPos);
            if (ring != NULL) delete ring;
    	   }
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

        if (isPrimElectron) fHM->H3F("hStsRichDetAcc3D_El_Acc")->Fill(mcP, mcY, mcPt);
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

         if (isPrimElectron) fHM->H3F("hStsRichTrdDetAcc3D_El_Acc")->Fill(mcP, mcY, mcPt);
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

         if (isPrimElectron) fHM->H3F("hStsRichTrdTofDetAcc3D_El_Acc")->Fill(mcP, mcY, mcPt);
      }
   } // Loop over MCTracks
}

void CbmLitTrackingQaCalculator::FillGlobalReconstructionHistos(
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

   if (par != -1.) {
      // Fill histograms for accepted tracks
      fHM->H1F(hist + "_All_Acc")->Fill(par);
      if (isPrim) { fHM->H1F(hist + "_Prim_Acc")->Fill(par); }
      if (isRef) { fHM->H1F(hist + "_Ref_Acc")->Fill(par); }
      if (!isPrim) { fHM->H1F(hist + "_Sec_Acc")->Fill(par); }
      if (isPrim && isMuon) { fHM->H1F(hist + "_Mu_Acc")->Fill(par); }
      if (isPrim && isElectron) { fHM->H1F(hist + "_El_Acc")->Fill(par); }

      // Fill histograms for reconstructed tracks which are accepted
      if (mcMap.find(mcId) != mcMap.end() ) {
         fHM->H1F(hist + "_All_Rec")->Fill(par);
         if (isPrim) { fHM->H1F(hist + "_Prim_Rec")->Fill(par); }
         if (isRef) { fHM->H1F(hist + "_Ref_Rec")->Fill(par); }
         if (!isPrim) { fHM->H1F(hist + "_Sec_Rec")->Fill(par); }
         if (isPrim && isMuon) { fHM->H1F(hist + "_Mu_Rec")->Fill(par); }
         if (isPrim && isElectron) { fHM->H1F(hist + "_El_Rec")->Fill(par); }
      }
   } else {
      // Fill histograms for accepted tracks
      fHM->H3F(hist+"_All_Acc")->Fill(p, y, pt);
      if (isPrim) { fHM->H3F(hist + "_Prim_Acc")->Fill(p, y, pt); }
      if (isRef) { fHM->H3F(hist + "_Ref_Acc")->Fill(p, y, pt); }
      if (!isPrim) { fHM->H3F(hist + "_Sec_Acc")->Fill(p, y, pt); }
      if (isPrim && isMuon) { fHM->H3F(hist + "_Mu_Acc")->Fill(p, y, pt); }
      if (isPrim && isElectron) { fHM->H3F(hist + "_El_Acc")->Fill(p, y, pt); }

      // Fill histograms for reconstructed tracks which are accepted
      if (mcMap.find(mcId) != mcMap.end() ) {
         fHM->H3F(hist + "_All_Rec")->Fill(p, y, pt);
         if (isPrim) { fHM->H3F(hist + "_Prim_Rec")->Fill(p, y, pt); }
         if (isRef) { fHM->H3F(hist + "_Ref_Rec")->Fill(p, y, pt); }
         if (!isPrim) { fHM->H3F(hist + "_Sec_Rec")->Fill(p, y, pt); }
         if (isPrim && isMuon) { fHM->H3F(hist + "_Mu_Rec")->Fill(p, y, pt); }
         if (isPrim && isElectron) { fHM->H3F(hist + "_El_Rec")->Fill(p, y, pt); }
      }
   }
}

void CbmLitTrackingQaCalculator::FillGlobalElIdHistos3D(
   const CbmMCTrack* mcTrack,
   Int_t mcId,
   const multimap<Int_t, Int_t>& mcMap,
   const string& hist,
   const string& opt)
{
   if (!fDet.GetElectronSetup()) return;

   Bool_t isMCPrim = mcTrack->GetMotherId() == -1;
   Bool_t isMCElectron = std::abs(mcTrack->GetPdgCode()) == 11;
   Double_t p = mcTrack->GetP();
   Double_t y = mcTrack->GetRapidity();
   Double_t pt = mcTrack->GetPt();

   if (isMCPrim && isMCElectron) fHM->H3F(hist+"_Acc")->Fill(p, y, pt);

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

      if (isMCPrim && isMCElectron && isElectron) fHM->H3F(hist + "_Rec")->Fill(p, y, pt);
   }
}

void CbmLitTrackingQaCalculator::FillGlobalReconstructionHistosRich(
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
      fHM->H1F(hist + "_All_Acc")->Fill(par);
      if (isRef) { fHM->H1F(hist + "_AllRef_Acc")->Fill(par); }
      if (isElectron && isPrim) { fHM->H1F(hist + "_El_Acc")->Fill(par); }
      if (isElectron && isRef) { fHM->H1F(hist + "_ElRef_Acc")->Fill(par); }
      if (isPion) { fHM->H1F(hist + "_Pi_Acc")->Fill(par); }
      if (isPion && isRef) { fHM->H1F(hist + "_PiRef_Acc")->Fill(par); }

      // Fill histograms for reconstructed rings and tracks which are accepted
      if (mcMap.find(mcId) != mcMap.end() ) {
         fHM->H1F(hist + "_All_Rec")->Fill(par);
         if (isRef) { fHM->H1F(hist + "_AllRef_Rec")->Fill(par); }
         if (isElectron && isPrim) { fHM->H1F(hist + "_El_Rec")->Fill(par); }
         if (isElectron && isRef) { fHM->H1F(hist + "_ElRef_Rec")->Fill(par); }
         if (isPion) { fHM->H1F(hist + "_Pi_Rec")->Fill(par); }
         if (isPion && isRef) { fHM->H1F(hist + "_PiRef_Rec")->Fill(par); }
      }
   } else {
      // Fill histograms for accepted tracks and rings
      fHM->H3F(hist+"_All_Acc")->Fill(p, y, pt);
      if (isRef) { fHM->H3F(hist+"_AllRef_Acc")->Fill(p, y, pt); }
      if (isElectron && isPrim) { fHM->H3F(hist+"_El_Acc")->Fill(p, y, pt); }
      if (isElectron && isRef) { fHM->H3F(hist+"_ElRef_Acc")->Fill(p, y, pt); }
      if (isPion) { fHM->H3F(hist+"_Pi_Acc")->Fill(p, y, pt); }
      if (isPion && isRef) { fHM->H3F(hist+"_PiRef_Acc")->Fill(p, y, pt); }

      // Fill histograms for reconstructed rings and tracks which are accepted
      if (mcMap.find(mcId) != mcMap.end() ) {
         fHM->H3F(hist+"_All_Rec")->Fill(p, y, pt);
         if (isRef) { fHM->H3F(hist+"_AllRef_Rec")->Fill(p, y, pt); }
         if (isElectron && isPrim) { fHM->H3F(hist+"_El_Rec")->Fill(p, y, pt); }
         if (isElectron && isRef) { fHM->H3F(hist+"_ElRef_Rec")->Fill(p, y, pt); }
         if (isPion) { fHM->H3F(hist+"_Pi_Rec")->Fill(p, y, pt); }
         if (isPion && isRef) { fHM->H3F(hist+"_PiRef_Rec")->Fill(p, y, pt); }
      }
   }
}

void CbmLitTrackingQaCalculator::PionSuppression3D()
{
   if (!fDet.GetElectronSetup()) return;

   Int_t nGlobal = fGlobalTracks->GetEntries();
   for (Int_t iTrack=0; iTrack<nGlobal; iTrack++) {

      CbmGlobalTrack* gTrack = (CbmGlobalTrack*)fGlobalTracks->At(iTrack);

      // get track segments indices
      Int_t stsId = gTrack->GetStsTrackIndex();
      Int_t trdId = gTrack->GetTrdTrackIndex();
      Int_t tofId = gTrack->GetTofHitIndex();
      Int_t richId = gTrack->GetRichRingIndex();

      // check track segments
      Bool_t isStsOk = stsId > -1 && fDet.GetDet(kSTS);
      Bool_t isTrdOk = trdId > -1 && fDet.GetDet(kTRD);
      Bool_t isTofOk = tofId > -1 && fDet.GetDet(kTOF);
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

      if (isTrdOk) fHM->H3F("hStsTrd3DElId_PiSupp_Acc")->Fill(p, y, pt);
      if (isTrdOk && isTofOk) fHM->H3F("hStsTrd3DElIdNormStsTrdTof_PiSupp_Acc")->Fill(p, y, pt);
      if (isTrdOk && isTofOk) fHM->H3F("hStsTrdTof3DElId_PiSupp_Acc")->Fill(p, y, pt);
      if (isRichOk) fHM->H3F("hStsRich3DElId_PiSupp_Acc")->Fill(p, y, pt);
      if (isRichOk && isTrdOk) fHM->H3F("hStsRich3DElIdNormStsRichTrd_PiSupp_Acc")->Fill(p, y, pt);
      if (isRichOk && isTrdOk) fHM->H3F("hStsRichTrd3DElId_PiSupp_Acc")->Fill(p, y, pt);
      if (isRichOk && isTrdOk && isTofOk) fHM->H3F("hStsRich3DElIdNormStsRichTrdTof_PiSupp_Acc")->Fill(p, y, pt);
      if (isRichOk && isTrdOk && isTofOk) fHM->H3F("hStsRichTrd3DElIdNormStsRichTrdTof_PiSupp_Acc")->Fill(p, y, pt);
      if (isRichOk && isTrdOk && isTofOk) fHM->H3F("hStsRichTrdTof3DElId_PiSupp_Acc")->Fill(p, y, pt);

//      H3("hStsTrd3DElId_PiSupp_Acc")->Fill(p, y, pt);
//      H3("hStsTrd3DElIdNormStsTrdTof_PiSupp_Acc")->Fill(p, y, pt);
//      H3("hStsTrdTof3DElId_PiSupp_Acc")->Fill(p, y, pt);
//      H3("hStsRich3DElId_PiSupp_Acc")->Fill(p, y, pt);
//      H3("hStsRich3DElIdNormStsRichTrd_PiSupp_Acc")->Fill(p, y, pt);
//      H3("hStsRichTrd3DElId_PiSupp_Acc")->Fill(p, y, pt);
//      H3("hStsRich3DElIdNormStsRichTrdTof_PiSupp_Acc")->Fill(p, y, pt);
//      H3("hStsRichTrd3DElIdNormStsRichTrdTof_PiSupp_Acc")->Fill(p, y, pt);
//      H3("hStsRichTrdTof3DElId_PiSupp_Acc")->Fill(p, y, pt);

      Bool_t isRichElectron = fElectronId->IsRichElectron(gTrack, mcTrack->GetP());
      Bool_t isTrdElectron = fElectronId->IsTrdElectron(gTrack, mcTrack->GetP());
      Bool_t isTofElectron = fElectronId->IsTofElectron(gTrack, mcTrack->GetP());

      if (isTrdElectron) fHM->H3F("hStsTrd3DElId_PiSupp_Rec")->Fill(p, y, pt);
      if (isTrdElectron) fHM->H3F("hStsTrd3DElIdNormStsTrdTof_PiSupp_Rec")->Fill(p, y, pt);
      if (isTrdElectron && isTofElectron) fHM->H3F("hStsTrdTof3DElId_PiSupp_Rec")->Fill(p, y, pt);
      if (isRichElectron) fHM->H3F("hStsRich3DElId_PiSupp_Rec")->Fill(p, y, pt);
      if (isRichElectron) fHM->H3F("hStsRich3DElIdNormStsRichTrd_PiSupp_Rec")->Fill(p, y, pt);
      if (isRichElectron && isTrdElectron) fHM->H3F("hStsRichTrd3DElId_PiSupp_Rec")->Fill(p, y, pt);
      if (isRichElectron) fHM->H3F("hStsRich3DElIdNormStsRichTrdTof_PiSupp_Rec")->Fill(p, y, pt);
      if (isRichElectron && isTrdElectron) fHM->H3F("hStsRichTrd3DElIdNormStsRichTrdTof_PiSupp_Rec")->Fill(p, y, pt);
      if (isRichElectron && isTrdElectron && isTofElectron) fHM->H3F("hStsRichTrdTof3DElId_PiSupp_Rec")->Fill(p, y, pt);
   }
}

void CbmLitTrackingQaCalculator::StsTracksQa()
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
      fHM->H1F("hStsChiprim")->Fill(chiPrimary);

      FairTrackParam vtxTrack;
      fKFFitter->FitToVertex(stsTrack, fPrimVertex, &vtxTrack);
      TVector3 momRec;
      vtxTrack.Momentum(momRec);

      Double_t dpp = 100. * (momMC.Mag() - momRec.Mag()) / momMC.Mag();
      fHM->H2F("hStsMomresVsMom")->Fill(momMC.Mag(), dpp);
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
            fHM->H1F("hTrackLength")->Fill(dll);
            break;
         }
      }
   }
}

void CbmLitTrackingQaCalculator::FillMCMomVsAngle(
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

   fHM->H2F("hMCMomVsAngle_All")->Fill(p, angle);
   if (isPrim) { fHM->H2F("hMCMomVsAngle_Prim")->Fill(p, angle); }
   if (isRef) { fHM->H2F("hMCMomVsAngle_Ref")->Fill(p, angle); }
   if (!isPrim) { fHM->H2F("hMCMomVsAngle_Sec")->Fill(p, angle); }
   if (isPrim && isMuon) { fHM->H2F("hMCMomVsAngle_Mu")->Fill(p, angle); }
   if (isPrim && isElectron) { fHM->H2F("hMCMomVsAngle_El")->Fill(p, angle); }
}

void CbmLitTrackingQaCalculator::CreateProjections3D(
   const string& hist,
   LitQaNameOption opt)
{
   const vector<string>& type = CbmLitTrackingQaHistNames::GetTypes(opt);
   const vector<string>& cat = CbmLitTrackingQaHistNames::GetCategories(opt);

   string yTitle = "Yield";
   for (Int_t i = 0; i < cat.size(); i++) {
      for (Int_t j = 0; j < type.size(); j++) {
         string histName = hist + "_" + cat[i] + "_" + type[j];
         TH3F* hist3D = fHM->H3F(histName);

         string pxName = histName + "_px";
         string pyName = histName + "_py";
         string pzName = histName + "_pz";

         Int_t nofBinsX = hist3D->GetNbinsX();
         Int_t nofBinsY = hist3D->GetNbinsY();
         Int_t nofBinsZ = hist3D->GetNbinsZ();

         TH1D* px = hist3D->ProjectionX(pxName.c_str());
         TH1D* py = hist3D->ProjectionY(pyName.c_str());
         TH1D* pz = hist3D->ProjectionZ(pzName.c_str());

         // set Y axis titles
         if (type[j] == "Eff") yTitle = "Efficiency [%]";
         if (opt == kElid && type[j] == "Eff" && cat[i] == "PiSupp") yTitle = "Pion suppression";
         px->GetYaxis()->SetTitle(yTitle.c_str());
         py->GetYaxis()->SetTitle(yTitle.c_str());
         pz->GetYaxis()->SetTitle(yTitle.c_str());
         fHM->Add(px->GetName(), px);
         fHM->Add(py->GetName(), py);
         fHM->Add(pz->GetName(), pz);
      }
   }
}

void CbmLitTrackingQaCalculator::DivideHistos(
   TH1* histo1,
   TH1* histo2,
   TH1* histo3,
   Double_t c)
{
   //histo1->Sumw2();
   //histo2->Sumw2();
   histo3->Sumw2();
   histo3->Divide(histo1, histo2, 1., 1., "B");
   histo3->Scale(c);
}

void CbmLitTrackingQaCalculator::DivideHistos(
   const string& hist,
   LitQaNameOption opt,
   Bool_t addProjections)
{
   const vector<string>& type = CbmLitTrackingQaHistNames::GetTypes(opt);
   const vector<string>& cat = CbmLitTrackingQaHistNames::GetCategories(opt);

   if (addProjections) CreateProjections3D(hist, opt);

   string yTitle = "Efficiency [%]";
   for (Int_t i = 0; i < cat.size(); i++) {
      Double_t c = 100.;
      Int_t acc = 0;
      Int_t rec = 1;

      if (opt == kElid && i == 1){
         acc = 1;
         rec = 0;
         c = 1.;
         yTitle = "Pion suppression";
      }

      string histNameRec = hist + "_" + cat[i] + "_" + type[rec];
      string histNameAcc = hist + "_" + cat[i] + "_" + type[acc];
      string histNameEff = hist + "_" + cat[i] + "_" + type[2];

      DivideHistos(fHM->H1(histNameRec), fHM->H1(histNameAcc), fHM->H1(histNameEff), c);

      if (addProjections){
         DivideHistos(fHM->H1(histNameRec + "_px"), fHM->H1(histNameAcc + "_px"), fHM->H1(histNameEff + "_px"), c);
         DivideHistos(fHM->H1(histNameRec + "_py"), fHM->H1(histNameAcc + "_py"), fHM->H1(histNameEff + "_py"), c);
         DivideHistos(fHM->H1(histNameRec + "_pz"), fHM->H1(histNameAcc + "_pz"), fHM->H1(histNameEff + "_pz"), c);
         fHM->H1(histNameEff + "_px")->GetYaxis()->SetTitle(yTitle.c_str());
         fHM->H1(histNameEff + "_py")->GetYaxis()->SetTitle(yTitle.c_str());
         fHM->H1(histNameEff + "_pz")->GetYaxis()->SetTitle(yTitle.c_str());
      }
   }
}

void CbmLitTrackingQaCalculator::CalculateEfficiencyHistos()
{
   Double_t nofEvents = (Double_t)fHM->H1("hEventNo")->GetEntries();

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
   DivideHistos("hRichBoA", kRich);
   DivideHistos("hRichRadPos", kRich);
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
}

void CbmLitTrackingQaCalculator::IncreaseCounters()
{
   fHM->H1F("hNofGlobalTracks")->Fill(fGlobalTracks->GetEntriesFast());
   if (fDet.GetDet(kSTS)) { fHM->H1F("hNofStsTracks")->Fill(fStsMatches->GetEntriesFast()); }
   if (fDet.GetDet(kRICH)) {
      fHM->H1F("hNofRichRings")->Fill(fRichRings->GetEntriesFast());
      fHM->H1F("hNofRichProjections")->Fill(fRichProjections->GetEntriesFast());
   }
   if (fDet.GetDet(kTRD)) { fHM->H1F("hNofTrdTracks")->Fill(fTrdMatches->GetEntriesFast()); }
   if (fDet.GetDet(kMUCH)) { fHM->H1F("hNofMuchTracks")->Fill(fMuchMatches->GetEntriesFast()); }
}

void CbmLitTrackingQaCalculator::ShrinkEmptyBins()
{
   fHM->ShrinkEmptyBins("hMvdTrackHits_All");
   fHM->ShrinkEmptyBins("hMvdTrackHits_True");
   fHM->ShrinkEmptyBins("hMvdTrackHits_Fake");
   fHM->ShrinkEmptyBins("hStsTrackHits_All");
   fHM->ShrinkEmptyBins("hStsTrackHits_True");
   fHM->ShrinkEmptyBins("hStsTrackHits_Fake");
   fHM->ShrinkEmptyBins("hTrdTrackHits_All");
   fHM->ShrinkEmptyBins("hTrdTrackHits_True");
   fHM->ShrinkEmptyBins("hMuchTrackHits_Fake");
   fHM->ShrinkEmptyBins("hMuchTrackHits_All");
   fHM->ShrinkEmptyBins("hMuchTrackHits_True");
   fHM->ShrinkEmptyBins("hMuchTrackHits_Fake");
   fHM->ShrinkEmptyBins("hRichRingHits_All");
   fHM->ShrinkEmptyBins("hRichRingHits_True");
   fHM->ShrinkEmptyBins("hRichRingHits_Fake");
}
