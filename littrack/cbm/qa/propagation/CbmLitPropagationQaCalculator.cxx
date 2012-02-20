/**
 * \file CbmLitPropagationQaCalculator.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
#include "CbmLitPropagationQaCalculator.h"
#include "CbmLitPropagationQaHistCreator.h"
#include "../base/CbmLitHistManager.h"
#include "../mc/CbmLitMCTrackCreator.h"
#include "../mc/CbmLitMCPoint.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitStripHit.h"
#include "data/CbmLitPixelHit.h"
#include "base/CbmLitToolFactory.h"
#include "base/CbmLitDetectorSetup.h"
#include "utils/CbmLitConverter.h"
#include "utils/CbmLitMemoryManagment.h"
#include "CbmGlobalTrack.h"
#include "CbmStsTrack.h"
#include "CbmMuchTrack.h"
#include "CbmTrdTrack.h"
#include "CbmPixelHit.h"
#include "CbmStripHit.h"
#include "CbmTrackMatch.h"
#include "CbmTrdDetectorId.h"
#include "CbmMuchGeoScheme.h"
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TH1.h"

#include <cassert>
#include <algorithm>

using std::for_each;

CbmLitPropagationQaCalculator::CbmLitPropagationQaCalculator(
      CbmLitHistManager* histManager):
   fGlobalTracks(NULL),
   fStsTracks(NULL),
   fStsTrackMatches(NULL),
   fMuchTracks(NULL),
   fMuchPixelHits(NULL),
   fMuchStrawHits(NULL),
   fMuchTrackMatches(NULL),
   fTrdTracks(NULL),
   fTrdHits(NULL),
   fTrdTrackMatches(NULL),
   fTofHits(NULL),
   fLitTracks(),
   fPropagator(),
   fFilter(),
   fFitter(),
   fSmoother(),
   fParallelFitter(),
   fIsTestFastPropagation(false),
   fMCTrackCreator(),
   fNofPlanes(0),
   fPDGCode(13),
   fMinNofHits(8),
   fHM(histManager)
{

}

CbmLitPropagationQaCalculator::~CbmLitPropagationQaCalculator()
{

}

void CbmLitPropagationQaCalculator::Init()
{
   ReadDataBranches();

   // Create tools
   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   if (!fIsTestFastPropagation) {
      fPropagator = factory->CreateTrackPropagator("lit");
      fFilter = factory->CreateTrackUpdate("kalman");
      fFitter = factory->CreateTrackFitter("lit_kalman");
      fSmoother = factory->CreateTrackFitter("kalman_smoother");
   } else {
      fPropagator = factory->CreateTrackPropagator("lit");
      CbmLitDetectorSetup det;
      det.DetermineSetup();
      if (det.GetDet(kMUCH)) { fParallelFitter = factory->CreateTrackFitter("kalman_parallel_muon"); }
      else { fParallelFitter = factory->CreateTrackFitter("kalman_parallel_electron"); }
   }

   fMCTrackCreator = CbmLitMCTrackCreator::Instance();
}

void CbmLitPropagationQaCalculator::Exec()
{
   fMCTrackCreator->Create();
   CreateTrackArrays();
   RunTest();
   DeleteTrackArrays();
}

void CbmLitPropagationQaCalculator::Finish()
{

}

void CbmLitPropagationQaCalculator::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   assert(ioman != NULL);

   fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
   fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
   fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
   assert(fGlobalTracks != NULL && fStsTracks != NULL && fStsTrackMatches != NULL);

   fMuchTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
   fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
   fMuchStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
   fMuchTrackMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
   fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
   fTrdHits  = (TClonesArray*) ioman->GetObject("TrdHit");
   fTrdTrackMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
   fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
}

void CbmLitPropagationQaCalculator::CreateTrackArrays()
{
   Int_t nofGlobalTracks = fGlobalTracks->GetEntriesFast();
   for (Int_t iTrack = 0; iTrack < nofGlobalTracks; iTrack++) {
      const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(fGlobalTracks->At(iTrack));
      CbmLitTrack* litTrack = new CbmLitTrack();
      GlobalTrackToLitTrack(globalTrack, litTrack);
      litTrack->SetRefId(iTrack);
      litTrack->SetPDG(fPDGCode);
      fLitTracks.push_back(litTrack);
   }
}

void CbmLitPropagationQaCalculator::GlobalTrackToLitTrack(
   const CbmGlobalTrack* globalTrack,
   CbmLitTrack* litTrack)
{
   Int_t stsId = globalTrack->GetStsTrackIndex();
   Int_t trdId = globalTrack->GetTrdTrackIndex();
   Int_t muchId = globalTrack->GetMuchTrackIndex();
   Int_t tofId = globalTrack->GetTofHitIndex();

   // STS: set initial track parameters from STS track
   if (stsId > -1) {
      CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(stsId);
      CbmLitTrackParam par;
      CbmLitConverter::TrackParamToLitTrackParam((const_cast<CbmStsTrack*> (stsTrack))->GetParamLast(), &par);
      litTrack->SetParamLast(&par);
      litTrack->SetParamFirst(&par);
   }
   //MUCH: attach hits from MUCH track
   if (muchId > -1) {
      CbmMuchTrack* muchTrack = (CbmMuchTrack*) fMuchTracks->At(muchId);
      for (Int_t iHit = 0; iHit < muchTrack->GetNofHits(); iHit++) {
         Int_t index = muchTrack->GetHitIndex(iHit);
         HitType type = muchTrack->GetHitType(iHit);
         if (type == kMUCHPIXELHIT) {
            CbmPixelHit* hit = (CbmPixelHit*) fMuchPixelHits->At(index);
            CbmLitPixelHit litHit;
            CbmLitConverter::PixelHitToLitPixelHit(hit, index, &litHit);
            litHit.SetDetectorId(kLITMUCH);
            litTrack->AddHit(&litHit);
         } else if (type == kMUCHSTRAWHIT) {
            CbmStripHit* hit = (CbmStripHit*) fMuchStrawHits->At(index);
            CbmLitStripHit litHit;
            CbmLitConverter::StripHitToLitStripHit(hit, index, &litHit);
            litHit.SetDetectorId(kLITMUCH);
            litTrack->AddHit(&litHit);
         }
      }
   }
   //TRD: attach hits from TRD track
   if (trdId > -1) {
      CbmTrdTrack* trdTrack = (CbmTrdTrack*) fTrdTracks->At(trdId);
      for (Int_t iHit = 0; iHit < trdTrack->GetNofHits(); iHit++) {
         Int_t index = trdTrack->GetHitIndex(iHit);
         CbmPixelHit* hit = (CbmPixelHit*) fTrdHits->At(index);
         CbmLitPixelHit litHit;
         CbmLitConverter::PixelHitToLitPixelHit(hit, index, &litHit);
         litHit.SetDetectorId(kLITTRD);
         litTrack->AddHit(&litHit);
      }
   }
   //TOF: attach TOF hit
   if (tofId > -1) {
      CbmPixelHit* tofHit = (CbmPixelHit*) fTofHits->At(tofId);
      CbmLitPixelHit litHit;
      CbmLitConverter::PixelHitToLitPixelHit(tofHit, tofId, &litHit);
      litHit.SetDetectorId(kLITTOF);
      litHit.SetPlaneId(fNofPlanes - 1);
      litTrack->AddHit(&litHit);
   }
}

void CbmLitPropagationQaCalculator::DeleteTrackArrays()
{
   for_each(fLitTracks.begin(), fLitTracks.end(), DeleteObject());
   fLitTracks.clear();
}

void CbmLitPropagationQaCalculator::RunTest()
{
   for(Int_t i = 0; i < fLitTracks.size(); i++) {
      if (fLitTracks[i]->GetNofHits() < fMinNofHits) { continue; }
      fLitTracks[i]->SortHits();
      if (!fIsTestFastPropagation) {
         TestPropagation(fLitTracks[i]);
         TestFitter(fLitTracks[i]);
      } else {
         TestFastPropagation(fLitTracks[i]);
      }
   }
}

void CbmLitPropagationQaCalculator::TestPropagation(
   const CbmLitTrack* track)
{
   const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(fGlobalTracks->At(track->GetRefId()));
   CbmLitTrackParam par(*track->GetParamLast());
   for (Int_t i = 0; i < track->GetNofHits(); i++) {
      const CbmLitHit* hit = track->GetHit(i);
      Double_t zOut = hit->GetZ();
      if (fPropagator->Propagate(&par, zOut, track->GetPDG()) == kLITERROR) { continue; }
//    if (fFilter->Update(&par, track->GetHit(i)) == kLITERROR) continue;
      Int_t mcTrackId = GetMcTrackId(globalTrack);
      if (fMCTrackCreator->TrackExists(mcTrackId)) {
         CbmLitMCTrack mcTrack = fMCTrackCreator->GetTrack(mcTrackId);
         CbmLitMCPoint mcPoint = GetMcPointByHit(hit, &mcTrack);
         FillHistosPropagation(&par, &mcPoint, hit->GetPlaneId());
      }
//    if (fFilter->Update(&par, track->GetHit(i)) == kLITERROR) continue;
   }
}

void CbmLitPropagationQaCalculator::TestFitter(
   CbmLitTrack* track)
{
   LitStatus fitStatus, smoothStatus;
   fitStatus = fFitter->Fit(track);
   if (fitStatus == kLITSUCCESS) { smoothStatus = fSmoother->Fit(track); }
   if (fitStatus == kLITSUCCESS && smoothStatus == kLITSUCCESS) {
      const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(fGlobalTracks->At(track->GetRefId()));
      Int_t mcTrackId = GetMcTrackId(globalTrack);
      if (fMCTrackCreator->TrackExists(mcTrackId)) {
         CbmLitMCTrack mcTrack = fMCTrackCreator->GetTrack(mcTrackId);
         FillHistosFitter(track, &mcTrack);
      }
   }
}

void CbmLitPropagationQaCalculator::TestFastPropagation(
   CbmLitTrack* track)
{
   // Propagate tracks to the last STS station
   CbmLitTrackParam par(*track->GetParamLast());
   fPropagator->Propagate(&par, 100., track->GetPDG());
   track->SetParamLast(&par);
   LitStatus fitStatus = fParallelFitter->Fit(track);
   if (fitStatus == kLITSUCCESS) {
      const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(fGlobalTracks->At(track->GetRefId()));
      Int_t mcTrackId = GetMcTrackId(globalTrack);
      if (fMCTrackCreator->TrackExists(mcTrackId)) {
         CbmLitMCTrack mcTrack = fMCTrackCreator->GetTrack(mcTrackId);
         for (Int_t i = 0; i < track->GetNofHits(); i++) {
            const CbmLitHit* hit = track->GetHit(i);
            const CbmLitFitNode* node = track->GetFitNode(i);
            CbmLitMCPoint mcPoint = GetMcPointByHit(hit, &mcTrack);
            FillHistosPropagation(node->GetPredictedParam(), &mcPoint, hit->GetPlaneId());
            FillHistosFilter(node->GetUpdatedParam(), &mcPoint, hit->GetPlaneId(), node->GetChiSqFiltered());
         }
      }
   }
}

void CbmLitPropagationQaCalculator::FillHistosPropagation(
   const CbmLitTrackParam* par,
   const CbmLitMCPoint* mcPoint,
   Int_t plane)
{
   vector<Double_t> r = CalcResidualsAndPulls(par, mcPoint);
   for (Int_t iParam = 0; iParam < CbmLitPropagationQaHistCreator::NofQaParameters; iParam++) {
      fHM->H1(CbmLitPropagationQaHistCreator::HistName(iParam, 0, plane))->Fill(r[iParam]);
   }
//   fHM->H1(HistName(11, 0, plane))->Fill(0.);
}

void CbmLitPropagationQaCalculator::FillHistosFilter(
   const CbmLitTrackParam* par,
   const CbmLitMCPoint* mcPoint,
   Int_t plane,
   float chisq)
{
   vector<Double_t> r = CalcResidualsAndPulls(par, mcPoint);
   for (Int_t iParam = 0; iParam < CbmLitPropagationQaHistCreator::NofQaParameters; iParam++) {
      fHM->H1(CbmLitPropagationQaHistCreator::HistName(iParam, 1, plane))->Fill(r[iParam]);
   }
//   fHM->H1(HistName(11, 1, plane))->Fill(chisq);
}

void CbmLitPropagationQaCalculator::FillHistosFitter(
   const CbmLitTrack* track,
   const CbmLitMCTrack* mcTrack)
{
   Int_t nofHits = track->GetNofHits();
   for (Int_t i = 0; i < nofHits; i++) {
      const CbmLitHit* hit = track->GetHit(i);
      const CbmLitFitNode* node = track->GetFitNode(i);
      CbmLitMCPoint mcPoint = GetMcPointByHit(hit, mcTrack);
      vector<Double_t> rFilter = CalcResidualsAndPulls(node->GetUpdatedParam(), &mcPoint);
      vector<Double_t> rSmoother = CalcResidualsAndPulls(node->GetSmoothedParam(), &mcPoint);
      for (Int_t iParam = 0; iParam < CbmLitPropagationQaHistCreator::NofQaParameters; iParam++) {
         fHM->H1(CbmLitPropagationQaHistCreator::HistName(iParam, 1, hit->GetPlaneId()))->Fill(rFilter[iParam]);
         fHM->H1(CbmLitPropagationQaHistCreator::HistName(iParam, 2, hit->GetPlaneId()))->Fill(rSmoother[iParam]);
      }
//      fHM->H1(HistName(11, 1, hit->GetPlaneId()))->Fill(node->GetChiSqFiltered());
//      fHM->H1(HistName(11, 2, hit->GetPlaneId()))->Fill(node->GetChiSqSmoothed());
   }
}

vector<Double_t> CbmLitPropagationQaCalculator::CalcResidualsAndPulls(
   const CbmLitTrackParam* par,
   const CbmLitMCPoint* mcPoint)
{
   vector<Double_t> r(11);
   r[0] = par->GetX() - mcPoint->GetX();
   r[1] = par->GetY() - mcPoint->GetY();
   r[2] = par->GetTx() - mcPoint->GetTx();
   r[3] = par->GetTy() - mcPoint->GetTy();
   r[4] = par->GetQp() - mcPoint->GetQp();
   r[5] = (par->GetCovariance(0) > 0.) ? (r[0]) / (std::sqrt(par->GetCovariance(0))) : 0.;
   r[6] = (par->GetCovariance(5) > 0.) ? (r[1]) / (std::sqrt(par->GetCovariance(5))) : 0.;
   r[7] = (par->GetCovariance(9) > 0.) ? (r[2]) / (std::sqrt(par->GetCovariance(9))) : 0.;
   r[8] = (par->GetCovariance(12) > 0.) ? (r[3]) / (std::sqrt(par->GetCovariance(12))) : 0.;
   r[9] = (par->GetCovariance(14) > 0.) ? (r[4]) / (std::sqrt(par->GetCovariance(14))) : 0.;
//   r[10] = 100 * ((1./par->GetQp() - 1./mcPoint->GetQp()) / (std::abs(1./mcPoint->GetQp())));
   return r;
}

Int_t CbmLitPropagationQaCalculator::GetMcTrackId(
      const CbmGlobalTrack* track)
{
   Int_t stsId = track->GetStsTrackIndex();
   Int_t trdId = track->GetTrdTrackIndex();
   Int_t muchId = track->GetMuchTrackIndex();
   Int_t tofId = track->GetTofHitIndex();

   if (stsId < 0) return -1;
   CbmTrackMatch* stsMatch = static_cast<CbmTrackMatch*>(fStsTrackMatches->At(stsId));

   if (fTrdTrackMatches && fMuchTrackMatches) {
      if (trdId < 0 || muchId < 0) return -1;
      CbmTrackMatch* trdMatch = static_cast<CbmTrackMatch*>(fTrdTrackMatches->At(trdId));
      CbmTrackMatch* muchMatch = static_cast<CbmTrackMatch*>(fMuchTrackMatches->At(muchId));
      if (stsMatch->GetMCTrackId() == trdMatch->GetMCTrackId() == muchMatch->GetMCTrackId()) {
         return stsMatch->GetMCTrackId();
      } else {
         return -1;
      }
   } else if (fMuchTrackMatches) {
      if (muchId < 0) return -1;
      CbmTrackMatch* muchMatch = static_cast<CbmTrackMatch*>(fMuchTrackMatches->At(muchId));
      if (stsMatch->GetMCTrackId() == muchMatch->GetMCTrackId()) {
         return stsMatch->GetMCTrackId();
      } else {
         return -1;
      }
   } else if (fTrdTrackMatches) {
      if (trdId < 0) return -1;
      CbmTrackMatch* trdMatch = static_cast<CbmTrackMatch*>(fTrdTrackMatches->At(trdId));
      if (stsMatch->GetMCTrackId() == trdMatch->GetMCTrackId()) {
        return stsMatch->GetMCTrackId();
      } else {
        return -1;
      }
   }
   return -1;
}

CbmLitMCPoint CbmLitPropagationQaCalculator::GetMcPointByHit(
      const CbmLitHit* hit,
      const CbmLitMCTrack* mcTrack)
{
   if (hit->GetDetectorId() == kLITTRD) {
      CbmTrdDetectorId trdDetectorId;
      const CbmBaseHit* trdHit = static_cast<const CbmBaseHit*>(fTrdHits->At(hit->GetRefId()));
      Int_t* detInfo = trdDetectorId.GetDetectorInfo(trdHit->GetDetectorId());
      Int_t trdStationId = 10 * detInfo[1] + detInfo[2];
      if (mcTrack->GetNofPointsAtStation(kTRD, trdStationId) > 0) {
         return mcTrack->GetPointAtStation(kTRD, trdStationId, 0);
      }
   } else if (hit->GetDetectorId() == kLITMUCH) {
     // Int_t muchStationId = hit->GetPlaneId();
      const CbmBaseHit* muchHit = (hit->GetType() == kLITPIXELHIT) ?
            static_cast<const CbmBaseHit*>(fMuchPixelHits->At(hit->GetRefId())):
            static_cast<const CbmBaseHit*>(fMuchStrawHits->At(hit->GetRefId()));
      Int_t muchStationId = 100 * CbmMuchGeoScheme::GetStationIndex(muchHit->GetDetectorId())
         + 10 * CbmMuchGeoScheme::GetLayerIndex(muchHit->GetDetectorId())
         + CbmMuchGeoScheme::GetLayerSideIndex(muchHit->GetDetectorId());
      if (mcTrack->GetNofPointsAtStation(kMUCH, muchStationId) > 0) {
         return mcTrack->GetPointAtStation(kMUCH, muchStationId, 0);
      }
   } else if (hit->GetDetectorId() == kLITTOF) {
      if (mcTrack->GetNofPointsAtStation(kTOF, 0) > 0) {
         return mcTrack->GetPointAtStation(kTOF, 0, 0);
      }
   }
   return CbmLitMCPoint();
}
