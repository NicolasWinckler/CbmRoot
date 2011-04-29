/** CbmLitReconstructionQa.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 2.0
 **/

#include "qa/CbmLitReconstructionQa.h"

#include "base/CbmLitEnvironment.h"
#include "utils/CbmLitDrawHist.h"
#include "utils/CbmLitUtils.h"

#include "CbmGlobalTrack.h"
#include "CbmTrackMatch.h"
#include "CbmTofHit.h"
#include "CbmTofPoint.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "CbmDetectorList.h"
#include "CbmHit.h"
#include "CbmStsTrack.h"
#include "CbmMvdPoint.h"
#include "CbmMvdHitMatch.h"
#include "CbmRichRingMatch.h"
#include "CbmRichHit.h"
#include "CbmVertex.h"
#include "CbmStsKFTrackFitter.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TList.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPad.h"
#include "TFile.h"

#include <iostream>
#include <map>
#include <cmath>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>

// histogram types
const Int_t ACC=0; // accepted tracks histogram
const Int_t REC=1; // reconstructed tracks histogram
const Int_t EFF=2; // efficiency histogram
// track categories
const Int_t ALL=0; // all tracks
const Int_t REF=1; // reference tracks
const Int_t PRIM=2; // primary tracks
const Int_t SEC=3; // secondary tracks
const Int_t MU=4; // primary muon tracks
const Int_t EL=5; // primary electron tracks
// RICH categories
const Int_t RICHALL=0; // all rings
const Int_t RICHALLREF=1; // all reference rings
const Int_t RICHEL=2; // electron rings
const Int_t RICHELREF=3; // electron reference rings
const Int_t RICHPI=4; // pion rings
const Int_t RICHPIREF=5; // pion reference rings
// for hits in the track distribution
const Int_t ALLHITS = 0;
const Int_t TRUEHITS = 1;
const Int_t FAKEHITS = 2;
const Int_t TRUEALL = 3;
const Int_t FAKEALL = 4;

CbmLitReconstructionQa::CbmLitReconstructionQa():
   FairTask("LitReconstructionQA", 1),

   fMinNofPointsSts(4),
   fMinNofPointsTrd(8),
   fMinNofPointsMuch(10),
   fMinNofPointsTof(1),
   fQuota(0.7),
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
   fStsMatches(NULL),
   fRichHits(NULL),
   fRichRings(NULL),
   fRichProjections(NULL),
   fRichRingMatches(NULL),
   fRichPoints(NULL),
   fMuchPixelHits(NULL),
   fMuchStrawHits(NULL),
   fMuchMatches(NULL),
   fTrdMatches(NULL),
   fTrdHits(NULL),
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

   fhStsGhostNh(NULL),
   fhRecGhostNh(NULL),
   fhRichGhostNh(NULL),

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
   fhStsChiprim(NULL),
   fhStsMomresVsMom(NULL),

   fhEventNo(NULL),

   fOutputDir("")
{
}

CbmLitReconstructionQa::~CbmLitReconstructionQa()
{
   fHistoList->Delete();
   delete fHistoList;
}

InitStatus CbmLitReconstructionQa::Init()
{
   DetermineSetup();
   ReadDataBranches();
   CreateHistos(NULL);

   return kSUCCESS;
}

void CbmLitReconstructionQa::Exec(
   Option_t* opt)
{
   // Increase event counter
   fhEventNo->Fill(0.5);
   std::cout << "Event: " << fhEventNo->GetEntries() << std::endl;

   FillRichRingNofHits();
   ProcessHits();
   ProcessGlobalTracks();
   ProcessMcTracks();
   StsTracksQa();
   PrintEventStatistics();
   IncreaseCounters();
}

void CbmLitReconstructionQa::Finish()
{
   CalculateEfficiencyHistos();
   WriteToFile();
   PrintFinalStatistics(std::cout);
   Draw();
}

void CbmLitReconstructionQa::DetermineSetup()
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

void CbmLitReconstructionQa::SetDetectorPresence(
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

void CbmLitReconstructionQa::ReadDataBranches()
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
   }

   if (fIsSts) {
      fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
      if (NULL == fStsTracks) { Fatal("Init",": No StsTrack array!"); }
      fStsMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
      if (NULL == fStsMatches) { Fatal("Init",": No StsTrackMatch array!"); }
      fStsHits = (TClonesArray*) ioman->GetObject("StsHit");
      if (NULL == fStsHits) { Fatal("Init",": No StsHit array!"); }
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
      if (NULL == fMuchPixelHits && NULL == fMuchStrawHits) { Fatal("CbmLitReconstructionQa::Init", "No MuchPixelHit AND MuchStrawHit arrays!"); }
      fMuchMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
      if (NULL == fMuchMatches) { Fatal("Init","No MuchTrackMatch array!"); }
   }

   if (fIsTrd) {
      fTrdMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
      if (NULL == fTrdMatches) { Fatal("Init","No TrdTrackMatch array!"); }
      fTrdHits = (TClonesArray*) ioman->GetObject("TrdHit");
      if (NULL == fTrdHits) { Fatal("Init","No TrdHit array!"); }
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

void CbmLitReconstructionQa::ProcessHits()
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

void CbmLitReconstructionQa::FillRichRingNofHits()
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

void CbmLitReconstructionQa::ProcessGlobalTracks()
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
         tofMCId = tofPoint->GetTrackID();
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

void CbmLitReconstructionQa::ProcessMvd(
   Int_t stsId)
{
   CbmStsTrack* track = static_cast<CbmStsTrack*>(fStsTracks->At(stsId));
   Int_t nofHits = track->GetNMvdHits();
   fhMvdTrackHits[ALLHITS]->Fill(nofHits);

   CbmTrackMatch* stsTrackMatch = static_cast<CbmTrackMatch*>(fStsMatches->At(stsId));
   Int_t stsMcTrackId = stsTrackMatch->GetMCTrackId();

   Int_t nofTrueHits = 0, nofFakeHits = 0;
   for (Int_t iHit = 0; iHit < nofHits; iHit++) {
      Int_t hitId = track->GetMvdHitIndex(iHit);
      CbmMvdHitMatch* hitMatch = static_cast<CbmMvdHitMatch*>(fMvdHitMatches->At(hitId));
      Int_t pointId = hitMatch->GetPointId();
      CbmMvdPoint* point = static_cast<CbmMvdPoint*>(fMvdPoints->At(pointId));
      Int_t mcTrackId = point->GetTrackID();
      if (mcTrackId == stsMcTrackId) { // true hit
         nofTrueHits++;
      } else { // fake hit
         nofFakeHits++;
      }
   }
   fhMvdTrackHits[TRUEHITS]->Fill(nofTrueHits);
   fhMvdTrackHits[FAKEHITS]->Fill(nofFakeHits);
   if (nofHits != 0) {
      fhMvdTrackHits[TRUEALL]->Fill(Float_t(nofTrueHits) / Float_t(nofHits));
      fhMvdTrackHits[FAKEALL]->Fill(Float_t(nofFakeHits) / Float_t(nofHits));
   }
}

Bool_t CbmLitReconstructionQa::CheckTrackQuality(
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
      fhStsTrackHits[ALLHITS]->Fill(nofHits);
      fhStsTrackHits[TRUEHITS]->Fill(nofTrue);
      fhStsTrackHits[FAKEHITS]->Fill(nofFake + nofWrong);
      fhStsTrackHits[TRUEALL]->Fill(quali);
      fhStsTrackHits[FAKEALL]->Fill(fakequali);
   }
   if(detId == kTRD) {
      fhTrdTrackHits[ALLHITS]->Fill(nofHits);
      fhTrdTrackHits[TRUEHITS]->Fill(nofTrue);
      fhTrdTrackHits[FAKEHITS]->Fill(nofFake + nofWrong);
      fhTrdTrackHits[TRUEALL]->Fill(quali);
      fhTrdTrackHits[FAKEALL]->Fill(fakequali);
   }
   if(detId == kMUCH) {
      fhMuchTrackHits[ALLHITS]->Fill(nofHits);
      fhMuchTrackHits[TRUEHITS]->Fill(nofTrue);
      fhMuchTrackHits[FAKEHITS]->Fill(nofFake + nofWrong);
      fhMuchTrackHits[TRUEALL]->Fill(quali);
      fhMuchTrackHits[FAKEALL]->Fill(fakequali);
   }

   if (quali < fQuota) { return false; }
   return true;
}

Bool_t CbmLitReconstructionQa::CheckRingQuality(
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

   fhRichRingHits[ALLHITS]->Fill(nofHits);
   fhRichRingHits[TRUEHITS]->Fill(nofTrue);
   fhRichRingHits[FAKEHITS]->Fill(nofFake + nofWrong);
   fhRichRingHits[TRUEALL]->Fill(quali);
   fhRichRingHits[FAKEALL]->Fill(fakequali);

   if (quali < fQuotaRich) { return false; }
   return true;
}

void CbmLitReconstructionQa::ProcessMcTracks()
{
   Int_t nofMcTracks = fMCTracks->GetEntriesFast();
   for (Int_t iMCTrack = 0; iMCTrack < nofMcTracks; iMCTrack++) {
      CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iMCTrack);
      if (mcTrack == NULL) { continue; }

      // Check accepted tracks cutting on minimal number of MC points
      Int_t nofPointsSts = mcTrack->GetNPoints(kSTS);
      Int_t nofPointsTrd = mcTrack->GetNPoints(kTRD);
      Int_t nofPointsMuch = mcTrack->GetNPoints(kMUCH);
      Int_t nofPointsTof = mcTrack->GetNPoints(kTOF);
      Int_t nofHitsRich = fNofHitsInRingMap[iMCTrack];

      // Check local tracks
      Bool_t isStsOk = nofPointsSts >= fMinNofPointsSts && fIsSts;
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

      // Check accepted tracks cutting on minimal number of MC points
      // acceptance: STS tracks only
      if (isStsOk) {
         // momentum dependence histograms
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, fhStsMom, mcTrack->GetP());
         // number of points dependence histograms
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, fhStsNp, nofPointsSts);
         // number of points dependence histograms
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, fhStsAngle, angle);
      }
      // acceptance: STS+TRD(MUCH)
      if (isStsOk && isRecOk) {
    	 // std::cout << "isStsOk && isRecOk" << std::endl;
         // momentum dependence histograms
    	 // STS normalized to STS+TRD(MUCH)
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, fhStsMomNormHalfGlobal, mcTrack->GetP());
    	 // STS+TRD(MUCH)
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, fhHalfGlobalMom, mcTrack->GetP());
      }
      // acceptance: STS+TRD(MUCH)+TOF
      if (isStsOk && isRecOk && isTofOk) {
    	 // std::cout << "isStsOk && isRecOk && isTofOk" << std::endl;
         // momentum dependence histograms
    	 // STS normalized to STS+TRD(MUCH)+TOF
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcStsMap, fhStsMomNormGlobal, mcTrack->GetP());
         // STS+TRD(MUCH) normalized to STS+TRD(MUCH)+TOF
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcHalfGlobalMap, fhHalfGlobalMomNormGlobal, mcTrack->GetP());
    	 // STS+TRD(MUCH)+TOF
         FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcGlobalMap, fhGlobalMom, mcTrack->GetP());
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
         }
      } else { // for STS+TOF setup
         if (isStsOk && isTofOk) {
            // momentum dependence histograms
            FillGlobalReconstructionHistos(mcTrack, iMCTrack, fMcGlobalMap, fhTofMom, mcTrack->GetP());
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
    	  // STS normalized to STS+RICH
          FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsMap, fhStsMomNormStsRich, mcTrack->GetP());
    	  // STS+RICH momentum dependence histograms
    	  FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsRichMap, fhStsRichMom, mcTrack->GetP());

      }
      // acceptance: STS+RICH+TRD
      if (isStsOk && isRichOk && isTrdOk) {
    	 // STS normalized to STS+RICH+TRD
         FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsMap, fhStsMomNormStsRichTrd, mcTrack->GetP());
         // STS+RICH normalized to STS+RICH+TRD
         FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsRichMap, fhStsRichMomNormStsRichTrd, mcTrack->GetP());
         // STS+RICH+TRD normalized to STS+RICH+TRD
         FillGlobalReconstructionHistosRich(mcTrack, iMCTrack, fMcStsRichTrdMap, fhStsRichTrdMom, mcTrack->GetP());
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
      }
   } // Loop over MCTracks
}

void CbmLitReconstructionQa::FillGlobalReconstructionHistos(
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
   hist[ALL][ACC]->Fill(par);
   if (isPrim) { hist[PRIM][ACC]->Fill(par); }
   if (isRef) { hist[REF][ACC]->Fill(par); }
   if (!isPrim) { hist[SEC][ACC]->Fill(par); }
   if (isPrim && isMuon) { hist[MU][ACC]->Fill(par); }
   if (isPrim && isElectron) { hist[EL][ACC]->Fill(par); }

   // Fill histograms for reconstructed tracks which are accepted
   if (mcMap.find(mcId) != mcMap.end() ) {
      hist[ALL][REC]->Fill(par);
      if (isPrim) { hist[PRIM][REC]->Fill(par); }
      if (isRef) { hist[REF][REC]->Fill(par); }
      if (!isPrim) { hist[SEC][REC]->Fill(par); }
      if (isPrim && isMuon) { hist[MU][REC]->Fill(par); }
      if (isPrim && isElectron) { hist[EL][REC]->Fill(par); }
   }
}

void CbmLitReconstructionQa::FillGlobalReconstructionHistosRich(
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
   hist[RICHALL][ACC]->Fill(par);
   if (isRef) { hist[RICHALLREF][ACC]->Fill(par); }
   if (isElectron && isPrim) { hist[RICHEL][ACC]->Fill(par); }
   if (isElectron && isRef) { hist[RICHELREF][ACC]->Fill(par); }
   if (isPion) { hist[RICHPI][ACC]->Fill(par); }
   if (isPion && isRef) { hist[RICHPIREF][ACC]->Fill(par); }

   // Fill histograms for reconstructed rings and tracks which are accepted
   if (mcMap.find(mcId) != mcMap.end() ) {
	   hist[RICHALL][REC]->Fill(par);
	   if (isRef) { hist[RICHALLREF][REC]->Fill(par); }
	   if (isElectron && isPrim) { hist[RICHEL][REC]->Fill(par); }
	   if (isElectron && isRef) { hist[RICHELREF][REC]->Fill(par); }
	   if (isPion) { hist[RICHPI][REC]->Fill(par); }
	   if (isPion && isRef) { hist[RICHPIREF][REC]->Fill(par); }
   }
}

void CbmLitReconstructionQa::StsTracksQa()
{
   Int_t nSts = fStsTracks->GetEntriesFast();
   for (Int_t i = 0; i < nSts; i++) {
      CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(i);
      if (!stsTrack) continue;
      CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*) fStsMatches->At(i);
      if (!stsTrackMatch) continue;
      Int_t mcIdSts = stsTrackMatch->GetMCTrackId();
      if (mcIdSts < 0) continue;
      CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(mcIdSts);

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
      //fh_count_mom_vs_mom_signal->Fill(momMC.Mag());
      //fh_mean_mom_vs_mom_signal->Fill(momMC.Mag(), dpp);
   }
}

void CbmLitReconstructionQa::CreateEffHisto(
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
		cat[ALL] = "All"; cat[REF] = "Ref"; cat[PRIM] = "Prim";
		cat[SEC] = "Sec"; cat[MU] = "Muon"; cat[EL] = "Electron";
	} else if (opt == "rich"){
		cat[RICHALL] = "All"; cat[RICHALLREF] = "AllRef"; cat[RICHEL] = "Electron";
		cat[RICHELREF] = "ElectronRef"; cat[RICHPI] = "Pion"; cat[RICHPIREF] = "PionRef";
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

void CbmLitReconstructionQa::CreateHistos(
		TFile* file)
{
   // Histogram list
   fHistoList = new TList();

   // Number of points distributions
   Double_t minNofPoints =  0.;
   Double_t maxNofPoints = 100.;
   Int_t nBinsNofPoints = 100;

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

   //Create histograms for ghost tracks
   if (file == NULL){
	   fhStsGhostNh = new TH1F("hStsGhostNh", "STS: ghost tracks", nBinsNofPoints, minNofPoints, maxNofPoints);
	   fhRecGhostNh = new TH1F("hRecGhostNh", "TRD(MUCH): ghost tracks", nBinsNofPoints, minNofPoints, maxNofPoints);
	   fhRichGhostNh = new TH1F("hRichGhostNh", "RICH: ghost rings", nBinsNofPoints, minNofPoints, maxNofPoints);
   } else {
	   fhStsGhostNh = (TH1F*)file->Get("hStsGhostNh");
	   fhRecGhostNh = (TH1F*)file->Get("hRecGhostNh");
	   fhRichGhostNh = (TH1F*)file->Get("hRichGhostNh");
   }
   fHistoList->Add(fhStsGhostNh);
   fHistoList->Add(fhRecGhostNh);
   fHistoList->Add(fhRichGhostNh);

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
	   fhNofGlobalTracks = new TH1F("hNofGlobalTracks","hNofGlobalTracks", 100, 1., 1.);
	   fhNofStsTracks = new TH1F("hNofStsTracks","hNofStsTracks", 100, 1., 1.);
	   fhNofTrdTracks = new TH1F("hNofTrdTracks","hNofTrdTracks", 100, 1., 1.);
	   fhNofMuchTracks = new TH1F("hNofMuchTracks","hNofMuchTracks", 100, 1., 1.);
	   fhNofRichRings = new TH1F("hNofRichRings","hNofRichRings", 100, 1., 1.);
	   fhNofRichProjections = new TH1F("hNofRichProjections","hNofRichProjections", 100, 1., 1.);
	   fhNofMvdHits = new TH1F("hNofMvdHits","hNofMvdHits", 100, 1., 1.);
	   fhNofStsHits = new TH1F("hNofStsHits","hNofStsHits", 100, 1., 1.);
	   fhNofRichHits = new TH1F("hNofRichHits","hNofRichHits", 100, 1., 1.);
	   fhNofTrdHits = new TH1F("hNofTrdHits","hNofTrdHits", 100, 1., 1.);
	   fhNofMuchPixelHits = new TH1F("hNofMuchPixelHits","hNofMuchPixelHits", 100, 1., 1.);
	   fhNofMuchStrawHits = new TH1F("hNofMuchStrawHits","hNofMuchStrawHits", 100, 1., 1.);
	   fhNofTofHits = new TH1F("hNofTofHits","hNofTofHits", 100, 1., 1.);
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
   fHistoList->Add(fhStsChiprim);
   fHistoList->Add(fhStsMomresVsMom);

   // Histogram store number of events
   if (file == NULL){
	   fhEventNo = new TH1F("hEventNo","hEventNo", 1, 0, 1.);
   } else {
	   fhEventNo = (TH1F*)file->Get("hEventNo");
   }
   fHistoList->Add(fhEventNo);
}

void CbmLitReconstructionQa::DivideHistos(
   TH1* histo1,
   TH1* histo2,
   TH1* histo3)
{
   histo1->Sumw2();
   histo2->Sumw2();
   histo3->Divide(histo1, histo2, 1, 1, "");
}

void CbmLitReconstructionQa::CalculateEfficiencyHistos()
{
   // Divide histograms for efficiency calculation
   for (Int_t i = 0; i < fNofCategories; i++) {
      DivideHistos(fhStsMom[i][REC], fhStsMom[i][ACC], fhStsMom[i][EFF]);
      DivideHistos(fhStsMomNormHalfGlobal[i][REC], fhStsMomNormHalfGlobal[i][ACC], fhStsMomNormHalfGlobal[i][EFF]);
      DivideHistos(fhStsMomNormGlobal[i][REC], fhStsMomNormGlobal[i][ACC], fhStsMomNormGlobal[i][EFF]);
      DivideHistos(fhStsNp[i][REC], fhStsNp[i][ACC], fhStsNp[i][EFF]);
      DivideHistos(fhStsAngle[i][REC], fhStsAngle[i][ACC], fhStsAngle[i][EFF]);
      DivideHistos(fhHalfGlobalMom[i][REC], fhHalfGlobalMom[i][ACC], fhHalfGlobalMom[i][EFF]);
      DivideHistos(fhHalfGlobalMomNormGlobal[i][REC], fhHalfGlobalMomNormGlobal[i][ACC], fhHalfGlobalMomNormGlobal[i][EFF]);
      DivideHistos(fhGlobalMom[i][REC], fhGlobalMom[i][ACC], fhGlobalMom[i][EFF]);
      DivideHistos(fhRecMom[i][REC], fhRecMom[i][ACC], fhRecMom[i][EFF]);
      DivideHistos(fhRecNp[i][REC], fhRecNp[i][ACC], fhRecNp[i][EFF]);
      DivideHistos(fhRecAngle[i][REC], fhRecAngle[i][ACC], fhRecAngle[i][EFF]);
      DivideHistos(fhTofMom[i][REC], fhTofMom[i][ACC], fhTofMom[i][EFF]);
      DivideHistos(fhRichMom[i][REC], fhRichMom[i][ACC], fhRichMom[i][EFF]);
      DivideHistos(fhRichNh[i][REC], fhRichNh[i][ACC], fhRichNh[i][EFF]);

      DivideHistos(fhStsMomNormStsRich[i][REC], fhStsMomNormStsRich[i][ACC], fhStsMomNormStsRich[i][EFF]);
      DivideHistos(fhStsRichMom[i][REC], fhStsRichMom[i][ACC], fhStsRichMom[i][EFF]);

      DivideHistos(fhStsMomNormStsRichTrd[i][REC], fhStsMomNormStsRichTrd[i][ACC], fhStsMomNormStsRichTrd[i][EFF]);
      DivideHistos(fhStsRichMomNormStsRichTrd[i][REC], fhStsRichMomNormStsRichTrd[i][ACC], fhStsRichMomNormStsRichTrd[i][EFF]);
      DivideHistos(fhStsRichTrdMom[i][REC], fhStsRichTrdMom[i][ACC], fhStsRichTrdMom[i][EFF]);

      DivideHistos(fhStsMomNormStsRichTrdTof[i][REC], fhStsMomNormStsRichTrdTof[i][ACC], fhStsMomNormStsRichTrdTof[i][EFF]);
      DivideHistos(fhStsRichMomNormStsRichTrdTof[i][REC], fhStsRichMomNormStsRichTrdTof[i][ACC], fhStsRichMomNormStsRichTrdTof[i][EFF]);
      DivideHistos(fhStsRichTrdMomNormStsRichTrdTof[i][REC], fhStsRichTrdMomNormStsRichTrdTof[i][ACC], fhStsRichTrdMomNormStsRichTrdTof[i][EFF]);
      DivideHistos(fhStsRichTrdTofMom[i][REC], fhStsRichTrdTofMom[i][ACC], fhStsRichTrdTofMom[i][EFF]);
   }
   Int_t nofEvents = fhEventNo->GetEntries();
   fhMvdNofHitsInStation->Scale(1./nofEvents);
   fhStsNofHitsInStation->Scale(1./nofEvents);
   fhTrdNofHitsInStation->Scale(1./nofEvents);
   fhMuchNofHitsInStation->Scale(1./nofEvents);
   fhTofNofHitsInStation->Scale(1./nofEvents);
}

void CbmLitReconstructionQa::WriteToFile()
{
   TIter next(fHistoList);
   while ( TH1* histo = ((TH1*)next()) ) { histo->Write(); }
}

void CbmLitReconstructionQa::IncreaseCounters()
{
   fhNofGlobalTracks->Fill(fGlobalTracks->GetEntriesFast());
   if (fIsSts) {
	   fhNofStsTracks->Fill(fStsMatches->GetEntriesFast());
	   fhNofStsHits->Fill(fStsHits->GetEntriesFast());
   }
   if (fIsRich) {
	   fhNofRichRings->Fill(fRichRings->GetEntriesFast());
	   fhNofRichHits->Fill(fRichHits->GetEntriesFast());
	   fhNofRichProjections->Fill(fRichProjections->GetEntriesFast());
   }
   if (fIsTrd) {
	   fhNofTrdTracks->Fill(fTrdMatches->GetEntriesFast());
	   fhNofTrdHits->Fill(fTrdHits->GetEntriesFast());
   }
   if (fIsMuch) {
	   fhNofMuchTracks->Fill(fMuchMatches->GetEntriesFast());
	   fhNofMuchPixelHits->Fill(fMuchPixelHits->GetEntriesFast());
	   fhNofMuchStrawHits->Fill(fMuchStrawHits->GetEntriesFast());
   }
   if (fIsTof) { fhNofTofHits->Fill(fTofHits->GetEntriesFast()); }
}

std::string CbmLitReconstructionQa::RecDetector()
{
   std::string recDet = "";
   if (fIsTrd && !fIsMuch) { recDet = "TRD"; }
   else if (fIsMuch && !fIsTrd) { recDet = "MUCH"; }
   else if (fIsMuch && fIsTrd) { recDet = "MUCH+TRD"; }
   return recDet;
}

void CbmLitReconstructionQa::PrintEventStatistics()
{
   std::cout << "Number of hits: ";
   if (fIsMvd) { std::cout << "MVD=" << fMvdHits->GetEntriesFast(); }
   if (fIsSts) { std::cout << " STS=" << fStsHits->GetEntriesFast(); }
   if (fIsRich) { std::cout << " RICH=" << fRichHits->GetEntriesFast(); }
   if (fIsTrd) { std::cout << " TRD=" << fTrdHits->GetEntriesFast(); }
   if (fIsMuch && fMuchPixelHits) { std::cout << " MUCH(pixel)=" << fMuchPixelHits->GetEntriesFast(); }
   if (fIsMuch && fMuchStrawHits) { std::cout << " MUCH(straw)=" << fMuchStrawHits->GetEntriesFast(); }
   if (fIsTof) { std::cout << " TOF=" << fTofHits->GetEntriesFast(); }
   std::cout << std::endl;

   std::cout << "Number of reconstructed tracks: global=" << fGlobalTracks->GetEntriesFast();
   if (fIsSts) { std::cout << " STS=" << fStsMatches->GetEntriesFast(); }
   if (fIsTrd) { std::cout << " TRD=" << fTrdMatches->GetEntriesFast(); }
   if (fIsMuch) { std::cout << " MUCH=" << fMuchMatches->GetEntriesFast(); }
   std::cout << std::endl;

   if (fIsRich) {
	   std::cout << "RICH: nof rings=" << fRichRings->GetEntriesFast()
			   << " nof projections=" << fRichProjections->GetEntriesFast()
			   << std::endl;
   }

   std::string det = RecDetector();
   Int_t w = 17;

   std::cout << std::setfill('-') << std::setw(w) << "-" << std::setw(w)
	   << "all" << std::setw(w) << "reference" << std::setw(w) << "signal"<< std::endl;

   std::cout << std::setfill(' ');
   std::cout << std::left << std::setw(w) << "STS" << EventEfficiencyStatisticsToString(fhStsMom, "event");
   std::cout << std::setw(w) << det << EventEfficiencyStatisticsToString(fhRecMom, "event");
   std::cout << std::setw(w) << "TOF matching" << EventEfficiencyStatisticsToString(fhTofMom, "event");

   std::string s = "STS+" + det;
   std::cout << std::setw(w) << s << EventEfficiencyStatisticsToString(fhHalfGlobalMom, "event");
   std::string s2 = s + "+TOF";
   std::cout << std::setw(w) << s2 << EventEfficiencyStatisticsToString(fhGlobalMom, "event");

   std::cout << std::right;
   std::cout << std::setfill('-') << std::setw(w) << "-" << std::setw(w)
	   << "all" << std::setw(w) << "electron"<< std::setw(w) << "electron ref" << std::endl;

   std::cout << std::setfill(' ') << std::left;
   std::cout << std::setw(w) << "RICH" << EventEfficiencyStatisticsRichToString(fhRichMom, "event");
   std::cout << std::setw(w) << "STS+RICH" << EventEfficiencyStatisticsRichToString(fhStsRichMom, "event");
   std::cout << std::setw(w) << "STS+RICH+TRD" << EventEfficiencyStatisticsRichToString(fhStsRichTrdMom, "event");
   std::cout << std::setw(w) << "STS+RICH+TRD+TOF" << EventEfficiencyStatisticsRichToString(fhStsRichTrdTofMom, "event");

   PrintGhostStatistics(std::cout);
}

void CbmLitReconstructionQa::PrintFinalStatistics(
   std::ostream& out)
{
   out << "-I- CbmLitReconstructionQa final statistics:" << std::endl;
   out << "Number of events: " << (Int_t)fhEventNo->GetEntries() << std::endl;
   out << "Number of hits: ";
   if (fIsMvd) { out << "MVD=" << fhNofMvdHits->GetMean(); }
   if (fIsSts) { out << " STS=" << fhNofStsHits->GetMean(); }
   if (fIsRich) { out << " RICH=" << fhNofRichHits->GetMean(); }
   if (fIsTrd) { out << " TRD=" << fhNofTrdHits->GetMean(); }
   if (fIsMuch && fMuchPixelHits) { out << " MUCH(pixel)=" << fhNofMuchPixelHits->GetMean(); }
   if (fIsMuch && fMuchStrawHits) { out << " MUCH(straw)=" << fhNofMuchStrawHits->GetMean(); }
   if (fIsTof) { out << " TOF=" << fhNofTofHits->GetMean(); }
   out << std::endl;

   out << "Number of reconstructed tracks per event: ";
   out << "global=" << fhNofGlobalTracks->GetMean();
   if (fIsSts) { out << "  STS=" << fhNofStsTracks->GetMean(); }
   if (fIsTrd) { out << "  TRD=" << fhNofTrdTracks->GetMean(); }
   if (fIsMuch) { out << "  MUCH=" << fhNofMuchTracks->GetMean(); }
   out << std::endl;

   if (fIsRich) {
	   out << "RICH: nof rings=" << fhNofRichRings->GetMean()
			   << " nof projections=" << fhNofRichProjections->GetMean() << std::endl;
   }

   std::string det = RecDetector();
   Int_t w = 17;
   out << std::setfill('_') << std::setw(7*17) << "_"<< std::endl;
   out << std::setfill(' ') << std::setw(w) << " " << std::setw(w)
	   << "all" << std::setw(w) << "reference"
	   << std::setw(w) << "primary"<< std::setw(w) << "secondary"
	   << std::setw(w) << "electron"<< std::setw(w) << "muon" << std::endl;
   out << std::setfill('_') << std::setw(7*17) << "_"<< std::endl;

   out << std::setfill(' ');
   out << std::left << std::setw(w) << "STS" << EventEfficiencyStatisticsToString(fhStsMom, "final");
   out << std::setw(w) << det << EventEfficiencyStatisticsToString(fhRecMom, "final");
   out << std::setw(w) << "TOF matching" << EventEfficiencyStatisticsToString(fhTofMom, "final");

   out << std::setfill('-') << std::left;
   out << std::setw(7*17) << "Normalization STS+TRD(MUCH)" << std::endl;
   out << std::setfill(' ') << std::left;
   out << std::setw(w) << "STS" << EventEfficiencyStatisticsToString(fhStsMomNormHalfGlobal, "final");
   std::string s = "STS+" + det;
   out << std::setw(w) << s << EventEfficiencyStatisticsToString(fhHalfGlobalMom, "final");

   out << std::setfill('-') << std::left;
   out << std::setw(7*17) << "Normalization STS+TRD(MUCH)+TOF" << std::endl;
   out << std::setfill(' ') << std::left;
   out << std::setw(w) << "STS" << EventEfficiencyStatisticsToString(fhStsMomNormGlobal, "final");
   out << std::setw(w) << s << EventEfficiencyStatisticsToString(fhHalfGlobalMomNormGlobal, "final");
   std::string s2 = s + "+TOF";
   out << std::setw(w) << s2 << EventEfficiencyStatisticsToString(fhGlobalMom, "final");
   out << std::setfill('_') << std::setw(7*17) << "_"<< std::endl;

   out << std::right;
   out << std::setfill('_') << std::setw(7*17) << "_"<< std::endl;
   out << std::setfill(' ') << std::setw(w) << " " << std::setw(w)
	   << "all" << std::setw(w) << "all reference"
	   << std::setw(w) << "electron"<< std::setw(w) << "electron ref"
	   << std::setw(w) << "pion"<< std::setw(w) << "pion ref" << std::endl;
   out << std::setfill('_') << std::setw(7*17) << "_"<< std::endl;

   out << std::setfill(' ') << std::left;
   out << std::setw(w) << "RICH" << EventEfficiencyStatisticsRichToString(fhRichMom, "final");

   out << std::setfill('-') << std::left;
   out << std::setw(7*17) << "Normalization STS+RICH " << std::endl;
   out << std::setfill(' ') << std::left;
   out << std::setw(w) << "STS" << EventEfficiencyStatisticsRichToString(fhStsMomNormStsRich, "final");
   out << std::setw(w) << "STS+RICH" << EventEfficiencyStatisticsRichToString(fhStsRichMom, "final");

   out << std::setfill('-') << std::left;
   out << std::setw(7*17) << "Normalization STS+RICH+TRD " << std::endl;
   out << std::setfill(' ') << std::left;
   out << std::setw(w) << "STS" << EventEfficiencyStatisticsRichToString(fhStsMomNormStsRichTrd, "final");
   out << std::setw(w) << "STS+RICH" << EventEfficiencyStatisticsRichToString(fhStsRichMomNormStsRichTrd, "final");
   out << std::setw(w) << "STS+RICH+TRD" << EventEfficiencyStatisticsRichToString(fhStsRichTrdMom, "final");

   out << std::setfill('-') << std::left;
   out << std::setw(7*17) << "Normalization STS+RICH+TRD+TOF " << std::endl;
   out << std::setfill(' ') << std::left;
   out << std::setw(w) << "STS" << EventEfficiencyStatisticsRichToString(fhStsMomNormStsRichTrdTof, "final");
   out << std::setw(w) << "STS+RICH" << EventEfficiencyStatisticsRichToString(fhStsRichMomNormStsRichTrdTof, "final");
   out << std::setw(w) << "STS+RICH+TRD" << EventEfficiencyStatisticsRichToString(fhStsRichTrdMomNormStsRichTrdTof, "final");
   out << std::setw(w) << "STS+RICH+TRD+TOF" << EventEfficiencyStatisticsRichToString(fhStsRichTrdTofMom, "final");
   out << std::setfill('_') << std::setw(7*17) << "_"<< std::endl;

   PrintGhostStatistics(out);

   out << "Chi2 to primary vertex: mean = " << fhStsChiprim->GetMean()
         << " RMS = " << fhStsChiprim->GetRMS() << std::endl;
   out << "Momentum resolution: mean = " << fhStsMomresVsMom->ProjectionY()->GetMean()
         << " RMS = " << fhStsMomresVsMom->ProjectionY()->GetRMS() << std::endl;

   out << "Polar angle efficiency:" << std::endl;
   out << "STS:" << std::endl;
   out << PolarAngleEfficiency(fhStsAngle);
   out << det << ":" << std::endl;
   out << PolarAngleEfficiency(fhRecAngle);

   out << std::setfill('-') << std::setw(7*17) << std::endl;
}

std::string CbmLitReconstructionQa::EventEfficiencyStatisticsToString(
   const std::vector<std::vector<TH1F*> >& hist,
   const std::string& opt)
{
   Double_t allEff = 0., refEff = 0., primEff = 0., secEff = 0., muEff = 0., elEff = 0.;
   Double_t allRec = hist[ALL][REC]->GetEntries();
   Double_t allAcc = hist[ALL][ACC]->GetEntries();
   if (allAcc != 0.) { allEff = 100.*allRec / allAcc; }
   Double_t refRec = hist[REF][REC]->GetEntries();
   Double_t refAcc = hist[REF][ACC]->GetEntries();
   if (refAcc != 0.) { refEff = 100.*refRec / refAcc; }
   Double_t primRec = hist[PRIM][REC]->GetEntries();
   Double_t primAcc = hist[PRIM][ACC]->GetEntries();
   if (primAcc != 0.) { primEff = 100.*primRec / primAcc; }
   Double_t secRec = hist[SEC][REC]->GetEntries();
   Double_t secAcc = hist[SEC][ACC]->GetEntries();
   if (secAcc != 0.) { secEff = 100.*secRec / secAcc; }
   Double_t muRec = hist[MU][REC]->GetEntries();
   Double_t muAcc = hist[MU][ACC]->GetEntries();
   if (muAcc != 0.) { muEff = 100.*muRec / muAcc; }
   Double_t elRec = hist[EL][REC]->GetEntries();
   Double_t elAcc = hist[EL][ACC]->GetEntries();
   if (elAcc != 0.) { elEff = 100.*elRec / elAcc; }

   std::stringstream ss, ss1, ss2, ss3, ss4, ss5, ss6;
   ss1.precision(3);
   ss2.precision(3);
   ss3.precision(3);
   ss4.precision(3);
   ss5.precision(3);
   ss6.precision(3);
   Int_t w = 17;

   Int_t nofEvents = fhEventNo->GetEntries();
   ss1 << allEff << "("<< allRec/nofEvents << "/" << allAcc/nofEvents << ")";
   ss2 << refEff << "("<< refRec/nofEvents << "/" << refAcc/nofEvents << ")";
   ss3 << primEff << "(" << primRec/nofEvents << "/" << primAcc/nofEvents << ")";
   ss4 << secEff << "(" << secRec/nofEvents << "/" << secAcc/nofEvents << ")";
   ss5 << elEff << "(" << elRec/nofEvents << "/" << elAcc/nofEvents << ")";
   ss6 << muEff << "(" << muRec/nofEvents << "/" << muAcc/nofEvents << ")";

   if (opt == "final"){
	   ss << std::setw(w) << ss1.str() << std::setw(w) << ss2.str() <<
			   std::setw(w) << ss3.str() << std::setw(w) << ss4.str() <<
			   std::setw(w) << ss5.str() << std::setw(w) << ss6.str() << std::endl;
   } else if (opt == "event"){
	   ss << std::setw(w) << ss1.str() << std::setw(w) << ss2.str();
	   if (fIsElectronSetup){
		   ss << std::setw(w) << ss5.str() << std::endl;
	   }else {
		   ss << std::setw(w) << ss6.str() << std::endl;
	   }
   }

   return ss.str();
}

std::string CbmLitReconstructionQa::EventEfficiencyStatisticsRichToString(
   const std::vector<std::vector<TH1F*> >& hist,
   const std::string& opt)
{
   Double_t allEff = 0., allRefEff = 0., elEff = 0., elRefEff = 0., piEff = 0., piRefEff = 0.;
   Double_t allRec = hist[RICHALL][REC]->GetEntries();
   Double_t allAcc = hist[RICHALL][ACC]->GetEntries();
   if (allAcc != 0.) { allEff = 100.*allRec / allAcc; }

   Double_t allRefRec = hist[RICHALLREF][REC]->GetEntries();
   Double_t allRefAcc = hist[RICHALLREF][ACC]->GetEntries();
   if (allRefAcc != 0.) { allRefEff = 100.*allRefRec / allRefAcc; }

   Double_t elRec = hist[RICHEL][REC]->GetEntries();
   Double_t elAcc = hist[RICHEL][ACC]->GetEntries();
   if (elAcc != 0.) { elEff = 100.*elRec / elAcc; }

   Double_t elRefRec = hist[RICHELREF][REC]->GetEntries();
   Double_t elRefAcc = hist[RICHELREF][ACC]->GetEntries();
   if (elRefAcc != 0.) { elRefEff = 100.*elRefRec / elRefAcc; }

   Double_t piRec = hist[RICHPI][REC]->GetEntries();
   Double_t piAcc = hist[RICHPI][ACC]->GetEntries();
   if (piAcc != 0.) { piEff = 100.*piRec / piAcc; }

   Double_t piRefRec = hist[RICHPIREF][REC]->GetEntries();
   Double_t piRefAcc = hist[RICHPIREF][ACC]->GetEntries();
   if (piRefAcc != 0.) { piRefEff = 100.*piRefRec / piRefAcc; }

   std::stringstream ss, ss1, ss2, ss3, ss4, ss5, ss6;
   ss1.precision(3);
   ss2.precision(3);
   ss3.precision(3);
   ss4.precision(3);
   ss5.precision(3);
   ss6.precision(3);
   Int_t w = 17;

   Int_t nofEvents = fhEventNo->GetEntries();
   ss1 << allEff << "(" << allRec/nofEvents << "/" << allAcc/nofEvents << ")";
   ss2 << allRefEff << "(" << allRefRec/nofEvents << "/" << allRefAcc/nofEvents << ")";
   ss3 << elEff << "(" << elRec/nofEvents << "/" << elAcc/nofEvents << ")";
   ss4 << elRefEff << "(" << elRefRec/nofEvents << "/" << elRefAcc/nofEvents << ")";
   ss5 << piEff << "(" << piRec/nofEvents << "/" << piAcc/nofEvents << ")";
   ss6 << piRefEff << "(" << piRefRec/nofEvents << "/" << piRefAcc/nofEvents << ")";
   if (opt == "event"){
	   ss << std::setw(w) << ss1.str() << std::setw(w) << ss3.str()
			   << std::setw(w) << ss4.str() << std::endl;
   } else if (opt == "final") {
	   ss << std::setw(w) << ss1.str() << std::setw(w) << ss2.str() <<
			   std::setw(w) << ss3.str() << std::setw(w) << ss4.str() <<
			   std::setw(w) << ss5.str() << std::setw(w) << ss6.str() << std::endl;
   }

   return ss.str();
}

void CbmLitReconstructionQa::PrintGhostStatistics(
   std::ostream& out)
{
	Int_t nofEvents = fhEventNo->GetEntries();
	Double_t stsGhosts = fhStsGhostNh->GetEntries() / nofEvents;
	Double_t recGhosts = fhRecGhostNh->GetEntries() / nofEvents;
	Double_t richGhosts = fhRichGhostNh->GetEntries() / nofEvents;
	out.precision(3);
	out << "Ghosts per events:";
	if (fIsSts) { out << " STS=" << stsGhosts; }
	if (fIsMuch || fIsTrd) { out << " " << RecDetector() << "=" << recGhosts; }
	if (fIsRich) { out << " RICH=" << richGhosts; }
	out << std::endl;
}


std::string CbmLitReconstructionQa::PolarAngleEfficiency(
   const std::vector<std::vector<TH1F*> >& hist)
{
   Double_t step = (fMaxAngle - fMinAngle) / fNofBinsAngle;
   Double_t allEff[fNofBinsAngle], refEff[fNofBinsAngle];
   Double_t allRec[fNofBinsAngle], refRec[fNofBinsAngle];
   Double_t allAcc[fNofBinsAngle], refAcc[fNofBinsAngle];
   for(Int_t i = 0; i < fNofBinsAngle; i++) {
      allRec[i] = hist[ALL][REC]->GetBinContent(i+1);
      allAcc[i] = hist[ALL][ACC]->GetBinContent(i+1);
      if (allAcc[i] != 0.) { allEff[i] = allRec[i] / allAcc[i]; }
      refRec[i] = hist[REF][REC]->GetBinContent(i+1);
      refAcc[i] = hist[REF][ACC]->GetBinContent(i+1);
      if (refAcc[i] != 0.) { refEff[i] = refRec[i] / refAcc[i]; }
   }

   Int_t nofEvents = fhEventNo->GetEntries();
   std::stringstream ss;
   for (Int_t i = 0; i < fNofBinsAngle; i++) {
      Double_t angle0 = i*step;
      Double_t angle1 = angle0 + step;
      ss << "(" << angle0 << "-" << angle1 << ") "
         << "all=" << allEff[i] << "(" << allRec[i]/nofEvents << "/" << allAcc[i]/nofEvents << ")"
         << "  ref=" << refEff[i] << "(" << refRec[i]/nofEvents << "/" << refAcc[i]/nofEvents << ")" << std::endl;
   }
   return ss.str();
}

void CbmLitReconstructionQa::Draw()
{
   SetStyles();
   DrawEfficiencyHistos();
   DrawHitsHistos();
   DrawHitsStationHistos();
   DrawStsTracksQaHistos();
   std::ofstream fout(std::string(fOutputDir + "rec_qa.txt").c_str());
   PrintFinalStatistics(fout);
}

void CbmLitReconstructionQa::DrawEfficiencyHistos()
{
   std::string sname("STS");
   std::string rname;
   if (fIsMuch && !fIsTrd) { rname = "MUCH"; }
   else if (fIsTrd && !fIsMuch) { rname = "TRD"; }
   else if (fIsTrd && fIsMuch) { rname = "MUCH+TRD"; }
   std::string hgname(sname + "+" + rname);
   std::string gname = hgname + "+TOF";

   std::string signal = fIsMuch ? "muons" : "electrons";
   Int_t cat = fIsMuch ? MU : EL;

   // Draw global tracking efficiency STS+TRD(MUCH)+TOF for all tracks
   DrawEfficiency("rec_qa_global_efficiency_all", fhStsMomNormGlobal,
		   fhHalfGlobalMomNormGlobal, fhGlobalMom, fhGlobalMom, sname, hgname, gname, "", ALL, ALL, ALL, 0);
   // Draw global tracking efficiency STS+TRD(MUCH)+TOF for signal tracks
   DrawEfficiency("rec_qa_global_efficiency_signal", fhStsMomNormGlobal,
		   fhHalfGlobalMomNormGlobal, fhGlobalMom, fhGlobalMom, sname, hgname, gname, "", cat, cat, cat, 0);

   // Draw half global tracking efficiency STS+TRD(MUCH) for all tracks
   DrawEfficiency("rec_qa_half_global_efficiency_all", fhStsMomNormHalfGlobal,
		   fhHalfGlobalMom, fhGlobalMom, fhGlobalMom, sname, hgname, "", "", ALL, ALL, 0, 0);

   // Draw half global tracking efficiency STS+TRD(MUCH) for signal tracks
   DrawEfficiency("rec_qa_half_global_efficiency_signal", fhStsMomNormHalfGlobal,
		   fhHalfGlobalMom, fhGlobalMom, fhGlobalMom, sname, hgname, "", "", cat, cat, 0, 0);

   // Draw efficiency for STS
   DrawEfficiency("rec_qa_sts_efficiency", fhStsMom,
  		   fhStsMom, fhStsMom, fhStsMom, "STS: all", "STS: " + signal, "", "", ALL, cat, 0, 0);

   if (fIsTrd || fIsMuch) {
	   // Draw efficiency for TRD(MUCH)
	   DrawEfficiency("rec_qa_rec_efficiency", fhRecMom,
				 fhRecMom, fhRecMom, fhRecMom, rname + ": all", rname + ": " + signal, "", "", ALL, cat, 0, 0);
   }

   if (fIsTof) {
	   // Draw efficiency for TOF
	   DrawEfficiency("rec_qa_tof_efficiency", fhTofMom,
			   	 fhTofMom, fhTofMom, fhTofMom, "TOF: all", "TOF: " + signal, "", "", ALL, cat, 0, 0);
   }

   if (fIsRich) {
	   // Draw efficiency for RICH for all set
	   DrawEfficiency("rec_qa_rich_efficiency_all", fhRichMom, fhRichMom, fhRichMom, fhRichMom,
			   "RICH: all", "RICH: all ref", "", "", RICHALL, RICHALLREF, 0, 0);

	   // Draw efficiency for RICH for electron set
	   DrawEfficiency("rec_qa_rich_efficiency_electrons", fhRichMom, fhRichMom, fhRichMom, fhRichMom,
			   "RICH: electrons", "RICH: electrons ref", "", "", RICHEL, RICHELREF, 0, 0);

	   // Draw efficiency for RICH for pion set
	   DrawEfficiency("rec_qa_rich_efficiency_pions", fhRichMom, fhRichMom, fhRichMom, fhRichMom,
			   "RICH: pions", "RICH: pions ref", "", "", RICHPI, RICHPIREF, 0, 0);

	   // Draw efficiency for STS+RICH for electron set
	   DrawEfficiency("rec_qa_sts_rich_efficiency_electrons", fhStsMomNormStsRich,
			   fhStsRichMom, fhStsRichMom, fhStsRichMom,
			   "STS", "STS+RICH", "", "", RICHEL, RICHEL, 0, 0);

	   // Draw efficiency for STS+RICH+TRD for electron set
	   DrawEfficiency("rec_qa_sts_rich_trd_efficiency_electrons", fhStsMomNormStsRichTrd,
			   fhStsRichMomNormStsRichTrd, fhStsRichTrdMom, fhStsRichTrdMom,
			   "STS", "STS+RICH", "STS+RICH+TRD", "", RICHEL, RICHEL, RICHEL, 0);

	   // Draw efficiency for STS+RICH+TRD+TOF for electron set
	   DrawEfficiency("rec_qa_sts_rich_trd_tof_efficiency_electrons", fhStsMomNormStsRichTrdTof,
			   fhStsRichMomNormStsRichTrdTof, fhStsRichTrdMomNormStsRichTrdTof, fhStsRichTrdTofMom,
			   "STS", "STS+RICH", "STS+RICH+TRD", "STS+RICH+TRD+TOF", RICHEL, RICHEL, RICHEL, RICHEL);
   }
}

void CbmLitReconstructionQa::DrawEfficiency(
		const std::string& canvasName,
		const std::vector<std::vector<TH1F*> >& hist1,
		const std::vector<std::vector<TH1F*> >& hist2,
		const std::vector<std::vector<TH1F*> >& hist3,
		const std::vector<std::vector<TH1F*> >& hist4,
		const std::string& name1,
		const std::string& name2,
		const std::string& name3,
		const std::string& name4,
		Int_t category1,
		Int_t category2,
		Int_t category3,
		Int_t category4)
{
	TCanvas* canvas = new TCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
	canvas->SetGrid();
	canvas->cd();
	std::string hname1 = name1 + "(" + lit::ToString<Double_t>(CalcEfficiency(hist1[category1][REC], hist1[category1][ACC])) + ")";;
	std::string hname2 = name2 + "(" + lit::ToString<Double_t>(CalcEfficiency(hist2[category2][REC], hist2[category2][ACC])) + ")";;
	std::string hname3 = name3 + "(" + lit::ToString<Double_t>(CalcEfficiency(hist3[category3][REC], hist3[category3][ACC])) + ")";;
	std::string hname4 = name4 + "(" + lit::ToString<Double_t>(CalcEfficiency(hist4[category4][REC], hist4[category4][ACC])) + ")";;

	hist1[category1][EFF]->SetMinimum(0.);
	hist1[category1][EFF]->SetMaximum(1.);

	if (name3 != "" && name4 != ""){
		DrawHist1D(hist1[category1][EFF], hist2[category2][EFF], hist3[category3][EFF], hist4[category4][EFF],
						 "Efficiency", "Momentum [GeV/c]", "Efficiency", hname1, hname2, hname3, hname4,
						 false, false, true, 0.3,0.3,0.85,0.6);
	} else if (name3 != "") {
	  DrawHist1D(hist1[category1][EFF], hist2[category2][EFF], hist3[category3][EFF], NULL,
				 "Efficiency", "Momentum [GeV/c]", "Efficiency", hname1, hname2, hname3, "",
				 false, false, true, 0.3,0.3,0.85,0.6);
	} else {
	  DrawHist1D(hist1[category1][EFF], hist2[category2][EFF], NULL, NULL,
				 "Efficiency", "Momentum [GeV/c]", "Efficiency", hname1, hname2, hname3, "",
				 false, false, true, 0.3,0.3,0.85,0.6);
	}
	lit::SaveCanvasAsImage(canvas, fOutputDir);
}

Double_t CbmLitReconstructionQa::CalcEfficiency(
   TH1* histRec,
   TH1* histAcc)
{
   if (histAcc->GetEntries() == 0) { return 0.; }
   else { return Double_t(histRec->GetEntries()) / Double_t(histAcc->GetEntries()); }
}

void CbmLitReconstructionQa::DrawHitsHistos()
{
   if (fIsMvd) { DrawHitsHistos("rec_qa_mvd_hits", fhMvdTrackHits); }
   if (fIsSts) { DrawHitsHistos("rec_qa_sts_hits", fhStsTrackHits); }
   if (fIsTrd) { DrawHitsHistos("rec_qa_trd_hits", fhTrdTrackHits); }
   if (fIsMuch) { DrawHitsHistos("rec_qa_much_hits", fhMuchTrackHits); }
   if (fIsRich) { DrawHitsHistos("rec_qa_rich_ring_hits", fhRichRingHits); }
}

void CbmLitReconstructionQa::DrawHitsHistos(
   const std::string& canvasName,
   std::vector<TH1F*>& histos)
{
   TCanvas* c = new TCanvas(canvasName.c_str(), canvasName.c_str(), 1200, 600);
   c->Divide(2,1);
   c->SetGrid();

   c->cd(1);
   DrawHist1D(histos[0], histos[1], histos[2], NULL,
              "Number of hits", "Number of hits", "Counter",
              "all: " + lit::ToString<Double_t>(histos[0]->GetMean()),
              "true: " + lit::ToString<Double_t>(histos[1]->GetMean()),
              "fake: " + lit::ToString<Double_t>(histos[2]->GetMean()),  "",
              false, true, true, 0.25,0.99,0.55,0.75);

   c->cd(2);
   DrawHist1D(histos[3], histos[4], NULL, NULL,
              "Ratio", "Ratio", "Counter",
              "true/all: " + lit::ToString<Double_t>(histos[3]->GetMean()),
              "fake/all: " + lit::ToString<Double_t>(histos[4]->GetMean()),
              "", "",
              false, true, true, 0.25,0.99,0.55,0.75);

   lit::SaveCanvasAsImage(c, fOutputDir);
}

void CbmLitReconstructionQa::DrawHitsStationHisto(
		const std::string& name,
		TH1F* hist)
{
	TCanvas* canvas = new TCanvas(name.c_str(), name.c_str(), 1200, 600);
	DrawHist1D(hist, "Station number", "Number of hits",
			LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
			LIT_MARKER_STYLE1, false, false, "HIST TEXT0");
	lit::SaveCanvasAsImage(canvas, fOutputDir);
}

void CbmLitReconstructionQa::DrawHitsStationHistos()
{
	if (fIsMvd) {DrawHitsStationHisto("rec_qa_mvd_hits_station", fhMvdNofHitsInStation);}
	if (fIsSts) {DrawHitsStationHisto("rec_qa_sts_hits_station", fhStsNofHitsInStation);}
	if (fIsTrd) {DrawHitsStationHisto("rec_qa_trd_hits_station", fhTrdNofHitsInStation);}
	if (fIsMuch) {DrawHitsStationHisto("rec_qa_much_hits_station", fhMuchNofHitsInStation);}
	if (fIsTof) {DrawHitsStationHisto("rec_qa_tof_hits_station", fhTofNofHitsInStation);}
}

void CbmLitReconstructionQa::DrawStsTracksQaHistos()
{
   TCanvas* canvas1 = new TCanvas("rec_qa_sts_tracks_qa_chi2Vertex", "rec_qa_sts_tracks_qa_chi2Vertex", 600, 500);
   DrawHist1D(fhStsChiprim, "Chi2Vertex", "Counter",
            LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
            LIT_MARKER_STYLE1, false, false, "HIST TEXT0");
   lit::SaveCanvasAsImage(canvas1, fOutputDir);

   TCanvas* canvas2 = new TCanvas("rec_qa_sts_tracks_qa_momres", "rec_qa_sts_tracks_qa_momres", 600, 500);
   DrawHist1D(fhStsMomresVsMom->ProjectionY(), "dP [%]", "Counter",
            LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
            LIT_MARKER_STYLE1, false, false, "HIST TEXT0");
   lit::SaveCanvasAsImage(canvas2, fOutputDir);
}

void CbmLitReconstructionQa::DrawHistosFromFile(const std::string& fileName)
{
	TFile* file = new TFile(fileName.c_str());
    //DetermineSetup();
	CreateHistos(file);
	CalculateEfficiencyHistos();
	//WriteToFile();
	PrintFinalStatistics(std::cout);
	Draw();
}

ClassImp(CbmLitReconstructionQa);
