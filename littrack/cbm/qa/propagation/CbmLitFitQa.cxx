/**
 * \file CbmLitFitQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitFitQa.h"
#include "cbm/qa/mc/CbmLitMCTrackCreator.h"
#include "CbmGlobalTrack.h"
#include "CbmTrackMatch.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"
#include "CbmMuchTrack.h"
#include "CbmStsHit.h"
#include "CbmMvdHit.h"
#include "utils/CbmLitDrawHist.h"
#include "utils/CbmLitUtils.h"

#include "FairRootManager.h"
#include "CbmTrdDetectorId.h"
#include "CbmMuchGeoScheme.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TF1.h"
#include "TCanvas.h"

#include <boost/assign/list_of.hpp>

CbmLitFitQa::CbmLitFitQa():
   fIsFixedBounds(true),
   fMvdMinNofHits(0),
   fStsMinNofHits(0),
   fTrdMinNofHits(0),
   fMuchMinNofHits(0),
   fOutputDir("./test/")
{

}

CbmLitFitQa::~CbmLitFitQa()
{

}

InitStatus CbmLitFitQa::Init()
{
   ReadDataBranches();
   CreateHistograms();
   fMCTrackCreator = CbmLitMCTrackCreator::Instance();
   return kSUCCESS;
}

void CbmLitFitQa::Exec(
   Option_t* opt)
{
   static Int_t nofEvents = 0;
   std::cout << "CbmLitFitQa: event=" << nofEvents++ << std::endl;

   fMCTrackCreator->Create();
   ProcessGlobalTracks();
}

void CbmLitFitQa::Finish()
{
   if (NULL != fStsTracks) {
      DrawHistos("fit_qa_sts_first_param", fStsHistosFirst, fStsHistosFirstWrongCov);
      DrawHistos("fit_qa_sts_last_param", fStsHistosLast, fStsHistosLastWrongCov);
   }
   if (NULL != fTrdTracks) {
      DrawHistos("fit_qa_trd_first_param", fTrdHistosFirst, fTrdHistosFirstWrongCov);
      DrawHistos("fit_qa_trd_last_param", fTrdHistosLast, fTrdHistosLastWrongCov);
   }
   if (NULL != fMuchTracks) {
      DrawHistos("fit_qa_much_first_param", fMuchHistosFirst, fMuchHistosFirstWrongCov);
      DrawHistos("fit_qa_much_last_param", fMuchHistosLast, fMuchHistosLastWrongCov);
   }
}

void CbmLitFitQa::SetParContainers()
{

}

void CbmLitFitQa::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("CbmLitFitQa::Init","CbmRootManager is not instantiated"); }

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

void CbmLitFitQa::CreateHistograms()
{
   //
   // Residual and pull histograms
   //

   fStsHistosFirst.resize(NOF_PARAMS);
   fStsHistosLast.resize(NOF_PARAMS);
   fTrdHistosFirst.resize(NOF_PARAMS);
   fTrdHistosLast.resize(NOF_PARAMS);
   fMuchHistosFirst.resize(NOF_PARAMS);
   fMuchHistosLast.resize(NOF_PARAMS);

   std::string names[] = { "ResX", "ResY", "ResTx", "ResTy", "ResQp",
                           "PullX", "PullY", "PullTx", "PullTy", "PullQp" };

   std::string xtitles[] = {
       "Residual X [cm]", "Residual Y [cm]", "Residual Tx",
       "Residual Ty", "Residual q/p [(GeV/c)^{-1}]",
       "Pull X", "Pull Y", "Pull Tx", "Pull Ty", "Pull q/p" };

   std::vector<Int_t> bins(NOF_PARAMS, 200);
   std::vector<std::pair<Float_t, Float_t> > bounds;
   if (fIsFixedBounds) {
      bounds = boost::assign::list_of
            (std::make_pair(-1., 1.))(std::make_pair(-1., 1.)) // X, Y residuals
            (std::make_pair(-.01, .01))(std::make_pair(-.01, .01)) // Tx, Ty residuals
            (std::make_pair(-.1, .1)) // Qp residual
            (std::make_pair(-5., 5.))(std::make_pair(-5., 5.)) // X, Y pull
            (std::make_pair(-5., 5.))(std::make_pair(-5., 5.)) // Tx, Ty pull
            (std::make_pair(-7., 7.)); // Qp pull
   } else {
      bounds.assign(NOF_PARAMS, std::make_pair(0.,0.));
   }

   for (Int_t j = 0; j < NOF_PARAMS; j++) {
      std::string histName1 = "hStsFirst" + names[j];
      fStsHistosFirst[j] = new TH1F(histName1.c_str(), std::string(histName1 + ";" + xtitles[j] + ";Counter").c_str(),
            bins[j], bounds[j].first, bounds[j].second);

      std::string histName2 = "hStsLast" + names[j];
      fStsHistosLast[j] = new TH1F(histName2.c_str(), std::string(histName2 + ";" + xtitles[j] + ";Counter").c_str(),
            bins[j], bounds[j].first, bounds[j].second);

      std::string histName3 = "hTrdFirst" + names[j];
      fTrdHistosFirst[j] = new TH1F(histName3.c_str(), std::string(histName3 + ";" + xtitles[j] + ";Counter").c_str(),
            bins[j], bounds[j].first, bounds[j].second);

      std::string histName4 = "hTrdLast" + names[j];
      fTrdHistosLast[j] = new TH1F(histName4.c_str(), std::string(histName4 + ";" + xtitles[j] + ";Counter").c_str(),
            bins[j], bounds[j].first, bounds[j].second);

      std::string histName5 = "hMuchFirst" + names[j];
      fMuchHistosFirst[j] = new TH1F(histName5.c_str(), std::string(histName5 + ";" + xtitles[j] + ";Counter").c_str(),
            bins[j], bounds[j].first, bounds[j].second);

      std::string histName6 = "hMuchLast" + names[j];
      fMuchHistosLast[j] = new TH1F(histName6.c_str(), std::string(histName6 + ";" + xtitles[j] + ";Counter").c_str(),
            bins[j], bounds[j].first, bounds[j].second);
   }

   //
   // Number of wrong fits
   //

   fStsHistosFirstWrongCov.resize(NOF_PARAMS_WRONG_COV);
   fStsHistosLastWrongCov.resize(NOF_PARAMS_WRONG_COV);
   fTrdHistosFirstWrongCov.resize(NOF_PARAMS_WRONG_COV);
   fTrdHistosLastWrongCov.resize(NOF_PARAMS_WRONG_COV);
   fMuchHistosFirstWrongCov.resize(NOF_PARAMS_WRONG_COV);
   fMuchHistosLastWrongCov.resize(NOF_PARAMS_WRONG_COV);

   Int_t minNofHits = 0;
   Int_t maxNofHits = 20;

   for (Int_t j = 0; j < NOF_PARAMS_WRONG_COV; j++) {
      int k = j + 5; // to start with pulls
       std::string histName1 = "hStsFirstWrongCov" + names[k];
       fStsHistosFirstWrongCov[j] = new TH1F(histName1.c_str(), std::string(histName1 + ";Number of hits;Counter").c_str(),
             bins[k], minNofHits, maxNofHits);

       std::string histName2 = "hStsLastWrongCov" + names[k];
       fStsHistosLastWrongCov[j] = new TH1F(histName2.c_str(), std::string(histName2 + ";Number of hits;Counter").c_str(),
             bins[k], minNofHits, maxNofHits);

       std::string histName3 = "hTrdFirstWrongCov" + names[k];
       fTrdHistosFirstWrongCov[j] = new TH1F(histName3.c_str(), std::string(histName3 + ";Number of hits;Counter").c_str(),
             bins[k], minNofHits, maxNofHits);

       std::string histName4 = "hTrdLastWrongCov" + names[k];
       fTrdHistosLastWrongCov[j] = new TH1F(histName4.c_str(), std::string(histName4 + ";Number of hits;Counter").c_str(),
             bins[k], minNofHits, maxNofHits);

       std::string histName5 = "hMuchFirstWrongCov" + names[k];
       fMuchHistosFirstWrongCov[j] = new TH1F(histName5.c_str(), std::string(histName5 + ";Number of hits;Counter").c_str(),
             bins[k], minNofHits, maxNofHits);

       std::string histName6 = "hMuchLastWrongCov" + names[k];
       fMuchHistosLastWrongCov[j] = new TH1F(histName6.c_str(), std::string(histName6 + ";Number of hits;Counter").c_str(),
             bins[k], minNofHits, maxNofHits);
    }
}

void CbmLitFitQa::ProcessGlobalTracks()
{
   Int_t nofGlobalTracks = fGlobalTracks->GetEntriesFast();
   for (Int_t iTrack = 0; iTrack < nofGlobalTracks; iTrack++) {
      const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(fGlobalTracks->At(iTrack));
      ProcessStsTrack(globalTrack->GetStsTrackIndex());
      ProcessTrdTrack(globalTrack->GetTrdTrackIndex());
      ProcessMuchTrack(globalTrack->GetMuchTrackIndex());
   }
}

void CbmLitFitQa::ProcessStsTrack(
      Int_t trackId)
{
   if (NULL == fStsTracks || NULL == fStsTrackMatches || trackId < 0) return;

   CbmTrackMatch* match = static_cast<CbmTrackMatch*>(fStsTrackMatches->At(trackId));
   Int_t mcId = match->GetMCTrackId();
   if (mcId < 0) return; // Ghost or fake track

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
         FillResidualsAndPulls(firstParam, &firstPoint, fStsHistosFirst, fStsHistosFirstWrongCov, nofMvdHits + nofStsHits);
      }
   } else { // first track parameters in STS
      const CbmStsHit* firstHit = static_cast<const CbmStsHit*>(fStsHits->At(track->GetStsHitIndex(0)));
      Int_t firstStation = firstHit->GetStationNr() - 1; // to start with 0
      if (mcTrack.GetNofPointsAtStation(kSTS, firstStation) > 0) {
         const CbmLitMCPoint& firstPoint = mcTrack.GetPointAtStation(kSTS, firstStation, 0);
         FillResidualsAndPulls(firstParam, &firstPoint, fStsHistosFirst, fStsHistosFirstWrongCov, nofMvdHits + nofStsHits);
      }
   }

   // Fill histograms for last track parameters
   const CbmStsHit* lastHit = static_cast<const CbmStsHit*>(fStsHits->At(track->GetStsHitIndex(nofStsHits - 1)));
   Int_t lastStation = lastHit->GetStationNr() - 1; // to start with 0
   if (mcTrack.GetNofPointsAtStation(kSTS, lastStation) > 0) {
      const CbmLitMCPoint& lastPoint = mcTrack.GetPointAtStation(kSTS, lastStation, 0);
      FillResidualsAndPulls(lastParam, &lastPoint, fStsHistosLast, fStsHistosLastWrongCov, nofMvdHits + nofStsHits);
   }
}

void CbmLitFitQa::ProcessTrdTrack(
      Int_t trackId)
{
   if (NULL == fTrdTracks || NULL == fTrdTrackMatches || trackId < 0) return;

   CbmTrackMatch* match = static_cast<CbmTrackMatch*>(fTrdTrackMatches->At(trackId));
   Int_t mcId = match->GetMCTrackId();
   if (mcId < 0) return; // Ghost or fake track

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
      FillResidualsAndPulls(firstParam, &firstPoint, fTrdHistosFirst, fTrdHistosFirstWrongCov, nofHits);
   }

   // Fill histograms for last track parameters
   const CbmBaseHit* lastHit = static_cast<const CbmBaseHit*>(fTrdHits->At(track->GetHitIndex(nofHits - 1)));
   detInfo = trdDetectorId.GetDetectorInfo(lastHit->GetDetectorId());
   Int_t lastStation = 10 * detInfo[1] + detInfo[2];
   if (mcTrack.GetNofPointsAtStation(kTRD, lastStation) > 0) {
      const CbmLitMCPoint& lastPoint = mcTrack.GetPointAtStation(kTRD, lastStation, 0);
      FillResidualsAndPulls(lastParam, &lastPoint, fTrdHistosLast, fTrdHistosLastWrongCov, nofHits);
   }
}

void CbmLitFitQa::ProcessMuchTrack(
      Int_t trackId)
{
   if (NULL == fMuchTracks || NULL == fMuchTrackMatches || trackId < 0) return;

   CbmTrackMatch* match = static_cast<CbmTrackMatch*>(fMuchTrackMatches->At(trackId));
   Int_t mcId = match->GetMCTrackId();
   if (mcId < 0) return; // Ghost or fake track

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
      FillResidualsAndPulls(firstParam, &firstPoint, fMuchHistosFirst, fMuchHistosFirstWrongCov, nofHits);
   }

   // Fill histograms for last track parameters
   const CbmBaseHit* lastHit = static_cast<const CbmBaseHit*>(fMuchPixelHits->At(track->GetHitIndex(nofHits - 1)));
//   Int_t lastStation = lastHit->GetPlaneId();
   Int_t lastStation = 100 * CbmMuchGeoScheme::GetStationIndex(lastHit->GetDetectorId())
              + 10 * CbmMuchGeoScheme::GetLayerIndex(lastHit->GetDetectorId())
              + CbmMuchGeoScheme::GetLayerSideIndex(lastHit->GetDetectorId());
   if (mcTrack.GetNofPointsAtStation(kMUCH, lastStation) > 0) {
      const CbmLitMCPoint& lastPoint = mcTrack.GetPointAtStation(kMUCH, lastStation, 0);
      FillResidualsAndPulls(lastParam, &lastPoint, fMuchHistosLast, fMuchHistosLastWrongCov, nofHits);
   }
}

void CbmLitFitQa::FillResidualsAndPulls(
   const FairTrackParam* par,
   const CbmLitMCPoint* mcPoint,
   std::vector<TH1F*>& histos,
   std::vector<TH1F*>& wrongCov,
   Float_t wrongPar)
{
   // Residuals
   Float_t res[5] = {0., 0., 0., 0., 0.}; // [x, y, tx, ty, qp]
   res[0] = par->GetX() - mcPoint->GetX();
   res[1] = par->GetY() - mcPoint->GetY();
   res[2] = par->GetTx() - mcPoint->GetTx();
   res[3] = par->GetTy() - mcPoint->GetTy();
   res[4] = par->GetQp() - mcPoint->GetQp();
   for (Int_t i = 0; i < 5; i++) { histos[i]->Fill(res[i]); }

   // Pulls
   Double_t C[15];
   par->CovMatrix(C);

   Float_t sigma[5] = {0., 0., 0., 0., 0.}; // [x, y, tx, ty, qp]
   sigma[0] = (C[0] > 0.) ? std::sqrt(C[0]) : -1.;
   sigma[1] = (C[5] > 0.) ? std::sqrt(C[5]) : -1.;
   sigma[2] = (C[9] > 0.) ? std::sqrt(C[9]) : -1.;
   sigma[3] = (C[12] > 0.) ? std::sqrt(C[12]) : -1.;
   sigma[4] = (C[14] > 0.) ? std::sqrt(C[14]) : -1.;
   for (Int_t i = 0; i < 5; i++) {
      if (sigma[i] < 0) { // wrong covariance
         wrongCov[i]->Fill(wrongPar);
      } else { // good covariance
         histos[i + 5]->Fill(res[i] / sigma[i]);
      }
   }
}

void CbmLitFitQa::DrawHistos(
      const std::string& name,
      std::vector<TH1F*>& histos,
      std::vector<TH1F*>& wrongHistos)
{
   TCanvas* canvas = new TCanvas(name.c_str(), name.c_str(), 1400, 900);
   canvas->Divide(5, 3);

   for (int i = 0; i < NOF_PARAMS; i++) {
      canvas->cd(i+1);
      TH1F* hist = histos[i];
      hist->Fit("gaus");
      hist->SetMaximum(hist->GetMaximum() * 1.50);
      DrawHist1D(hist, kLitLinear, kLitLog);

      TF1* fit = hist->GetFunction("gaus");
      Double_t sigma = 0.;
      if (NULL != fit) { sigma = fit->GetParameter(2);}
      Double_t rms = hist->GetRMS();
      DrawHistSigmaRMS(i, sigma, rms);
   }

   for (int i = 0; i < NOF_PARAMS_WRONG_COV; i++) {
      canvas->cd(i + 1 + 10);
      DrawHist1D(wrongHistos[i], kLitLinear, kLitLog);
   }

   lit::SaveCanvasAsImage(canvas, fOutputDir);
}

ClassImp(CbmLitFitQa)
