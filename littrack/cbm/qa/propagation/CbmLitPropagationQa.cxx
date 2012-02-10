/**
 * \file CbmLitPropagationQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 **/
#include "qa/propagation/CbmLitPropagationQa.h"

#include "qa/mc/CbmLitMCTrackCreator.h"
#include "qa/mc/CbmLitMCPoint.h"
#include "base/CbmLitEnvironment.h"
#include "base/CbmLitToolFactory.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitStripHit.h"
#include "interface/CbmLitTrackUpdate.h"
#include "interface/CbmLitTrackPropagator.h"
#include "interface/CbmLitTrackFitter.h"
#include "utils/CbmLitConverter.h"
#include "utils/CbmLitMemoryManagment.h"
#include "utils/CbmLitMath.h"
#include "utils/CbmLitDrawHist.h"
#include "utils/CbmLitUtils.h"

#include "CbmBaseHit.h"
#include "CbmPixelHit.h"
#include "CbmStripHit.h"
#include "CbmGlobalTrack.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"
#include "CbmMuchTrack.h"
#include "CbmTrackMatch.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmTrdDetectorId.h"
#include "CbmMuchGeoScheme.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TF1.h"

#include <boost/assign/list_of.hpp>

#include <iostream>
#include <iomanip>
#include <cmath>
#include <sstream>
#include <string>
#include <fstream>


CbmLitPropagationQa::CbmLitPropagationQa():
   fDet(),

   fLitTracks(),

   fNofPlanes(0),
   fPDGCode(-1),

   fMinNofHits(8),

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

   fPropagator(),
   fFilter(),
   fFitter(),
   fSmoother(),
   fParallelFitter(),

   fPropagationHistos(),
   fFilterHistos(),
   fSmootherHistos(),

   fPropagationWatch(),
   fFitterWatch(),
   fSmootherWatch(),

   fVerbose(1),
   fEvents(0),

   fIsTestFastPropagation(true),

   fIsDrawPropagation(false),
   fIsDrawFilter(true),
   fIsDrawSmoother(false),

   fIsCloseCanvas(true),

   fOutputDir(""),

   fIsFixedBounds(true),

   fSigma(),
   fRms()
{
}

CbmLitPropagationQa::~CbmLitPropagationQa()
{
}

InitStatus CbmLitPropagationQa::Init()
{
   fDet.DetermineSetup();
   std::cout << fDet.ToString();

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
      if (fDet.GetDet(kMUCH)) { fParallelFitter = factory->CreateTrackFitter("kalman_parallel_muon"); }
      else { fParallelFitter = factory->CreateTrackFitter("kalman_parallel_electron"); }
   }

   CreateHistograms();

   fMCTrackCreator = CbmLitMCTrackCreator::Instance();

   return kSUCCESS;
}

void CbmLitPropagationQa::SetParContainers()
{
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb = ana->GetRuntimeDb();
   rtdb->getContainer("CbmGeoMuchPar");
}

void CbmLitPropagationQa::Exec(
   Option_t* opt)
{
   fMCTrackCreator->Create();
   CreateTrackArrays();
   RunTest();
   DeleteTrackArrays();
   std::cout << "Event: " << fEvents++ << std::endl;
}

void CbmLitPropagationQa::Finish()
{
   for(Int_t i = 0; i < fNofPlanes; i++) {
      for(Int_t j = 0; j < NOF_PARAMS; j++) {
         fPropagationHistos[i][j]->Write();
         fFilterHistos[i][j]->Write();
         fSmootherHistos[i][j]->Write();
      }
   }

   PrintStopwatchStatistics();
   Draw();
}

void CbmLitPropagationQa::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("CbmLitPropagationQa::Init","CbmRootManager is not instantiated"); }

   fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
   if (NULL == fGlobalTracks) { Fatal("CbmLitPropagationQa::Init","No GlobalTrack array!"); }

   if (fDet.GetDet(kSTS)) {
      fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
      if (NULL == fStsTracks) { Fatal("CbmLitPropagationQa::Init", "No STSTrack array!"); }
      fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
      if (NULL == fStsTrackMatches) Fatal("CbmLitPropagationQa::Init", "No StsTrackMatch array!");
   }

   if (fDet.GetDet(kMUCH)) {
      fMuchTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
      if (NULL == fMuchTracks) { Fatal("CbmLitPropagationQa::Init", "No MuchTrack array!"); }
      fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
      fMuchStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
      if (NULL == fMuchPixelHits && NULL == fMuchStrawHits) { Fatal("CbmLitPropagationQa::Init", "No MuchPixelHit AND MuchStrawHit arrays!"); }
      fMuchTrackMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
      if (NULL == fMuchTrackMatches) { Fatal("CbmLitPropagationQa::Init", "No MuchTrackMatch array!"); }
   }

   if (fDet.GetDet(kTRD)) {
      fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
      if (NULL == fTrdTracks) { Fatal("CbmLitPropagationQa::Init", "No TrdTrack array!"); }
      fTrdHits  = (TClonesArray*) ioman->GetObject("TrdHit");
      if (NULL == fTrdHits) { Fatal("CbmLitPropagationQa::Init", "No TRDHit array!"); }
      fTrdTrackMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
      if (!fTrdTrackMatches) { Fatal("CbmLitPropagationQa::Init", "No TrdTrackMatch array!"); }
   }

   if (fDet.GetDet(kTOF)) {
      fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
      if (NULL == fTofHits) { Fatal("CbmLitPropagationQa::Init", "No TofHit array!"); }
   }
}

void CbmLitPropagationQa::CreateHistograms()
{
   //0,1,2,3,4 - residuals (x,y,tx,ty,q/p)
   //5,6,7,8,9 - pulls (x,y,tx,ty,q/p)
   //11 - relative momentum error
   //12 - chi-square of the track param and hit

   fPropagationHistos.resize(fNofPlanes);
   fFilterHistos.resize(fNofPlanes);
   fSmootherHistos.resize(fNofPlanes);
   for(Int_t i = 0; i < fNofPlanes; i++) {
      fPropagationHistos[i].resize(NOF_PARAMS);
      fFilterHistos[i].resize(NOF_PARAMS);
      fSmootherHistos[i].resize(NOF_PARAMS);
   }

   std::string names[] = { "h_resx", "h_resy", "h_restx", "h_resty", "h_resqp",
                           "h_pullx", "h_pully", "h_pulltx", "h_pullty", "h_pullqp",
                           "h_resp", "h_chisq" };

   std::string xtitles[] = { "Residual X [cm]", "Residual Y [cm]", "Residual Tx",
                            "Residual Ty", "Residual q/p [(GeV/c)^{-1}]", "Pull X", "Pull Y", "Pull Tx", "Pull Ty",
                            "Pull q/p", "Resolution p [%]", "Chi-square" };

   std::vector<Int_t> bins(NOF_PARAMS, 200);
   std::vector<std::pair<Double_t, Double_t> > bounds;
   if (fIsFixedBounds) {
      bounds = boost::assign::list_of
            (std::make_pair(-10., 10.))(std::make_pair(-10., 10.)) // X, Y residuals
            (std::make_pair(-.15, .15))(std::make_pair(-.15, .15)) // Tx, Ty residuals
            (std::make_pair(-.1, .1)) // Qp residual
            (std::make_pair(-5., 5.))(std::make_pair(-5., 5.)) // X, Y pull
            (std::make_pair(-5., 5.))(std::make_pair(-5., 5.)) // Tx, Ty pull
            (std::make_pair(-7., 7.)) // Qp pull
            (std::make_pair(-30., 30.)) // momentum resolution
            (std::make_pair(-1., 20.)); // chi-square
   } else {
      bounds.assign(NOF_PARAMS, std::make_pair(0.,0.));
   }

   std::string var[] = {"p", "f", "s"};
   for (Int_t v = 0; v < 3; v++) {
      for (Int_t i = 0; i < fNofPlanes; i++) {
         for (Int_t j = 0; j < NOF_PARAMS; j++) {
            std::stringstream histName;
            histName << names[j] << "_" << var[v] << "_" << i;
            TH1F* hist = new TH1F(histName.str().c_str(), std::string(histName.str() + ";" + xtitles[j] + ";Counter").c_str(),
                                  bins[j], bounds[j].first, bounds[j].second);
            if (v == 0) { fPropagationHistos[i][j] = hist; }
            if (v == 1) { fFilterHistos[i][j] = hist; }
            if (v == 2) { fSmootherHistos[i][j] = hist; }
         }
      }
   }
}

void CbmLitPropagationQa::CreateTrackArrays()
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

void CbmLitPropagationQa::GlobalTrackToLitTrack(
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
      for (int iHit = 0; iHit < muchTrack->GetNofHits(); iHit++) {
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
      for (int iHit = 0; iHit < trdTrack->GetNofHits(); iHit++) {
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
      litHit.SetPlaneId(fNofPlanes-1);
      litTrack->AddHit(&litHit);
   }
}

void CbmLitPropagationQa::DeleteTrackArrays()
{
   for_each(fLitTracks.begin(), fLitTracks.end(), DeleteObject());
   fLitTracks.clear();
}

void CbmLitPropagationQa::RunTest()
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

void CbmLitPropagationQa::TestPropagation(
   const CbmLitTrack* track)
{
   const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(fGlobalTracks->At(track->GetRefId()));
   CbmLitTrackParam par(*track->GetParamLast());
   fPropagationWatch.Start(kFALSE);
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
   fPropagationWatch.Stop();
}

void CbmLitPropagationQa::TestFitter(
   CbmLitTrack* track)
{
   LitStatus fitStatus, smoothStatus;
   fFitterWatch.Start(kFALSE);
   fitStatus = fFitter->Fit(track);
   fFitterWatch.Stop();
   fSmootherWatch.Start(kFALSE);
   if (fitStatus == kLITSUCCESS) { smoothStatus = fSmoother->Fit(track); }
   fSmootherWatch.Stop();
   if (fitStatus == kLITSUCCESS && smoothStatus == kLITSUCCESS) {
      const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(fGlobalTracks->At(track->GetRefId()));
      Int_t mcTrackId = GetMcTrackId(globalTrack);
      if (fMCTrackCreator->TrackExists(mcTrackId)) {
         CbmLitMCTrack mcTrack = fMCTrackCreator->GetTrack(mcTrackId);
         FillHistosFitter(track, &mcTrack);
      }
   }
}

void CbmLitPropagationQa::TestFastPropagation(
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

void CbmLitPropagationQa::FillHistosPropagation(
   const CbmLitTrackParam* par,
   const CbmLitMCPoint* mcPoint,
   Int_t plane)
{
   std::vector<Double_t> r = CalcResidualsAndPulls(par, mcPoint);
   for (Int_t i = 0; i < 11; i++) {
      fPropagationHistos[plane][i]->Fill(r[i]);
   }
   fPropagationHistos[plane][11]->Fill(0.);
}

void CbmLitPropagationQa::FillHistosFilter(
   const CbmLitTrackParam* par,
   const CbmLitMCPoint* mcPoint,
   Int_t plane,
   float chisq)
{
   std::vector<Double_t> r = CalcResidualsAndPulls(par, mcPoint);
   for (Int_t i = 0; i < 11; i++) {
      fFilterHistos[plane][i]->Fill(r[i]);
   }
   fFilterHistos[plane][11]->Fill(chisq);
}

void CbmLitPropagationQa::FillHistosFitter(
   const CbmLitTrack* track,
   const CbmLitMCTrack* mcTrack)
{
   Int_t nofHits = track->GetNofHits();
   for (Int_t i = 0; i < nofHits; i++) {
      const CbmLitHit* hit = track->GetHit(i);
      const CbmLitFitNode* node = track->GetFitNode(i);
      CbmLitMCPoint mcPoint = GetMcPointByHit(hit, mcTrack);
      std::vector<Double_t> rFilter = CalcResidualsAndPulls(node->GetUpdatedParam(), &mcPoint);
      std::vector<Double_t> rSmoother = CalcResidualsAndPulls(node->GetSmoothedParam(), &mcPoint);
      for (Int_t j = 0; j < 11; j++) {
         fFilterHistos[hit->GetPlaneId()][j]->Fill(rFilter[j]);
         fSmootherHistos[hit->GetPlaneId()][j]->Fill(rSmoother[j]);
      }
      fFilterHistos[hit->GetPlaneId()][11]->Fill(node->GetChiSqFiltered());
      fSmootherHistos[hit->GetPlaneId()][11]->Fill(node->GetChiSqSmoothed());
   }
}

std::vector<Double_t> CbmLitPropagationQa::CalcResidualsAndPulls(
   const CbmLitTrackParam* par,
   const CbmLitMCPoint* mcPoint)
{
   std::vector<Double_t> r(11);
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
   r[10] = 100 * ((1./par->GetQp() - 1./mcPoint->GetQp()) / (std::abs(1./mcPoint->GetQp())));
   return r;
}

Int_t CbmLitPropagationQa::GetMcTrackId(
      const CbmGlobalTrack* track)
{
   Int_t stsId = track->GetStsTrackIndex();
   Int_t trdId = track->GetTrdTrackIndex();
   Int_t muchId = track->GetMuchTrackIndex();
   Int_t tofId = track->GetTofHitIndex();

   if (stsId < 0) return -1;
   CbmTrackMatch* stsMatch = static_cast<CbmTrackMatch*>(fStsTrackMatches->At(stsId));

   if (fDet.GetDet(kMUCH) && fDet.GetDet(kTRD)) {
      if (trdId < 0 || muchId < 0) return -1;
      CbmTrackMatch* trdMatch = static_cast<CbmTrackMatch*>(fTrdTrackMatches->At(trdId));
      CbmTrackMatch* muchMatch = static_cast<CbmTrackMatch*>(fMuchTrackMatches->At(muchId));
      if (stsMatch->GetMCTrackId() == trdMatch->GetMCTrackId() == muchMatch->GetMCTrackId()) {
         return stsMatch->GetMCTrackId();
      } else {
         return -1;
      }
   } else if (fDet.GetDet(kMUCH)) {
      if (muchId < 0) return -1;
      CbmTrackMatch* muchMatch = static_cast<CbmTrackMatch*>(fMuchTrackMatches->At(muchId));
      if (stsMatch->GetMCTrackId() == muchMatch->GetMCTrackId()) {
         return stsMatch->GetMCTrackId();
      } else {
         return -1;
      }
   } else if (fDet.GetDet(kTRD)) {
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

CbmLitMCPoint CbmLitPropagationQa::GetMcPointByHit(
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

void CbmLitPropagationQa::PrintStopwatchStatistics()
{
   std::cout << "Stopwatch: " << std::endl;
   std::cout << "propagation: counts=" << fPropagationWatch.Counter()
             << ", real=" << fPropagationWatch.RealTime()/fPropagationWatch.Counter()
             << "/" << fPropagationWatch.RealTime()
             << " s, cpu=" << fPropagationWatch.CpuTime()/fPropagationWatch.Counter()
             << "/" << fPropagationWatch.CpuTime() << std::endl;
   std::cout << "fitter: real=" << fFitterWatch.Counter()
             << ", real=" << fFitterWatch.RealTime()/fFitterWatch.Counter()
             << "/" << fFitterWatch.RealTime()
             << " s, cpu=" << fFitterWatch.CpuTime()/fFitterWatch.Counter()
             << "/" << fFitterWatch.CpuTime() << std::endl;
   std::cout << "smoother: real=" << fSmootherWatch.Counter()
             << ", real=" << fSmootherWatch.RealTime()/fSmootherWatch.Counter()
             << "/" << fSmootherWatch.RealTime()
             << " s, cpu=" << fSmootherWatch.CpuTime()/fSmootherWatch.Counter()
             << "/" << fSmootherWatch.CpuTime() << std::endl;
}

void CbmLitPropagationQa::Draw()
{
   SetStyles();

   // Canvases for drawing.
   // First index is parameter (0-propagation, 1-filter, 2-smoother).
   // Second index is a plane number.
   TCanvas* canvas[3][fNofPlanes];
   std::string cnames[] = {"c_propagation_", "c_filter_", "c_smoother_"};
   for (int c = 0; c < 3; c++) {
      if (c == 0 && !fIsDrawPropagation) { continue; }
      if (c == 1 && !fIsDrawFilter) { continue; }
      if (c == 2 && !fIsDrawSmoother) { continue; }
      for (int i = 0; i < fNofPlanes; i++) {
         std::string name = cnames[c] + lit::ToString<int>(i);
         canvas[c][i] = new TCanvas(name.c_str(),name.c_str(),1200,800);
         canvas[c][i]->Divide(4, 3);
      }
   }

   // resize vectors
   fSigma.resize(3);
   fRms.resize(3);
   for (int c = 0; c < 3; c++) {
      fSigma[c].resize(fNofPlanes);
      fRms[c].resize(fNofPlanes);
      for (int i = 0; i < fNofPlanes; i++) {
         fSigma[c][i].resize(NOF_PARAMS);
         fRms[c][i].resize(NOF_PARAMS);
      }
   }

   if (fIsDrawPropagation) {
      DrawHistos(canvas[0], 0);
   }
   if (fIsDrawFilter) {
      DrawHistos(canvas[1], 1);
   }
   if (fIsDrawSmoother) {
      DrawHistos(canvas[2], 2);
   }

   std::ofstream fout(std::string(fOutputDir + "propagation_analysis.txt").c_str());
   if (fIsDrawPropagation) {
      PrintResults(std::cout, 0);
      PrintResults(fout, 0);
   }
   if (fIsDrawFilter) {
      PrintResults(std::cout, 1);
      PrintResults(fout, 1);
   }
   if (fIsDrawSmoother) {
      PrintResults(std::cout, 2);
      PrintResults(fout, 2);
   }

//   DrawForPhd();
}

void CbmLitPropagationQa::DrawHistos(
   TCanvas* c[],
   int v)
{
   for (int plane = 0; plane < fNofPlanes; plane++) {
      for (int i = 0; i < NOF_PARAMS; i++) {
         c[plane]->cd(i+1);
         TH1F* hist = NULL;
         if (v == 0) { hist = fPropagationHistos[plane][i]; }
         if (v == 1) { hist = fFilterHistos[plane][i]; }
         if (v == 2) { hist = fSmootherHistos[plane][i]; }
         if (i != 11) { hist->Fit("gaus"); }
         hist->SetMaximum(hist->GetMaximum() * 1.50);
         DrawH1(hist, kLitLinear, kLitLog);

         TF1* fit = hist->GetFunction("gaus");
         Double_t sigma = 0., rms = 0.;
         if (fit) {
            fSigma[v][plane][i] = sigma = fit->GetParameter(2);
            fRms[v][plane][i] = rms = hist->GetRMS();
         }
         DrawHistSigmaRMS(sigma, rms);
      }
      lit::SaveCanvasAsImage(c[plane], fOutputDir);
      if (fIsCloseCanvas) { c[plane]->Close(); }
   }
}

void CbmLitPropagationQa::PrintResults(
   std::ostream& out,
   int v)
{
   std::string var[] = {"[propagation]", "[filter]", "[smoother]"};
   out << var[v] << std::endl;

   out.precision(3);
   out << "plane     resx             resy           restx             resty           resqp            pullx         pully        pulltx       pullty      pullqp      resmom        chisq" << std::endl;
   for (int i = 0; i < fNofPlanes; i++) {
      out << std::setw(3) << i << " ";
      for (int j = 0; j < NOF_PARAMS; j++) {
         int w = (j < 5)? 7 : 5;
         out << std::setw(w) << fSigma[v][i][j] << "(" << std::setw(w) << fRms[v][i][j] << ") ";
      }
      out << std::endl;
   }
}

ClassImp(CbmLitPropagationQa);
