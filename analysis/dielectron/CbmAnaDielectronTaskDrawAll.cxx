/** CbmAnaDielectronTaskDrawAll.cxx
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2011
 * @version 1.0
 **/

#include "CbmAnaDielectronTaskDrawAll.h"

#include "cbm/utils/CbmLitDrawHist.h"
#include "std/utils/CbmLitUtils.h"

#include <string>
#include <iostream>
#include <iomanip>

#include <boost/assign/list_of.hpp>

#include "TText.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TMath.h"
#include "TKey.h"
#include "TClass.h"
#include "TF1.h"
#include "TEllipse.h"
#include "TStyle.h"
#include "TCanvas.h"


using namespace std;
using namespace lit;
using boost::assign::list_of;



void CbmAnaDielectronTaskDrawAll::DrawHistosFromFile(
      const string& fileNameRho0,
      const string& fileNameOmega,
      const string& fileNamePhi,
      const string& fileNameOmegaDalitz,
      Bool_t useMvd)
{
   fFileNames.clear();
   //[0]=rho0, [1]=omega, [2]=phi, [3]=omegaDalitz
   fFileNames.push_back(fileNameRho0);
   fFileNames.push_back(fileNameOmega);
   fFileNames.push_back(fileNamePhi);
   fFileNames.push_back(fileNameOmegaDalitz);

   fNames.push_back("rho0");
   fNames.push_back("omega");
   fNames.push_back("phi");
   fNames.push_back("omegaD");

   fCutNames.push_back("ID");
   fCutNames.push_back("m_{#gamma}");
   fCutNames.push_back("dsts");
   fCutNames.push_back("dsts2");
   fCutNames.push_back("ST");
   fCutNames.push_back("TT");
   fCutNames.push_back("P_{t}");

   // first index: [0]=rho0, [1]=omega, [2]=phi, [3]=omegaDalitz
   // second index: [0]=signal, [1]=background, [2]=eta, [3]=pi0
   // third index: [0]=el_id, [1]=gammacut, [2]=dstscut, [3]=dsts2cut, [4]=stcut, [5]=ttcut, [6]=ptcut
   fh_minv.resize(fNofSignals);
   for (int i = 0; i < fNofSignals; i++){
      fh_minv[i].resize(4);
      for (int j = 0; j < 4; j++){
         fh_minv[i][j].resize(fNofCuts);
      }
   }

   // first index: [0]=signal(only for convenience), [1]=background, [2]=eta, [3]=pi0
   // second index: [0]=el_id, [1]=gammacut, [2]=dstscut, [3]=dsts2cut, [4]=stcut, [5]=ttcut, [6]=ptcut
   fh_mean_minv.resize(4);
   for (int i = 0; i < 4; i++){
      fh_mean_minv[i].resize(fNofCuts);
   }

   fh_sum_signals_minv.resize(fNofCuts);

   fh_pty.resize(fNofSignals);
   for (int i = 0; i < fNofSignals; i++){
      fh_pty[i].resize(fNofCuts);
   }

   fh_mc_pty.resize(fNofSignals);

   // loop over all files
   for (int iF = 0; iF < fNofSignals; iF++){
      TFile *file = new TFile(fFileNames[iF].c_str());
      Int_t nofEvents = ((TH1D*)file->Get("fh_event_number"))->GetEntries();
      ScaleAllHistogramms(file, nofEvents);
      for (int iH = 0; iH < 4; iH++){
         string histType = "";
         switch(iH){
            case 0: histType = "signal"; break;
            case 1: histType = "bg"; break;
            case 2: histType = "eta"; break;
            case 3: histType = "pi0"; break;
            default: histType = ""; break;
         }

         for (int iC = 0; iC < fNofCuts; iC++){
            string cutType = "";
            switch(iC){
               case 0: cutType = "el_id"; break;
               case 1: cutType = "gammacut"; break;
               case 2: cutType = "dstscut"; break;
               case 3: cutType = "dsts2cut"; break;
               case 4: cutType = "stcut"; break;
               case 5: cutType = "ttcut"; break;
               case 6: cutType = "ptcut"; break;
               default: cutType = ""; break;
            }
            fh_minv[iF][iH][iC] = (TH1D*)file->Get(string("fh_"+cutType+"_"+histType+"_minv").c_str())->Clone();
            if(iH == 0){
               fh_pty[iF][iC] = (TH1D*)file->Get(string("fh_"+cutType+"_"+histType+"_pty").c_str())->Clone();
            }

         }// iC
      } // iH
      fh_mc_pty[iF] = (TH1D*)file->Get("fh_mc_signal_pty")->Clone();

   }// iF

   FillMeanHist();

   FillSumSignalsHist();

   CalcCutEff(0.0, 0.2);
   CalcCutEff(0.2, 0.6);
   CalcCutEff(0.6, 1.2);

   DrawSBGRegion();

   DrawSBGSignals();

   Draw();
}

void CbmAnaDielectronTaskDrawAll::ScaleAllHistogramms(
      TFile* file,
      Int_t nofEvents)
{
   cout << "Scale all histogramms:" << endl;
   TDirectory * dir = gDirectory;
   TIter nextkey( dir->GetListOfKeys());
   TKey *key;

   Int_t count = 0;
   while (key = (TKey*) nextkey()) {
      TObject* obj = key->ReadObj();

      if (obj->IsA()->InheritsFrom (TH1::Class())) {
         TH1* h = (TH1*) obj;
         //cout << h->GetName() << endl;
         TH1* h1 = (TH1*)file->Get(h->GetName());
         h1->Scale(1./(Double_t)nofEvents);
         count++;
      }
   }
   cout << count << " histogramms were scaled." << endl;
}

void CbmAnaDielectronTaskDrawAll::Draw()
{
   TCanvas *c10 = new TCanvas("minv_all", "minv_all", 1200, 800);
   c10->Divide(3,2);
   c10->cd(1);
   DrawMinv(1);
   c10->cd(2);
   DrawMinv(2);
   c10->cd(3);
   DrawMinv(3);
   c10->cd(4);
   DrawMinv(4);
   c10->cd(5);
   DrawMinv(5);
   c10->cd(6);
   DrawMinv(6);
}

void CbmAnaDielectronTaskDrawAll::DrawMinv(
      int cutType)
{
   if (cutType < 0 || cutType > 6){
      cout << "-E- CbmAnaDielectronTaskDrawAll::DrawMinv , cutType must be [0, 6]" << endl;
   }
   TH1D* sRho = (TH1D*)fh_minv[0][0][cutType]->Clone();
   sRho->SetFillColor(kMagenta-3);
   sRho->SetLineColor(kBlack);

   TH1D* sOmega = (TH1D*)fh_minv[1][0][cutType]->Clone();
   sOmega->SetFillColor(kOrange+7);
   sOmega->SetLineColor(kBlack);

   TH1D* sPhi = (TH1D*)fh_minv[2][0][cutType]->Clone();
   sPhi->SetFillColor(kRed+1);
   sPhi->SetLineColor(kBlack);

   TH1D* bg = fh_mean_minv[1][cutType];
   bg->SetFillColor(kYellow-10);
   bg->SetLineColor(kBlack);

   TH1D* sEta = fh_mean_minv[2][cutType];
   sEta->SetFillColor(kAzure+2);
   sEta->SetLineColor(kBlack);

   TH1D* sPi0 = fh_mean_minv[3][cutType];
   sPi0->SetFillColor(kGreen-3);
   sPi0->SetLineColor(kBlack);

   TH1D* sOmegaDalitz = (TH1D*)fh_minv[3][0][cutType]->Clone();
   sOmegaDalitz->SetFillColor(kCyan+2);
   sOmegaDalitz->SetLineColor(kBlack);

   TH1D* sbg = (TH1D*)bg->Clone();
   sbg->SetFillColor(kBlack);
   sbg->SetLineColor(kBlack);
   sbg->Add(sRho);
   sbg->Add(sOmega);
   sbg->Add(sPhi);
   sbg->Add(sEta);
   sbg->Add(sPi0);
   sbg->Add(sOmegaDalitz);
   sbg->SetMinimum(1e-8);
   sbg->SetMaximum(1e-2);

   sbg->Rebin(10);
   bg->Rebin(10);
   sPi0->Rebin(10);
   sEta->Rebin(10);
   sOmegaDalitz->Rebin(10);
   sOmega->Rebin(10);
   sRho->Rebin(10);
   sPhi->Rebin(10);

   sbg->Draw();
   bg->Draw("same");
   sPi0->Draw("same");
   sEta->Draw("same");
   sOmegaDalitz->Draw("same");
   sOmega->Draw("same");
   sRho->Draw("same");
   sPhi->Draw("same");

   gPad->SetGridx(true);
   gPad->SetGridy(true);
   gPad->SetLogy(true);
}

void CbmAnaDielectronTaskDrawAll::FillMeanHist()
{
   // [0] is not needed as it represents signal
   for (int iH = 1; iH < 4; iH++){
      for (int iC = 0; iC < fNofCuts; iC++){
         TH1D* mean = (TH1D*)fh_minv[0][iH][iC]->Clone();
         mean->Add((TH1D*)fh_minv[1][iH][iC]->Clone());
         mean->Add((TH1D*)fh_minv[2][iH][iC]->Clone());
         mean->Add((TH1D*)fh_minv[3][iH][iC]->Clone());
         mean->Scale(1./4.);
         fh_mean_minv[iH][iC] = mean;
      }
   }
}

void CbmAnaDielectronTaskDrawAll::FillSumSignalsHist()
{
   for (int i = 0; i < fNofCuts; i++){
      fh_sum_signals_minv[i] = (TH1D*)fh_minv[0][0][i]->Clone(); // rho0
      fh_sum_signals_minv[i]->Add( (TH1D*)fh_minv[1][0][i]->Clone() ); // omega
      fh_sum_signals_minv[i]->Add( (TH1D*)fh_minv[2][0][i]->Clone() ); // phi
      fh_sum_signals_minv[i]->Add( (TH1D*)fh_mean_minv[2][i]->Clone() ); // eta
      fh_sum_signals_minv[i]->Add( (TH1D*)fh_mean_minv[3][i]->Clone() ); // pi0
      fh_sum_signals_minv[i]->Add( (TH1D*)fh_minv[3][0][i]->Clone() );// omega Dalitz
   }
}

void CbmAnaDielectronTaskDrawAll::CalcCutEff(
      Double_t min,
      Double_t max)
{
    Double_t yS[fNofCuts];
    Double_t yB[fNofCuts];
    for (int iC = 0; iC < fNofCuts; iC++){
       Int_t x1bin = fh_sum_signals_minv[iC]->FindBin(min);
       Int_t x2bin = fh_sum_signals_minv[iC]->FindBin(max);
       yS[iC] = fh_sum_signals_minv[iC]->Integral(x1bin, x2bin);
       yB[iC] = fh_mean_minv[1][iC]->Integral(x1bin, x2bin);
    }

    Double_t y1[fNofCuts];
    Double_t y2[fNofCuts];
    for (int iC = 0; iC < fNofCuts; iC++){
       y1[iC] = 100. * yS[iC] / yS[0];
       y2[iC] = 100. * yB[iC] / yB[0];
    }

    TH1D* grS = new TH1D("grS", "grS;Cuts;Efficiency [%]", fNofCuts, 0, fNofCuts);
    TH1D* grBg = new TH1D("grBg", "grBg;Cuts;Efficiency [%]", fNofCuts, 0, fNofCuts);

    grBg->GetXaxis()->SetLabelSize(0.06);
    for (Int_t i = 1; i <= fNofCuts; i++){
       grBg->GetXaxis()->SetBinLabel(i, fCutNames[i-1].c_str());
       grBg->SetBinContent(i, y2[i-1]);
       grS->SetBinContent(i, y1[i-1]);
    }

    stringstream ss;
    ss << "cut_eff_" << min << "_" << max;
    TCanvas* can = new TCanvas(ss.str().c_str(), ss.str().c_str(), 600, 600);
    grS->SetLineWidth(4);
    grBg->SetLineWidth(4);
    grS->SetLineColor(kRed);
    grBg->SetLineColor(kBlue);
    grBg->SetMinimum(1);
    grBg->SetMaximum(105);
    grBg->Draw("");
    grS->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    TLegend* leg = new TLegend(0.75, 0.85, 1.0, 1.0);
    leg->AddEntry(grS, "signal", "l");
    leg->AddEntry(grBg, "BG", "l");
    leg->Draw();

    stringstream ss2;
    ss2 << min <<"<M [GeV/c^2]<" << max;
    TText *t = new TText(0.1, 110, ss2.str().c_str());
    t->Draw();
}


TH1D* CbmAnaDielectronTaskDrawAll::CreateSBGRegionHist(
      Double_t min,
      Double_t max)
{
   Double_t y[fNofCuts];
   for (int iC = 0; iC < fNofCuts; iC++){
      Int_t bin1 = fh_sum_signals_minv[iC]->FindBin(min);
      Int_t bin2 = fh_sum_signals_minv[iC]->FindBin(max);
      y[iC] = fh_sum_signals_minv[iC]->Integral(bin1, bin2) / fh_mean_minv[1][iC]->Integral(bin1, bin2);
   }
   stringstream ss;
   ss << "dielectron_s_bg_region_" << min << "_" << max;
   TH1D* h_s_bg = new TH1D(ss.str().c_str(), string(ss.str()+";Cuts;S/BG").c_str(), fNofCuts, 0, fNofCuts);
   h_s_bg->GetXaxis()->SetLabelSize(0.06);
   for (Int_t i = 1; i <= fNofCuts; i++){
      h_s_bg->GetXaxis()->SetBinLabel(i, fCutNames[i-1].c_str());
      h_s_bg->SetBinContent(i, y[i-1]);
   }
   return h_s_bg;
}

void CbmAnaDielectronTaskDrawAll::DrawSBGRegion()
{
   TH1D* h_00_02 = CreateSBGRegionHist(0.0, 0.2);
   TH1D* h_02_06 = CreateSBGRegionHist(0.2, 0.6);
   TH1D* h_06_12 = CreateSBGRegionHist(0.6, 1.2);

   TCanvas* can = new TCanvas("s_bg_regions", "s_bg_regions", 600, 600);
   DrawH1(list_of(h_00_02)(h_02_06)(h_06_12),
         list_of("0.0<M [GeV/c^{2}]<0.2")("0.2<M [GeV/c^{2}]<0.6")("0.6<M [GeV/c^{2}]<1.2"),
         kLitLinear, kLitLog, true, 0.7, 0.7, 0.99, 0.99);

   h_00_02->SetMinimum(1e-3);
   h_00_02->SetMaximum(2);
   h_00_02->SetLineWidth(4);
   h_02_06->SetLineWidth(4);
   h_06_12->SetLineWidth(4);
   gPad->SetGridx(true);
   gPad->SetGridy(true);
}

void CbmAnaDielectronTaskDrawAll::DrawSBGSignals()
{
   Double_t y[fNofCuts];
   TCanvas* cFit = new TCanvas("dielectron_signal_fit", "dielectron_signal_fit", 600, 600);
   for (int iF = 0; iF < fNofSignals; iF++){
      cout << "Signal: " << fNames[iF] << endl;
      stringstream ss;
      ss << "dielectron_s_bg_cuts_" << fNames[iF];

      TH1D* h_s_bg = new TH1D(ss.str().c_str(), string(ss.str()+";Cuts;S/BG").c_str(), fNofCuts, 0, fNofCuts);
      h_s_bg->GetXaxis()->SetLabelSize(0.06);
      h_s_bg->SetLineWidth(4);
      for (int iC = 0; iC < fNofCuts; iC++){
         TH1D* s = (TH1D*)fh_minv[iF][0][iC]->Clone();
         TH1D* bg = (TH1D*)fh_mean_minv[1][iC]->Clone(); // BG
         cFit->cd();
         s->Fit("gaus", "Q");

         Double_t mean = s->GetFunction("gaus")->GetParameter("Mean");
         Double_t sigma = s->GetFunction("gaus")->GetParameter("Sigma");

         Int_t minInd = s->FindBin(mean - 2.*sigma);
         Int_t maxInd = s->FindBin(mean + 2.*sigma);

         Double_t sumSignal = 0.;
         Double_t sumBg = 0.;
         for (Int_t i = minInd + 1; i <= maxInd - 1; i++){
            sumSignal += s->GetBinContent(i);
            sumBg += bg->GetBinContent(i);
         }
         Double_t sbg = sumSignal/sumBg;
         Double_t eff = 100. * fh_pty[iF][iC]->GetEntries() / fh_mc_pty[iF]->GetEntries();
         cout.precision(3);
         cout << left << setw(10) << fCutNames[iC]
              << left << setw(10) << sbg
              << left << setw(10) << eff << endl;

         h_s_bg->GetXaxis()->SetBinLabel(iC+1, fCutNames[iC].c_str());
         h_s_bg->SetBinContent(iC+1, sbg);
      }
      TCanvas* c = new TCanvas(ss.str().c_str(), ss.str().c_str(), 600, 600);
      DrawH1(h_s_bg);
      gPad->SetGridx(true);
      gPad->SetGridy(true);
   }
}

ClassImp(CbmAnaDielectronTaskDrawAll);
