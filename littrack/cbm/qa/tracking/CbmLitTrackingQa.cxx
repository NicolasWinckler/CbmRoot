/**
 * \file CbmLitTrackingQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 */

#include "CbmLitTrackingQa.h"
#include "CbmLitTrackingQaHistCreator.h"
#include "CbmLitTrackingQaStudyReport.h"
#include "CbmLitTrackingQaReport.h"
#include "CbmHistManager.h"
#include "utils/CbmLitUtils.h"
#include "qa/mc/CbmLitMCTrackCreator.h"
#include "CbmGlobalTrack.h"
#include "CbmTrackMatch.h"
#include "CbmRichRing.h"
#include "CbmMCTrack.h"
#include "CbmBaseHit.h"
#include "FairMCPoint.h"
#include "CbmStsTrack.h"
#include "CbmMvdHitMatch.h"

#include "TH1.h"
#include "TClonesArray.h"

#include <boost/assign/list_of.hpp>

#include <fstream>
#include <iostream>

using std::cout;
using boost::assign::list_of;
using lit::Split;
using lit::FindAndReplace;

CbmLitTrackingQa::CbmLitTrackingQa():
   FairTask("LitTrackingQA", 1),
   fHM(NULL),
   fOutputDir(""),
   fDet(),
   fMCTrackCreator(NULL),
   fMinNofPointsSts(4),
   fMinNofPointsTrd(8),
   fMinNofPointsMuch(10),
   fMinNofPointsTof(1),
   fMinNofHitsRich(7),
   fQuota(0.7),
   fQuotaRich(0.6),
   fMinNofHitsTrd(8),
   fMinNofHitsMuch(10),
   fUseConsecutivePointsInSts(true),
   fPRangeMin(0.),
   fPRangeMax(10.),
   fPRangeBins(20.),
   fYRangeMin(0.),
   fYRangeMax(4.),
   fYRangeBins(16),
   fPtRangeMin(0.),
   fPtRangeMax(3.),
   fPtRangeBins(12.),
   fMcToRecoMap(),
   fMCTracks(NULL),
   fGlobalTracks(NULL),
   fMvdPoints(NULL),
   fMvdHitMatches(NULL),
   fStsTracks(NULL),
   fStsMatches(NULL),
   fRichRings(NULL),
   fRichProjections(NULL),
   fRichRingMatches(NULL),
   fMuchMatches(NULL),
   fTrdMatches(NULL),
   fTofPoints(NULL),
   fTofHits(NULL)
{
}

CbmLitTrackingQa::~CbmLitTrackingQa()
{
   if (fHM) delete fHM;
}

InitStatus CbmLitTrackingQa::Init()
{
   // Create histogram manager which is used throughout the program
   fHM = new CbmHistManager();

   fDet.DetermineSetup();

   // Create histograms
   CbmLitTrackingQaHistCreator* hc = CbmLitTrackingQaHistCreator::Instance();
   hc->SetPRange(fPRangeBins, fPRangeMin, fPRangeMax);
   hc->SetPtRange(fPtRangeBins, fPtRangeMin, fPtRangeMax);
   hc->SetYRange(fYRangeBins, fYRangeMin, fYRangeMax);
   hc->Create(fHM);

   fDet.DetermineSetup();
   std::cout << fDet.ToString();

   ReadDataBranches();

   fMCTrackCreator = CbmLitMCTrackCreator::Instance();

   fMcToRecoMap.clear();
   vector<string> trackVariants = CbmLitTrackingQaHistCreator::GlobalTrackVariants();
   for (Int_t i = 0; i < trackVariants.size(); i++) {
      fMcToRecoMap.insert(make_pair(trackVariants[i], multimap<Int_t, Int_t>()));
   }

   return kSUCCESS;
}

void CbmLitTrackingQa::Exec(
   Option_t* opt)
{
   // Increase event counter
   fHM->H1("hen_EventNo_TrackingQa")->Fill(0.5);
   std::cout << "CbmLitTrackingQa::Exec: event=" << fHM->H1("hen_EventNo_TrackingQa")->GetEntries() << std::endl;

   fMCTrackCreator->Create();

   ProcessGlobalTracks();
   ProcessMcTracks();
   IncreaseCounters();
}

void CbmLitTrackingQa::Finish()
{
   CalculateEfficiencyHistos();
   fHM->WriteToFile();
   CreateSimulationReport();
}

void CbmLitTrackingQa::ReadDataBranches()
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
   }

   if (fDet.GetDet(kRICH)) {
     fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
     if (NULL == fRichRings) { Fatal("Init","No RichRing array!"); }
     fRichProjections = (TClonesArray*) ioman->GetObject("RichProjection");
     if (NULL == fRichProjections) { Fatal("Init","No RichProjection array!"); }
     fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
     if (NULL == fRichRingMatches) { Fatal("Init","No RichRingMatch array!"); }
   }

   if (fDet.GetDet(kMUCH)) {
      fMuchMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
      if (NULL == fMuchMatches) { Fatal("Init","No MuchTrackMatch array!"); }
   }

   if (fDet.GetDet(kTRD)) {
      fTrdMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
      if (NULL == fTrdMatches) { Fatal("Init","No TrdTrackMatch array!"); }
   }

   if (fDet.GetDet(kTOF)) {
      fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
      if (NULL == fTofPoints) { Fatal("Init", "No TofPoint array!"); }
      fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
      if (NULL == fTofHits) { Fatal("Init", "No TofHit array!"); }
   }
}

void CbmLitTrackingQa::ProcessGlobalTracks()
{
   // Clear all maps for MC to reco matching
   map<string, multimap<Int_t, Int_t> >::iterator it;
   for (it = fMcToRecoMap.begin(); it != fMcToRecoMap.end(); it++) {
      (*it).second.clear();
   }

   ProcessRichRings();

   Int_t nofGlobalTracks = fGlobalTracks->GetEntriesFast();
   for(Int_t iTrack = 0; iTrack < nofGlobalTracks; iTrack++) {
      const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(fGlobalTracks->At(iTrack));

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
      const CbmTrackMatch* stsTrackMatch;
      if (isStsOk) {
         stsTrackMatch = static_cast<const CbmTrackMatch*>(fStsMatches->At(stsId));
         isStsOk = CheckTrackQuality(stsTrackMatch, kSTS);
         if (!isStsOk) { // ghost track
            Int_t nofHits = stsTrackMatch->GetNofTrueHits() + stsTrackMatch->GetNofWrongHits() + stsTrackMatch->GetNofFakeHits();
            fHM->H1("hng_NofGhosts_Sts_Nh")->Fill(nofHits);

            // calculate number of ghost after RICH matching
            if (isRichOk){
               const CbmRichRing* ring = static_cast<const CbmRichRing*>(fRichRings->At(richId));
               if (NULL != ring){
                  if (ring->GetDistance() < 1.) fHM->H1("hng_NofGhosts_StsRichMatching_Nh")->Fill(nofHits);
               }
            }
         } else {
            ProcessMvd(stsId);
         }
      }
      const CbmTrackMatch* trdTrackMatch;
      if (isTrdOk) {
         trdTrackMatch = static_cast<const CbmTrackMatch*>(fTrdMatches->At(trdId));
         Int_t nofHits = trdTrackMatch->GetNofTrueHits() + trdTrackMatch->GetNofWrongHits() + trdTrackMatch->GetNofFakeHits();
         if (nofHits >= fMinNofHitsTrd) {
            isTrdOk = CheckTrackQuality(trdTrackMatch, kTRD);
            if (!isTrdOk) { // ghost track
               fHM->H1("hng_NofGhosts_Trd_Nh")->Fill(nofHits);
            }
         } else {
            isTrdOk = false;
         }
      }
      const CbmTrackMatch* muchTrackMatch;
      if (isMuchOk) {
         muchTrackMatch = static_cast<const CbmTrackMatch*>(fMuchMatches->At(muchId));
         Int_t nofHits = muchTrackMatch->GetNofTrueHits() + muchTrackMatch->GetNofWrongHits() + muchTrackMatch->GetNofFakeHits();
         if (nofHits >= fMinNofHitsMuch) {
            isMuchOk = CheckTrackQuality(muchTrackMatch, kMUCH);
            if (!isMuchOk) { // ghost track
               fHM->H1("hng_NofGhosts_Much_Nh")->Fill(nofHits);
            }
         } else {
            isMuchOk = false;
         }
      }
      const CbmTrackMatch* richRingMatch;
      if (isRichOk) {
         richRingMatch = static_cast<const CbmTrackMatch*>(fRichRingMatches->At(richId));
         if (richRingMatch == NULL) { continue; }
         Int_t nofHits = richRingMatch->GetNofTrueHits() + richRingMatch->GetNofWrongHits() + richRingMatch->GetNofFakeHits();
         isRichOk = CheckTrackQuality(richRingMatch, kRICH);
         if (!isRichOk) { // ghost ring
            fHM->H1("hng_NofGhosts_Rich_Nh")->Fill(nofHits);

            // calculate number of ghost after STS matching and electron identification
            const CbmRichRing* ring = static_cast<const CbmRichRing*>(fRichRings->At(richId));
            if (NULL != ring) {
               if (ring->GetDistance() < 1.) fHM->H1("hng_NofGhosts_RichStsMatching_Nh")->Fill(nofHits);

               Double_t momentumMc = 0.;
               if (stsTrackMatch != NULL) {
                  const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(fMCTracks->At(stsTrackMatch->GetMCTrackId()));
                  if (mcTrack != NULL) momentumMc = mcTrack->GetP();
                }
//                if (ring->GetDistance() < 1. && fElectronId->IsRichElectron(globalTrack, momentumMc))
//                   fHM->H1("hng_NofGhosts_RichElId_Nh")->Fill(nofHits);
            }
         }
      }

      // Get MC indices of track segments
      Int_t stsMCId = -1, trdMCId = -1, muchMCId = -1, tofMCId = -1, richMCId = -1;
      if (isStsOk) { stsMCId = stsTrackMatch->GetMCTrackId(); }
      if (isTrdOk) { trdMCId = trdTrackMatch->GetMCTrackId(); }
      if (isMuchOk) { muchMCId = muchTrackMatch->GetMCTrackId(); }
      if (isTofOk) {
         const CbmBaseHit* tofHit = static_cast<const CbmBaseHit*>(fTofHits->At(tofId));
         const FairMCPoint* tofPoint = static_cast<const FairMCPoint*>(fTofPoints->At(tofHit->GetRefId()));
         if (tofPoint != NULL) tofMCId = tofPoint->GetTrackID();
      }
      if (isRichOk) { richMCId = richRingMatch->GetMCTrackId(); }

      map<string, multimap<Int_t, Int_t> >::iterator it;
      for (it = fMcToRecoMap.begin(); it != fMcToRecoMap.end(); it++) {
        string name = (*it).first;
        multimap<Int_t, Int_t>& mcRecoMap = (*it).second;
        Bool_t sts = (name.find("Sts") != string::npos) ? isStsOk && stsMCId != -1 : true;
        Bool_t trd = (name.find("Trd") != string::npos) ? isTrdOk && stsMCId == trdMCId : true;
        Bool_t much = (name.find("Much") != string::npos) ? isMuchOk && stsMCId == muchMCId : true;
        Bool_t tof = (name.find("Tof") != string::npos) ? isTofOk && stsMCId == tofMCId : true;
        Bool_t rich = (name.find("Rich") != string::npos) ? isRichOk && stsMCId == richMCId : true;

        if (sts && trd && much && tof && rich) {
           mcRecoMap.insert(make_pair<Int_t, Int_t>(stsMCId, iTrack));
        }
      }
   }
}

void CbmLitTrackingQa::ProcessRichRings()
{
   if (!fDet.GetDet(kRICH)) return;
   Int_t nofRings = fRichRings->GetEntriesFast();
   for(Int_t iRing = 0; iRing < nofRings; iRing++) {
     const CbmTrackMatch* richRingMatch = static_cast<const CbmTrackMatch*>(fRichRingMatches->At(iRing));
     Bool_t isRichOk = CheckTrackQuality(richRingMatch, kRICH);
     Int_t richMCId = (isRichOk) ? richRingMatch->GetMCTrackId() : -1;
     if (isRichOk && -1 != richMCId) {
         fMcToRecoMap["Rich"].insert(make_pair<Int_t, Int_t>(richMCId, iRing));
     }
   }
}

void CbmLitTrackingQa::ProcessMvd(
   Int_t stsId)
{
   if (!fDet.GetDet(kMVD)) return;
   const CbmStsTrack* track = static_cast<const CbmStsTrack*>(fStsTracks->At(stsId));
   Int_t nofHits = track->GetNMvdHits();
   fHM->H1("hth_Mvd_TrackHits_All")->Fill(nofHits);

   const CbmTrackMatch* stsTrackMatch = static_cast<const CbmTrackMatch*>(fStsMatches->At(stsId));
   Int_t stsMcTrackId = stsTrackMatch->GetMCTrackId();

   Int_t nofTrueHits = 0, nofFakeHits = 0;
   for (Int_t iHit = 0; iHit < nofHits; iHit++) {
      Int_t hitId = track->GetMvdHitIndex(iHit);
      const CbmMvdHitMatch* hitMatch = static_cast<const CbmMvdHitMatch*>(fMvdHitMatches->At(hitId));
      if (NULL == hitMatch) continue;
      Int_t pointId = hitMatch->GetPointId();
      const FairMCPoint* point = static_cast<const FairMCPoint*>(fMvdPoints->At(pointId));
      if (NULL == point) continue;
      Int_t mcTrackId = point->GetTrackID();
      if (mcTrackId == stsMcTrackId) { // true hit
         nofTrueHits++;
      } else { // fake hit
         nofFakeHits++;
      }
   }
   fHM->H1("hth_Mvd_TrackHits_True")->Fill(nofTrueHits);
   fHM->H1("hth_Mvd_TrackHits_Fake")->Fill(nofFakeHits);
   if (nofHits != 0) {
      fHM->H1("hth_Mvd_TrackHits_TrueOverAll")->Fill(Double_t(nofTrueHits) / Double_t(nofHits));
      fHM->H1("hth_Mvd_TrackHits_FakeOverAll")->Fill(Double_t(nofFakeHits) / Double_t(nofHits));
   }
}

Bool_t CbmLitTrackingQa::CheckTrackQuality(
   const CbmTrackMatch* trackMatch,
   DetectorId detId)
{
   Int_t mcId = trackMatch->GetMCTrackId();
   if (mcId < 0) { return false; }

   Int_t nofTrue = trackMatch->GetNofTrueHits();
   Int_t nofWrong = trackMatch->GetNofWrongHits();
   Int_t nofFake = trackMatch->GetNofFakeHits();
   Int_t nofHits = nofTrue + nofWrong + nofFake;
   Double_t quali = Double_t(nofTrue) / Double_t(nofHits);
   Double_t fakequali = Double_t(nofFake + nofWrong) / Double_t(nofHits);

   string detName = (detId == kSTS) ? "Sts" : (detId == kTRD) ? "Trd" : (detId == kMUCH) ? "Much" : (detId == kRICH) ? "Rich" : "";
   assert(detName != "");

   if (detName != "") {
     string histName = "hth_" + detName + "_TrackHits";
      fHM->H1(histName + "_All")->Fill(nofHits);
      fHM->H1(histName + "_True")->Fill(nofTrue);
      fHM->H1(histName + "_Fake")->Fill(nofFake + nofWrong);
      fHM->H1(histName + "_TrueOverAll")->Fill(quali);
      fHM->H1(histName + "_FakeOverAll")->Fill(fakequali);
   }
   return (detId == kRICH) ? quali >= fQuotaRich : quali >= fQuota;
}

void CbmLitTrackingQa::ProcessMcTracks()
{
    vector<TH1*> effHistos = fHM->H1Vector("hte_.*_Eff_.*");
    Int_t nofEffHistos = effHistos.size();
//    for (Int_t iHist = 0; iHist < nofEffHistos; iHist++) {
//    TH1* hist = effHistos[iHist];
//    string name = hist->GetName();
//    vector<string> parse = CbmLitTrackingQaHistCreator::ParseHistoName(name);
//    cout << name;
//    for (Int_t j = 0; j < parse.size(); j++) {
//       cout << " [" << j << "]=" << parse[j];
//    }
//    cout << std::endl;
//    }

   Int_t nofMcTracks = fMCTracks->GetEntriesFast();
   for (Int_t iMCTrack = 0; iMCTrack < nofMcTracks; iMCTrack++) {
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(fMCTracks->At(iMCTrack));

      // Check accepted tracks cutting on minimal number of MC points
      if (!fMCTrackCreator->TrackExists(iMCTrack)) continue;
      const CbmLitMCTrack& litMCTrack = fMCTrackCreator->GetTrack(iMCTrack);
      Int_t nofPointsSts = litMCTrack.GetNofPointsInDifferentStations(kSTS);
      Int_t nofPointsTrd = litMCTrack.GetNofPointsInDifferentStations(kTRD);
      Int_t nofPointsMuch = litMCTrack.GetNofPointsInDifferentStations(kMUCH);
      Int_t nofPointsTof = litMCTrack.GetNofPoints(kTOF);
      Int_t nofHitsRich = litMCTrack.GetNofRichHits();

      // Check local tracks
      Bool_t stsConsecutive = (fUseConsecutivePointsInSts) ? litMCTrack.GetNofConsecutivePoints(kSTS) >= fMinNofPointsSts : true;
      Bool_t isStsOk = nofPointsSts >= fMinNofPointsSts && fDet.GetDet(kSTS) && stsConsecutive;
      Bool_t isTrdOk = nofPointsTrd >= fMinNofPointsTrd && fDet.GetDet(kTRD);
      Bool_t isMuchOk = nofPointsMuch >= fMinNofPointsMuch && fDet.GetDet(kMUCH);
      Bool_t isTofOk = nofPointsTof >= fMinNofPointsTof && fDet.GetDet(kTOF);
      Bool_t isRichOk = nofHitsRich >= fMinNofHitsRich && fDet.GetDet(kRICH);

      // calculate polar angle
      TVector3 mom;
      mcTrack->GetMomentum(mom);
      Double_t angle = std::abs(mom.Theta() * 180 / TMath::Pi());
      Double_t mcP = mcTrack->GetP();
      Double_t mcY = mcTrack->GetRapidity();
      Double_t mcPt = mcTrack->GetPt();

      // Fill parameter name to value map
      map<string, vector<Double_t> > parMap;
      parMap["p"] = list_of(mcP);
      parMap["y"] = list_of(mcY);
      parMap["pt"] = list_of(mcPt);
      parMap["Angle"] = list_of(angle);
      parMap["Np"] = list_of(0); // FIXME : correct to number of points in concrete detector!
                        // Currently as a  temporary solution it is reassigned later
      parMap["BoA"] = list_of(1);
      parMap["Nh"] = list_of(1);
      parMap["RadPos"] = list_of(1);
      parMap["YPt"] = list_of(mcY)(mcPt);

      for (Int_t iHist = 0; iHist < nofEffHistos; iHist++) {
         TH1* hist = effHistos[iHist];
         string histName = hist->GetName();
         vector<string> split = Split(histName, '_');
         string effName = split[1];
         string normName = split[2];
         string accName = split[3];
         string parName = split[5];
         assert(parMap.count(parName) != 0);

         vector<Double_t> par = list_of(0);
         if (parName == "Np") {
            par = list_of((effName == "Sts") ? nofPointsSts : (effName == "Trd") ? nofPointsTrd : (effName == "Much") ? nofPointsMuch : (effName == "Tof") ? nofPointsTof : 0);
         } else {
            par = parMap[parName];
         }

         Bool_t sts = (normName.find("Sts") != string::npos) ? isStsOk : true;
         Bool_t trd = (normName.find("Trd") != string::npos) ? isTrdOk : true;
         Bool_t much = (normName.find("Much") != string::npos) ? isMuchOk : true;
         Bool_t tof = (normName.find("Tof") != string::npos) ? isTofOk : true;
         Bool_t rich = (normName.find("Rich") != string::npos) ? isRichOk : true;

         if (effName == "Trd" || effName == "Much" || effName == "Tof") {
            string prevRecName = FindAndReplace(normName, effName, "");
            Bool_t isPrevRec = fMcToRecoMap[prevRecName].find(iMCTrack) != fMcToRecoMap[prevRecName].end();
            Bool_t accOk = isPrevRec && sts && trd && much && tof && rich;
            if (accOk) {
               FillGlobalReconstructionHistos(iMCTrack, fMcToRecoMap[normName], histName, accName, par);
            }
         } else {
            Bool_t accOk = sts && trd && much && tof && rich;
            if (accOk) {
              if (histName.find("Rich") == string::npos) {
               FillGlobalReconstructionHistos(iMCTrack, fMcToRecoMap[effName], histName, accName, par);
              } else {
                 FillGlobalReconstructionHistosRich(iMCTrack, fMcToRecoMap[effName], histName, accName, par);
              }
            }
         }
      } // Loop over efficiency histograms
   } // Loop over MCTracks
}

void CbmLitTrackingQa::FillGlobalReconstructionHistos(
   Int_t mcId,
   const multimap<Int_t, Int_t>& mcMap,
   const string& histName,
   const string& accName,
   const vector<Double_t>& par)
{
   string accHistName = FindAndReplace(histName, "_Eff_", "_Acc_");
   string recHistName = FindAndReplace(histName, "_Eff_", "_Rec_");
   LitTrackAcceptanceFunction function = (*CbmLitTrackingQaHistCreator::Instance()->GetTrackAcceptanceFunctions().find(accName)).second;
   Bool_t accOk = function(fMCTracks, mcId);
   Int_t nofParams = par.size();
   assert(nofParams < 3 && nofParams > 0);
   if (nofParams == 1) {
      if (accOk) { fHM->H1(accHistName)->Fill(par[0]); }
      if (mcMap.find(mcId) != mcMap.end() && accOk) { fHM->H1(recHistName)->Fill(par[0]); }
   } else if (nofParams == 2) {
      if (accOk) { fHM->H1(accHistName)->Fill(par[0], par[1]); }
      if (mcMap.find(mcId) != mcMap.end() && accOk) { fHM->H1(recHistName)->Fill(par[0], par[1]); }
   }
}

void CbmLitTrackingQa::FillGlobalReconstructionHistosRich(
   Int_t mcId,
   const multimap<Int_t, Int_t>& mcMap,
   const string& histName,
   const string& accName,
   const vector<Double_t>& par)
{
   Int_t nofHitsInRing = fMCTrackCreator->GetTrack(mcId).GetNofRichHits();
   string accHistName = FindAndReplace(histName, "_Eff_", "_Acc_");
   string recHistName = FindAndReplace(histName, "_Eff_", "_Rec_");
   LitRingAcceptanceFunction function = (*CbmLitTrackingQaHistCreator::Instance()->GetRingAcceptanceFunctions().find(accName)).second;
   Bool_t accOk = function(fMCTracks, mcId, nofHitsInRing);
   Int_t nofParams = par.size();
   assert(nofParams < 3 && nofParams > 0);
   if (nofParams == 1) {
      if (accOk) { fHM->H1(accHistName)->Fill(par[0]); }
      if (mcMap.find(mcId) != mcMap.end() && accOk) { fHM->H1(recHistName)->Fill(par[0]); }
   } else if (nofParams == 2) {
      if (accOk) { fHM->H1(accHistName)->Fill(par[0], par[1]); }
      if (mcMap.find(mcId) != mcMap.end() && accOk) { fHM->H1(recHistName)->Fill(par[0], par[1]); }
   }
}

void CbmLitTrackingQa::DivideHistos(
   TH1* histo1,
   TH1* histo2,
   TH1* histo3,
   Double_t scale)
{
   //histo1->Sumw2();
   //histo2->Sumw2();
   histo3->Sumw2();
   histo3->Divide(histo1, histo2, 1., 1., "B");
   histo3->Scale(scale);
}

void CbmLitTrackingQa::CalculateEfficiencyHistos()
{
    vector<TH1*> effHistos = fHM->H1Vector("hte_.+_Eff_.+");
    Int_t nofEffHistos = effHistos.size();
    for (Int_t iHist = 0; iHist < nofEffHistos; iHist++) {
        TH1* effHist = effHistos[iHist];
        string effHistName = effHist->GetName();
      string accHistName = FindAndReplace(effHistName, "_Eff_", "_Acc_");
      string recHistName = FindAndReplace(effHistName, "_Eff_", "_Rec_");
      DivideHistos(fHM->H1(recHistName), fHM->H1(accHistName), effHist, 100.);
      effHist->SetMinimum(0.);
      effHist->SetMaximum(100.);
    }
}

void CbmLitTrackingQa::IncreaseCounters()
{
   fHM->H1("hno_NofObjects_GlobalTracks")->Fill(fGlobalTracks->GetEntriesFast());
   if (fDet.GetDet(kSTS)) { fHM->H1("hno_NofObjects_StsTracks")->Fill(fStsTracks->GetEntriesFast()); }
   if (fDet.GetDet(kRICH)) {
      fHM->H1("hno_NofObjects_RichRings")->Fill(fRichRings->GetEntriesFast());
      fHM->H1("hno_NofObjects_RichProjections")->Fill(fRichProjections->GetEntriesFast());
   }
   if (fDet.GetDet(kTRD)) { fHM->H1("hno_NofObjects_TrdTracks")->Fill(fTrdMatches->GetEntriesFast()); }
   if (fDet.GetDet(kMUCH)) { fHM->H1("hno_NofObjects_MuchTracks")->Fill(fMuchMatches->GetEntriesFast()); }
}


void CbmLitTrackingQa::CreateSimulationReport()
{
   CbmSimulationReport* report = new CbmLitTrackingQaReport();
   report->Create(fHM, fOutputDir);
   delete report;
}

void CbmLitTrackingQa::CreateStudyReport(
      const string& title,
      const vector<string>& fileNames,
      const vector<string>& studyNames)
{
   CbmStudyReport* report = new CbmLitTrackingQaStudyReport();
   report->Create(fileNames, studyNames, fOutputDir);
   delete report;
}

ClassImp(CbmLitTrackingQa);
