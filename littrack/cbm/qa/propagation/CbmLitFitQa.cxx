/**
 * \file CbmLitFitQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitFitQa.h"
#include "CbmLitFitQaPTreeCreator.h"
#include "CbmLitFitQaReport.h"
#include "../mc/CbmLitMCTrackCreator.h"
#include "../base/CbmLitHistManager.h"
#include "../base/CbmLitResultChecker.h"
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
#include "TSystem.h"

#include <boost/assign/list_of.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;
using std::make_pair;

CbmLitFitQa::CbmLitFitQa():
   fIsFixedBounds(true),
   fMvdMinNofHits(0),
   fStsMinNofHits(0),
   fTrdMinNofHits(0),
   fMuchMinNofHits(0),
   fOutputDir("./test/"),
   fQuota(0.7)
{

}

CbmLitFitQa::~CbmLitFitQa()
{
   if (fHM) delete fHM;
}

InitStatus CbmLitFitQa::Init()
{
   fHM = new CbmLitHistManager();
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
      DrawHistos("fit_qa_sts_first_param", "hStsFirst", "hStsFirstWrongCov");
      DrawHistos("fit_qa_sts_last_param", "hStsLast", "hStsLastWrongCov");
   }
   if (NULL != fTrdTracks) {
      DrawHistos("fit_qa_trd_first_param", "hTrdFirst", "hTrdFirstWrongCov");
      DrawHistos("fit_qa_trd_last_param", "hTrdLast", "hTrdLastWrongCov");
   }
   if (NULL != fMuchTracks) {
      DrawHistos("fit_qa_much_first_param", "hMuchFirst", "hMuchFirstWrongCov");
      DrawHistos("fit_qa_much_last_param", "hMuchLast", "hMuchLastWrongCov");
   }

   fHM->WriteToFile();

   string qaFile = fOutputDir + "/fit_qa.json";
   string idealFile = string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/propagation/fit_qa_ideal.json");
   string checkFile = fOutputDir + "/fit_qa_check.json";

   CbmLitFitQaPTreeCreator ptc;
   ptree qa = ptc.Create(fHM);
   if (fOutputDir != "") { write_json(qaFile.c_str(), qa); }

   CbmLitResultChecker qaChecker;
   qaChecker.DoCheck(qaFile, idealFile, checkFile);

   CreateSimulationReport("Fit QA", fOutputDir);
}

void CbmLitFitQa::SetParContainers()
{

}

void CbmLitFitQa::CreateSimulationReport(
      const string& title,
      const string& resultDirectory)
{
   CbmLitSimulationReport* report = new CbmLitFitQaReport();
   report->SetTitle(title);
   ofstream foutHtml(string(fOutputDir + "/fit_qa.html").c_str());
   ofstream foutLatex(string(fOutputDir + "/fit_qa.tex").c_str());
   ofstream foutText(string(fOutputDir + "/fit_qa.txt").c_str());
//   report.Create(kLitText, cout, resultDirectory);
   report->Create(kLitHtml, foutHtml, resultDirectory);
   report->Create(kLitLatex, foutLatex, resultDirectory);
   report->Create(kLitText, foutText, resultDirectory);
   delete report;
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
   std::string names[] = {
         "ResX", "ResY", "ResTx", "ResTy", "ResQp",
         "PullX", "PullY", "PullTx", "PullTy", "PullQp"
   };

   // Axis titles for residual and pull histograms
   std::string xtitles[] = {
       "Residual X [cm]", "Residual Y [cm]", "Residual Tx", "Residual Ty", "Residual q/p [(GeV/c)^{-1}]",
       "Pull X", "Pull Y", "Pull Tx", "Pull Ty", "Pull q/p"
   };

   // Histogram names for residuals and pulls
   std::string rpHistNames[] = {
         "hStsFirst", "hStsLast", "hTrdFirst", "hTrdLast","hMuchFirst", "hMuchLast"
   };

   // Histogram names for wrong parameters
   std::string wHistNames[] = {
         "hStsFirstWrongCov", "hStsLastWrongCov", "hTrdFirstWrongCov", "hTrdLastWrongCov", "hMuchFirstWrongCov", "hMuchLastWrongCov"
   };

   std::vector<Int_t> bins(NOF_PARAMS, 200);
   std::vector<std::pair<Float_t, Float_t> > bounds;
   if (fIsFixedBounds) {
      bounds = boost::assign::list_of
            (make_pair(-1., 1.)) // X residual
            (make_pair(-1., 1.)) // Y residual
            (make_pair(-.01, .01)) // Tx residual
            (make_pair(-.01, .01)) // Ty residual
            (make_pair(-.1, .1)) // Qp residual
            (make_pair(-5., 5.)) // X pull
            (make_pair(-5., 5.)) // Y pull
            (make_pair(-5., 5.)) // Tx pull
            (make_pair(-5., 5.)) // Ty pull
            (make_pair(-7., 7.)); // Qp pull
   } else {
      bounds.assign(NOF_PARAMS, make_pair(0.,0.));
   }

   Int_t minNofHits = 0;
   Int_t maxNofHits = 20;

   for (Int_t iHist = 0; iHist < 6; iHist++) {
      // Residual and pull histograms
      for (Int_t iPar = 0; iPar < NOF_PARAMS; iPar++) {
         std::string histName = rpHistNames[iHist] + names[iPar];
         fHM->Add(histName, new TH1F(histName.c_str(), std::string(histName + ";" + xtitles[iPar] + ";Counter").c_str(),
               bins[iPar], bounds[iPar].first, bounds[iPar].second));
      }

      // Number of wrong parameters histograms
      for (Int_t iPar = 0; iPar < NOF_PARAMS_WRONG_COV; iPar++) {
         std::string histName = wHistNames[iHist] + names[iPar];
         fHM->Add(histName, new TH1F(histName.c_str(), std::string(histName + ";Number of hits;Counter").c_str(),
               bins[iPar], minNofHits, maxNofHits));
      }
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
         FillResidualsAndPulls(firstParam, &firstPoint, "hStsFirst", "hStsFirstWrongCov", nofMvdHits + nofStsHits, kMVD);
      }
   } else { // first track parameters in STS
      const CbmStsHit* firstHit = static_cast<const CbmStsHit*>(fStsHits->At(track->GetStsHitIndex(0)));
      Int_t firstStation = firstHit->GetStationNr() - 1; // to start with 0
      if (mcTrack.GetNofPointsAtStation(kSTS, firstStation) > 0) {
         const CbmLitMCPoint& firstPoint = mcTrack.GetPointAtStation(kSTS, firstStation, 0);
         FillResidualsAndPulls(firstParam, &firstPoint, "hStsFirst", "hStsFirstWrongCov", nofMvdHits + nofStsHits, kSTS);
      }
   }

   // Fill histograms for last track parameters
   const CbmStsHit* lastHit = static_cast<const CbmStsHit*>(fStsHits->At(track->GetStsHitIndex(nofStsHits - 1)));
   Int_t lastStation = lastHit->GetStationNr() - 1; // to start with 0
   if (mcTrack.GetNofPointsAtStation(kSTS, lastStation) > 0) {
      const CbmLitMCPoint& lastPoint = mcTrack.GetPointAtStation(kSTS, lastStation, 0);
      FillResidualsAndPulls(lastParam, &lastPoint, "hStsLast", "hStsLastWrongCov", nofMvdHits + nofStsHits, kSTS);
   }
}

void CbmLitFitQa::ProcessTrdTrack(
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
      FillResidualsAndPulls(firstParam, &firstPoint, "hTrdFirst", "hTrdFirstWrongCov", nofHits, kTRD);
   }

   // Fill histograms for last track parameters
   const CbmBaseHit* lastHit = static_cast<const CbmBaseHit*>(fTrdHits->At(track->GetHitIndex(nofHits - 1)));
   detInfo = trdDetectorId.GetDetectorInfo(lastHit->GetDetectorId());
   Int_t lastStation = 10 * detInfo[1] + detInfo[2];
   if (mcTrack.GetNofPointsAtStation(kTRD, lastStation) > 0) {
      const CbmLitMCPoint& lastPoint = mcTrack.GetPointAtStation(kTRD, lastStation, 0);
      FillResidualsAndPulls(lastParam, &lastPoint, "hTrdLast", "hTrdLastWrongCov", nofHits, kTRD);
   }
}

void CbmLitFitQa::ProcessMuchTrack(
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
      FillResidualsAndPulls(firstParam, &firstPoint, "hMuchFirst", "hMuchFirstWrongCov", nofHits, kMUCH);
   }

   // Fill histograms for last track parameters
   const CbmBaseHit* lastHit = static_cast<const CbmBaseHit*>(fMuchPixelHits->At(track->GetHitIndex(nofHits - 1)));
//   Int_t lastStation = lastHit->GetPlaneId();
   Int_t lastStation = 100 * CbmMuchGeoScheme::GetStationIndex(lastHit->GetDetectorId())
              + 10 * CbmMuchGeoScheme::GetLayerIndex(lastHit->GetDetectorId())
              + CbmMuchGeoScheme::GetLayerSideIndex(lastHit->GetDetectorId());
   if (mcTrack.GetNofPointsAtStation(kMUCH, lastStation) > 0) {
      const CbmLitMCPoint& lastPoint = mcTrack.GetPointAtStation(kMUCH, lastStation, 0);
      FillResidualsAndPulls(lastParam, &lastPoint, "hMuchLast", "hMuchLastWrongCov", nofHits, kMUCH);
   }
}

void CbmLitFitQa::FillResidualsAndPulls(
   const FairTrackParam* par,
   const CbmLitMCPoint* mcPoint,
   const string& histName,
   const string& wrongName,
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
   fHM->H1(histName + "ResX")->Fill(resX);
   fHM->H1(histName + "ResY")->Fill(resY);
   fHM->H1(histName + "ResTx")->Fill(resTx);
   fHM->H1(histName + "ResTy")->Fill(resTy);
   fHM->H1(histName + "ResQp")->Fill(resQp);

   // Pulls
   Double_t C[15];
   par->CovMatrix(C);

   Double_t sigmaX = (C[0] > 0.) ? std::sqrt(C[0]) : -1.;
   Double_t sigmaY = (C[5] > 0.) ? std::sqrt(C[5]) : -1.;
   Double_t sigmaTx = (C[9] > 0.) ? std::sqrt(C[9]) : -1.;
   Double_t sigmaTy = (C[12] > 0.) ? std::sqrt(C[12]) : -1.;
   Double_t sigmaQp = (C[14] > 0.) ? std::sqrt(C[14]) : -1.;
   if (sigmaX < 0) fHM->H1(wrongName + "ResX")->Fill(wrongPar); else fHM->H1(histName + "PullX")->Fill(resX / sigmaX);
   if (sigmaY < 0) fHM->H1(wrongName + "ResY")->Fill(wrongPar); else fHM->H1(histName + "PullY")->Fill(resY / sigmaY);
   if (sigmaTx < 0) fHM->H1(wrongName + "ResTx")->Fill(wrongPar); else fHM->H1(histName + "PullTx")->Fill(resTx / sigmaTx);
   if (sigmaTy < 0) fHM->H1(wrongName + "ResTy")->Fill(wrongPar); else fHM->H1(histName + "PullTy")->Fill(resTy / sigmaTy);
   if (sigmaQp < 0) fHM->H1(wrongName + "ResQp")->Fill(wrongPar); else fHM->H1(histName + "PullQp")->Fill(resQp / sigmaQp);
}

void CbmLitFitQa::DrawHistos(
      const std::string& name,
      const string& histName,
      const string& wrongName)
{
   TCanvas* canvas = new TCanvas(name.c_str(), name.c_str(), 1400, 900);
   canvas->Divide(5, 3);

   std::string names[] = { "ResX", "ResY", "ResTx", "ResTy", "ResQp",
                           "PullX", "PullY", "PullTx", "PullTy", "PullQp" };

   for (int i = 0; i < NOF_PARAMS; i++) {
      canvas->cd(i+1);
      TH1* hist = fHM->H1(histName + names[i]);
      hist->Fit("gaus");
      hist->SetMaximum(hist->GetMaximum() * 1.50);
      DrawH1(hist, kLitLinear, kLitLog);

      TF1* fit = hist->GetFunction("gaus");
      Double_t sigma = (NULL != fit) ? fit->GetParameter(2) : 0.;
      Double_t rms = hist->GetRMS();
      DrawHistSigmaRMS(i, sigma, rms);
   }

   for (int i = 0; i < NOF_PARAMS_WRONG_COV; i++) {
      canvas->cd(i + 1 + 10);
      DrawH1(fHM->H1(wrongName + names[i]), kLitLinear, kLitLog);
   }

   lit::SaveCanvasAsImage(canvas, fOutputDir);
}

ClassImp(CbmLitFitQa)
