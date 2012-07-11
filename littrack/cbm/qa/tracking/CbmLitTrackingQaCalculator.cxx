/**
 * \file CbmLitTrackingQaCalculator.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 */
#include "CbmLitTrackingQaCalculator.h"
#include "CbmLitTrackingQaHistCreator.h"
#include "CbmHistManager.h"
#include "qa/mc/CbmLitMCTrackCreator.h"
#include "utils/CbmLitUtils.h"
//#include "elid/CbmLitGlobalElectronId.h"

#include "CbmBaseHit.h"
#include "FairMCPoint.h"
#include "CbmStsTrack.h"
#include "CbmRichRing.h"
#include "CbmGlobalTrack.h"
#include "CbmTrackMatch.h"
#include "CbmMvdHitMatch.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "FairGeoNode.h"
#include "CbmDetectorList.h"
#include "CbmVertex.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmStsDetectorId.h"
#include "CbmTrdDetectorId.h"
#include "CbmMuchGeoScheme.h"
#include "CbmGeoStsPar.h"

#include "fitter/CbmRichRingFitterEllipseTau.h"

#include "TClonesArray.h"

#include <iostream>
#include <map>
#include <string>

using std::cout;
using std::endl;
using lit::FindAndReplace;
using lit::Split;

CbmLitTrackingQaCalculator::CbmLitTrackingQaCalculator(
      CbmHistManager* histManager):

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
   fTofHits(NULL),

//   fPrimVertex(NULL),
//   fKFFitter(NULL),

//   fElectronId(NULL),

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

//   if (fDet.GetElectronSetup()) {
//      fElectronId = new CbmLitGlobalElectronId();
//      fElectronId->Init();
//      fRichEllipseFitter = new CbmRichRingFitterEllipseTau();
//   }

   fMCTrackCreator = CbmLitMCTrackCreator::Instance();

   	fMcToRecoMap.clear();
   	vector<string> trackVariants = CbmLitTrackingQaHistCreator::GlobalTrackVariants();
   	for (Int_t i = 0; i < trackVariants.size(); i++) {
   		fMcToRecoMap.insert(make_pair(trackVariants[i], multimap<Int_t, Int_t>()));
   	}
//   	cout << "fMcToRecoMap:" << endl;
//   	map<string, multimap<Int_t, Int_t> >::const_iterator it;
//   	for (it = fMcToRecoMap.begin(); it != fMcToRecoMap.end(); it++) {
//		cout << " > " << (*it).first << endl;
//	}
}

void CbmLitTrackingQaCalculator::Exec()
{
   // Increase event counter
   fHM->H1("hen_EventNo_TrackingQa")->Fill(0.5);
   std::cout << "CbmLitTrackingQaCalculator::Exec: event=" << fHM->H1("hen_EventNo_TrackingQa")->GetEntries() << std::endl;

   fMCTrackCreator->Create();
//   Int_t nofTracks = fMCTracks->GetEntriesFast();
//   for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
//	   if (!fMCTrackCreator->TrackExists(iTrack)) continue;
//	   const CbmLitMCTrack& track = fMCTrackCreator->GetTrack(iTrack);
//	   std::cout << iTrack << " " << track;
//   }

   ProcessGlobalTracks();
   ProcessMcTracks();
//   PionSuppression3D();
//   StsTracksQa();
   IncreaseCounters();
}

void CbmLitTrackingQaCalculator::Finish()
{
   CalculateEfficiencyHistos();
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

//   fPrimVertex = (CbmVertex*) ioman->GetObject("PrimaryVertex");
//   if (NULL == fPrimVertex) {Fatal("Init", "No Primary Vertex!");}
//
//   fKFFitter = new CbmStsKFTrackFitter();
//   fKFFitter->Init();
}

void CbmLitTrackingQaCalculator::ProcessGlobalTracks()
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

void CbmLitTrackingQaCalculator::ProcessRichRings()
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

void CbmLitTrackingQaCalculator::ProcessMvd(
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

Bool_t CbmLitTrackingQaCalculator::CheckTrackQuality(
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

void CbmLitTrackingQaCalculator::ProcessMcTracks()
{
    vector<TH1*> effHistos = fHM->H1Vector("hte_.*_Eff_.*");
    Int_t nofEffHistos = effHistos.size();
//    for (Int_t iHist = 0; iHist < nofEffHistos; iHist++) {
//		TH1* hist = effHistos[iHist];
//		string name = hist->GetName();
//		vector<string> parse = CbmLitTrackingQaHistCreator::ParseHistoName(name);
//		cout << name;
//		for (Int_t j = 0; j < parse.size(); j++) {
//		   cout << " [" << j << "]=" << parse[j];
//		}
//		cout << std::endl;
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
      map<string, Double_t> parMap;
      parMap["p"] = mcP;
      parMap["y"] = mcY;
      parMap["pt"] = mcPt;
      parMap["Angle"] = angle;
      parMap["Np"] = 0; // FIXME : correct to number of points in concrete detector!
                        // Currently as a  temporary solution it is reassigned later
      parMap["BoA"] = 1;
      parMap["Nh"] = 1;
      parMap["RadPos"] = 1;

      for (Int_t iHist = 0; iHist < nofEffHistos; iHist++) {
  		TH1* hist = effHistos[iHist];
  		string histName = hist->GetName();
  		vector<string> split = Split(histName, '_');
  		string effName = split[1];
  		string normName = split[2];
  		string accName = split[3];
  		string parName = split[5];
  		assert(parMap.count(parName) != 0);

  		Double_t par = 0;
  		if (parName == "Np") {
  			par = (effName == "Sts") ? nofPointsSts : (effName == "Trd") ? nofPointsTrd : (effName == "Much") ? nofPointsMuch : (effName == "Tof") ? nofPointsTof : 0;
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

void CbmLitTrackingQaCalculator::FillGlobalReconstructionHistos(
   Int_t mcId,
   const multimap<Int_t, Int_t>& mcMap,
   const string& histName,
   const string& accName,
   Double_t par)
{
	string accHistName = FindAndReplace(histName, "_Eff_", "_Acc_");
	string recHistName = FindAndReplace(histName, "_Eff_", "_Rec_");
	LitTrackAcceptanceFunction function = (*CbmLitTrackingQaHistCreator::Instance()->GetTrackAcceptanceFunctions().find(accName)).second;
	Bool_t accOk = function(fMCTracks, mcId);
	if (accOk) { fHM->H1(accHistName)->Fill(par); }
	if (mcMap.find(mcId) != mcMap.end() && accOk) { fHM->H1(recHistName)->Fill(par); }
}

void CbmLitTrackingQaCalculator::FillGlobalReconstructionHistosRich(
   Int_t mcId,
   const multimap<Int_t, Int_t>& mcMap,
   const string& histName,
   const string& accName,
   Double_t par)
{
    Int_t nofHitsInRing = fMCTrackCreator->GetTrack(mcId).GetNofRichHits();
	string accHistName = FindAndReplace(histName, "_Eff_", "_Acc_");
	string recHistName = FindAndReplace(histName, "_Eff_", "_Rec_");
	LitRingAcceptanceFunction function = (*CbmLitTrackingQaHistCreator::Instance()->GetRingAcceptanceFunctions().find(accName)).second;
	Bool_t accOk = function(fMCTracks, mcId, nofHitsInRing);
	if (accOk) { fHM->H1(accHistName)->Fill(par); }
	if (mcMap.find(mcId) != mcMap.end() && accOk) { fHM->H1(recHistName)->Fill(par); }
}

//void CbmLitTrackingQaCalculator::FillGlobalElIdHistos3D(
//   const CbmMCTrack* mcTrack,
//   Int_t mcId,
//   const multimap<Int_t, Int_t>& mcMap,
//   const string& hist,
//   const string& opt)
//{
//   if (!fDet.GetElectronSetup()) return;
//
//   Bool_t isMCPrim = mcTrack->GetMotherId() == -1;
//   Bool_t isMCElectron = std::abs(mcTrack->GetPdgCode()) == 11;
//   Double_t p = mcTrack->GetP();
//   Double_t y = mcTrack->GetRapidity();
//   Double_t pt = mcTrack->GetPt();
//
//   if (isMCPrim && isMCElectron) fHM->H3F(hist+"_Acc")->Fill(p, y, pt);
//
//   Bool_t isRichId = (opt.find("rich") != std::string::npos);
//   Bool_t isTrdId = (opt.find("trd") != std::string::npos);
//   Bool_t isTofId = (opt.find("tof") != std::string::npos);
//
//   // Fill histograms for identified tracks which are accepted
//   multimap<Int_t, Int_t>::const_iterator it = mcMap.find(mcId);
//   if (it != mcMap.end() ) {
//      // Make electron identification
//      Int_t globalId = (*it).second;
//      CbmGlobalTrack* track = static_cast<CbmGlobalTrack*>(fGlobalTracks->At(globalId));
//
//      Bool_t isRichElectron = isRichId ? fElectronId->IsRichElectron(track, mcTrack->GetP()) : true;
//      Bool_t isTrdElectron = isTrdId ? fElectronId->IsTrdElectron(track, mcTrack->GetP()) : true;
//      Bool_t isTofElectron = isTofId ? fElectronId->IsTofElectron(track, mcTrack->GetP()) : true;
//
//      Bool_t isElectron = isRichElectron && isTrdElectron && isTofElectron;
//
//      if (isMCPrim && isMCElectron && isElectron) fHM->H3F(hist + "_Rec")->Fill(p, y, pt);
//   }
//}



//void CbmLitTrackingQaCalculator::PionSuppression3D()
//{
//   if (!fDet.GetElectronSetup()) return;
//
//   Int_t nofGlobalTracks = fGlobalTracks->GetEntries();
//   for (Int_t iTrack = 0; iTrack < nofGlobalTracks; iTrack++) {
//      const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(fGlobalTracks->At(iTrack));
//
//      // get track segments indices
//      Int_t stsId = globalTrack->GetStsTrackIndex();
//      Int_t trdId = globalTrack->GetTrdTrackIndex();
//      Int_t tofId = globalTrack->GetTofHitIndex();
//      Int_t richId = globalTrack->GetRichRingIndex();
//
//      // check track segments
//      Bool_t isStsOk = stsId > -1 && fDet.GetDet(kSTS);
//      Bool_t isTrdOk = trdId > -1 && fDet.GetDet(kTRD);
//      Bool_t isTofOk = tofId > -1 && fDet.GetDet(kTOF);
//      Bool_t isRichOk = false;//richId > -1 && fIsRich;
//      const FairTrackParam* proj = static_cast<FairTrackParam*>(fRichProjections->At(iTrack));
//      if (proj != NULL && proj->GetX()!= 0 &&  proj->GetY()!= 0) isRichOk = true;
//
//      if (!isStsOk) continue;
//      const CbmTrackMatch* stsTrackMatch = static_cast<const CbmTrackMatch*>(fStsMatches->At(stsId));
//      if (NULL == stsTrackMatch) continue;
//      Int_t mcIdSts = stsTrackMatch->GetMCTrackId();
//      if (mcIdSts < 0) continue;
//      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(fMCTracks->At(mcIdSts));
//      if (NULL == mcTrack) continue;
//      Int_t pdg = TMath::Abs(mcTrack->GetPdgCode());
//      Int_t motherId = mcTrack->GetMotherId();
//      Double_t p = mcTrack->GetP();
//      Double_t y = mcTrack->GetRapidity();
//      Double_t pt = mcTrack->GetPt();
//
//      Bool_t isPion = (pdg == 211);
//
//      if (!isPion) continue;
//
//      if (isTrdOk) fHM->H3F("hStsTrd3DElId_PiSupp_Acc")->Fill(p, y, pt);
//      if (isTrdOk && isTofOk) fHM->H3F("hStsTrd3DElIdNormStsTrdTof_PiSupp_Acc")->Fill(p, y, pt);
//      if (isTrdOk && isTofOk) fHM->H3F("hStsTrdTof3DElId_PiSupp_Acc")->Fill(p, y, pt);
//      if (isRichOk) fHM->H3F("hStsRich3DElId_PiSupp_Acc")->Fill(p, y, pt);
//      if (isRichOk && isTrdOk) fHM->H3F("hStsRich3DElIdNormStsRichTrd_PiSupp_Acc")->Fill(p, y, pt);
//      if (isRichOk && isTrdOk) fHM->H3F("hStsRichTrd3DElId_PiSupp_Acc")->Fill(p, y, pt);
//      if (isRichOk && isTrdOk && isTofOk) fHM->H3F("hStsRich3DElIdNormStsRichTrdTof_PiSupp_Acc")->Fill(p, y, pt);
//      if (isRichOk && isTrdOk && isTofOk) fHM->H3F("hStsRichTrd3DElIdNormStsRichTrdTof_PiSupp_Acc")->Fill(p, y, pt);
//      if (isRichOk && isTrdOk && isTofOk) fHM->H3F("hStsRichTrdTof3DElId_PiSupp_Acc")->Fill(p, y, pt);
//
////      H3("hStsTrd3DElId_PiSupp_Acc")->Fill(p, y, pt);
////      H3("hStsTrd3DElIdNormStsTrdTof_PiSupp_Acc")->Fill(p, y, pt);
////      H3("hStsTrdTof3DElId_PiSupp_Acc")->Fill(p, y, pt);
////      H3("hStsRich3DElId_PiSupp_Acc")->Fill(p, y, pt);
////      H3("hStsRich3DElIdNormStsRichTrd_PiSupp_Acc")->Fill(p, y, pt);
////      H3("hStsRichTrd3DElId_PiSupp_Acc")->Fill(p, y, pt);
////      H3("hStsRich3DElIdNormStsRichTrdTof_PiSupp_Acc")->Fill(p, y, pt);
////      H3("hStsRichTrd3DElIdNormStsRichTrdTof_PiSupp_Acc")->Fill(p, y, pt);
////      H3("hStsRichTrdTof3DElId_PiSupp_Acc")->Fill(p, y, pt);
//
//      Bool_t isRichElectron = fElectronId->IsRichElectron(globalTrack, mcTrack->GetP());
//      Bool_t isTrdElectron = fElectronId->IsTrdElectron(globalTrack, mcTrack->GetP());
//      Bool_t isTofElectron = fElectronId->IsTofElectron(globalTrack, mcTrack->GetP());
//
//      if (isTrdElectron) fHM->H3F("hStsTrd3DElId_PiSupp_Rec")->Fill(p, y, pt);
//      if (isTrdElectron) fHM->H3F("hStsTrd3DElIdNormStsTrdTof_PiSupp_Rec")->Fill(p, y, pt);
//      if (isTrdElectron && isTofElectron) fHM->H3F("hStsTrdTof3DElId_PiSupp_Rec")->Fill(p, y, pt);
//      if (isRichElectron) fHM->H3F("hStsRich3DElId_PiSupp_Rec")->Fill(p, y, pt);
//      if (isRichElectron) fHM->H3F("hStsRich3DElIdNormStsRichTrd_PiSupp_Rec")->Fill(p, y, pt);
//      if (isRichElectron && isTrdElectron) fHM->H3F("hStsRichTrd3DElId_PiSupp_Rec")->Fill(p, y, pt);
//      if (isRichElectron) fHM->H3F("hStsRich3DElIdNormStsRichTrdTof_PiSupp_Rec")->Fill(p, y, pt);
//      if (isRichElectron && isTrdElectron) fHM->H3F("hStsRichTrd3DElIdNormStsRichTrdTof_PiSupp_Rec")->Fill(p, y, pt);
//      if (isRichElectron && isTrdElectron && isTofElectron) fHM->H3F("hStsRichTrdTof3DElId_PiSupp_Rec")->Fill(p, y, pt);
//   }
//}

//void CbmLitTrackingQaCalculator::StsTracksQa()
//{
//   Int_t nSts = fStsTracks->GetEntriesFast();
//   for (Int_t i = 0; i < nSts; i++) {
//      CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(i);
//      if (stsTrack == NULL) continue;
//      const CbmTrackMatch* stsTrackMatch = static_cast<const CbmTrackMatch*>(fStsMatches->At(i));
//      if (stsTrackMatch == NULL) continue;
//      Int_t mcIdSts = stsTrackMatch->GetMCTrackId();
//      if (mcIdSts < 0) continue;
//      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(fMCTracks->At(mcIdSts));
//      if (mcTrack == NULL) continue;
//      Int_t motherId = mcTrack->GetMotherId();
//      if (motherId != -1) continue;
//
//      TVector3 momMC;
//      mcTrack->GetMomentum(momMC);
//      //fKFFitter.DoFit(stsTrack,11);
//      Double_t chiPrimary = fKFFitter->GetChiToVertex(stsTrack, fPrimVertex);
//      fHM->H1("hStsChiprim")->Fill(chiPrimary);
//
//      FairTrackParam vtxTrack;
//      fKFFitter->FitToVertex(stsTrack, fPrimVertex, &vtxTrack);
//      TVector3 momRec;
//      vtxTrack.Momentum(momRec);
//
//      Double_t dpp = 100. * (momMC.Mag() - momRec.Mag()) / momMC.Mag();
//      fHM->H2F("hStsMomresVsMom")->Fill(momMC.Mag(), dpp);
//   }
//
//   // TODO: refactor track length to a seperate method!
//   //Track length in TOF detector
//   Int_t nofGlobalTracks = fGlobalTracks->GetEntriesFast();
//   for (Int_t iG = 0; iG < nofGlobalTracks; iG++){
//      const CbmGlobalTrack* gTrack  = static_cast<const CbmGlobalTrack*>(fGlobalTracks->At(iG));
//      if (!gTrack) continue;
//
//      Int_t tofIndex = gTrack->GetTofHitIndex();
//      if (tofIndex < 0) continue;
//      CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofIndex);
//      if (NULL == tofHit) continue;
//
//      Double_t trackLength = gTrack->GetLength();
//      Int_t stsInd = gTrack->GetStsTrackIndex();
//      if (stsInd < 0) continue;
//      const CbmTrackMatch* stsMatch  = static_cast<const CbmTrackMatch*>(fStsMatches->At(stsInd));
//      if (stsMatch == NULL) continue;
//      Int_t stsMCTrackId = stsMatch->GetMCTrackId();
//      if (stsMCTrackId < 0) continue;
////      CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMCTracks->At(stsMCTrackId);
////      if (mcTrack1 == NULL) continue;
//
//      Int_t nofTofPoints = fTofPoints->GetEntriesFast();
//      for (Int_t iTof = 0; iTof < nofTofPoints; iTof++) {
//         const CbmTofPoint* tofPoint = static_cast<const CbmTofPoint*>(fTofPoints->At(iTof));
//         if (NULL == tofPoint) continue;
//         if (tofPoint->GetTrackID() == stsMCTrackId) {
//            Double_t dll = 100. * (tofPoint->GetLength() - trackLength) / tofPoint->GetLength();
//            fHM->H1("hTrackLength")->Fill(dll);
//            break;
//         }
//      }
//   }
//}

//void CbmLitTrackingQaCalculator::FillMCMomVsAngle(
//      const CbmMCTrack* mcTrack)
//{
//   // Get MC track properties (vertex, momentum, primary/secondary, pdg, etc...)
//   TVector3 vertex;
//   mcTrack->GetStartVertex(vertex);
//   Bool_t isPrim = mcTrack->GetMotherId() == -1;
//   Double_t p = mcTrack->GetP();
//   Bool_t isRef = isPrim && p > fRefMomentum;
//   Bool_t isMuon = std::abs(mcTrack->GetPdgCode()) == 13;
//   Bool_t isElectron = std::abs(mcTrack->GetPdgCode()) == 11;
//   TVector3 mom;
//   mcTrack->GetMomentum(mom);
//   Double_t angle = std::abs(mom.Theta() * 180 / TMath::Pi());
//
//   fHM->H2F("hMCMomVsAngle_All")->Fill(p, angle);
//   if (isPrim) { fHM->H2F("hMCMomVsAngle_Prim")->Fill(p, angle); }
//   if (isRef) { fHM->H2F("hMCMomVsAngle_Ref")->Fill(p, angle); }
//   if (!isPrim) { fHM->H2F("hMCMomVsAngle_Sec")->Fill(p, angle); }
//   if (isPrim && isMuon) { fHM->H2F("hMCMomVsAngle_Mu")->Fill(p, angle); }
//   if (isPrim && isElectron) { fHM->H2F("hMCMomVsAngle_El")->Fill(p, angle); }
//}

void CbmLitTrackingQaCalculator::DivideHistos(
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

void CbmLitTrackingQaCalculator::CalculateEfficiencyHistos()
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

void CbmLitTrackingQaCalculator::IncreaseCounters()
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
