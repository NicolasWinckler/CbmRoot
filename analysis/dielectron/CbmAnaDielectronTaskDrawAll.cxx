/** CbmAnaDielectronTaskDrawAll.cxx
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2011
 * @version 2.0
 **/

#include "CbmAnaDielectronTaskDrawAll.h"

#include "CbmDrawHist.h"
#include "std/utils/CbmLitUtils.h"
#include "CbmHistManager.h"

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
#include "TSystem.h"
#include "TLatex.h"

#include "CbmAnaPTree.h"

using namespace std;
using namespace lit;
using boost::assign::list_of;



void CbmAnaDielectronTaskDrawAll::DrawHistosFromFile(
      const string& fileNameRho0,
      const string& fileNameOmega,
      const string& fileNamePhi,
      const string& fileNameOmegaDalitz,
      const string& outputDir,
      Bool_t useMvd)
{
   fOutputDir = outputDir;
   fPt = new CbmAnaPTree();
   fUseMvd = useMvd;

   SetDefaultDrawStyle();
   vector<string> fileNames;
   fileNames.push_back(fileNameRho0);
   fileNames.push_back(fileNameOmega);
   fileNames.push_back(fileNamePhi);
   fileNames.push_back(fileNameOmegaDalitz);

   fHM.resize(fNofSignals);
   for (int i = 0; i < fNofSignals; i++){
      fHM[i] = new CbmHistManager();
      TFile* file = new TFile(fileNames[i].c_str());
      fHM[i]->ReadFromFile(file);
      Int_t fNofEvents = (int) H1(i, "fh_event_number")->GetEntries();
      fHM[i]->ScaleByPattern(".*", 1./fNofEvents);
   }

   // index: AnalysisSteps
   fh_mean_bg_minv.resize(CbmAnaLmvmNames::fNofAnaSteps);
   fh_mean_eta_minv.resize(CbmAnaLmvmNames::fNofAnaSteps);
   fh_mean_pi0_minv.resize(CbmAnaLmvmNames::fNofAnaSteps);
   fh_sum_s_minv.resize(CbmAnaLmvmNames::fNofAnaSteps);

   FillMeanHist();
   FillSumSignalsHist();
   CalcCutEffRange(0.0, 0.2);
   CalcCutEffRange(0.2, 0.6);
   CalcCutEffRange(0.6, 1.2);
   SBgRangeAll();
   DrawSBgSignals();
   DrawMinvAll();
   SaveCanvasToImage();

   string qaFile = fOutputDir + "/lmvm_results_all.json";
   fPt->Write(qaFile);
}

TCanvas* CbmAnaDielectronTaskDrawAll::CreateCanvas(
      const string& name,
      const string& title,
      int width,
      int height)
{
   TCanvas *c = new TCanvas(name.c_str(), title.c_str(), width, height);
   fCanvas.push_back(c);
   return c;
}

TH1D* CbmAnaDielectronTaskDrawAll::H1(
      int signalType,
      const string& name)
{
   return (TH1D*) fHM[signalType]->H1(name);
}

TH2D* CbmAnaDielectronTaskDrawAll::H2(
      int signalType,
      const string& name)
{
   return (TH2D*) fHM[signalType]->H1(name);
}

void CbmAnaDielectronTaskDrawAll::DrawMinvAll()
{
   TCanvas *c = CreateCanvas("minv_all_ptcut", "minv_all_ptcut", 800, 800);
   DrawMinv(kPtCut);

   TCanvas *cTT = CreateCanvas("minv_all_ttcut", "minv_all_ttcut", 800, 800);
   DrawMinv(kTtCut);
}

void CbmAnaDielectronTaskDrawAll::DrawMinv(
      AnalysisSteps step)
{
   TH1D* sRho = (TH1D*) H1(kRho0, "fh_signal_minv_" + CbmAnaLmvmNames::fAnaSteps[step])->Clone();
   TH1D* sOmega = (TH1D*) H1(kOmega, "fh_signal_minv_" + CbmAnaLmvmNames::fAnaSteps[step])->Clone();
   TH1D* sPhi = (TH1D*) H1(kPhi, "fh_signal_minv_" + CbmAnaLmvmNames::fAnaSteps[step])->Clone();
   TH1D* bg = fh_mean_bg_minv[step];
   TH1D* sEta = fh_mean_eta_minv[step];
   TH1D* sPi0 = fh_mean_pi0_minv[step];
   TH1D* sOmegaDalitz = (TH1D*) H1(kOmegaD, "fh_signal_minv_" + CbmAnaLmvmNames::fAnaSteps[step])->Clone();

   TH1D* coctail = (TH1D*)sRho->Clone();
   coctail->Add(sOmega);
   coctail->Add(sPhi);
   coctail->Add(sEta);
   coctail->Add(sPi0);
   coctail->Add(sOmegaDalitz);

   TH1D* sbg = (TH1D*)bg->Clone();
   sbg->Add(sRho);
   sbg->Add(sOmega);
   sbg->Add(sPhi);
   sbg->Add(sEta);
   sbg->Add(sPi0);
   sbg->Add(sOmegaDalitz);
   sbg->SetMinimum(1e-8);
   sbg->SetMaximum(1e-2);

   int nRebin = 20;
   sbg->Rebin(nRebin);
   coctail->Rebin(nRebin);
   bg->Rebin(nRebin);
   sPi0->Rebin(nRebin);
   sEta->Rebin(nRebin);
   sOmegaDalitz->Rebin(nRebin);
   sOmega->Rebin(nRebin);
   sRho->Rebin(nRebin);
   sPhi->Rebin(nRebin);

   DrawH1(list_of(sbg)(bg)(coctail)(sPi0)(sEta)(sOmegaDalitz)(sOmega)(sRho)(sPhi),
         list_of("")("")("")("")("")("")("")("")(""), kLinear, kLog, false, 0.8, 0.8, 0.99, 0.99);

   TLatex latex;
  // latex.SetTextSize(0.08);

   latex.SetTextColor(kMagenta-3);
   latex.DrawLatex(0.85, sRho->GetMaximum() * 1.1, "#rho^{0}");
   sRho->SetFillColor(kMagenta-3);
   sRho->SetLineColor(kMagenta-2);
   sRho->SetLineStyle(0);
   sRho->SetLineWidth(2);
   sRho->SetFillStyle(3112);

   latex.SetTextColor(kOrange+7);
   latex.DrawLatex(0.83, sOmega->GetMaximum() * 1.1, "#omega");
   sOmega->SetFillColor(kOrange+7);
   sOmega->SetLineColor(kOrange+4);
   sOmega->SetLineStyle(0);
   sOmega->SetLineWidth(2);

   latex.SetTextColor(kAzure+2);
   latex.DrawLatex(1.05, sPhi->GetMaximum() * 1.1, "#phi");
   sPhi->SetFillColor(kAzure+2);
   sPhi->SetLineColor(kAzure+3);
   sPhi->SetLineStyle(0);
   sPhi->SetLineWidth(2);
   sPhi->SetFillStyle(3112);
   gStyle->SetHatchesLineWidth(1);
   gStyle->SetHatchesSpacing(1.);

   bg->SetFillColor(kGray);
   bg->SetLineColor(kBlack);
   bg->SetLineStyle(0);
   bg->SetLineWidth(1);

   latex.SetTextColor(kRed-4);
   latex.DrawLatex(0.15, sEta->GetMaximum() * 1.1, "#eta");
   sEta->SetFillColor(kRed-4);
   sEta->SetLineColor(kRed+2);
   sEta->SetLineStyle(0);
   sEta->SetLineWidth(2);

   latex.SetTextColor(kGreen-3);
   latex.DrawLatex(0.1, sPi0->GetMaximum() * 1.1, "#pi^{0}");
   sPi0->SetFillColor(kGreen-3);
   sPi0->SetLineColor(kGreen+3);
   sPi0->SetLineStyle(0);
   sPi0->SetLineWidth(2);

   latex.SetTextColor(kCyan+2);
   latex.DrawLatex(0.2, sOmegaDalitz->GetMaximum() * 1.1, "#omega-Dalitz");
   sOmegaDalitz->SetFillColor(kCyan+2);
   sOmegaDalitz->SetLineColor(kCyan+4);
   sOmegaDalitz->SetLineStyle(0);
   sOmegaDalitz->SetLineWidth(2);

   sbg->SetFillColor(kBlack);
   sbg->SetLineColor(kBlack);
   sbg->SetLineStyle(0);
   sbg->SetLineWidth(1);

   coctail->SetLineColor(kRed+2);
   coctail->SetFillStyle(0);
   coctail->SetLineWidth(3);

   gPad->SetLogy(true);

   // signal-to-background ration vs. minv
   TH1D* sbgVsMinv = new TH1D(("sbgVsMinv_" + CbmAnaLmvmNames::fAnaSteps[step]).c_str(),
         ("sbgVsMinv_"+CbmAnaLmvmNames::fAnaSteps[step]+";M_{ee} [GeV/c^{2}];Cocktail/Background").c_str(),
         bg->GetNbinsX(), bg->GetXaxis()->GetXmin(), bg->GetXaxis()->GetXmax());
   sbgVsMinv->Divide(coctail, bg, 1., 1., "B");
   TCanvas* c = CreateCanvas(("lmvm_sbg_vs_minv_"+CbmAnaLmvmNames::fAnaSteps[step]).c_str(),
         ("lmvm_sbg_vs_minv_"+CbmAnaLmvmNames::fAnaSteps[step]).c_str(), 800, 800);
   DrawH1(sbgVsMinv);
   gPad->SetLogy(true);
}

void CbmAnaDielectronTaskDrawAll::FillMeanHist()
{
   for (int step = 0; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      for (int iS = 0; iS < fNofSignals; iS++){
         if (iS == 0) {
            fh_mean_bg_minv[step] = (TH1D*)H1(iS, "fh_bg_minv_" + CbmAnaLmvmNames::fAnaSteps[step])->Clone();
            fh_mean_eta_minv[step] = (TH1D*)H1(iS, "fh_eta_minv_" + CbmAnaLmvmNames::fAnaSteps[step])->Clone();
            fh_mean_pi0_minv[step] = (TH1D*)H1(iS, "fh_pi0_minv_" + CbmAnaLmvmNames::fAnaSteps[step])->Clone();
         } else {
            fh_mean_bg_minv[step]->Add( (TH1D*)H1(iS, "fh_bg_minv_" + CbmAnaLmvmNames::fAnaSteps[step])->Clone() );
            fh_mean_eta_minv[step]->Add( (TH1D*)H1(iS, "fh_eta_minv_" + CbmAnaLmvmNames::fAnaSteps[step])->Clone() );
            fh_mean_pi0_minv[step]->Add( (TH1D*)H1(iS, "fh_pi0_minv_" + CbmAnaLmvmNames::fAnaSteps[step])->Clone() );
         }
      }
      fh_mean_bg_minv[step]->Scale(1./(double) fNofSignals);
      fh_mean_eta_minv[step]->Scale(1./(double) fNofSignals);
      fh_mean_pi0_minv[step]->Scale(1./(double) fNofSignals);
   }

   if (fOutputDir != ""){
      gSystem->mkdir(fOutputDir.c_str(), true);
      TFile* f = TFile::Open( string(fOutputDir + "/mean_hist.root").c_str(), "RECREATE" );
      for (int i = 0; i < CbmAnaLmvmNames::fNofAnaSteps; i++){
         fh_mean_bg_minv[i]->Write();
      }
      f->Close();
   }

}

void CbmAnaDielectronTaskDrawAll::FillSumSignalsHist()
{
   for (int step = 0; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      fh_sum_s_minv[step] = (TH1D*)H1(kRho0, "fh_signal_minv_" + CbmAnaLmvmNames::fAnaSteps[step])->Clone();
      fh_sum_s_minv[step]->Add( (TH1D*)H1(kOmega, "fh_signal_minv_" + CbmAnaLmvmNames::fAnaSteps[step])->Clone() );
      fh_sum_s_minv[step]->Add( (TH1D*)H1(kPhi, "fh_signal_minv_" + CbmAnaLmvmNames::fAnaSteps[step])->Clone() );
      fh_sum_s_minv[step]->Add( (TH1D*)H1(kOmegaD, "fh_signal_minv_" + CbmAnaLmvmNames::fAnaSteps[step])->Clone() );
      fh_sum_s_minv[step]->Add( (TH1D*)fh_mean_eta_minv[step]->Clone() );
      fh_sum_s_minv[step]->Add( (TH1D*)fh_mean_pi0_minv[step]->Clone() );
   }
}

void CbmAnaDielectronTaskDrawAll::CalcCutEffRange(
      Double_t minMinv,
      Double_t maxMinv)
{
    TH1D* grS = new TH1D("grS", ";Analysis step;Efficiency [%]", CbmAnaLmvmNames::fNofAnaSteps, 0, CbmAnaLmvmNames::fNofAnaSteps);
    TH1D* grB = new TH1D("grB", ";Analysis step;Efficiency [%]", CbmAnaLmvmNames::fNofAnaSteps, 0, CbmAnaLmvmNames::fNofAnaSteps);
    int x = 1;
    for (int step = kElId; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
       if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)){
          continue;
       }
       Int_t x1 = fh_sum_s_minv[step]->FindBin(minMinv);
       Int_t x2 = fh_sum_s_minv[step]->FindBin(maxMinv);

       double yS = 100.* fh_sum_s_minv[step]->Integral(x1, x2) / fh_sum_s_minv[kElId]->Integral(x1, x2);
       double yB = 100.* fh_mean_bg_minv[step]->Integral(x1, x2) / fh_mean_bg_minv[kElId]->Integral(x1, x2);

       grB->GetXaxis()->SetBinLabel(x, CbmAnaLmvmNames::fAnaStepsLatex[step].c_str());
       grB->SetBinContent(x, yB);
       grS->SetBinContent(x, yS);
       x++;
    }

    grB->GetXaxis()->SetLabelSize(0.06);
    grB->GetXaxis()->SetRange(1, x - 1);
    grS->GetXaxis()->SetRange(1, x - 1);

    stringstream ss;
    ss << "lmvm_cut_eff_" << minMinv << "_" << maxMinv;
    TCanvas* c = CreateCanvas(ss.str().c_str(), ss.str().c_str(), 700, 700);
    DrawH1(list_of(grB)(grS), list_of("BG")("Signal"), kLinear, kLinear, true, 0.75, 0.85, 1.0, 1.0);
    grS->SetLineWidth(4);
    grB->SetLineWidth(4);
    grB->SetMinimum(1);
    grB->SetMaximum(105);

    stringstream ss2;
    ss2 << minMinv <<"<M [GeV/c^2]<" << maxMinv;
    TText *t = new TText(0.5, 110, ss2.str().c_str());
    t->Draw();
}


TH1D* CbmAnaDielectronTaskDrawAll::SBgRange(
      Double_t min,
      Double_t max)
{
   stringstream ss;
   ss << "lmvm_s_bg_region_" << min << "_" << max;
   TH1D* h_s_bg = new TH1D(ss.str().c_str(), string(ss.str()+";Analysis steps;S/BG").c_str(), CbmAnaLmvmNames::fNofAnaSteps, 0, CbmAnaLmvmNames::fNofAnaSteps);
   h_s_bg->GetXaxis()->SetLabelSize(0.06);
   int x = 1;
   for (int step = kElId; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)){
         continue;
      }
      Int_t bin1 = fh_sum_s_minv[step]->FindBin(min);
      Int_t bin2 = fh_sum_s_minv[step]->FindBin(max);
      double y = fh_sum_s_minv[step]->Integral(bin1, bin2) / fh_mean_bg_minv[step]->Integral(bin1, bin2);

      h_s_bg->GetXaxis()->SetBinLabel(x, CbmAnaLmvmNames::fAnaStepsLatex[step].c_str());
      h_s_bg->SetBinContent(x, y);
      // replace "." with "_"
      string str = ss.str();
      for (string::iterator it = str.begin() ; it < str.end(); it++){
         if (*it == '.') *it = '_';
      }

      fPt->Put(string(str+"_"+CbmAnaLmvmNames::fAnaSteps[step]), y);
      x++;
   }
   h_s_bg->GetXaxis()->SetRange(1, x - 1);
   return h_s_bg;
}

void CbmAnaDielectronTaskDrawAll::SBgRangeAll()
{
   TH1D* h_00_02 = SBgRange(0.0, 0.2);
   TH1D* h_02_06 = SBgRange(0.2, 0.6);
   TH1D* h_06_12 = SBgRange(0.6, 1.2);

   TCanvas* c = CreateCanvas("lmvm_s_bg_ranges", "lmvm_s_bg_ranges", 700, 700);
   DrawH1(list_of(h_00_02)(h_02_06)(h_06_12),
         list_of("0.0<M [GeV/c^{2}]<0.2")("0.2<M [GeV/c^{2}]<0.6")("0.6<M [GeV/c^{2}]<1.2"),
         kLinear, kLog, true, 0.25, 0.8, 0.75, 0.99);

   h_00_02->SetMinimum(1e-3);
   h_00_02->SetMaximum(3);
   h_00_02->SetLineWidth(4);
   h_02_06->SetLineWidth(4);
   h_06_12->SetLineWidth(4);

   TH1D* h_05_06 = SBgRange(0.5, 0.6);
   TCanvas* c1 = CreateCanvas("lmvm_s_bg_ranges_05_06", "lmvm_s_bg_ranges_05_06", 700, 700);
   DrawH1(h_05_06, kLinear, kLinear);
   h_05_06->SetMinimum(1e-3);
   h_05_06->SetMaximum(2e-2);
   h_05_06->SetLineWidth(4);
}

void CbmAnaDielectronTaskDrawAll::DrawSBgSignals()
{
   Double_t y[CbmAnaLmvmNames::fNofAnaSteps];
   TCanvas* cFit = CreateCanvas("lmvm_signal_fit", "lmvm_signal_fit", 600, 600);
   for (int iF = 0; iF < fNofSignals - 1; iF++){
      string signalName = CbmAnaLmvmNames::fSignalNames[iF];
      cout << "Signal: " << signalName << endl;
      stringstream ss;
      ss << "lmvm_s_bg_cuts_" << signalName;

      TH1D* h_s_bg = new TH1D(ss.str().c_str(), string(ss.str()+";Analysis steps;S/BG").c_str(), CbmAnaLmvmNames::fNofAnaSteps, 0, CbmAnaLmvmNames::fNofAnaSteps);
      h_s_bg->GetXaxis()->SetLabelSize(0.06);
      h_s_bg->SetLineWidth(4);
      int x = 1;
      for (int step = 0; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
         if (step < kElId) continue;
         if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)){
            continue;
         }

         TH1D* s = (TH1D*)H1(iF, "fh_signal_minv_" + CbmAnaLmvmNames::fAnaSteps[step])->Clone();
         TH1D* bg = (TH1D*)fh_mean_bg_minv[step]->Clone();
         cFit->cd();
         if (iF == 2){
            s->Fit("gaus", "Q", "", 0.95, 1.05);
         } else if (iF == 1){
            s->Fit("gaus", "Q", "", 0.69, 0.81);
         } else{
            s->Fit("gaus", "Q");
         }

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
         double eff = 100. * H1(iF, "fh_signal_pty_" + CbmAnaLmvmNames::fAnaSteps[step])->GetEntries() /
                             H1(iF, "fh_signal_pty_" + CbmAnaLmvmNames::fAnaSteps[kMc])->GetEntries();

         fPt->Put(signalName + "_sbg_"+CbmAnaLmvmNames::fAnaSteps[step], sumSignal/sumBg);
         fPt->Put(signalName + "_eff_"+CbmAnaLmvmNames::fAnaSteps[step], eff);
         fPt->Put(signalName + "_signal_minv_mean_"+CbmAnaLmvmNames::fAnaSteps[step], 1000.*mean);
         fPt->Put(signalName + "_signal_minv_rms_"+CbmAnaLmvmNames::fAnaSteps[step], 1000.*sigma);

         h_s_bg->GetXaxis()->SetBinLabel(x, CbmAnaLmvmNames::fAnaStepsLatex[step].c_str());
         if (sbg < 1000.) h_s_bg->SetBinContent(x, sbg);
         x++;
      }
      h_s_bg->GetXaxis()->SetRange(1, x - 1);
      TCanvas* c = CreateCanvas(ss.str().c_str(), ss.str().c_str(), 800, 800);
      DrawH1(h_s_bg);
      h_s_bg->SetLineWidth(4);
   }
}

void CbmAnaDielectronTaskDrawAll::SaveCanvasToImage()
{
   for (int i = 0; i < fCanvas.size(); i++){
      lit::SaveCanvasAsImage(fCanvas[i], fOutputDir);
   }
}

ClassImp(CbmAnaDielectronTaskDrawAll);
