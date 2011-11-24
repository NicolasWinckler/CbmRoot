#include "qa/material/CbmLitCheckMultipleScattering.h"

#include "geo/CbmLitDetPoint.h"
#include "utils/CbmLitDrawHist.h"
#include "utils/CbmLitUtils.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairMCPoint.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TF1.h"
#include "TLatex.h"

#include <iostream>
#include <cmath>
#include <sstream>

CbmLitCheckMultipleScattering::CbmLitCheckMultipleScattering():
   fNEvents(0),
   fOutputDir("eloss/")
{
}

CbmLitCheckMultipleScattering::~CbmLitCheckMultipleScattering()
{

}

InitStatus CbmLitCheckMultipleScattering::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) { Fatal("Init", "No CbmRootManager"); }

   fMCTrackArray  = (TClonesArray*) ioman->GetObject("MCTrack");
   if (!fMCTrackArray) { Fatal("Init", "No MCTrack array!"); }

   fMCPointArray  = (TClonesArray*) ioman->GetObject("LitDetPoint");
   if (!fMCPointArray) { Fatal("Init", "No LitPoint array!"); }

   Int_t nBins = 500;
   Double_t minTheta = -0.001;
   Double_t maxTheta =  0.001;
   Double_t minEloss = 0.0;
   Double_t maxEloss = 1.;

   fh_theta_mc = new TH1F("fh_theta_mc", "theta_mc", nBins, minTheta, maxTheta);
   fh_eloss_mc = new TH1F("fh_eloss_mc", "eloss_mc", nBins, minEloss, maxEloss);
   fh_eloss_rec = new TH1F("fh_eloss_rec", "eloss_rec", nBins, minEloss, maxEloss);
   fh_dep_energy = new TH1F("fh_dep_energy", "dep_energy", nBins, minEloss, maxEloss);

   return kSUCCESS;
}

void CbmLitCheckMultipleScattering::SetParContainers()
{
}

void CbmLitCheckMultipleScattering::Exec(
   Option_t* opt)
{
   Int_t nofTracks = fMCTrackArray->GetEntriesFast();
// std::cout << "nofTracks=" << nofTracks << std::endl;
   for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
      CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTrackArray->At(iTrack);
      Int_t motherId = mcTrack->GetMotherId();
      if (motherId != -1) { continue; }

      CbmLitDetPoint* point = NULL;

      Int_t nofPoints = fMCPointArray->GetEntriesFast();
      for (Int_t iPoint = 0; iPoint < nofPoints; iPoint++) {
         CbmLitDetPoint* pnt = (CbmLitDetPoint*) fMCPointArray->At(iPoint);
         Int_t trackId = pnt->GetTrackID();
         if (trackId != iTrack) { continue; }
         point = pnt;
         break;
      }
      if (point == NULL) { continue; }

      TVector3 mom, momOut;
      point->Momentum(mom);
      point->MomentumOut(momOut);

      // Material properties
      Double_t length = 0.1; //cm
//        Double_t X0 = 1.76; // cm - iron
      Double_t X0 = 9.34961; // cm -silicon

      // Particle properties
      Double_t mass = 0.139; // GeV
      Double_t p = mom.Mag();
      Double_t E = std::sqrt(mass * mass + p * p);
//    Double_t beta = p / E;
//    Double_t bcp = beta * p;

      Double_t mc_theta = momOut.Theta() * std::cos(momOut.Phi());
//       std::cout << "mc_theta=" << mc_theta << std::endl;
      fh_theta_mc->Fill(mc_theta);

//        myf theta = 0.0136 * (1./bcp) * 1. * std::sqrt(length/X0) *
//                      (1. + 0.038 * std::log(length/X0));
//        std::cout << "calc_theta=" << theta << std::endl;


      Double_t mc_eloss = mom.Mag() - momOut.Mag();
//        std::cout << "mc_eloss=" << mc_eloss << std::endl;
      fh_eloss_mc->Fill(mc_eloss);

      Double_t dep_energy = point->GetEnergyLoss();
//        std::cout << "dep_energy=" << dep_energy << std::endl;
      fh_dep_energy->Fill(dep_energy);

//        Double_t length = 0.1;
//        Double_t p = mom.Mag();
//        Double_t m = 0.000511;
//        Double_t initialE = std::sqrt(p*p+m*m);
//        Double_t X0 = 9.34961;
      Double_t dEdX = E / X0;
      Double_t rec_eloss = dEdX * length;

//        std::cout << "rec_eloss=" << rec_eloss << std::endl;
      fh_eloss_rec->Fill(rec_eloss);
   } //loop over MC tracks

   std::cout << "Event no: " << fNEvents++ << std::endl;
}

void CbmLitCheckMultipleScattering::Finish()
{
   Draw();
}

void CbmLitCheckMultipleScattering::Draw()
{
   SetStyles();

   TCanvas* canvas1 = new TCanvas("multiple_scattering_theta", "multiple_scattering_theta", 700, 500);
   canvas1->cd(1);
   DrawHist1D(fh_theta_mc, "projected scattering angle [rad]", "counter", kLitLinear,
         kLitLog, "", kBlue, 2, 1, 1, kDot);

   fh_theta_mc->Fit("gaus");
   TF1* fit1 = fh_theta_mc->GetFunction("gaus");
   Double_t sigma = fit1->GetParameter(2);
   std::stringstream oss1;
   oss1.precision(3);
   oss1 << "#sigma _{fit}=" << sigma;
   TLatex text;
// text.SetTextAlign(21);
   text.SetTextSize(0.065); //0.1
   text.DrawLatex(0.0003, 1000, oss1.str().c_str());
   text.DrawLatex(0.0003, 400, "#sigma _{calc}=0.000116");
// text.DrawTextNDC(0.8, 0.7, oss2.str().c_str());

   lit::SaveCanvasAsImage(canvas1, fOutputDir);

   TCanvas* canvas2 = new TCanvas("energy_loss", "energy_loss", 700, 500);
   canvas2->cd(1);
   std::string eloss_mc = "MC(" + lit::ToString<Double_t>(fh_eloss_mc->GetMean()) + ")";
   std::string eloss_rec = "Reco(" + lit::ToString<Double_t>(fh_eloss_rec->GetMean()) + ")";
   DrawHist1D(fh_eloss_mc, fh_eloss_rec, NULL, NULL,
              "Energy loss", "energy loss [GeV/c]", "counter",
              eloss_mc, eloss_rec, "","", kLitLog, kLitLog, true, 0.6, 0.6, 0.9, 0.9);

   lit::SaveCanvasAsImage(canvas2, fOutputDir);
}

ClassImp(CbmLitCheckMultipleScattering)
