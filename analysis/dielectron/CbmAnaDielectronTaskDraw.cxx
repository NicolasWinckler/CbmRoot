/** CbmAnaDielectronTaskDraw.cxx
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2011
 * @version 1.0
 **/

#include "CbmAnaDielectronTaskDraw.h"


#include "CbmDrawHist.h"
#include "CbmHistManager.h"
#include "std/utils/CbmLitUtils.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <boost/assign/list_of.hpp>
#include "CbmAnaPTree.h"

#include "TText.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLine.h"
#include "TMath.h"
#include "TKey.h"
#include "TClass.h"
#include "TF1.h"
#include "TEllipse.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TPad.h"

ClassImp(CbmAnaDielectronTaskDraw);

using namespace std;
using namespace lit;
using boost::assign::list_of;
CbmAnaDielectronTaskDraw::CbmAnaDielectronTaskDraw()
  :TObject(),
   fNofEvents(0),
   fUseMvd(kFALSE),
   fDrawSignificance(kFALSE),
   fTrdAnnCut(0.),	    
   fRichAnnCut(0.),	    
   fPtCut(0.),	    
   fAngleCut(0.),	    
   fChiPrimCut(0.),	    
   fGammaCut(0.),	    
   fStCutAngle(0.),	    
   fStCutPP(0.),	    
   fTtCutAngle(0.),	    
   fTtCutPP(0.),
   fRtCutAngle(0.),
   fRtCutPP(0.),
   fMvd1CutP(0.),	    
   fMvd1CutD(0.),	    
   fMvd2CutP(0.),	    
   fMvd2CutD(0.),       
   fHM(NULL),
   fCanvas(),
   fOutputDir(""),
   fPt(NULL)
{
}

void CbmAnaDielectronTaskDraw::DrawHistFromFile(
      const string& fileName,
      const string& outputDir,
      Bool_t useMvd,
      Bool_t drawSig)
{
   SetDefaultDrawStyle();
   fOutputDir = outputDir;
   fUseMvd = useMvd;
   fDrawSignificance = drawSig;

   fTrdAnnCut = 0.85;
   fRichAnnCut = 0.0;
   fPtCut = 0.2;
   fAngleCut = 1.;
   fChiPrimCut = 2.;
   fGammaCut = 0.025;
   fStCutAngle = 1.5;
   fStCutPP = 1.5;
   fTtCutAngle = 0.75;
   fTtCutPP = 4.0;
   fRtCutAngle = 1.25;
   fRtCutPP = 2.0;
   fMvd1CutP = 1.2;
   fMvd1CutD = 0.4;
   fMvd2CutP = 1.5;
   fMvd2CutD = 0.5;

   fPt = new CbmAnaPTree();

   fHM = new CbmHistManager();
   TFile* file = new TFile(fileName.c_str());
   fHM->ReadFromFile(file);

   fNofEvents = (Int_t)H1("fh_event_number")->GetEntries();
   cout << "File name = " << fileName << endl;
   cout << "Number of events = " << fNofEvents<< endl;

   fHM->ScaleByPattern(".*", 1./fNofEvents);
   SOverBgAll();
   RebinMinvHist();
   if (!fUseMvd) RemoveMvdCutBins();
   DrawPtYDistributionAll();
   DrawPtYEfficiencyAll();
   DrawMomentumDistributionAll();
   DrawMomentumEfficiencyAll();
   DrawMotherPdg();
   DrawSourcesBgPairsAll();
   DrawGammaVertex();
   DrawCutDistributions();
   DrawMinvForEachAnalysisStep();
   DrawMinvSandBgAll();
   DrawMinvSourceAll();
   DrawBgSourceTracks();
   DrawMismatchesAndGhosts();
   DrawMinvPtAll();
   DrawBgSourcesVsMomentum();
   DrawMvdCutQa();
   DrawMvdAndStsHist();
   DrawElPiMomHis();

   SaveCanvasToImage();

   string qaFile = fOutputDir + "/lmvm_results.json";
   fPt->Write(qaFile);
}

void CbmAnaDielectronTaskDraw::RebinMinvHist()
{
   int nRebin = 10;
   for (int i = 0; i < CbmAnaLmvmNames::fNofAnaSteps; i++){
      H1("fh_signal_minv_" + CbmAnaLmvmNames::fAnaSteps[i])->Rebin(nRebin);
      H1("fh_bg_minv_" + CbmAnaLmvmNames::fAnaSteps[i])->Rebin(nRebin);
      H1("fh_pi0_minv_" + CbmAnaLmvmNames::fAnaSteps[i])->Rebin(nRebin);
      H1("fh_eta_minv_" + CbmAnaLmvmNames::fAnaSteps[i])->Rebin(nRebin);
      H1("fh_bg_truematch_minv_" + CbmAnaLmvmNames::fAnaSteps[i])->Rebin(2*nRebin);
      H1("fh_bg_mismatch_minv_" + CbmAnaLmvmNames::fAnaSteps[i])->Rebin(2*nRebin);
      H1("fh_bg_truematch_el_minv_" + CbmAnaLmvmNames::fAnaSteps[i])->Rebin(2*nRebin);
      H1("fh_bg_truematch_notel_minv_" + CbmAnaLmvmNames::fAnaSteps[i])->Rebin(2*nRebin);

      for (int iP = 0; iP < CbmAnaLmvmNames::fNofBgPairSources; iP++){
         stringstream ss;
         ss << "fh_source_bg_minv_" << iP << "_" << CbmAnaLmvmNames::fAnaSteps[i];
         H1(ss.str())->Rebin(8*nRebin);
      }
   }
}

TH1D* CbmAnaDielectronTaskDraw::H1(
      const string& name)
{
   return (TH1D*) fHM->H1(name);
}

TH2D* CbmAnaDielectronTaskDraw::H2(
      const string& name)
{
   return (TH2D*) fHM->H1(name);
}

TCanvas* CbmAnaDielectronTaskDraw::CreateCanvas(
      const string& name,
      const string& title,
      int width,
      int height)
{
   TCanvas *c = new TCanvas(name.c_str(), title.c_str(), width, height);
   fCanvas.push_back(c);
   return c;
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

TH2D* CbmAnaDielectronTaskDraw::DivideH2D(
      TH2D* h1,
      TH2D* h2)
{
    Int_t nBinsX = h1->GetNbinsX();
    Double_t minX = h1->GetXaxis()->GetXmin();
    Double_t maxX = h1->GetXaxis()->GetXmax();
    Int_t nBinsY = h1->GetNbinsY();
    Double_t minY = h1->GetYaxis()->GetXmin();
    Double_t maxY = h1->GetYaxis()->GetXmax();

    string hname = string(h1->GetName()) + "_divide";
    TH2D* h3 = new TH2D(hname.c_str(), h1->GetTitle(), nBinsX, minX, maxX, nBinsY, minY, maxY);
    //h1->Sumw2();
    //h2->Sumw2();
    h3->Divide(h1,h2,100.,1.,"B");
    return h3;
}

TH1D* CbmAnaDielectronTaskDraw::DivideH1D(
      TH1D* h1,
      TH1D* h2)
{
    Int_t nBins = h1->GetNbinsX();
    Double_t min = h1->GetXaxis()->GetXmin();
    Double_t max = h1->GetXaxis()->GetXmax();
    string hname = string(h1->GetName()) + "_divide";
    TH1D* h3 = new TH1D(hname.c_str(), h1->GetTitle(), nBins, min, max);
    h3->Sumw2();
    h3->Divide(h1, h2, 100.,1., "B");
    return h3;
}

TH1D* CbmAnaDielectronTaskDraw::CreateSignificanceH1D(
      TH1D* s,
      TH1D* bg,
      const string& name,
      const string& option)
{
    Int_t nBins = s->GetNbinsX();
    Double_t xmin = s->GetXaxis()->GetXmin();
    Double_t xmax = s->GetXaxis()->GetXmax();
    TH1D* hsig = new TH1D(name.c_str(), name.c_str(), nBins, xmin, xmax);
    hsig->GetXaxis()->SetTitle(s->GetXaxis()->GetTitle());

    Double_t sumSignal = 0., sumBg = 0., significance = 0.;
    // "right" - one wants to reject right part of the histogram.
    // value > cut -> reject
    if(option == "right"){
        for (Int_t i = 1; i <= nBins; i++) {
            sumSignal = s->Integral(1, i, "width");
            sumBg = bg->Integral(1, i, "width");
            Double_t prov = TMath::Sqrt(sumSignal+sumBg);
            if (prov != 0. ) significance = sumSignal / prov;
            else significance = 0.;
            hsig->SetBinContent(i, significance);
        }
    // "left" - one wants to reject left part of the histogram.
    // value < cut -> reject
    } else if (option == "left"){
        for (Int_t i = nBins; i >= 1; i--) {
            sumSignal = s->Integral(i, nBins,"width");
            sumBg = bg->Integral(i, nBins,"width");
            Double_t prov = TMath::Sqrt(sumSignal+sumBg);
            if (prov != 0. ) significance = sumSignal / prov;
            else significance = 0.;
            hsig->SetBinContent(i, significance);
        }
    }
    return hsig;
}

TH2D* CbmAnaDielectronTaskDraw::CreateSignificanceH2D(
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

void CbmAnaDielectronTaskDraw::SOverBg(
      AnalysisSteps step)
{
   TH1D* s = H1("fh_signal_minv_" + CbmAnaLmvmNames::fAnaSteps[step]);
   TH1D* bg = H1("fh_bg_minv_" + CbmAnaLmvmNames::fAnaSteps[step]);
   TH2D* pty = H2("fh_signal_pty_" + CbmAnaLmvmNames::fAnaSteps[step]);
   TH2D* ptymc = H2("fh_signal_pty_" + CbmAnaLmvmNames::fAnaSteps[0]);

   if (s->GetEntries() < 1) return;

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
   fPt->Put("sbg_"+CbmAnaLmvmNames::fAnaSteps[step], sumSignal/sumBg);
   fPt->Put("eff_"+CbmAnaLmvmNames::fAnaSteps[step], (Double_t)pty->GetEntries()/ptymc->GetEntries()*100.);
   fPt->Put("signal_minv_mean_"+CbmAnaLmvmNames::fAnaSteps[step], 1000.*mean);
   fPt->Put("signal_minv_rms_"+CbmAnaLmvmNames::fAnaSteps[step], 1000.*sigma);
}

void CbmAnaDielectronTaskDraw::SOverBgAll()
{
   TCanvas *c1 = CreateCanvas("lmvm_signal_fitting", "lmvm_signal_fitting", 600, 600);
   SOverBg(kReco);
   SOverBg(kChi2Prim);
   SOverBg(kElId);
   SOverBg(kGammaCut);
   if (fUseMvd) SOverBg(kMvd1Cut);
   if (fUseMvd) SOverBg(kMvd2Cut);
   SOverBg(kStCut);
   SOverBg(kTtCut);
   SOverBg(kPtCut);
}

void CbmAnaDielectronTaskDraw::DrawPtYDistribution(
      int step,
      bool drawAnaStep)
{
   TH2D* h = H2("fh_signal_pty_" + CbmAnaLmvmNames::fAnaSteps[step]);
   TH2D* hmc = H2("fh_signal_pty_" + CbmAnaLmvmNames::fAnaSteps[0]);

   DrawH2(h, kLinear, kLinear, kLinear, "COLZ");
   if (drawAnaStep) DrawEfficiencyOnHist(h, hmc, 0.2, 1.8);
   if (drawAnaStep) DrawTextOnHist(CbmAnaLmvmNames::fAnaStepsLatex[step], 0.50, 0.78, 0.70, 0.9);
}

void CbmAnaDielectronTaskDraw::DrawPtYDistributionAll()
{
   Int_t hi = 1;
   TCanvas *c = CreateCanvas("lmvm_pty", "lmvm_pty", 750, 1000);
   c->Divide(3, 4);
   for (int step = 0; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)) continue;
      c->cd(hi++);
      DrawPtYDistribution(step);
   }

   TCanvas *cMc = CreateCanvas("lmvm_pty_"+CbmAnaLmvmNames::fAnaSteps[kPtCut], "lmvm_pty_"+CbmAnaLmvmNames::fAnaSteps[kPtCut], 800, 800);
   //cMc->Divide(2,1);
   //cMc->cd(1);
  // DrawPtYDistribution(kMc, false);
  // cMc->cd(2);
   DrawPtYDistribution(kPtCut, false);
}

void CbmAnaDielectronTaskDraw::DrawPtYEfficiency(
      int step)
{
   TH2D* h = H2("fh_signal_pty_" + CbmAnaLmvmNames::fAnaSteps[step]);
   // efficiency is normalized to the previous step (step - 1)
   TH2D* hmc = H2("fh_signal_pty_" + CbmAnaLmvmNames::fAnaSteps[step - 1]);

   TH2D* eff = DivideH2D(h, hmc);
   eff->GetZaxis()->SetTitle("Efficiency [%]");
   DrawH2(eff);
   DrawEfficiencyOnHist(h, hmc, 0.2, 1.8);
   DrawTextOnHist(CbmAnaLmvmNames::fAnaStepsLatex[step], 0.50, 0.78, 0.70, 0.9);
}

void CbmAnaDielectronTaskDraw::DrawPtYEfficiencyAll()
{
   Int_t hi = 1;
   TCanvas *c = CreateCanvas("lmvm_pty_efficiency", "lmvm_pty_efficiency", 750, 1000);
   c->Divide(3,4);
   for (int step = kAcc; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)) continue;
      c->cd(hi++);
      DrawPtYEfficiency(step);
   }
}

void CbmAnaDielectronTaskDraw::DrawMomentumDistributionAll()
{
    TCanvas *c = CreateCanvas("lmvm_signal_momentum_distribution", "lmvm_signal_momentum_distribution", 600, 600);
    Draw1DHistoForEachAnalysisStep("fh_signal_mom", true);
}

void CbmAnaDielectronTaskDraw::DrawMomentumEfficiencyAll()
{
//EFFICIENCY vs. MOMENTUM
/*    TCanvas *c5 = CreateCanvas("signal_momentum_efficiency","signal_momentum_efficiency", 600, 600);
    Draw1DHistoForEachAnalysisStep(
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
          false
    );*/
}

void CbmAnaDielectronTaskDraw::DrawMotherPdg()
{
   TCanvas *c = CreateCanvas("lmvm_mother_pdg", "lmvm_mother_pdg", 500, 500);
   DrawH1(list_of(H1("fh_mc_mother_pdg"))(H1("fh_acc_mother_pdg")), list_of("MC")("acc"), kLinear, kLog, true, 0.7, 0.7, 0.99, 0.99);
}

void CbmAnaDielectronTaskDraw::Draw1DSourceTypes(
      const string& hName,
      bool doScale)
{
   vector<TH1*> h;
   vector<string> hLegend;
   for (int i = 0; i < CbmAnaLmvmNames::fNofSourceTypes; i++){
      string fullName = hName+"_"+CbmAnaLmvmNames::fSourceTypes[i];
      h.push_back( H1(fullName) );
      h[i]->SetLineWidth(2);
      h[i]->SetLineColor(CbmAnaLmvmNames::fSourceTypesColor[i]);
      if (doScale) h[i]->Scale(1. / h[i]->Integral());
      hLegend.push_back( CbmAnaLmvmNames::fSourceTypesLatex[i] );
   }
   DrawH1(h, hLegend, kLinear, kLog, true, 0.90, 0.7, 0.99, 0.99);
}

void CbmAnaDielectronTaskDraw::Draw1DCut(
      const string& hName,
      const string& sigOption,
      double cutValue)
{
   Int_t w = 600;
   Int_t h = 600;
   if (fDrawSignificance) w = 1200;
   TCanvas *c = CreateCanvas( ("lmvm_" + hName).c_str(), ("lmvm_" + hName).c_str(), w, h);
   if (fDrawSignificance){
      c->Divide(2,1);
      c->cd(1);
   }
   Draw1DSourceTypes(hName);
   if (cutValue != -999999.){
      TLine* cutLine = new TLine(cutValue, 0.0, cutValue, 1.0);
      cutLine->SetLineWidth(2);
      cutLine->Draw();
   }
   if (fDrawSignificance){
      c->cd(2);
      string sName = hName+"_"+CbmAnaLmvmNames::fSourceTypes[kSignal];
      string bgName = hName+"_"+CbmAnaLmvmNames::fSourceTypes[kBg];
      TH1D* sign = CreateSignificanceH1D(H1(sName), H1(bgName), hName+"_significance", sigOption);
      DrawH1(sign);
   }
}

void CbmAnaDielectronTaskDraw::DrawCutDistributions()
{
   Draw1DCut("fh_richann", "left", fRichAnnCut);
   Draw1DCut("fh_trdann", "left", fTrdAnnCut);
   Draw2DCut("fh_tofm2");

   Draw1DCut("fh_chi2prim", "right", fChiPrimCut);
   Draw1DCut("fh_pt", "left", fPtCut);
   Draw1DCut("fh_mom", "left");
   Draw1DCut("fh_chi2sts", "right");

   Draw2DCut("fh_stcut", fStCutPP, fStCutAngle);
   Draw2DCut("fh_ttcut", fTtCutPP, fTtCutAngle);
   Draw2DCut("fh_rtcut", fRtCutPP, fRtCutAngle);

   Draw2DCut("fh_stcut_pion", fStCutPP, fStCutAngle);
   Draw2DCut("fh_ttcut_pion", fTtCutPP, fTtCutAngle);
   Draw2DCut("fh_rtcut_pion", fRtCutPP, fRtCutAngle);
   Draw2DCut("fh_stcut_truepair", fStCutPP, fStCutAngle);
   Draw2DCut("fh_ttcut_truepair", fTtCutPP, fTtCutAngle);
   Draw2DCut("fh_rtcut_truepair", fRtCutPP, fRtCutAngle);

 /*  TH2D* st = H2("fh_stcut_signal");
   double sumT = 0.;
   double sumAll = 0;
   for (int x = 1; x <= st->GetNbinsX(); x++){
      for (int y = 1; y <= st->GetNbinsY(); y++){
         double c = st->GetBinContent(x, y);
         double xc = (st->GetXaxis()->GetBinLowEdge(x) + st->GetXaxis()->GetBinUpEdge(x))/2.0;
         double yc = (st->GetYaxis()->GetBinLowEdge(y) + st->GetXaxis()->GetBinUpEdge(y))/2.0;

         Double_t val = -1.*(fStCutAngle/fStCutPP)*xc + fStCutAngle;
         if ( (xc < fStCutPP && val > yc) ) {
         // Double_t val = -1.*(fTtCutAngle/fTtCutPP)*xc + fTtCutAngle;
         // if ( (xc < fTtCutPP && val > yc) ) {
            sumT += c;
         }
         sumAll += c;
      }
   }
   cout << endl << endl << endl << "sumT/sumAll = " << 100*sumT/sumAll << endl;
*/
   Draw2DCut("fh_apmcut");
   Draw2DCut("fh_apcut");
   if (fUseMvd) {
      Draw2DCut("fh_mvd1cut", fMvd1CutD, fMvd1CutP);
      Draw2DCut("fh_mvd2cut", fMvd2CutD, fMvd2CutP);
   }
}

void CbmAnaDielectronTaskDraw::DrawSourcesBgPairsEpEm(
      int step,
      bool inPercent,
      bool drawAnaStep)
{
   TH2D* h = (TH2D*)H2("fh_source_pairs_epem_" + CbmAnaLmvmNames::fAnaSteps[step])->Clone();
   gStyle->SetPaintTextFormat("4.1");
   string labels[3] = {"#gamma", "#pi^{0}", "oth"};
   for (Int_t i = 1; i <= 3; i++){
      h->GetYaxis()->SetBinLabel(i, labels[i-1].c_str());
      h->GetXaxis()->SetBinLabel(i, labels[i-1].c_str());
   }
   //h->SetMarkerColor(0);
   h->SetMarkerSize(3);
   if (inPercent) {
      h->Scale(100. / h->Integral());
      h->GetZaxis()->SetTitle("[%]");
   } else {
      h->Scale(1000.);
      h->GetZaxis()->SetTitle("Number of pairs/event x10^{3}");
   }
   DrawH2(h, kLinear, kLinear, kLinear, "text COLZ");
   h->GetXaxis()->SetLabelSize(0.1);
   h->GetYaxis()->SetLabelSize(0.1);
   if (drawAnaStep) DrawTextOnHist(CbmAnaLmvmNames::fAnaStepsLatex[step], 0.50, 0.90, 0.70, 0.99);
}

void CbmAnaDielectronTaskDraw::DrawSourcesBgPairsAll()
{
   Int_t hi = 1;
   TCanvas *c1 = CreateCanvas("lmvm_bg_sources_pairs_epem_abs", "lmvm_bg_sources_pairs_epem_abs", 900, 900);
   c1->Divide(3,3);
   for (int step = kReco; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)) continue;
      c1->cd(hi++);
      DrawSourcesBgPairsEpEm(step, false);
   }

   hi = 1;
   TCanvas *c2 = CreateCanvas("lmvm_bg_sources_pairs_epem_percent", "lmvm_bg_sources_pairs_epem_percent", 900, 900);
   c2->Divide(3,3);
   for (int step = kReco; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)) continue;
      c2->cd(hi++);
      DrawSourcesBgPairsEpEm(step, true);
   }

   //Draw pair source histogram for the las step (ptcut)
   TCanvas *c1PtCut = CreateCanvas("lmvm_bg_sources_pairs_epem_abs_" + CbmAnaLmvmNames::fAnaSteps[kPtCut],
         "lmvm_bg_sources_pairs_epem_abs_" + CbmAnaLmvmNames::fAnaSteps[kPtCut], 600, 600);
   DrawSourcesBgPairsEpEm(kPtCut, false, false);

   TCanvas *c2PtCut = CreateCanvas("lmvm_bg_sources_pairs_epem_percent_" + CbmAnaLmvmNames::fAnaSteps[kPtCut],
         "lmvm_bg_sources_pairs_epem_percent_" + CbmAnaLmvmNames::fAnaSteps[kPtCut], 600, 600);
   DrawSourcesBgPairsEpEm(kPtCut, true, false);

   // Draw 2D histogram for sources of BG pairs
   DrawBgSource2D("lmvm_source_pairs_2d", "fh_source_pairs", CbmAnaLmvmNames::fBgPairSourceLatex, 1000., "Pairs per event x10^{3}");
}

void CbmAnaDielectronTaskDraw::Draw2DCutTriangle(
      double xCross,
      double yCross)
{
   if (xCross == -999999. || yCross == -999999.) return;
   TLine* line1 = new TLine(0., 0., xCross, 0.);
   line1->SetLineWidth(2.);
   line1->Draw();
   TLine* line2 = new TLine(0., 0., 0., yCross);
   line2->SetLineWidth(2.);
   line2->Draw();
   TLine* line3 = new TLine(xCross, 0., 0., yCross);
   line3->SetLineWidth(2.);
   line3->Draw();
}


void CbmAnaDielectronTaskDraw::Draw2DCut(
      const string& hist,
      double cutCrossX,
      double cutCrossY)
{
   TCanvas *c = CreateCanvas(("lmvm_" + hist).c_str(), ("lmvm_" + hist).c_str(), 600, 900);
   c->Divide(2,3);
   vector<TH1*> projX, projY;
   for (int i = 0; i < CbmAnaLmvmNames::fNofSourceTypes; i++){
      c->cd(i+1);
      DrawH2(H2( hist + "_"+ CbmAnaLmvmNames::fSourceTypes[i] ));
      double nofPerEvent = H2( hist + "_"+ CbmAnaLmvmNames::fSourceTypes[i] )->GetEntries()/(double)fNofEvents;
      cout << hist << "_" << CbmAnaLmvmNames::fSourceTypes[i] << " = " << nofPerEvent << endl;
      DrawTextOnHist( ( lit::NumberToString(nofPerEvent, 2) + "/ev."), 0.1, 0.9, 0.5, 0.99);
      DrawTextOnHist(CbmAnaLmvmNames::fSourceTypesLatex[i], 0.6, 0.89, 0.7, 0.99);
      Draw2DCutTriangle(cutCrossX, cutCrossY);
      projX.push_back( H2( hist + "_"+ CbmAnaLmvmNames::fSourceTypes[i] )->ProjectionX() );
      projY.push_back( H2( hist + "_"+ CbmAnaLmvmNames::fSourceTypes[i] )->ProjectionY() );
   }

   //Draw X projection
   c->cd(5);
   DrawH1(projX, CbmAnaLmvmNames::fSourceTypesLatex, kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);

   //Draw Y projection
   c->cd(6);
   DrawH1(projY, CbmAnaLmvmNames::fSourceTypesLatex, kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);

   TCanvas* c2 = CreateCanvas(("lmvm_" + hist+"_signal").c_str(), ("lmvm_" + hist+"_signal").c_str(), 800, 800);
   DrawH2(H2( hist + "_"+ CbmAnaLmvmNames::fSourceTypes[kSignal] ));
   Draw2DCutTriangle(cutCrossX, cutCrossY);
   TCanvas* c3 = CreateCanvas(("lmvm_" + hist+"_gamma").c_str(), ("lmvm_" + hist+"_gamma").c_str(), 800, 800);
   DrawH2(H2( hist + "_"+ CbmAnaLmvmNames::fSourceTypes[kGamma] ));
   Draw2DCutTriangle(cutCrossX, cutCrossY);
   //c->cd(9);
  //TH2D* fh_significance = CalculateSignificance2D(fh_stcut_signal, fh_stcut_bg, "stcut_2dsignificance", "significance");
  //fh_significance->Draw("COLZ");
}

void CbmAnaDielectronTaskDraw::DrawGammaVertex()
{
   TH2D* xz = H2("fh_vertex_el_gamma_xz_mc");
   TH2D* yz = H2("fh_vertex_el_gamma_yz_mc");
   TH2D* xy = H2("fh_vertex_el_gamma_xy_mc");
   TCanvas *c = CreateCanvas("lmvm_vertex_el_gamma_mc","lmvm_vertex_el_gamma_mc", 1200, 400);
   c->Divide(3,1);
   c->cd(1);
   DrawH2(xz);
   xz->SetMinimum(1e-3);
   gPad->SetLogz(true);
   c->cd(2);
   DrawH2(yz);
   yz->SetMinimum(1e-3);
   gPad->SetLogz(true);
   c->cd(3);
   DrawH2(xy);
   xy->SetMinimum(1e-3);
   gPad->SetLogz(true);

   // number of e+- from gamma vs Z.
   TCanvas *cZ = CreateCanvas("lmvm_vertex_el_gamma_z_mc","lmvm_vertex_el_gamma_z_mc", 600, 600);
   TH1D* zProj = (TH1D*)xz->ProjectionX("fh_vertex_el_gamma_pz_mc")->Clone();
   zProj->GetYaxis()->SetTitle("Counter per event");
   zProj->GetXaxis()->SetRangeUser(-2., 17.);
   DrawH1(zProj);

   TCanvas *cZPtCut = CreateCanvas("lmvm_vertex_el_gamma_z_ptcut","lmvm_vertex_el_gamma_z_ptcut", 600, 600);
   TH1D* zProjPtCut = (TH1D*)(H2("fh_vertex_el_gamma_xz_ptcut")->ProjectionX("fh_vertex_el_gamma_ptcut_pz")->Clone());
   zProjPtCut->GetYaxis()->SetTitle("Counter per event");
   zProjPtCut->GetXaxis()->SetRangeUser(-2., 17.);
   DrawH1(zProjPtCut);

   TH2D* xzZoom = (TH2D*)xz->Clone();
   TH2D* yzZoom = (TH2D*)yz->Clone();
   TH2D* xyZoom = (TH2D*)xy->Clone();
   TCanvas *cZoom = CreateCanvas("lmvm_vertex_el_gamma_mc_target_zoom","lmvm_vertex_el_gamma_mc_target_zoom", 1200, 400);
   cZoom->Divide(3,1);
   cZoom->cd(1);
   xzZoom->GetXaxis()->SetRangeUser(-1., 11.);
   xzZoom->GetYaxis()->SetRangeUser(-10., 10.);
   DrawH2(xzZoom);
   xzZoom->SetMinimum(1e-3);
   gPad->SetLogz(true);
   cZoom->cd(2);
   yzZoom->GetXaxis()->SetRangeUser(-1., 11.);
   yzZoom->GetYaxis()->SetRangeUser(-10., 10.);
   DrawH2(yzZoom);
   yzZoom->SetMinimum(1e-3);
   gPad->SetLogz(true);
   cZoom->cd(3);
   xyZoom->GetXaxis()->SetRangeUser(-20., 20.);
   xyZoom->GetYaxis()->SetRangeUser(-20., 20.);
   DrawH2(xyZoom);
   xyZoom->SetMinimum(1e-3);
   gPad->SetLogz(true);

   TCanvas *cRZ = CreateCanvas("lmvm_vertex_el_gamma_rz_mc","lmvm_vertex_el_gamma_rz_mc", 600, 600);
   DrawH2(H2("fh_vertex_el_gamma_rz_mc"));
   H2("fh_vertex_el_gamma_rz_mc")->SetMinimum(1e-3);
   gPad->SetLogz(true);
}

void CbmAnaDielectronTaskDraw::Draw1DHistoForEachAnalysisStep(
      const string& hist,
      Bool_t logy)
{
   string drOpt = ""; // First histogram will be drawn without options.
   Double_t min = 9999999999.;
   TH1D* firstHisto = NULL;
   TLegend* leg = new TLegend(0.80, 0.32, 0.99, 0.99);
   for (int step = 0; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)) continue;
      string fullName = hist + "_" + CbmAnaLmvmNames::fAnaSteps[step];
      TH1D* h = H1(fullName);
      if (h == NULL || h->GetEntries() <= 0) continue;
      leg->AddEntry(h, CbmAnaLmvmNames::fAnaStepsLatex[step].c_str(), "l");
      int color = CbmAnaLmvmNames::fAnaStepsColor[step];
      DrawH1( h, kLinear, kLinear, drOpt, color);
      if (firstHisto == NULL) firstHisto = h;
      drOpt = "same"; // If the histogram is drawn, next histograms must be drawn with "same" option.
      if (min > h->GetMinimum()) { min = h->GetMinimum(); }
   }
   if (min <= 0.0) min = 1e-9;
   if (firstHisto != NULL) firstHisto->SetMinimum(min);

   leg->SetFillColor(kWhite);
   leg->Draw();

   gPad->SetGridx(true);
   gPad->SetGridy(true);
   gPad->SetLogy(logy);
}

void CbmAnaDielectronTaskDraw::DrawMinvForEachAnalysisStep()
{
   TCanvas *c1 = CreateCanvas("lmvm_minv_for_each_analysis_step_s_bg", "lmvm_minv_for_each_analysis_step_s_bg", 1200, 600);
   c1->Divide(2,1);
   c1->cd(1);
   Draw1DHistoForEachAnalysisStep("fh_signal_minv", true);
   c1->cd(2);
   Draw1DHistoForEachAnalysisStep("fh_bg_minv", true);

   TCanvas *c2 = CreateCanvas("lmvm_minv_for_each_analysis_step_pi0_eta", "lmvm_minv_for_each_analysis_step_pi0_eta", 1200, 600);
   c2->Divide(2,1);
   c2->cd(1);
   Draw1DHistoForEachAnalysisStep("fh_pi0_minv", true);
   c2->cd(2);
   Draw1DHistoForEachAnalysisStep("fh_eta_minv", true);

   TCanvas *c3 = CreateCanvas("lmvm_minv_for_each_analysis_step_gamma", "lmvm_minv_for_each_analysis_step_gamma", 600, 600);
  // H1("fh_gamma_minv_mc")->GetXaxis()->SetRangeUser(0., 0.05);
   Draw1DHistoForEachAnalysisStep("fh_gamma_minv", true);

}

void CbmAnaDielectronTaskDraw::DrawMinvSandBg(
      int step)
{
   TH1* s1 = H1("fh_signal_minv_" + CbmAnaLmvmNames::fAnaSteps[step]);
   TH1* bg1 = H1("fh_bg_minv_" + CbmAnaLmvmNames::fAnaSteps[step]);

   TH1D* s = (TH1D*)s1->Clone();
   TH1D* bg = (TH1D*)bg1->Clone();
   TH1D* sbg = (TH1D*)bg->Clone();
   sbg->Add(s);
   sbg->SetMinimum(1e-8);

   DrawH1(list_of(sbg)(bg)(s), list_of("")("")(""), kLinear, kLog, false, 0,0,0,0);
   s->SetFillColor(kRed);
   s->SetLineColor(kBlack);
   s->SetLineWidth(1);
   s->SetLineStyle(CbmDrawingOptions::MarkerStyle(1));
   bg->SetFillColor(kYellow - 10);
   bg->SetLineColor(kBlack);
   bg->SetLineWidth(2);
   bg->SetLineStyle(CbmDrawingOptions::MarkerStyle(1));
   sbg->SetFillColor(kBlue);
   sbg->SetLineColor(kBlack);
   sbg->SetLineWidth(1);
   sbg->SetLineStyle(CbmDrawingOptions::MarkerStyle(1));
   s->SetMarkerStyle(1);
   bg->SetMarkerStyle(1);
   sbg->SetMarkerStyle(1);

   DrawTextOnHist(CbmAnaLmvmNames::fAnaStepsLatex[step], 0.65, 0.78, 0.85, 0.9);
}

void CbmAnaDielectronTaskDraw::DrawMinvSandBgAll()
{
   Int_t hi = 1;
   TCanvas *c = CreateCanvas("lmvm_minv_both_s_and_bg", "lmvm_minv_both_s_and_bg", 900, 900);
   c->Divide(3,3);
   for (int step = kReco; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)) continue;
      c->cd(hi++);
      DrawMinvSandBg(step);
   }

   TCanvas *cPtCut = CreateCanvas("lmvm_minv_both_s_and_bg_ptcut", "lmvm_minv_both_s_and_bg_ptcut", 900, 900);
   DrawMinvSandBg(kPtCut);
}

void CbmAnaDielectronTaskDraw::DrawMinvSource(
      int step,
      bool drawAnaStep)
{
   double nofBg = H1("fh_bg_minv_" + CbmAnaLmvmNames::fAnaSteps[step])->GetEntries();

   vector<TH1*> hists;
   for (int i = 0; i < CbmAnaLmvmNames::fNofBgPairSources; i++){
      stringstream ss;
      ss << "fh_source_bg_minv_" <<i << "_" << CbmAnaLmvmNames::fAnaSteps[step];
      hists.push_back( H1(ss.str()) );
   }

   DrawH1(hists, CbmAnaLmvmNames::fBgPairSourceLatex, kLinear, kLinear, false, 0.85, 0.15, 0.99, 0.80);

   TLegend* legend = new TLegend(0.78, 0.15, 0.99, 0.90);
   for (int i = 0; i < CbmAnaLmvmNames::fNofBgPairSources; i++){
      hists[i]->SetMinimum(1e-8);
      legend->AddEntry(hists[i],
            (CbmAnaLmvmNames::fBgPairSourceLatex[i]  + "(" +lit::NumberToString(100. * hists[i]->GetEntries() / nofBg, 1)+ "%)").c_str(), "f");
   }
   legend->SetFillColor(kWhite);
   legend->Draw();
   if (drawAnaStep) DrawTextOnHist(CbmAnaLmvmNames::fAnaStepsLatex[step], 0.65, 0.78, 0.85, 0.9);
}

void CbmAnaDielectronTaskDraw::DrawMinvSourceAll()
{
   {
   Int_t hi = 1;
   TCanvas *c = CreateCanvas("lmvm_minv_source", "lmvm_minv_source", 900, 900);
   c->Divide(3,3);
   for (int step = kReco; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)) continue;
      c->cd(hi++);
      DrawMinvSource(step);
   }

   // Draw histogram for the last step (ptcut) on one histogram
   TCanvas *cPtCut = CreateCanvas("lmvm_minv_source_" + CbmAnaLmvmNames::fAnaSteps[kPtCut],
         "lmvm_minv_source_"+CbmAnaLmvmNames::fAnaSteps[kPtCut], 600, 600);
   DrawMinvSource(kPtCut, false);

   }

   // Draw mismatches and true matches minv
   {
   Int_t hi = 1;
   TCanvas *c = CreateCanvas("lmvm_minv_mismatches", "lmvm_minv_mismatches", 900, 900);
   c->Divide(3,3);
   for (int step = kReco; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)) continue;
      c->cd(hi++);
      DrawH1(list_of( H1("fh_bg_truematch_minv_" + CbmAnaLmvmNames::fAnaSteps[step]) )
            ( H1("fh_bg_truematch_el_minv_" + CbmAnaLmvmNames::fAnaSteps[step]) )
            ( H1("fh_bg_truematch_notel_minv_" + CbmAnaLmvmNames::fAnaSteps[step]) )
            ( H1("fh_bg_mismatch_minv_" + CbmAnaLmvmNames::fAnaSteps[step]) ),
            list_of("true match")("true match (e^{#pm})")("true match (not e^{#pm})")("mismatch"), kLinear, kLinear, true, 0.5, 0.7, 0.99, 0.99);
   }

   // Draw minv after PtCut
   double trueMatch = H1("fh_bg_truematch_minv_" + CbmAnaLmvmNames::fAnaSteps[kPtCut])->GetEntries();
   double trueMatchEl = H1("fh_bg_truematch_el_minv_" + CbmAnaLmvmNames::fAnaSteps[kPtCut])->GetEntries();
   double trueMatchNotEl = H1("fh_bg_truematch_notel_minv_" + CbmAnaLmvmNames::fAnaSteps[kPtCut])->GetEntries();
   double misMatch = H1("fh_bg_mismatch_minv_" + CbmAnaLmvmNames::fAnaSteps[kPtCut])->GetEntries();
   double nofBg = H1("fh_bg_minv_" + CbmAnaLmvmNames::fAnaSteps[kPtCut])->GetEntries();

   TCanvas *cPtCut = CreateCanvas("lmvm_minv_mismatches_" + CbmAnaLmvmNames::fAnaSteps[kPtCut],
            "lmvm_minv_mismatches_"+CbmAnaLmvmNames::fAnaSteps[kPtCut], 700, 700);
   DrawH1(list_of( H1("fh_bg_truematch_minv_" + CbmAnaLmvmNames::fAnaSteps[kPtCut]) )
         ( H1("fh_bg_truematch_el_minv_" + CbmAnaLmvmNames::fAnaSteps[kPtCut]) )
         ( H1("fh_bg_truematch_notel_minv_" + CbmAnaLmvmNames::fAnaSteps[kPtCut]) )
         ( H1("fh_bg_mismatch_minv_" + CbmAnaLmvmNames::fAnaSteps[kPtCut]) ),
         list_of("true match (" + lit::NumberToString(100. * trueMatch / nofBg, 1) + "%)")
         ("true match (e^{#pm}) (" + lit::NumberToString(100. * trueMatchEl / nofBg, 1)+ "%)")
         ("true match (not e^{#pm}) (" + lit::NumberToString(100. * trueMatchNotEl / nofBg, 1)+ "%)")
         ("mismatch (" + lit::NumberToString(100. * misMatch / nofBg)+ "%)"),
         kLinear, kLinear, true, 0.4, 0.7, 0.99, 0.99);
   }
}

void CbmAnaDielectronTaskDraw::DrawElPiMomHis()
{
   double binWEl = H1("fh_pi_mom_mc")->GetBinWidth(1);

   TCanvas *cPi = CreateCanvas("lmvm_pi_mom", "lmvm_pi_mom", 800, 800);
   H1("fh_pi_mom_mc")->Scale(1/binWEl);
   H1("fh_pi_mom_acc")->Scale(1/binWEl);
   H1("fh_pi_mom_rec")->Scale(1/binWEl);
   H1("fh_pi_mom_rec_only_sts")->Scale(1/binWEl);
   H1("fh_pi_mom_rec_sts_rich_trd")->Scale(1/binWEl);
   H1("fh_pi_mom_rec_sts_rich_trd_tof")->Scale(1/binWEl);
   H1("fh_pi_mom_mc")->SetMinimum(2);
   DrawH1( list_of(H1("fh_pi_mom_mc"))(H1("fh_pi_mom_acc"))(H1("fh_pi_mom_rec"))(H1("fh_pi_mom_rec_only_sts"))
         (H1("fh_pi_mom_rec_sts_rich_trd"))(H1("fh_pi_mom_rec_sts_rich_trd_tof")),
         list_of("MC ("+lit::NumberToString(H1("fh_pi_mom_mc")->GetEntries()/fNofEvents, 2) +" per event)")
         ("Acc (" + lit::NumberToString(H1("fh_pi_mom_acc")->GetEntries()/fNofEvents, 2) +" per event)")
         ("Rec (" +  lit::NumberToString(H1("fh_pi_mom_rec")->GetEntries()/fNofEvents, 2) +" per event)")
         ("Rec only STS (" + lit::NumberToString(H1("fh_pi_mom_rec_only_sts")->GetEntries()/fNofEvents, 2) +" per event)")
         ("Rec STS-RICH-TRD (" + lit::NumberToString(H1("fh_pi_mom_rec_sts_rich_trd")->GetEntries()/fNofEvents, 2) +" per event)")
         ("Rec STS-RICH-TRD-TOF (" + lit::NumberToString(H1("fh_pi_mom_rec_sts_rich_trd_tof")->GetEntries()/fNofEvents, 2) +" per event)"),
         kLinear, kLog, 0.1, 0.3, 0.99, 0.99);

   //primary pions vertex < 0.1 cm
   double binWElPrim = H1("fh_piprim_mom_mc")->GetBinWidth(1);
   TCanvas *cPiPrim = CreateCanvas("lmvm_piprim_mom", "lmvm_piprim_mom", 800, 800);
   H1("fh_piprim_mom_mc")->Scale(1/binWElPrim);
   H1("fh_piprim_mom_acc")->Scale(1/binWElPrim);
   H1("fh_piprim_mom_rec")->Scale(1/binWElPrim);
   H1("fh_piprim_mom_rec_only_sts")->Scale(1/binWElPrim);
   H1("fh_piprim_mom_rec_sts_rich_trd")->Scale(1/binWElPrim);
   H1("fh_piprim_mom_rec_sts_rich_trd_tof")->Scale(1/binWElPrim);
   H1("fh_piprim_mom_mc")->SetMinimum(2);
   DrawH1( list_of(H1("fh_piprim_mom_mc"))(H1("fh_piprim_mom_acc"))(H1("fh_piprim_mom_rec"))(H1("fh_piprim_mom_rec_only_sts"))
         (H1("fh_piprim_mom_rec_sts_rich_trd"))(H1("fh_piprim_mom_rec_sts_rich_trd_tof")),
         list_of("MC ("+lit::NumberToString(H1("fh_piprim_mom_mc")->GetEntries()/fNofEvents, 2) +" per event)")
         ("Acc (" + lit::NumberToString(H1("fh_piprim_mom_acc")->GetEntries()/fNofEvents, 2) +" per event)")
         ("Rec (" +  lit::NumberToString(H1("fh_piprim_mom_rec")->GetEntries()/fNofEvents, 2) +" per event)")
         ("Rec only STS (" + lit::NumberToString(H1("fh_piprim_mom_rec_only_sts")->GetEntries()/fNofEvents, 2) +" per event)")
         ("Rec STS-RICH-TRD (" + lit::NumberToString(H1("fh_piprim_mom_rec_sts_rich_trd")->GetEntries()/fNofEvents, 2) +" per event)")
         ("Rec STS-RICH-TRD-TOF (" + lit::NumberToString(H1("fh_piprim_mom_rec_sts_rich_trd_tof")->GetEntries()/fNofEvents, 2) +" per event)"),
         kLinear, kLog, 0.1, 0.3, 0.99, 0.99);

   TCanvas *cPiNotAcc = CreateCanvas("lmvm_pi_mom_notacc", "lmvm_pi_mom_notacc", 800, 800);
   TH1D* h1 = ((TH1D*)H1("fh_pi_mom_mc")->Clone());
   h1->Add(H1("fh_pi_mom_acc"), -1.);
   DrawH1( h1 );
   TCanvas *cPiPrimNotAcc = CreateCanvas("lmvm_piprim_mom_notacc", "lmvm_piprim_mom_notacc", 800, 800);
   TH1D* h2 = ((TH1D*)H1("fh_piprim_mom_mc")->Clone());
   h2->Add(H1("fh_piprim_mom_acc"), -1.);
   DrawH1( h2 );
   DrawH1(h2);

   cout << "Number of primary pions minus at rapidity 2 = " <<   H1("fh_piprim_minus_rapidity_mc")->GetBinContent(H1("fh_piprim_minus_rapidity_mc")->FindFixBin(2)) << endl;
   cout << "Number of primary pions minus at rapidity (1, 3) = " <<
         H1("fh_piprim_minus_rapidity_mc")->Integral(H1("fh_piprim_minus_rapidity_mc")->FindFixBin(1), H1("fh_piprim_minus_rapidity_mc")->FindFixBin(3)) << endl;

   cout << "Number of primary pions plus at rapidity 2 = " <<   H1("fh_piprim_plus_rapidity_mc")->GetBinContent(H1("fh_piprim_plus_rapidity_mc")->FindFixBin(2)) << endl;
   cout << "Number of primary pions plus at rapidity (1, 3) = " <<
         H1("fh_piprim_plus_rapidity_mc")->Integral(H1("fh_piprim_plus_rapidity_mc")->FindFixBin(1), H1("fh_piprim_plus_rapidity_mc")->FindFixBin(3)) << endl;


   double binWRapidity = H1("fh_piprim_minus_rapidity_mc")->GetBinWidth(1);
   H1("fh_pi_rapidity_mc")->Scale(1/binWRapidity);
   H1("fh_piprim_minus_rapidity_mc")->Scale(1/binWRapidity);
   H1("fh_piprim_plus_rapidity_mc")->Scale(1/binWRapidity);
   H1("fh_pi0prim_rapidity_mc")->Scale(1/binWRapidity);
   H1("fh_etaprim_rapidity_mc")->Scale(1/binWRapidity);

   TCanvas *cPiPrimPlusY = CreateCanvas("lmvm_piprim_plus_rapidity", "lmvm_piprim_plus_rapidity", 800, 800);
   DrawH1(H1("fh_piprim_plus_rapidity_mc"));
   TCanvas *cPiPrimMinusY = CreateCanvas("lmvm_piprim_minus_rapidity", "lmvm_piprim_minus_rapidity", 800, 800);
   DrawH1(H1("fh_piprim_minus_rapidity_mc"));
   TCanvas *cPi0PrimY = CreateCanvas("lmvm_pi0prim_rapidity", "lmvm_pi0prim_rapidity", 800, 800);
   DrawH1(H1("fh_pi0prim_rapidity_mc"));
   TCanvas *cEtaPrimY = CreateCanvas("lmvm_etaprim_rapidity", "lmvm_etaprim_rapidity", 800, 800);
   DrawH1(H1("fh_etaprim_rapidity_mc"));

}

void CbmAnaDielectronTaskDraw::RemoveMvdCutBins()
{
   for (int step = kMvd2Cut + 1 + 1; step < CbmAnaLmvmNames::fNofAnaSteps + 1; step++){
      H1("fh_nof_bg_tracks")->SetBinContent(step - 2, H1("fh_nof_bg_tracks")->GetBinContent(step));
      H1("fh_nof_el_tracks")->SetBinContent(step - 2, H1("fh_nof_el_tracks")->GetBinContent(step));

      H1("fh_nof_mismatches")->SetBinContent(step - 2, H1("fh_nof_mismatches")->GetBinContent(step));
      H1("fh_nof_ghosts")->SetBinContent(step - 2, H1("fh_nof_ghosts")->GetBinContent(step));

      int ny = H2("fh_source_tracks")->GetYaxis()->GetNbins();
      for (int y = 1; y <= ny; y++){
         H2("fh_source_tracks")->SetBinContent(step - 2, y, H2("fh_source_tracks")->GetBinContent(step, y));
      }

      ny = H2("fh_source_pairs")->GetYaxis()->GetNbins();
      for (int y = 1; y <= ny; y++){
         H2("fh_source_pairs")->SetBinContent(step - 2, y, H2("fh_source_pairs")->GetBinContent(step, y));
      }
   }
}

void CbmAnaDielectronTaskDraw::DrawBgSource2D(
      const string& canvasName,
      const string& histName,
      const vector<string>& yLabels,
      double scale,
      const string& zTitle)
{
   int rangeMax = CbmAnaLmvmNames::fNofAnaSteps;
   if (!fUseMvd){
      rangeMax = CbmAnaLmvmNames::fNofAnaSteps - 2;
   }

   TCanvas *c1 = CreateCanvas(string(canvasName+"_abs").c_str(), string(canvasName+"_abs").c_str(), 900, 600);
   TH2D* habs = (TH2D*)H2(histName)->Clone();
   habs->SetStats(false);
   habs->Scale(scale);
   habs->GetZaxis()->SetTitle(zTitle.c_str());
   habs->GetXaxis()->SetRange(kReco + 1, rangeMax);
   habs->SetMarkerSize(1.4);
   DrawH2(habs, kLinear, kLinear, kLog, "text COLZ");

   TCanvas *c2 = CreateCanvas(string(canvasName+"_percent").c_str(), string(canvasName+"_percent").c_str(), 900, 600);
   TH2D* hperc = (TH2D*)H2(histName)->Clone();
   hperc->SetStats(false);
   Int_t nBinsX = hperc->GetNbinsX();
   Int_t nBinsY = hperc->GetNbinsY();
   for (Int_t x = 1; x <= nBinsX; x++){
     // calculate total number of BG tracks (pairs) for a current step
     double nbg = 0.;
     for (Int_t y = 1; y <= nBinsY; y++){
        nbg += habs->GetBinContent(x, y);
     }
     Double_t sc =100. / (nbg / scale);
     for (Int_t y = 1; y <= nBinsY; y++){
        Double_t val = sc * hperc->GetBinContent(x, y);
        hperc->SetBinContent(x, y, val);
     }
   }
   hperc->GetZaxis()->SetTitle("[%]");
   hperc->GetXaxis()->SetLabelSize(0.06);
   hperc->GetYaxis()->SetLabelSize(0.06);
   hperc->SetMarkerColor(kBlack);
   hperc->SetMarkerSize(1.8);
   hperc->GetXaxis()->SetRange(kReco + 1, rangeMax);
   DrawH2(hperc, kLinear, kLinear, kLinear, "text COLZ");

   for (Int_t y = 1; y <= yLabels.size(); y++){
     hperc->GetYaxis()->SetBinLabel(y, yLabels[y-1].c_str());
     habs->GetYaxis()->SetBinLabel(y, yLabels[y-1].c_str());
   }

   SetAnalysisStepLabels(hperc);
   SetAnalysisStepLabels(habs);
}

void CbmAnaDielectronTaskDraw::DrawBgSourceTracks()
{
   gStyle->SetPaintTextFormat("4.1f");

   int rangeMax = CbmAnaLmvmNames::fNofAnaSteps;
   if (!fUseMvd){
      rangeMax = CbmAnaLmvmNames::fNofAnaSteps - 2;
   }

   TCanvas *c1 = CreateCanvas("lmvm_nof_bg_tracks", "lmvm_nof_bg_tracks", 600, 600);
   TH1D* hbg = (TH1D*)H1("fh_nof_bg_tracks")->Clone();
   hbg->Scale(10);
   hbg->GetYaxis()->SetTitle("Tracks/event x10");
   hbg->GetXaxis()->SetRange(kReco + 1, rangeMax);
   DrawH1( hbg, kLinear, kLog, "hist text0");
   hbg->SetMarkerSize(2.);

   TCanvas *c2 = CreateCanvas("lmvm_nof_el_tracks", "lmvm_nof_el_tracks", 600, 600);
   TH1D* hel = H1("fh_nof_el_tracks");
   hel->GetXaxis()->SetRange(kReco + 1, rangeMax);
   DrawH1( hel, kLinear, kLog);

   TCanvas *c3 = CreateCanvas("lmvm_purity", "lmvm_purity", 600, 600);
   TH1D* purity = new TH1D("purity","purity;Analysis steps;Purity",
         CbmAnaLmvmNames::fNofAnaSteps, 0., CbmAnaLmvmNames::fNofAnaSteps);
   purity->Divide(H1("fh_nof_bg_tracks"), H1("fh_nof_el_tracks"));
   purity->GetXaxis()->SetRange(kReco + 1, rangeMax);
   DrawH1( purity, kLinear, kLog, "hist text30");
   purity->SetMarkerSize(1.9);

   SetAnalysisStepLabels(hbg);
   SetAnalysisStepLabels(hel);
   SetAnalysisStepLabels(purity);

   DrawBgSource2D("lmvm_source_tracks_2d", "fh_source_tracks",
         list_of("#gamma")("#pi^{0}")("#pi^{#pm}")("p")("K")("e^{#pm}_{sec}")("oth."),
         100., "Tracks per event x10^{2}");


   TCanvas *c4 = CreateCanvas("fh_nof_topology_pairs_gamma", "fh_nof_topology_pairs_gamma", 600, 600);
   TH1D* htopologyGamma = (TH1D*)H1("fh_nof_topology_pairs_gamma")->Clone();
   htopologyGamma->Scale(1. / htopologyGamma->Integral());
   DrawH1( htopologyGamma, kLinear, kLinear);
   htopologyGamma->SetMarkerSize(1.);

   TCanvas *c5 = CreateCanvas("fh_nof_topology_pairs_pi0", "fh_nof_topology_pairs_pi0", 600, 600);
   TH1D* htopologyPi0 = (TH1D*)H1("fh_nof_topology_pairs_pi0")->Clone();
   htopologyPi0->Scale(1. / htopologyPi0->Integral());
   DrawH1( htopologyPi0, kLinear, kLinear);
   htopologyPi0->SetMarkerSize(1.);
}

void CbmAnaDielectronTaskDraw::DrawMismatchesAndGhosts()
{
   gStyle->SetPaintTextFormat("4.1f");

   int rangeMax = CbmAnaLmvmNames::fNofAnaSteps;
   if (!fUseMvd){
      rangeMax = CbmAnaLmvmNames::fNofAnaSteps - 2;
   }

   TCanvas *c1 = CreateCanvas("lmvm_nof_mismatches", "lmvm_nof_mismatches", 600, 600);
   c1->Divide(2,2);
   c1->cd(1);
   TH1D* hmismatches = (TH1D*)H1("fh_nof_mismatches")->Clone();
   hmismatches->Scale(10);
   hmismatches->GetYaxis()->SetTitle("Mismatch tracks/event x10");
   hmismatches->GetXaxis()->SetRange(kReco + 1, rangeMax);
   DrawH1( hmismatches, kLinear, kLog, "hist text0");

   hmismatches->SetMarkerSize(2.);
   SetAnalysisStepLabels(hmismatches);
   c1->cd(2);
   TH1D* hmismatches_rich = (TH1D*)H1("fh_nof_mismatches_rich")->Clone();
   hmismatches_rich->Scale(10);
   hmismatches_rich->GetYaxis()->SetTitle("Mismatch tracks (RICH)/event x10");
   hmismatches_rich->GetXaxis()->SetRange(kReco + 1, rangeMax);
   DrawH1( hmismatches_rich, kLinear, kLog, "hist text0");
   hmismatches_rich->SetMarkerSize(2.);
   SetAnalysisStepLabels(hmismatches_rich);
   c1->cd(3);
   TH1D* hmismatches_trd = (TH1D*)H1("fh_nof_mismatches_trd")->Clone();
   hmismatches_trd->Scale(10);
   hmismatches_trd->GetYaxis()->SetTitle("Mismatch tracks (TRD)/event x10");
   hmismatches_trd->GetXaxis()->SetRange(kReco + 1, rangeMax);
   DrawH1( hmismatches_trd, kLinear, kLog, "hist text0");
   hmismatches_trd->SetMarkerSize(2.);
   SetAnalysisStepLabels(hmismatches_trd);
   c1->cd(4);
   TH1D* hmismatches_tof = (TH1D*)H1("fh_nof_mismatches")->Clone();
   hmismatches_tof->Scale(10);
   hmismatches_tof->GetYaxis()->SetTitle("Mismatch tracks (TOF)/event x10");
   hmismatches_tof->GetXaxis()->SetRange(kReco + 1, rangeMax);
   DrawH1( hmismatches_tof, kLinear, kLog, "hist text0");
   hmismatches_tof->SetMarkerSize(2.);
   SetAnalysisStepLabels(hmismatches_tof);

   TCanvas *c2 = CreateCanvas("lmvm_nof_ghosts", "lmvm_nof_ghosts", 600, 600);
   TH1D* hghosts = H1("fh_nof_ghosts");
   hghosts->GetXaxis()->SetRange(kReco + 1, rangeMax);
   DrawH1( hghosts, kLinear, kLog);
   SetAnalysisStepLabels(hghosts);
}

void CbmAnaDielectronTaskDraw::SetAnalysisStepLabels(
      TH1* h)
{
   h->GetXaxis()->SetLabelSize(0.06);
   int x = 1;
   for (Int_t step = 0; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)){
         continue;
      }
      h->GetXaxis()->SetBinLabel(x, CbmAnaLmvmNames::fAnaStepsLatex[step].c_str());
      x++;
   }
}

void CbmAnaDielectronTaskDraw::DrawMinvPtAll()
{
   Int_t hi = 1;
   TCanvas *c = CreateCanvas("lmvm_fh_signal_minv_pt", "lmvm_fh_signal_minv_pt", 750, 1000);
   c->Divide(3, 4);
   for (int step = 0; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)) continue;
      c->cd(hi++);

      TH2D* h = H2("fh_signal_minv_pt_" + CbmAnaLmvmNames::fAnaSteps[step]);
      DrawH2(h, kLinear, kLinear, kLinear, "COLZ");
      DrawTextOnHist(CbmAnaLmvmNames::fAnaStepsLatex[step], 0.50, 0.78, 0.70, 0.9);
   }
}

void CbmAnaDielectronTaskDraw::DrawBgSourcesVsMomentum()
{
   TCanvas *c1mc = CreateCanvas("lmvm_source_mom_mc_signal","lmvm_source_mom_mc_signal", 600, 600);
   DrawH1( H1("fh_source_mom_mc_signal") );

   int hi = 1;
   TCanvas *c1 = CreateCanvas("lmvm_source_mom","lmvm_source_mom", 900, 900);
   c1->Divide(3,3);
   for (Int_t step = kReco; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)) continue;
      c1->cd (hi++);
      Draw1DSourceTypes("fh_source_mom_" + CbmAnaLmvmNames::fAnaSteps[step], false);
      DrawTextOnHist(CbmAnaLmvmNames::fAnaStepsLatex[step], 0.50, 0.90, 0.70, 0.99);
   }
   TCanvas *c1_1 = CreateCanvas("lmvm_source_mom_ttcut","lmvm_source_mom_ttcut", 600, 600);
   Draw1DSourceTypes("fh_source_mom_" + CbmAnaLmvmNames::fAnaSteps[kTtCut], false);

   hi = 1;
   TCanvas *c3 = CreateCanvas("lmvm_source_pt","lmvm_source_pt", 900, 900);
   c3->Divide(3,3);
   for (Int_t step = kReco; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)) continue;
      c3->cd (hi++);
      Draw1DSourceTypes("fh_source_pt_" + CbmAnaLmvmNames::fAnaSteps[step], false);
      DrawTextOnHist(CbmAnaLmvmNames::fAnaStepsLatex[step], 0.50, 0.90, 0.70, 0.99);
   }
   TCanvas *c1_2 = CreateCanvas("lmvm_source_pt_ttcut","lmvm_source_pt_ttcut", 600, 600);
   Draw1DSourceTypes("fh_source_pt_" + CbmAnaLmvmNames::fAnaSteps[kTtCut], false);

   hi = 1;
   TCanvas *c2 = CreateCanvas("lmvm_opening_angle","lmvm_opening_angle", 900, 900);
   c2->Divide(3,3);
   for (Int_t step = kReco; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
      //cout << "fh_opening_angle_" << step << endl;
      if ( !fUseMvd && (step == kMvd1Cut || step == kMvd2Cut)) continue;
      c2->cd (hi++);
      Draw1DSourceTypes("fh_opening_angle_" + CbmAnaLmvmNames::fAnaSteps[step]);
      DrawTextOnHist(CbmAnaLmvmNames::fAnaStepsLatex[step], 0.50, 0.90, 0.70, 0.99);
   }
   TCanvas *c1_3 = CreateCanvas("lmvm_opening_angle_ttcut","lmvm_opening_angle_ttcut", 600, 600);
   Draw1DSourceTypes("fh_opening_angle_" + CbmAnaLmvmNames::fAnaSteps[kTtCut], false);
}

void CbmAnaDielectronTaskDraw::DrawMvdCutQa()
{
   if (fUseMvd) {
      TCanvas *c1 = CreateCanvas("lmvm_mvd1cut_qa","lmvm_mvd1cut_qa", 600, 600);
      Draw1DSourceTypes("fh_mvd1cut_qa");
      TH1D* h1 = H1("fh_mvd1cut_qa_"+CbmAnaLmvmNames::fSourceTypes[0]);
      h1->GetXaxis()->SetLabelSize(0.06);
      h1->GetXaxis()->SetBinLabel(1, "Correct");
      h1->GetXaxis()->SetBinLabel(2, "Wrong");
      gPad->SetLogy(false);

      TCanvas *c2 = CreateCanvas("lmvm_mvd2cut_qa","lmvm_mvd2cut_qa", 600, 600);
      Draw1DSourceTypes("fh_mvd2cut_qa");
      TH1D* h2 = H1("fh_mvd2cut_qa_"+CbmAnaLmvmNames::fSourceTypes[0]);
      h2->GetXaxis()->SetLabelSize(0.07);
      h2->GetXaxis()->SetBinLabel(1, "Correct");
      h2->GetXaxis()->SetBinLabel(2, "Wrong");
      gPad->SetLogy(false);
   }
}

void CbmAnaDielectronTaskDraw::DrawMvdAndStsHist()
{
   if (!fUseMvd) return;
   TCanvas *c1 = CreateCanvas("lmvm_nofhits_mvd_sts","lmvm_nofhits_mvd_sts", 900, 450);
   c1->Divide(2,1);
   c1->cd(1);
   Draw1DSourceTypes("fh_nofMvdHits");
   c1->cd(2);
   Draw1DSourceTypes("fh_nofStsHits");

   Draw2DCut( "fh_mvd1xy");
   TCanvas *c2 = CreateCanvas("lmvm_mvd1r","lmvm_mvd1r", 600, 600);
   Draw1DSourceTypes("fh_mvd1r");


   Draw2DCut( "fh_mvd2xy");
   TCanvas *c3 = CreateCanvas("lmvm_mvd2r","lmvm_mvd2r", 600, 600);
   Draw1DSourceTypes("fh_mvd2r");
}


void CbmAnaDielectronTaskDraw::SaveCanvasToImage()
{
   for (int i = 0; i < fCanvas.size(); i++){
      lit::SaveCanvasAsImage(fCanvas[i], fOutputDir);
   }
}
