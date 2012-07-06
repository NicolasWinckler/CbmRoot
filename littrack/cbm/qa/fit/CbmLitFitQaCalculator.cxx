/**
 * \file CbmLitFitQaCalculator.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */

#include "CbmLitFitQaCalculator.h"
#include "../mc/CbmLitMCTrackCreator.h"
#include "../base/CbmHistManager.h"
#include "CbmGlobalTrack.h"
#include "CbmTrackMatch.h"
#include "CbmTrack.h"
#include "CbmStsTrack.h"
#include "CbmMvdHit.h"
#include "CbmStsHit.h"
#include "CbmTrdDetectorId.h"
#include "CbmMuchGeoScheme.h"
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TH1.h"

CbmLitFitQaCalculator::CbmLitFitQaCalculator(
		CbmHistManager* histManager):
		fGlobalTracks(NULL),
		fStsTracks(NULL),
		fStsTrackMatches(NULL),
		fStsHits(NULL),
		fMvdHits(NULL),
		fTrdTracks(NULL),
		fTrdTrackMatches(NULL),
		fTrdHits(NULL),
		fMuchTracks(NULL),
		fMuchTrackMatches(NULL),
		fMuchPixelHits(NULL),
		fMuchStripHits(NULL),
		fMvdMinNofHits(0),
		fStsMinNofHits(0),
		fTrdMinNofHits(0),
		fMuchMinNofHits(0),
		fQuota(0.7),
		fHM(histManager),
		fMCTrackCreator(NULL)
{

}

CbmLitFitQaCalculator::~CbmLitFitQaCalculator()
{

}

void CbmLitFitQaCalculator::Init()
{
	ReadDataBranches();
	fMCTrackCreator = CbmLitMCTrackCreator::Instance();
}

void CbmLitFitQaCalculator::Exec()
{
	static Int_t nofEvents = 0;
	std::cout << "CbmLitFitQaCalculator::Exec: event=" << nofEvents++ << std::endl;
	fMCTrackCreator->Create();
	ProcessGlobalTracks();
}

void CbmLitFitQaCalculator::Finish()
{
	fHM->ShrinkEmptyBinsByPattern("htf_.+_WrongCov_.+");
}

void CbmLitFitQaCalculator::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   assert(ioman != NULL);

   fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
   fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
   fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
   fStsHits = (TClonesArray*) ioman->GetObject("StsHit");
   fMvdHits = (TClonesArray*) ioman->GetObject("MvdHit");
   fMuchTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
   fMuchTrackMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
   fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
   fMuchStripHits = (TClonesArray*) ioman->GetObject("MuchStripHit");
   fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
   fTrdTrackMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
   fTrdHits = (TClonesArray*) ioman->GetObject("TrdHit");
}

void CbmLitFitQaCalculator::ProcessGlobalTracks()
{
   Int_t nofGlobalTracks = fGlobalTracks->GetEntriesFast();
   for (Int_t iTrack = 0; iTrack < nofGlobalTracks; iTrack++) {
      const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(fGlobalTracks->At(iTrack));
      ProcessStsTrack(globalTrack->GetStsTrackIndex());
      ProcessTrdTrack(globalTrack->GetTrdTrackIndex());
      ProcessMuchTrack(globalTrack->GetMuchTrackIndex());
   }
}

void CbmLitFitQaCalculator::ProcessStsTrack(
      Int_t trackId)
{
   if (NULL == fStsTracks || NULL == fStsTrackMatches || trackId < 0) return;

   CbmTrackMatch* match = static_cast<CbmTrackMatch*>(fStsTrackMatches->At(trackId));
   Int_t mcId = match->GetMCTrackId();
   if (mcId < 0) return; // Ghost or fake track

   // Check correctness of reconstructed track
   Int_t allHits = match->GetNofTrueHits() + match->GetNofWrongHits() + match->GetNofFakeHits();
   if ((match->GetNofTrueHits() / allHits) < fQuota) return;

   CbmStsTrack* track = static_cast<CbmStsTrack*>(fStsTracks->At(trackId));
   Int_t nofStsHits = track->GetNStsHits();
   Int_t nofMvdHits = track->GetNMvdHits();
   if (nofStsHits < 1) return; // No hits in STS
   if (nofMvdHits < fMvdMinNofHits || nofStsHits < fStsMinNofHits) return; // cut on number of hits in track

   const CbmLitMCTrack& mcTrack = fMCTrackCreator->GetTrack(mcId);

   FairTrackParam* firstParam = track->GetParamFirst();
   FairTrackParam* lastParam = track->GetParamLast();

   // Fill histograms for first track parameters
   if (nofMvdHits > 0) { // first track parameters in MVD
      const CbmMvdHit* firstHit = static_cast<const CbmMvdHit*>(fMvdHits->At(track->GetMvdHitIndex(0)));
      Int_t firstStation = firstHit->GetStationNr() - 1; // to start with 0
      if (mcTrack.GetNofPointsAtStation(kMVD, firstStation) > 0) {
         const CbmLitMCPoint& firstPoint = mcTrack.GetPointAtStation(kMVD, firstStation, 0);
         FillResidualsAndPulls(firstParam, &firstPoint, "htf_Sts_FirstParam_", nofMvdHits + nofStsHits, kMVD);
      }
   } else { // first track parameters in STS
      const CbmStsHit* firstHit = static_cast<const CbmStsHit*>(fStsHits->At(track->GetStsHitIndex(0)));
      Int_t firstStation = firstHit->GetStationNr() - 1; // to start with 0
      if (mcTrack.GetNofPointsAtStation(kSTS, firstStation) > 0) {
         const CbmLitMCPoint& firstPoint = mcTrack.GetPointAtStation(kSTS, firstStation, 0);
         FillResidualsAndPulls(firstParam, &firstPoint, "htf_Sts_FirstParam_", nofMvdHits + nofStsHits, kSTS);
      }
   }

   // Fill histograms for last track parameters
   const CbmStsHit* lastHit = static_cast<const CbmStsHit*>(fStsHits->At(track->GetStsHitIndex(nofStsHits - 1)));
   Int_t lastStation = lastHit->GetStationNr() - 1; // to start with 0
   if (mcTrack.GetNofPointsAtStation(kSTS, lastStation) > 0) {
      const CbmLitMCPoint& lastPoint = mcTrack.GetPointAtStation(kSTS, lastStation, 0);
      FillResidualsAndPulls(lastParam, &lastPoint, "htf_Sts_LastParam_", nofMvdHits + nofStsHits, kSTS);
   }
}

void CbmLitFitQaCalculator::ProcessTrdTrack(
      Int_t trackId)
{
   if (NULL == fTrdTracks || NULL == fTrdTrackMatches || trackId < 0) return;

   CbmTrackMatch* match = static_cast<CbmTrackMatch*>(fTrdTrackMatches->At(trackId));
   Int_t mcId = match->GetMCTrackId();
   if (mcId < 0) return; // Ghost or fake track

   // Check correctness of reconstructed track
   Int_t allHits = match->GetNofTrueHits() + match->GetNofWrongHits() + match->GetNofFakeHits();
   if ((match->GetNofTrueHits() / allHits) < fQuota) return;

   CbmTrack* track = static_cast<CbmTrack*>(fTrdTracks->At(trackId));
   Int_t nofHits = track->GetNofHits();
   if (nofHits < 1) return; // No hits
   if (nofHits < fTrdMinNofHits) return; // cut on number of hits in track

   const CbmLitMCTrack& mcTrack = fMCTrackCreator->GetTrack(mcId);

   const FairTrackParam* firstParam = track->GetParamFirst();
   const FairTrackParam* lastParam = track->GetParamLast();

   CbmTrdDetectorId trdDetectorId;

   // Fill histograms for first track parameters
   const CbmBaseHit* firstHit = static_cast<const CbmBaseHit*>(fTrdHits->At(track->GetHitIndex(0)));
   Int_t* detInfo = trdDetectorId.GetDetectorInfo(firstHit->GetDetectorId());
   Int_t firstStation = 10 * detInfo[1] + detInfo[2];
   if (mcTrack.GetNofPointsAtStation(kTRD, firstStation) > 0) {
      const CbmLitMCPoint& firstPoint = mcTrack.GetPointAtStation(kTRD, firstStation, 0);
      FillResidualsAndPulls(firstParam, &firstPoint, "htf_Trd_FirstParam_", nofHits, kTRD);
   }

   // Fill histograms for last track parameters
   const CbmBaseHit* lastHit = static_cast<const CbmBaseHit*>(fTrdHits->At(track->GetHitIndex(nofHits - 1)));
   detInfo = trdDetectorId.GetDetectorInfo(lastHit->GetDetectorId());
   Int_t lastStation = 10 * detInfo[1] + detInfo[2];
   if (mcTrack.GetNofPointsAtStation(kTRD, lastStation) > 0) {
      const CbmLitMCPoint& lastPoint = mcTrack.GetPointAtStation(kTRD, lastStation, 0);
      FillResidualsAndPulls(lastParam, &lastPoint, "htf_Trd_LastParam_", nofHits, kTRD);
   }
}

void CbmLitFitQaCalculator::ProcessMuchTrack(
      Int_t trackId)
{
   if (NULL == fMuchTracks || NULL == fMuchTrackMatches || trackId < 0) return;

   CbmTrackMatch* match = static_cast<CbmTrackMatch*>(fMuchTrackMatches->At(trackId));
   Int_t mcId = match->GetMCTrackId();
   if (mcId < 0) return; // Ghost or fake track

   // Check correctness of reconstructed track
   Int_t allHits = match->GetNofTrueHits() + match->GetNofWrongHits() + match->GetNofFakeHits();
   if ((match->GetNofTrueHits() / allHits) < fQuota) return;

   CbmTrack* track = static_cast<CbmTrack*>(fMuchTracks->At(trackId));
   Int_t nofHits = track->GetNofHits();
   if (nofHits < 1) return; // No hits
   if (nofHits < fMuchMinNofHits) return; // cut on number of hits in track

   const CbmLitMCTrack& mcTrack = fMCTrackCreator->GetTrack(mcId);

   const FairTrackParam* firstParam = track->GetParamFirst();
   const FairTrackParam* lastParam = track->GetParamLast();

   // Fill histograms for first track parameters
   const CbmBaseHit* firstHit = static_cast<const CbmBaseHit*>(fMuchPixelHits->At(track->GetHitIndex(0)));
//   Int_t firstStation = firstHit->GetPlaneId();
   Int_t firstStation = 100 * CbmMuchGeoScheme::GetStationIndex(firstHit->GetDetectorId())
            + 10 * CbmMuchGeoScheme::GetLayerIndex(firstHit->GetDetectorId())
            + CbmMuchGeoScheme::GetLayerSideIndex(firstHit->GetDetectorId());
   if (mcTrack.GetNofPointsAtStation(kMUCH, firstStation) > 0) {
      const CbmLitMCPoint& firstPoint = mcTrack.GetPointAtStation(kMUCH, firstStation, 0);
      FillResidualsAndPulls(firstParam, &firstPoint, "htf_Much_FirstParam_", nofHits, kMUCH);
   }

   // Fill histograms for last track parameters
   const CbmBaseHit* lastHit = static_cast<const CbmBaseHit*>(fMuchPixelHits->At(track->GetHitIndex(nofHits - 1)));
//   Int_t lastStation = lastHit->GetPlaneId();
   Int_t lastStation = 100 * CbmMuchGeoScheme::GetStationIndex(lastHit->GetDetectorId())
              + 10 * CbmMuchGeoScheme::GetLayerIndex(lastHit->GetDetectorId())
              + CbmMuchGeoScheme::GetLayerSideIndex(lastHit->GetDetectorId());
   if (mcTrack.GetNofPointsAtStation(kMUCH, lastStation) > 0) {
      const CbmLitMCPoint& lastPoint = mcTrack.GetPointAtStation(kMUCH, lastStation, 0);
      FillResidualsAndPulls(lastParam, &lastPoint, "htf_Much_LastParam_", nofHits, kMUCH);
   }
}

void CbmLitFitQaCalculator::FillResidualsAndPulls(
   const FairTrackParam* par,
   const CbmLitMCPoint* mcPoint,
   const string& histName,
   Float_t wrongPar,
   DetectorId detId)
{
   // Residuals
   Double_t resX = 0., resY = 0., resTx = 0., resTy = 0., resQp = 0.;
   if (detId == kMVD) { // Use MC average for MVD
      resX = par->GetX() - mcPoint->GetX();
      resY = par->GetY() - mcPoint->GetY();
      resTx = par->GetTx() - mcPoint->GetTx();
      resTy = par->GetTy() - mcPoint->GetTy();
      resQp = par->GetQp() - mcPoint->GetQp();
   } else if (detId == kSTS) { // Use MC average for STS
      resX = par->GetX() - mcPoint->GetX();
      resY = par->GetY() - mcPoint->GetY();
      resTx = par->GetTx() - mcPoint->GetTx();
      resTy = par->GetTy() - mcPoint->GetTy();
      resQp = par->GetQp() - mcPoint->GetQp();
   } else if (detId == kTRD) { // Use MC out for TRD
      resX = par->GetX() - mcPoint->GetXOut();
      resY = par->GetY() - mcPoint->GetYOut();
      resTx = par->GetTx() - mcPoint->GetTxOut();
      resTy = par->GetTy() - mcPoint->GetTyOut();
      resQp = par->GetQp() - mcPoint->GetQpOut();
   } else if (detId == kMUCH) { // Use MC average for MUCH
      resX = par->GetX() - mcPoint->GetX();
      resY = par->GetY() - mcPoint->GetY();
      resTx = par->GetTx() - mcPoint->GetTx();
      resTy = par->GetTy() - mcPoint->GetTy();
      resQp = par->GetQp() - mcPoint->GetQp();
   }
   fHM->H1(histName + "Res_X")->Fill(resX);
   fHM->H1(histName + "Res_Y")->Fill(resY);
   fHM->H1(histName + "Res_Tx")->Fill(resTx);
   fHM->H1(histName + "Res_Ty")->Fill(resTy);
   fHM->H1(histName + "Res_Qp")->Fill(resQp);

   // Pulls
   Double_t C[15];
   par->CovMatrix(C);

   Double_t sigmaX = (C[0] > 0.) ? std::sqrt(C[0]) : -1.;
   Double_t sigmaY = (C[5] > 0.) ? std::sqrt(C[5]) : -1.;
   Double_t sigmaTx = (C[9] > 0.) ? std::sqrt(C[9]) : -1.;
   Double_t sigmaTy = (C[12] > 0.) ? std::sqrt(C[12]) : -1.;
   Double_t sigmaQp = (C[14] > 0.) ? std::sqrt(C[14]) : -1.;
   if (sigmaX < 0) fHM->H1(histName + "WrongCov_X")->Fill(wrongPar); else fHM->H1(histName + "Pull_X")->Fill(resX / sigmaX);
   if (sigmaY < 0) fHM->H1(histName + "WrongCov_Y")->Fill(wrongPar); else fHM->H1(histName + "Pull_Y")->Fill(resY / sigmaY);
   if (sigmaTx < 0) fHM->H1(histName + "WrongCov_Tx")->Fill(wrongPar); else fHM->H1(histName + "Pull_Tx")->Fill(resTx / sigmaTx);
   if (sigmaTy < 0) fHM->H1(histName + "WrongCov_Ty")->Fill(wrongPar); else fHM->H1(histName + "Pull_Ty")->Fill(resTy / sigmaTy);
   if (sigmaQp < 0) fHM->H1(histName + "WrongCov_Qp")->Fill(wrongPar); else fHM->H1(histName + "Pull_Qp")->Fill(resQp / sigmaQp);
}
