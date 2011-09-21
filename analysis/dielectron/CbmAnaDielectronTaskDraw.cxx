/** CbmAnaDielectronTaskDraw.cxx
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2011
 * @version 1.0
 **/

#include "CbmAnaDielectronTaskDraw.h"

#include "cbm/utils/CbmLitDrawHist.h"
#include "std/utils/CbmLitUtils.h"

#include <string>
#include <iostream>
#include <iomanip>

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

ClassImp(CbmAnaDielectronTaskDraw);

using namespace std;
using namespace lit;

void CbmAnaDielectronTaskDraw::DrawHistosFromFile(
      const std::string& fileName,
      Bool_t useMvd)
{
   SetStyles();
   fUseMvd = useMvd;
   fFileName = fileName;
   fFile = new TFile(fileName.c_str());

   ReadAllHistosToMap(fFile);
   Int_t fNofEvents = (Int_t)H1("fh_event_number")->GetEntries();
   cout << "File name = " << fileName << endl;
   cout << "Number of events = " << fNofEvents<< endl;

   ScaleAllHistos(fNofEvents);
   SignalOverBgAll();

   RebinMinvHistos();
   DrawPtYDistributionAll();
   DrawPtYEfficiencyAll();
   DrawMomentumDistributionAll();
   DrawMomentumEfficiencyAll();
   DrawMotherPdg();
   DrawAnalysisCutsAll();
   DrawSourcesBgPairsAll();
   DrawAPCut();
   DrawAPMCut();
   DrawSTCut();
   DrawTTCut();
   DrawGammaVertex();
   DrawIdCutsAll();
   DrawDstsCut();
   DrawInvariantMassAfterEachCut();
   DrawInvariantMassSandBG();
   DrawInvariantMassPi0andEta();
   DrawBGSourceTracks();
   DrawMinvVsPt();
   DrawBgSourcesVsMomentum();
   DrawCumulativeProbabilitiesAll();
}

void CbmAnaDielectronTaskDraw::ReadAllHistosToMap(
      TFile* file)
{
   TDirectory * dir = gDirectory;
   TIter nextkey( dir->GetListOfKeys());
   TKey *key;

   Int_t c = 0;
   while (key = (TKey*) nextkey()) {
      TObject* obj = key->ReadObj();
      if (obj->IsA()->InheritsFrom (TH1::Class())) {
         TH1* h = (TH1*) obj;
         TH1* h1 = (TH1*)file->Get(h->GetName());
         fHistoMap[h->GetName()] = h1;
         c++;
      }
   }
   cout << c << " histograms were read from file." << endl;
}

void CbmAnaDielectronTaskDraw::ScaleAllHistos(
      Int_t nofEvents)
{
   map<string, TH1*>::iterator it;
   Int_t c = 0;
   for (it=fHistoMap.begin(); it!=fHistoMap.end(); it++) {
      (it->second)->Scale(1./(Double_t)nofEvents);
      c++;
   }
   cout << c << " histograms were scaled." << endl;
}

void CbmAnaDielectronTaskDraw::RebinMinvHistos()
{
   string hS[46] = {
         // signal
         "fh_mc_signal_minv", "fh_acc_signal_minv", "fh_rec_signal_minv",
         "fh_chi_prim_signal_minv", "fh_el_id_signal_minv", "fh_gammacut_signal_minv",
         "fh_dstscut_signal_minv", "fh_dsts2cut_signal_minv", "fh_stcut_signal_minv",
         "fh_ttcut_signal_minv", "fh_ptcut_signal_minv", "fh_anglecut_signal_minv",
         "fh_apmcut_signal_minv",
         // BG
         "fh_rec_bg_minv", "fh_chi_prim_bg_minv", "fh_el_id_bg_minv",
         "fh_gammacut_bg_minv", "fh_dstscut_bg_minv", "fh_dsts2cut_bg_minv",
         "fh_stcut_bg_minv", "fh_ttcut_bg_minv", "fh_ptcut_bg_minv",
         "fh_anglecut_bg_minv", "fh_apmcut_bg_minv",
         //Pi0
         "fh_rec_pi0_minv", "fh_chi_prim_pi0_minv", "fh_el_id_pi0_minv",
         "fh_gammacut_pi0_minv", "fh_dstscut_pi0_minv", "fh_dsts2cut_pi0_minv",
         "fh_stcut_pi0_minv", "fh_ttcut_pi0_minv", "fh_ptcut_pi0_minv",
         "fh_anglecut_pi0_minv", "fh_apmcut_pi0_minv",
         //eta
         "fh_rec_eta_minv", "fh_chi_prim_eta_minv", "fh_el_id_eta_minv",
         "fh_gammacut_eta_minv", "fh_dstscut_eta_minv", "fh_dsts2cut_eta_minv",
         "fh_stcut_eta_minv", "fh_ttcut_eta_minv", "fh_ptcut_eta_minv",
         "fh_anglecut_eta_minv", "fh_apmcut_eta_minv"};

   Int_t nRebin = 10;
   for (Int_t i = 0; i < 46; i++){
      TH1D* h = H1(hS[i]);
      h->Rebin(nRebin);
      h->Scale(1./(Double_t)nRebin);
   }

}

TH1D* CbmAnaDielectronTaskDraw::H1(
      const string& name)
{
   return (TH1D*)fHistoMap.find(name)->second;
}

TH2D* CbmAnaDielectronTaskDraw::H2(
      const string& name)
{
   return (TH2D*)fHistoMap.find(name)->second;
}

void CbmAnaDielectronTaskDraw::DrawEfficiencyOnHist(
      TH1* h1,
      TH1* h2,
      Double_t xPos,
      Double_t yPos)
{
   string effTxt = "";
   if (h2->GetEntries() != 0){
      effTxt = lit::NumberToString<Double_t>(((Double_t)h1->GetEntries()/h2->GetEntries()*100.), 1);
   }
   TText *t = new TText(xPos, yPos, effTxt.c_str());
   t->SetTextSize(0.1);
   t->Draw();
}

void CbmAnaDielectronTaskDraw::DrawTextOnHist(
      const string& text,
      Double_t x1,
      Double_t y1,
      Double_t x2,
      Double_t y2)
{
   TLegend* leg = new TLegend(x1, y1, x2, y2);
   leg->AddEntry(new TH2D(), text.c_str(), "");
   leg->SetFillColor(kWhite);
   leg->SetFillStyle(0);
   leg->SetBorderSize(0);
   leg->Draw();
}

TH2D* CbmAnaDielectronTaskDraw::DivideHisto2D(
      TH2D* h1,
      TH2D* h2)
{
    Int_t nBinsX = h1->GetNbinsX();
    Double_t minX = h1->GetXaxis()->GetXmin();
    Double_t maxX = h1->GetXaxis()->GetXmax();
    Int_t nBinsY = h1->GetNbinsY();
    Double_t minY = h1->GetYaxis()->GetXmin();
    Double_t maxY = h1->GetYaxis()->GetXmax();

    TH2D* h3 = new TH2D(h1->GetName(), h1->GetTitle(), nBinsX, minX, maxX, nBinsY, minY, maxY);
    //h1->Sumw2();
    //h2->Sumw2();
    h3->Divide(h1,h2,100.,1.,"B");
    return h3;
}

TH1D* CbmAnaDielectronTaskDraw::DivideHisto1D(
      TH1D* h1,
      TH1D* h2)
{
    Int_t nBins = h1->GetNbinsX();
    Double_t min = h1->GetXaxis()->GetXmin();
    Double_t max = h1->GetXaxis()->GetXmax();
    //h1->Sumw2();
    //h2->Sumw2();
    TH1D* h3 = new TH1D(h1->GetName(), h1->GetTitle(), nBins, min, max);
    h3->Divide(h1, h2, 100.,1., "B");
    return h3;
}

//option == forward or back
TH1D* CbmAnaDielectronTaskDraw::CreateSignificanceHisto1D(
      TH1D* signal,
      TH1D* bg,
      const string& name,
      const string& option)
{
    Int_t nBins = signal->GetNbinsX();
    Double_t xmin = signal->GetXaxis()->GetXmin();
    Double_t xmax = signal->GetXaxis()->GetXmax();
    TH1D* hsig = new TH1D(name.c_str(), name.c_str(), nBins, xmin, xmax);

    Double_t sumSignal = 0;
    Double_t sumBg = 0;
    Double_t significance = 0;
    // "right" - one wants to reject right part of the histogram.
    // value > cut -> reject
    if(option == "right"){
        for (Int_t i = 1; i <= nBins; i++) {
            sumSignal = signal->Integral(1, i,"width");
            sumBg = bg->Integral(1, i,"width");
            Double_t prov = TMath::Sqrt(sumSignal+sumBg);
            if (prov != 0. ) significance = sumSignal / prov;
            else significance = 0.;
            hsig->SetBinContent(i, significance);
        }
    // "left" - one wants to reject left part of the histogram.
    // value < cut -> reject
    } else if (option == "left"){
        for (Int_t i = nBins; i >= 1; i--) {
            sumSignal = signal->Integral(i, nBins,"width");
            sumBg = bg->Integral(i, nBins,"width");
            Double_t prov = TMath::Sqrt(sumSignal+sumBg);
            if (prov != 0. ) significance = sumSignal / prov;
            else significance = 0.;
            hsig->SetBinContent(i, significance);
        }
    }
    return hsig;
}

TH2D* CbmAnaDielectronTaskDraw::CreateSignificanceHisto2D(
      TH2D* signal,
      TH2D* bg,
      const string& name,
      const string& title)
{

   Double_t xmin = 1.0;
   Double_t xmax = 5.0;
   Double_t ymin = 1.0;
   Double_t ymax = 5.0;
   Double_t delta = 0.1;
   Int_t nStepsX = (Int_t)( (xmax - xmin) / delta );
   Int_t nStepsY = (Int_t)( (ymax - ymin) / delta );
   Int_t nBinsX = signal->GetNbinsX();
   Int_t nBinsY = signal->GetNbinsY();

   TH2D* hsig = new TH2D(name.c_str(), title.c_str(), nStepsX, xmin, xmax, nStepsY, ymin, ymax);

   Double_t sumSignal = 0;
   Double_t sumBg = 0;

   Int_t binX = 1;
   for (Double_t xcut = xmin; xcut <= xmax; xcut+=delta, binX++) {
      Int_t binY = 1;
      cout << "x " << xcut << endl;
      for (Double_t ycut = ymin; ycut <= ymax; ycut+=delta, binY++){

         sumSignal = 0;
         sumBg = 0;
         for (Int_t ix = 1; ix <= nBinsX; ix++){
            for (Int_t iy = 1; iy <=nBinsY; iy++){
               Double_t xcenter = signal->GetXaxis()->GetBinCenter(ix);
               Double_t ycenter = signal->GetYaxis()->GetBinCenter(iy);
               Double_t val = -1 * (ycut/xcut)*xcenter + ycut;

               if (!(xcenter < xcut && ycenter < val)) {
                  sumSignal += signal->GetBinContent(ix,iy);
                  sumBg += bg->GetBinContent(ix,iy);
               }
            }
         }
         Double_t prov = TMath::Sqrt(sumSignal+sumBg);
         Double_t significance = 0.;
         if (prov != 0) significance = sumSignal / prov;
         hsig->SetBinContent(binX, binY, significance);
      }
   }
   return hsig;
}

void CbmAnaDielectronTaskDraw::CalculateSignalOverBg(
      TH1D* s,
      TH1D* bg,
      TH2D* pty1,
      TH2D* pty2,
      const string& stepName)
{
   TH1D* sClone = (TH1D*)s->Clone();
   sClone->Fit("gaus", "Q");

   Double_t mean = sClone->GetFunction("gaus")->GetParameter("Mean");
   Double_t sigma = sClone->GetFunction("gaus")->GetParameter("Sigma");

   Int_t minInd = s->FindBin(mean - 2.*sigma);
   Int_t maxInd = s->FindBin(mean + 2.*sigma);

   Double_t sumSignal = 0.;
   Double_t sumBg = 0.;
   for (Int_t i = minInd + 1; i <= maxInd - 1; i++){
      sumSignal += s->GetBinContent(i);
      sumBg += bg->GetBinContent(i);
   }
   cout << left << setw(15) << stepName
         << left<< setw(15)<< lit::NumberToString(((Double_t)pty1->GetEntries()/pty2->GetEntries()*100.), 1)
         << left<< setw(15)<< lit::NumberToString(sumSignal/sumBg, 2)
         << left<< setw(15) << lit::NumberToString(1000.*mean, 1)
         << left<< setw(15)<<  lit::NumberToString(1000.*sigma,1)
          << endl;
}

void CbmAnaDielectronTaskDraw::SignalOverBgAll()
{
   TCanvas *c1 = new TCanvas("signal_fitting", "signal_fitting", 600, 600);
   cout << left << setw(15) << "Step" << left << setw(15) << "Efficiency"
         << left << setw(15) << "S/BG" << left << setw(15) << "Mean [MeV/c2]"
         << left << setw(15) << "Sigma [MeV/c2]" << endl;

   CalculateSignalOverBg( H1("fh_rec_signal_minv"), H1("fh_rec_bg_minv"),
         H2("fh_reco_signal_pty"), H2("fh_mc_signal_pty"), "rec");
   CalculateSignalOverBg( H1("fh_chi_prim_signal_minv"), H1("fh_chi_prim_bg_minv"),
         H2("fh_chi_prim_signal_pty"), H2("fh_mc_signal_pty"), "chi_prim");
   CalculateSignalOverBg( H1("fh_el_id_signal_minv"), H1("fh_el_id_bg_minv"),
         H2("fh_el_id_signal_pty"), H2("fh_mc_signal_pty"), "el_id");
   CalculateSignalOverBg( H1("fh_gammacut_signal_minv"), H1("fh_gammacut_bg_minv"),
         H2("fh_gammacut_signal_pty"), H2("fh_mc_signal_pty"), "gamma");
   if (fUseMvd) CalculateSignalOverBg( H1("fh_dstscut_signal_minv"), H1("fh_dstscut_bg_minv"),
         H2("fh_dstscut_signal_pty"), H2("fh_mc_signal_pty"), "dmvd");
   if (fUseMvd) CalculateSignalOverBg( H1("fh_dsts2cut_signal_minv"), H1("fh_dsts2cut_bg_minv"),
         H2("fh_dsts2cut_signal_pty"), H2("fh_mc_signal_pty"), "dmvd2");
   CalculateSignalOverBg( H1("fh_stcut_signal_minv"), H1("fh_stcut_bg_minv"),
         H2("fh_stcut_signal_pty"), H2("fh_mc_signal_pty"), "ST");
   CalculateSignalOverBg( H1("fh_ttcut_signal_minv"), H1("fh_ttcut_bg_minv"),
         H2("fh_ttcut_signal_pty"), H2("fh_mc_signal_pty"), "TT");
   CalculateSignalOverBg( H1("fh_ptcut_signal_minv"), H1("fh_ptcut_bg_minv"),
         H2("fh_ptcut_signal_pty"), H2("fh_mc_signal_pty"), "pt");
   CalculateSignalOverBg( H1("fh_anglecut_signal_minv"), H1("fh_anglecut_bg_minv"),
         H2("fh_anglecut_signal_pty"), H2("fh_mc_signal_pty"), "angle");
   CalculateSignalOverBg( H1("fh_apmcut_signal_minv"), H1("fh_apmcut_bg_minv"),
         H2("fh_apmcut_signal_pty"), H2("fh_mc_signal_pty"), "APM");
}

// pty distribution of signal for one step
void CbmAnaDielectronTaskDraw::DrawPtYDistribution(
      TH2D* h1,
      TH2D* h2,
      const string& text)
{
   DrawHist2D( h1, "", "", "per event", kLitLinearScale, kLitLinearScale, kLitLinearScale, "COLZ");
   DrawEfficiencyOnHist(h1, h2, 0.2, 1.8);
   DrawTextOnHist(text, 0.50, 0.78, 0.70, 0.9);
}

// pty distribution of signal for all steps
void CbmAnaDielectronTaskDraw::DrawPtYDistributionAll()
{
   Int_t hi = 1;
   TCanvas *c2 = new TCanvas("pty_distribution","pty_distribution",750,1000);
   c2->Divide(3,4);
   c2->cd(hi++);
   DrawPtYDistribution(H2("fh_mc_signal_pty"), H2("fh_mc_signal_pty"), "MC");
   c2->cd(hi++);
   DrawPtYDistribution(H2("fh_acc_signal_pty"), H2("fh_mc_signal_pty"), "acc");
   c2->cd(hi++);
   DrawPtYDistribution(H2("fh_reco_signal_pty"), H2("fh_mc_signal_pty"), "rec");
   c2->cd(hi++);
   DrawPtYDistribution(H2("fh_chi_prim_signal_pty"), H2("fh_mc_signal_pty"), "#chi^{2}_{prim}");
   c2->cd(hi++);
   DrawPtYDistribution(H2("fh_el_id_signal_pty"), H2("fh_mc_signal_pty"), "ID");
   c2->cd(hi++);
   DrawPtYDistribution(H2("fh_gammacut_signal_pty"), H2("fh_mc_signal_pty"), "m_{#gamma}");
   if (fUseMvd) {
      c2->cd(hi++);
      DrawPtYDistribution(H2("fh_dstscut_signal_pty"), H2("fh_mc_signal_pty"), "d_{mvd}");
      c2->cd(hi++);
      DrawPtYDistribution(H2("fh_dsts2cut_signal_pty"), H2("fh_mc_signal_pty"), "d_{mvd2}");
   }
   c2->cd(hi++);
   DrawPtYDistribution(H2("fh_stcut_signal_pty"), H2("fh_mc_signal_pty"), "ST");
   c2->cd(hi++);
   DrawPtYDistribution(H2("fh_ttcut_signal_pty"), H2("fh_mc_signal_pty"), "TT");
   c2->cd(hi++);
   DrawPtYDistribution(H2("fh_ptcut_signal_pty"), H2("fh_mc_signal_pty"), "p_{t}");
   //c2->cd(hi++);
   //DrawPtYDistribution(H2("fh_anglecut_signal_pty"), H2("fh_mc_signal_pty"),"#Theta_{1,2}");
   //c2->cd(hi++);
   //DrawPtYDistribution(H2("fh_apmcut_signal_pty"), H2("fh_mc_signal_pty"), "APM");
}

//pty efficiency of signal for one step
void CbmAnaDielectronTaskDraw::DrawPtYEfficiency(
      TH2D* h1,
      TH2D* h2,
      const string& text)
{
   TH2D* eff = DivideHisto2D(h1, h2);
   DrawHist2D(eff, "Rapidity", "p_{t} [GeV/c]", "Efficiency [%]", kLitLinearScale,
         kLitLinearScale, kLitLinearScale, "COLZ");
   DrawEfficiencyOnHist(h1, h2, 0.2, 1.8);
   DrawTextOnHist(text, 0.50, 0.78, 0.70, 0.9);
}

//pty efficiency of signal for all steps
void CbmAnaDielectronTaskDraw::DrawPtYEfficiencyAll()
{
    Int_t hi = 1;

    TCanvas *c3 = new TCanvas("pty_efficiency","pty_efficiency",750,1000);
    c3->Divide(3,4);
    c3->cd(hi++);
    DrawPtYEfficiency(H2("fh_acc_signal_pty"), H2("fh_mc_signal_pty"), "acc");
    c3->cd(hi++);
    DrawPtYEfficiency(H2("fh_reco_signal_pty"), H2("fh_acc_signal_pty"), "rec");
    c3->cd(hi++);
    DrawPtYEfficiency(H2("fh_chi_prim_signal_pty"), H2("fh_reco_signal_pty"), "#chi^{2}_{prim}");
    c3->cd(hi++);
    DrawPtYEfficiency(H2("fh_el_id_signal_pty"), H2("fh_chi_prim_signal_pty"), "ID");
    c3->cd(hi++);
    DrawPtYEfficiency(H2("fh_gammacut_signal_pty"), H2("fh_el_id_signal_pty"), "m_{#gamma}");
    if (fUseMvd) {
       c3->cd(hi++);
       DrawPtYEfficiency(H2("fh_dstscut_signal_pty"), H2("fh_gammacut_signal_pty"), "d_{mvd}");
       c3->cd(hi++);
       DrawPtYEfficiency(H2("fh_dsts2cut_signal_pty"), H2("fh_dstscut_signal_pty"), "d_{mvd2}");
    }
    c3->cd(hi++);
    DrawPtYEfficiency(H2("fh_stcut_signal_pty"), H2("fh_dsts2cut_signal_pty"), "ST");
    c3->cd(hi++);
    DrawPtYEfficiency(H2("fh_ttcut_signal_pty"), H2("fh_stcut_signal_pty"), "TT");
    c3->cd(hi++);
    DrawPtYEfficiency(H2("fh_ptcut_signal_pty"), H2("fh_ttcut_signal_pty"), "p_{t}");
    //c3->cd(hi++);
    //DrawPtYEfficiency(H2("fh_anglecut_signal_pty"), H2("fh_ptcut_signal_pty"),"#Theta_{1,2}");
    //c3->cd(hi++);
    //DrawPtYEfficiency(H2("fh_apmcut_signal_pty"), H2("fh_anglecut_signal_pty"), "APM");
}

// Momentum distribution of signal for all steps
void CbmAnaDielectronTaskDraw::DrawMomentumDistributionAll()
{
    TCanvas *c4 = new TCanvas("signal_momentum_distribution",
          "signal_momentum_distribution", 600, 600);
    Draw1DHistoAfterEachCut(
          H1("fh_mc_signal_mom"),
          H1("fh_acc_signal_mom"),
          H1("fh_rec_signal_mom"),
          H1("fh_chi_prim_signal_mom"),
          H1("fh_el_id_signal_mom"),
          H1("fh_gammacut_signal_mom"),
          H1("fh_dstscut_signal_mom"),
          H1("fh_dsts2cut_signal_mom"),
          H1("fh_stcut_signal_mom"),
          H1("fh_ttcut_signal_mom"),
          H1("fh_ptcut_signal_mom"),
          H1("fh_anglecut_signal_mom"),
          H1("fh_apmcut_signal_mom"),
          true); //logy
}

// Efficiency vs. momentum of pair for all steps
void CbmAnaDielectronTaskDraw::DrawMomentumEfficiencyAll()
{
//EFFICIENCY vs. MOMENTUM
    TCanvas *c5 = new TCanvas("signal_momentum_efficiency","signal_momentum_efficiency", 600, 600);
    Draw1DHistoAfterEachCut(
          NULL,
          NULL, //DivideHisto1D(H1("fh_acc_signal_mom"), H1("fh_mc_signal_mom")),
          DivideHisto1D(H1("fh_reco_signal_mom"), H1("fh_mc_signal_mom")),
          DivideHisto1D(H1("fh_chi_prim_signal_mom"), H1("fh_mc_signal_mom")),
          DivideHisto1D(H1("fh_el_id_signal_mom"), H1("fh_mc_signal_mom")),
          DivideHisto1D(H1("fh_gammacut_signal_mom"), H1("fh_mc_signal_mom")),
          DivideHisto1D(H1("fh_dstscut_signal_mom"), H1("fh_mc_signal_mom")),
          DivideHisto1D(H1("fh_dsts2cut_signal_mom"), H1("fh_mc_signal_mom")),
          DivideHisto1D(H1("fh_stcut_signal_mom"), H1("fh_mc_signal_mom")),
          DivideHisto1D(H1("fh_ttcut_signal_mom"), H1("fh_mc_signal_mom")),
          DivideHisto1D(H1("fh_ptcut_signal_mom"), H1("fh_mc_signal_mom")),
          DivideHisto1D(H1("fh_anglecut_signal_mom"), H1("fh_mc_signal_mom")),
          DivideHisto1D(H1("fh_apmcut_signal_mom"), H1("fh_mc_signal_mom")),
          false,
          "p [GeV/c]",
          "Efficiency [%]"
    );
}

// Mother PDG
void CbmAnaDielectronTaskDraw::DrawMotherPdg()
{
   TCanvas *c6 = new TCanvas("mother_pdg", "mother_pdg", 500, 500);
   DrawHist1D(H1("fh_mc_mother_pdg"), H1("fh_acc_mother_pdg"), NULL, NULL,
      "", "pdg", "Yield", "MC", "acc", "", "", kLitLinearScale, kLitLogScale,
      true, 0.7, 0.7, 0.99, 0.99);
   gPad->SetGridx(true);
   gPad->SetGridy(true);
}

// Draw distribution and significance of 1D analysis cut
void CbmAnaDielectronTaskDraw::Draw1DCutsForSandBg(
      TH1D* s,
      TH1D* bg)
{
   Double_t scaleSig = 1./s->Integral();
   Double_t scaleBg = 1./bg->Integral();
   s->SetLineColor(kRed);
   bg->SetLineColor(kBlue);
   s->SetLineWidth(2);
   bg->SetLineWidth(2);
   s->Scale(scaleSig);
   bg->Scale(scaleBg);
   DrawHist1D(s,bg,NULL,NULL,"","","", "S","BG","","",kLitLinearScale,kLitLogScale,
         true,0.7,0.7,0.99,0.99);
   gPad->SetGridx(true);
   gPad->SetGridy(true);
}

// Draw significance of 1D analysis cut
void CbmAnaDielectronTaskDraw::Draw1DSignificance(
      TH1D* s,
      TH1D* bg,
      const string& sigHistoName,
      const string& sigHistoOption)
{
   TH1D* significance = CreateSignificanceHisto1D(s, bg, sigHistoName.c_str(), sigHistoOption.c_str());
   significance->SetLineWidth(2);
   significance->GetXaxis()->SetTitle(s->GetXaxis()->GetTitle());

   DrawHist1D(significance, "", "Significance", kBlack, 2, LIT_LINE_STYLE1,
      2, LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, "");
   gPad->SetGridx(true);
   gPad->SetGridy(true);
}

// Draw distributions and significances of 1D histogram
void CbmAnaDielectronTaskDraw::DrawCutsAndSignificance(
      const string& canvasName,
      const string& sName,
      const string& bgName,
      const string& significanceName,
      const string& sigOption)
{
   TCanvas *c = new TCanvas(canvasName.c_str(), canvasName.c_str(), 900, 450);
   c->Divide(2,1);
   c->cd(1);
   Draw1DCutsForSandBg(H1(sName), H1(bgName));
   c->cd(2);
   Draw1DSignificance(H1(sName), H1(bgName), significanceName, sigOption);
}

//Draw identification cuts distributions
void CbmAnaDielectronTaskDraw::DrawIdCutsAll()
{
   DrawCutsAndSignificance("id_cuts_rich_ann","fh_rich_ann_signal","fh_rich_ann_bg",
         "rich_ann_significance", "left");

   DrawCutsAndSignificance("id_cuts_trd_ann", "fh_trd_ann_signal","fh_trd_ann_bg",
         "trd_ann_significance", "left");

   TCanvas *c3 = new TCanvas("id_cuts_tof","id_cuts_tof", 900, 450);
   c3->Divide(2,1);
   c3->cd(1);
   DrawHist2D(H2("fh_tof_m2_signal"));

   c3->cd(2);
   DrawHist2D(H2("fh_tof_m2_bg"));
}

// Draw distributions and significances of 1D analysis cuts for all cuts
void CbmAnaDielectronTaskDraw::DrawAnalysisCutsAll()
{
   DrawCutsAndSignificance("analysis_cuts_chi2_prim", "fh_chi2_prim_signal", "fh_chi2_prim_bg",
         "chi2_prim_significance", "right");

   DrawCutsAndSignificance("analysis_cuts_pt", "fh_pt_signal", "fh_pt_bg","pt_significance", "left");

   DrawCutsAndSignificance("analysis_cuts_angle", "fh_angle_signal", "fh_angle_bg",
         "angle_significance", "left");

   DrawCutsAndSignificance("analysis_cuts_momentum","fh_mom_signal","fh_mom_bg",
         "momentum_significance", "left");

   DrawCutsAndSignificance("analysis_cuts_chi2sts", "fh_chi2sts_signal", "fh_chi2sts_bg",
         "chi2sts_significance", "right");
}

// Draw sources of BG pairs for one step
void CbmAnaDielectronTaskDraw::DrawSourcesBgPairs(
      TH2D* h,
      const string& text)
{
   gStyle->SetPaintTextFormat("4.1f");
   string labels[3] = {"#gamma", "#pi^{0}", "oth"};
   for (Int_t i = 1; i <= 3; i++){
      h->GetYaxis()->SetBinLabel(i,labels[i-1].c_str());
      h->GetXaxis()->SetBinLabel(i,labels[i-1].c_str());
   }
   h->SetMarkerColor(0);
   h->SetMarkerSize(3);
   h->Scale(100. / h->Integral());
   DrawHist2D(h, "", "" , "", kLitLinearScale, kLitLinearScale, kLitLinearScale, "text COLZ");
   h->GetXaxis()->SetLabelSize(0.1);
   h->GetYaxis()->SetLabelSize(0.1);
   DrawTextOnHist(text, 0.50, 0.90, 0.70, 0.99);
}

// Draw sources of BG pairs for all steps
void CbmAnaDielectronTaskDraw::DrawSourcesBgPairsAll()
{
   Int_t hi = 1;
   TCanvas *c = new TCanvas("bg_sources_pair ", "bg_sources_pair", 900, 900);
   c->Divide(3,3);
   c->cd(hi++);
   DrawSourcesBgPairs(H2("fh_source_pair_reco"), "rec");
   c->cd(hi++);
   DrawSourcesBgPairs(H2("fh_source_pair_chi_prim"), "#chi^{2}_{prim}");
   c->cd(hi++);
   DrawSourcesBgPairs(H2("fh_source_pair_el_id"), "ID");
   c->cd(hi++);
   DrawSourcesBgPairs(H2("fh_source_pair_gammacut"), "m_{#gamma}");
   if (fUseMvd){
      c->cd(hi++);
      DrawSourcesBgPairs(H2("fh_source_pair_dstscut"), "d_{mvd}");
      c->cd(hi++);
      DrawSourcesBgPairs(H2("fh_source_pair_dsts2cut"), "d_{mvd2}");
   }
   c->cd(hi++);
   DrawSourcesBgPairs(H2("fh_source_pair_stcut"), "ST");
   c->cd(hi++);
   DrawSourcesBgPairs(H2("fh_source_pair_ttcut"), "TT");
   c->cd(hi++);
   DrawSourcesBgPairs(H2("fh_source_pair_ptcut"), "p_{t}");
   //c->cd(hi++);
   //DrawSourcesBgPairs(H2("fh_source_pair_anglecut"), "#Theta_{1,2}");
   //c->cd(hi++);
   //DrawSourcesBgPairs(H2("fh_source_pair_apmcut"), "APM");
}


//Draw AP CUT distribution
void CbmAnaDielectronTaskDraw::DrawAPCut()
{
   TCanvas *c9 = new TCanvas("apcut_distribution", "apcut_distribution", 800, 800);
   c9->Divide(2,2);
   c9->cd(1);
   DrawHist2D(H2("fh_apcut_signal"));
   TEllipse el1(0.5,0.5,0.2,0.3);
   el1.SetFillStyle(0);
   el1.SetLineWidth(3);
   el1.DrawEllipse(0.,0.,1.,0.45,0.,180.,0.);
   c9->cd(2);
   DrawHist2D(H2("fh_apcut_bg"));
   TEllipse el2(0.5,0.5,0.2,0.3);
   el2.SetFillStyle(0);
   el2.SetLineWidth(3);
   el2.DrawEllipse(0.,0.,1.,0.45,0.,180.,0.);
   c9->cd(3);
   DrawHist2D(H2("fh_apcut_pi0"));
   c9->cd(4);
   DrawHist2D(H2("fh_apcut_gamma"));
}


//Draw APM cut distribution
void CbmAnaDielectronTaskDraw::DrawAPMCut()
{
   TCanvas *c = new TCanvas("apmcut_distribution", "apmcut_distribution", 800, 800);
   c->Divide(2,2);
   c->cd(1);
   DrawHist2D(H2("fh_apmcut_signal"));
   c->cd(2);
   DrawHist2D(H2("fh_apmcut_bg"));
   c->cd(3);
   TH1D* apmcut_py_s = H2("fh_apmcut_signal")->ProjectionY();
   TH1D* apmcut_py_bg = H2("fh_apmcut_bg")->ProjectionY();
   Draw1DCutsForSandBg(apmcut_py_s, apmcut_py_bg);
   c->cd(4);
   Draw1DSignificance(apmcut_py_s, apmcut_py_bg, "apmcut_py_significance", "left");

}

//Draw ST cut distribution (segment tracks)
void CbmAnaDielectronTaskDraw::DrawSTCut()
{
    TCanvas *c = new TCanvas("stcut_distribution", "stcut_distribution", 1200, 600);
    c->Divide(4,2);
    c->cd(1);
    DrawHist2D(H2("fh_stcut_signal"));
    c->cd(2);
    DrawHist2D(H2("fh_stcut_bg"));
    c->cd(3);
    DrawHist2D(H2("fh_stcut_pi0"));
    c->cd(4);
    DrawHist2D(H2("fh_stcut_gamma"));
    c->cd(5);
    TH1D* stcut_px_s = H2("fh_stcut_signal")->ProjectionX();
    TH1D* stcut_px_bg = H2("fh_stcut_bg")->ProjectionX();
    Draw1DCutsForSandBg(stcut_px_s, stcut_px_bg);
    c->cd(6);
    Draw1DSignificance(stcut_px_s, stcut_px_bg, "stcut_px_significance", "left");

    c->cd(7);
    TH1D* stcut_py_s = H2("fh_stcut_signal")->ProjectionY();
    TH1D* stcut_py_bg = H2("fh_stcut_bg")->ProjectionY();
    Draw1DCutsForSandBg(stcut_py_s, stcut_py_bg);
    c->cd(8);
    Draw1DSignificance(stcut_py_s, stcut_py_bg, "stcut_py_significance", "left");

//   c->cd(9);
//   TH2D* fh_significance = CalculateSignificance2D(fh_stcut_signal, fh_stcut_bg, "stcut_2dsignificance", "significance");
//   fh_significance->Draw("COLZ");
}

//Draw TT cut distribution (full reco tracks)
void CbmAnaDielectronTaskDraw::DrawTTCut()
{
   TCanvas *c = new TCanvas("ttcut_distribution", "ttcut_distribution", 1200, 600);
   c->Divide(4,2);
   c->cd(1);
   DrawHist2D(H2("fh_ttcut_signal"));
   c->cd(2);
   DrawHist2D(H2("fh_ttcut_bg"));
   c->cd(3);
   DrawHist2D(H2("fh_ttcut_pi0"));
   c->cd(4);
   DrawHist2D(H2("fh_ttcut_gamma"));
   c->cd(5);
   TH1D* ttcut_px_s = H2("fh_ttcut_signal")->ProjectionX();
   TH1D* ttcut_px_bg = H2("fh_ttcut_bg")->ProjectionX();
   Draw1DCutsForSandBg(ttcut_px_s, ttcut_px_bg);
   c->cd(6);
   Draw1DSignificance(ttcut_px_s, ttcut_px_bg, "ttcut_px_significance", "left");

   c->cd(7);
   TH1D* ttcut_py_s = H2("fh_ttcut_signal")->ProjectionY();
   TH1D* ttcut_py_bg = H2("fh_ttcut_bg")->ProjectionY();
   Draw1DCutsForSandBg(ttcut_py_s, ttcut_py_bg);
   c->cd(8);
   Draw1DSignificance(ttcut_py_s, ttcut_py_bg, "ttcut_px_significance", "left");

   //c12->cd(9);
   //TH2D* fh_significance_tt = CalculateSignificance2D(fh_ttcut_signal, fh_ttcut_bg, "ttcut_2dsignificance", "significance");
   //fh_significance_tt->Draw("COLZ");
}

void CbmAnaDielectronTaskDraw::DrawGammaVertex()
{
   TCanvas *c8 = new TCanvas("gamma_vertex","gamma_vertex", 1200, 400);
   c8->Divide(3,1);
   c8->cd(1);
   DrawHist2D(H2("fh_mc_vertex_gamma_xz"), "X [cm]", "Z [cm]", "");
   c8->cd(2);
   DrawHist2D(H2("fh_mc_vertex_gamma_yz"), "Y [cm]", "Z [cm]", "");
   c8->cd(3);
   DrawHist2D(H2("fh_mc_vertex_gamma_xy"), "X [cm]", "Y [cm]", "");
}

//Draw Dsts cut distribution for the first and the second MVD stations
void CbmAnaDielectronTaskDraw::DrawDstsCut()
{
   if (fUseMvd) {
      TCanvas *c14 = new TCanvas("mvd1_cut_distribution", "mvd1_cut_distribution", 800, 800);
      c14->Divide(2,2);
      c14->cd(1);
      DrawHist2D(H2("fh_dsts_signal"));
      c14->cd(2);
      DrawHist2D(H2("fh_dsts_bg"));
      c14->cd(3);
      DrawHist2D(H2("fh_dsts_gamma"));
      c14->cd(4);
      DrawHist2D(H2("fh_dsts_pi0"));
      //c14->cd(5);
      //H2("fh_dsts_eta")->Draw("COLZ");
      //c14->cd(6);
      //TH2D* fh_significance_dsts = CalculateSignificance2D(fh_dsts_signal, fh_dsts_bg, "dsts_2dsignificance", "significance");
      //fh_significance_dsts->Draw("COLZ");
   }

   //DSTS 2 CUT
   if (fUseMvd) {
      TCanvas *c20 = new TCanvas("mvd2_cut_distribution", "mvd2_cut_distribution", 800, 800);
      c20->Divide(2,2);
      c20->cd(1);
      DrawHist2D(H2("fh_dsts2_signal"));
      c20->cd(2);
      DrawHist2D(H2("fh_dsts2_bg"));
      c20->cd(3);
      DrawHist2D(H2("fh_dsts2_gamma"));
      c20->cd(4);
      DrawHist2D(H2("fh_dsts2_pi0"));
      //c20->cd(5);
      //fh_dsts2_eta")->Draw("COLZ");
   }
}

void CbmAnaDielectronTaskDraw::Draw1DHistoAfterEachCut(
      TH1D* mc,
      TH1D* acc,
      TH1D* rec,
      TH1D* chi_prim,
      TH1D* elid,
      TH1D* gamma,
      TH1D* dsts,
      TH1D* dsts2,
      TH1D* st,
      TH1D* tt,
      TH1D* pt,
      TH1D* angle,
      TH1D* apm,
      Bool_t logy,
      const string& titleX,
      const string& titleY)
{
   Int_t lineW = 2;
   string drOpt = "";
   Double_t min = 9999999999.;
   TH1D* firstHisto = NULL;
   if (mc != NULL){
      DrawHist1D( mc, titleX, titleY, kGreen+3, lineW, LIT_LINE_STYLE1, 2, LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, drOpt);
      drOpt = "same";// if the hist is drawn, next histos must be drawn with "same" parameter
      if (firstHisto == NULL) firstHisto = mc;
      if (min > mc->GetMinimum()) { min = mc->GetMinimum(); }
   }
   if (acc != NULL){
      DrawHist1D( acc, titleX, titleY, kOrange+3, lineW, LIT_LINE_STYLE1, 2, LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, drOpt);
      drOpt = "same";
      if (firstHisto == NULL) firstHisto = acc;
      if (min > acc->GetMinimum()) { min = acc->GetMinimum(); }
   }
   if (rec != NULL){
      DrawHist1D( rec, titleX, titleY, kBlack, lineW, LIT_LINE_STYLE1, 2, LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, drOpt);
      drOpt = "same";
      if (firstHisto == NULL) firstHisto = rec;
      if (min > rec->GetMinimum()) { min = rec->GetMinimum(); }
   }
   if (chi_prim != NULL){
      DrawHist1D( chi_prim, titleX, titleY, kOrange+7, lineW, LIT_LINE_STYLE1, 2, LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, drOpt);
      drOpt = "same";
      if (firstHisto == NULL) firstHisto = chi_prim;
      if (min > chi_prim->GetMinimum()) { min = chi_prim->GetMinimum(); }
   }
   if (elid != NULL){
      DrawHist1D( elid, titleX, titleY, kRed, lineW, LIT_LINE_STYLE1, 2, LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, drOpt);
      drOpt = "same";
      if (firstHisto == NULL) firstHisto = elid;
      if (min > elid->GetMinimum()) { min = elid->GetMinimum(); }
   }
   if (gamma != NULL){
      DrawHist1D( gamma,titleX, titleY, kPink-6, lineW, LIT_LINE_STYLE1, 2, LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, drOpt);
      drOpt = "same";
      if (firstHisto == NULL) firstHisto = gamma;
      if (min > gamma->GetMinimum()) { min = gamma->GetMinimum(); }
   }
   if (dsts != NULL && fUseMvd){
      DrawHist1D( dsts, titleX, titleY, kGreen, lineW, LIT_LINE_STYLE1, 2, LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, drOpt);
      drOpt = "same";
      if (firstHisto == NULL) firstHisto = dsts;
      if (min > dsts->GetMinimum()) { min = dsts->GetMinimum(); }
   }
   if (dsts2 != NULL && fUseMvd){
      DrawHist1D( dsts2, titleX, titleY, kBlack, lineW, LIT_LINE_STYLE1, 2, LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, drOpt);
      drOpt = "same";
      if (firstHisto == NULL) firstHisto = dsts2;
      if (min > dsts2->GetMinimum()) { min = dsts2->GetMinimum(); }
   }
   if (st != NULL){
      DrawHist1D( st, titleX, titleY, kOrange-3, lineW, LIT_LINE_STYLE1, 2, LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, drOpt);
      drOpt = "same";
      if (firstHisto == NULL) firstHisto = st;
      if (min > st->GetMinimum()) { min = st->GetMinimum(); }
   }
   if (tt != NULL){
      DrawHist1D( tt, titleX, titleY, kYellow+1, lineW, LIT_LINE_STYLE1, 2, LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, drOpt);
      drOpt = "same";
      if (firstHisto == NULL) firstHisto = tt;
      if (min > tt->GetMinimum()) { min = tt->GetMinimum(); }
   }
   if (pt != NULL){
      DrawHist1D( pt, titleX, titleY, kMagenta, lineW, LIT_LINE_STYLE1, 2, LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, drOpt);
      drOpt = "same";
      if (firstHisto == NULL) firstHisto = pt;
      if (min > pt->GetMinimum()) { min = pt->GetMinimum(); }
   }
   if (angle != NULL){
      DrawHist1D( angle, titleX, titleY, kViolet+10, lineW, LIT_LINE_STYLE1, 2, LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, drOpt);
      drOpt = "same";
      if (firstHisto == NULL) firstHisto = angle;
      if (min > angle->GetMinimum()) { min = angle->GetMinimum(); }
   }
   if (apm != NULL){
      DrawHist1D( apm, titleX, titleY, kPink+8, lineW, LIT_LINE_STYLE1, 2, LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, drOpt);
      drOpt = "same";
      if (firstHisto == NULL) firstHisto = apm;
      if (min > apm->GetMinimum()) { min = apm->GetMinimum(); }
   }

   if (min <= 0.0) min = 1e-7;
   if (firstHisto != NULL) firstHisto->SetMinimum(min);

   TLegend* leg = new TLegend(0.80,0.32,0.99, 0.99);
   if (mc != NULL)leg->AddEntry(mc, "mc", "l");
   if (acc != NULL)leg->AddEntry(acc, "acc", "l");
   if (rec != NULL)leg->AddEntry(rec, "rec", "l");
   if (chi_prim != NULL)leg->AddEntry(chi_prim, "#chi^{2}_{prim}", "l");
   if (elid != NULL)leg->AddEntry(elid, "el id", "l");
   if (gamma != NULL)leg->AddEntry(gamma, "m_{#gamma}", "l");
   if (dsts != NULL && fUseMvd) leg->AddEntry(dsts, "d_{MVD}", "l");
   if (dsts2 != NULL && fUseMvd) leg->AddEntry(dsts2, "d_{MVD2}", "l");
   if (st != NULL)leg->AddEntry(st, "ST", "l");
   if (tt != NULL)leg->AddEntry(tt, "TT", "l");
   if (pt != NULL)leg->AddEntry(pt, "P_{t}", "l");
   if (angle != NULL)leg->AddEntry(angle, "#Theta_{1,2}", "l");
   if (apm != NULL)leg->AddEntry(apm, "APM", "l");
   leg->SetFillColor(kWhite);
   leg->Draw();

   gPad->SetGridx(true);
   gPad->SetGridy(true);
   gPad->SetLogy(logy);
}

//Draw Invariant mass distributions after each cut
void CbmAnaDielectronTaskDraw::DrawInvariantMassAfterEachCut()
{
   TCanvas *c15 = new TCanvas("minv_after_each_cut", "minv_after_each_cut", 1200, 600);
   c15->Divide(2,1);
   c15->cd(1);
   Draw1DHistoAfterEachCut(
         H1("fh_mc_signal_minv"),
         H1("fh_acc_signal_minv"),
         H1("fh_rec_signal_minv"),
         H1("fh_chi_prim_signal_minv"),
         H1("fh_el_id_signal_minv"),
         H1("fh_gammacut_signal_minv"),
         H1("fh_dstscut_signal_minv"),
         H1("fh_dsts2cut_signal_minv"),
         H1("fh_stcut_signal_minv"),
         H1("fh_ttcut_signal_minv"),
         H1("fh_ptcut_signal_minv"),
         H1("fh_anglecut_signal_minv"),
         H1("fh_apmcut_signal_minv"),
         true); //logy

   c15->cd(2);
   Draw1DHistoAfterEachCut(
         NULL,
         NULL,
         H1("fh_rec_bg_minv"),
         H1("fh_chi_prim_bg_minv"),
         H1("fh_el_id_bg_minv"),
         H1("fh_gammacut_bg_minv"),
         H1("fh_dstscut_bg_minv"),
         H1("fh_dsts2cut_bg_minv"),
         H1("fh_stcut_bg_minv"),
         H1("fh_ttcut_bg_minv"),
         H1("fh_ptcut_bg_minv"),
         H1("fh_anglecut_bg_minv"),
         H1("fh_apmcut_bg_minv"),
         true);//logy

}

void CbmAnaDielectronTaskDraw::DrawMinvBothSandBG(
      TH1* s1,
      TH1* bg1,
      const string& text)
{
   TH1D* s = (TH1D*)s1->Clone();
   TH1D* bg = (TH1D*)bg1->Clone();
   TH1D* sbg = (TH1D*)bg->Clone();
   sbg->Add(s);
   sbg->SetMinimum(1e-8);

   DrawHist1D(sbg, bg, s, NULL, "","","", "","","","", kLitLinearScale, kLitLogScale,
         false, 0,0,0,0);
   s->SetFillColor(kRed);
   s->SetLineColor(kBlack);
   s->SetLineWidth(1);
   s->SetLineStyle(LIT_MARKER_STYLE1);
   bg->SetFillColor(kYellow - 10);
   bg->SetLineColor(kBlack);
   bg->SetLineWidth(1);
   bg->SetLineStyle(LIT_MARKER_STYLE1);
   sbg->SetFillColor(kBlue);
   sbg->SetLineColor(kBlack);
   sbg->SetLineWidth(1);
   sbg->SetLineStyle(LIT_MARKER_STYLE1);
   s->SetMarkerStyle(1);
   bg->SetMarkerStyle(1);
   sbg->SetMarkerStyle(1);
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   DrawTextOnHist(text, 0.65, 0.78, 0.85, 0.9);
}

// Invariant mass distribution after each cut for BG and signal
void CbmAnaDielectronTaskDraw::DrawInvariantMassSandBG()
{
   Int_t hi = 1;
   TCanvas *c16 = new TCanvas("minv_both_s_and_bg", "minv_both_s_and_bg", 1200, 900);
   c16->Divide(4,3);
   c16->cd(hi++);
   DrawMinvBothSandBG(H1("fh_rec_signal_minv"),H1("fh_rec_bg_minv"), "rec");
   c16->cd(hi++);
   DrawMinvBothSandBG(H1("fh_chi_prim_signal_minv"),H1("fh_chi_prim_bg_minv"), "#chi^{2}_{prim}");
   c16->cd(hi++);
   DrawMinvBothSandBG(H1("fh_el_id_signal_minv"),H1("fh_el_id_bg_minv"), "ID");
   c16->cd(hi++);
   DrawMinvBothSandBG(H1("fh_gammacut_signal_minv"),H1("fh_gammacut_bg_minv"), "m_{#gamma}");
   if (fUseMvd) {
      c16->cd(hi++);
      DrawMinvBothSandBG(H1("fh_dstscut_signal_minv"),H1("fh_dstscut_bg_minv"), "d_{mvd}");
      c16->cd(hi++);
      DrawMinvBothSandBG(H1("fh_dsts2cut_signal_minv"),H1("fh_dsts2cut_bg_minv"), "d_{mvd2}");
   }
   c16->cd(hi++);
   DrawMinvBothSandBG(H1("fh_stcut_signal_minv"),H1("fh_stcut_bg_minv"), "ST");
   c16->cd(hi++);
   DrawMinvBothSandBG(H1("fh_ttcut_signal_minv"),H1("fh_ttcut_bg_minv"), "TT");
   c16->cd(hi++);
   DrawMinvBothSandBG(H1("fh_ptcut_signal_minv"),H1("fh_ptcut_bg_minv"), "p_{t}");
   c16->cd(hi++);
   DrawMinvBothSandBG(H1("fh_anglecut_signal_minv"),H1("fh_anglecut_bg_minv"), "#Theta_{1,2}");
   c16->cd(hi++);
   DrawMinvBothSandBG(H1("fh_apmcut_signal_minv"),H1("fh_apmcut_bg_minv"), "APM");
}

// Invariant mass distribution after each cut for Pi0 and Etal
void CbmAnaDielectronTaskDraw::DrawInvariantMassPi0andEta()
{
   TCanvas *c17 = new TCanvas("minv_pi0_eta", "minv_pi0_eta", 1200, 600);
   c17->Divide(2,1);
   c17->cd(1);
   Draw1DHistoAfterEachCut(
         NULL,
         NULL,
         H1("fh_rec_pi0_minv"),
         H1("fh_chi_prim_pi0_minv"),
         H1("fh_el_id_pi0_minv"),
         H1("fh_gammacut_pi0_minv"),
         H1("fh_dstscut_pi0_minv"),
         H1("fh_dsts2cut_pi0_minv"),
         H1("fh_stcut_pi0_minv"),
         H1("fh_ttcut_pi0_minv"),
         H1("fh_ptcut_pi0_minv"),
         H1("fh_anglecut_pi0_minv"),
         H1("fh_apmcut_pi0_minv"),
         true);

   c17->cd(2);
   Draw1DHistoAfterEachCut(
         NULL,
         NULL,
         H1("fh_rec_eta_minv"),
         H1("fh_chi_prim_eta_minv"),
         H1("fh_el_id_eta_minv"),
         H1("fh_gammacut_eta_minv"),
         H1("fh_dstscut_eta_minv"),
         H1("fh_dsts2cut_eta_minv"),
         H1("fh_stcut_eta_minv"),
         H1("fh_ttcut_eta_minv"),
         H1("fh_ptcut_eta_minv"),
         H1("fh_anglecut_eta_minv"),
         H1("fh_apmcut_eta_minv"),
         true);
}

//SOURCE TRACKS
void CbmAnaDielectronTaskDraw::DrawBGSourceTracks()
{
   gStyle->SetPaintTextFormat("4.1f");
   TCanvas *c18 = new TCanvas("nof_tracks_per_event", "nof_tracks_per_event", 1200, 400);
   c18->Divide(3,1);
   c18->cd(1);
   DrawHist1D( H1("fh_nof_bg_tracks"), "analysis step", "tracks/event", kBlack,
         2, LIT_LINE_STYLE1, 2, LIT_MARKER_STYLE1, kLitLinearScale, kLitLogScale, "hist text0");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   c18->cd(2);
   DrawHist1D( H1("fh_nof_el_tracks"), "analysis step", "tracks/event", kBlack,
         2, LIT_LINE_STYLE1, 2, LIT_MARKER_STYLE1, kLitLinearScale, kLitLogScale, "");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   c18->cd(3);
   TH1D* purity = new TH1D("purity","purity",7, 0., 7.);
   purity->Divide(H1("fh_nof_bg_tracks"), H1("fh_nof_el_tracks"));
   DrawHist1D( purity, "analysis step", "purity", kBlack,
         2, LIT_LINE_STYLE1, 2, LIT_MARKER_STYLE1, kLitLinearScale, kLitLogScale, "hist text0");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   TCanvas *c18_1 = new TCanvas("nof_tracks_2d", "nof_tracks_2d", 500, 500);
   TH2D* source_tracks_clone = (TH2D*)H2("fh_source_tracks")->Clone();
   source_tracks_clone->SetStats(false);
   Int_t nBinsX = source_tracks_clone->GetNbinsX();
   Int_t nBinsY = source_tracks_clone->GetNbinsY();
   for (Int_t x = 1; x <= nBinsX; x++){
      Double_t scale =100./(H1("fh_nof_bg_tracks")->GetBinContent(x));
      for (Int_t y = 1; y <= nBinsY; y++){
         Double_t val = scale * source_tracks_clone->GetBinContent(x,y);
         source_tracks_clone->SetBinContent(x,y,val);
      }
   }
   source_tracks_clone->GetXaxis()->SetLabelSize(0.06);
   source_tracks_clone->GetYaxis()->SetLabelSize(0.06);
   Int_t ny = 6;
   string yLabels[6] = {"#gamma", "#pi^{0}", "#pi^{#pm}", "p", "K", "oth"};
   for (Int_t y = 1; y <= ny; y++){
      source_tracks_clone->GetYaxis()->SetBinLabel(y,yLabels[y-1].c_str());
   }
   source_tracks_clone->SetMarkerColor(kBlack);
   source_tracks_clone->SetMarkerSize(1.8);
   DrawHist2D(source_tracks_clone, "Analysis step", "Particle" , "",
         kLitLinearScale, kLitLinearScale, kLitLinearScale, "text COLZ");

   Int_t nx = 7;
   H1("fh_nof_bg_tracks")->GetXaxis()->SetLabelSize(0.06);
   H1("fh_nof_el_tracks")->GetXaxis()->SetLabelSize(0.06);
   purity->GetXaxis()->SetLabelSize(0.06);
   source_tracks_clone->GetXaxis()->SetLabelSize(0.06);
   string xLabels[7] = {"rec", "#chi^{2}_{prim}", "ID", "m_{#gamma}", "ST", "TT", "P_{t}"};
   for (Int_t i = 1; i <= nx; i++){
      H1("fh_nof_bg_tracks")->GetXaxis()->SetBinLabel(i,xLabels[i-1].c_str());
      H1("fh_nof_el_tracks")->GetXaxis()->SetBinLabel(i,xLabels[i-1].c_str());
      purity->GetXaxis()->SetBinLabel(i,xLabels[i-1].c_str());
      source_tracks_clone->GetXaxis()->SetBinLabel(i,xLabels[i-1].c_str());
   }
}

//MINV vs. PT
void CbmAnaDielectronTaskDraw::DrawMinvVsPt()
{
   TCanvas *c19 = new TCanvas("minv_vs_pt", "minv_vs_pt",600, 600);
   DrawHist2D(H2("fh_ttcut_signal_minv_pt"));
}

void CbmAnaDielectronTaskDraw::DrawBgSourcesVsMomentum()
{
   TCanvas *c21 = new TCanvas("bg_sources_vs_momentum","bg_sources_vs_momentum",900, 600);
   c21->Divide(3,2);
   c21->cd(1);
   DrawHist1D(H1("fh_gamma_mom"), "", "", kBlack, 2, LIT_LINE_STYLE1, 2,
         LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, "");
   c21->cd(2);
   DrawHist1D(H1("fh_pi0_mom"), "", "", kBlack, 2, LIT_LINE_STYLE1, 2,
         LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, "");
   c21->cd(3);
   DrawHist1D(H1("fh_pions_mom"), "", "", kBlack, 2, LIT_LINE_STYLE1, 2,
         LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, "");
   c21->cd(4);
   DrawHist1D(H1("fh_proton_mom"), "", "", kBlack, 2, LIT_LINE_STYLE1, 2,
         LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, "");
   c21->cd(5);
   DrawHist1D(H1("fh_kaons_mom"), "", "", kBlack, 2, LIT_LINE_STYLE1, 2,
         LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, "");
   c21->cd(6);
   DrawHist1D(H1("fh_other_mom"), "", "", kBlack, 2, LIT_LINE_STYLE1, 2,
         LIT_MARKER_STYLE1, kLitLinearScale, kLitLinearScale, "");
}

TH1D* CbmAnaDielectronTaskDraw::CreateCumulativeProbabilityHisto(
      TH1D* histo,
      TString name,
      TString title)
{

   Int_t nBins = histo->GetNbinsX();
   Double_t xmin = histo->GetXaxis()->GetXmin();
   Double_t xmax = histo->GetXaxis()->GetXmax();
   TH1D* h1 = new TH1D(name,title, nBins, xmin, xmax);

   Double_t sumCut = 0;
   Double_t sumAll = 0;
   Double_t prob = 0;
   for(Int_t i = 1; i <= nBins; i++) {
      sumAll = histo->Integral();
      sumCut = histo->Integral(1, i);
      if (sumAll != 0.) prob = (sumCut /sumAll)*100.;
      else prob = 0;
      h1->SetBinContent(i, prob);
   }
   return h1;
}

void CbmAnaDielectronTaskDraw::DrawCumulativeProbabilities(
      TH1D* s,
      TH1D* bg)
{
   TH1D* probS = CreateCumulativeProbabilityHisto(s, "signal_cum_prob", "signal_cum_prob");
   TH1D* probBg = CreateCumulativeProbabilityHisto(bg, "bg_cum_prob", "bg_cum_prob");
   probS->SetLineColor(kRed);

   probS->Draw();
   probBg->SetLineColor(kBlue);
   probBg->Draw("same");
}

void CbmAnaDielectronTaskDraw::DrawCumulativeProbabilitiesAll()
{
   TCanvas *c22 = new TCanvas("cumulative_probability","cumulative_probability",1200, 800);
   c22->Divide(3,2);
   c22->cd(1);
   DrawCumulativeProbabilities(H1("fh_chi2_prim_signal"), H1("fh_chi2_prim_bg"));
   c22->cd(2);
   DrawCumulativeProbabilities(H1("fh_pt_signal"), H1("fh_pt_bg"));
   c22->cd(3);
   DrawCumulativeProbabilities(H1("fh_angle_signal"), H1("fh_angle_bg"));
   c22->cd(4);
   DrawCumulativeProbabilities(H1("fh_mom_signal"), H1("fh_mom_bg"));
}
