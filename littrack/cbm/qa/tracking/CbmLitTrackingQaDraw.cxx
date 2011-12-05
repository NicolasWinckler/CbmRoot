/**
 * \file CbmLitTrackingQaDraw.cxx
 * \brief Draw histograms for tracking QA.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "qa/tracking/CbmLitTrackingQaDraw.h"
#include "qa/base/CbmLitHistManager.h"

#include "TCanvas.h"
#include "TLine.h"
#include "TDirectory.h"

#include "utils/CbmLitDrawHist.h"
#include "utils/CbmLitUtils.h"
#include "TH1D.h"
#include "TH3F.h"

#include <iostream>
#include <string>

void CbmLitTrackingQaDraw::Draw()
{
   SetStyles();
   DrawEfficiencyHistos();
   DrawHitsHistos();
   DrawHitsStationHistos();
   DrawStsTracksQaHistos();
   DrawMcEfficiencyGraph();
   DrawMCMomVsAngle();
}

TH1F* CbmLitTrackingQaDraw::H1(
      const string& name)
{
   return fHM->H1(name);
}

TH2F* CbmLitTrackingQaDraw::H2(
      const string& name)
{
   return fHM->H2(name);
}

TH3F* CbmLitTrackingQaDraw::H3(
      const string& name)
{
   return fHM->H3(name);
}

TH1* CbmLitTrackingQaDraw::H(
      const string& name)
{
   return fHM->H(name);
}

void CbmLitTrackingQaDraw::DrawEfficiencyHistos()
{
   //return;
   string sname("STS");
   string rname;
   if (fIsMuch && !fIsTrd) { rname = "MUCH"; }
   else if (fIsTrd && !fIsMuch) { rname = "TRD"; }
   else if (fIsTrd && fIsMuch) { rname = "MUCH+TRD"; }
   string hgname(sname + "+" + rname);
   string gname = hgname + "+TOF";

   string signal = fIsMuch ? "muons" : "electrons";
   string cat = fIsMuch ? "Mu" : "El";

   // Draw global tracking efficiency STS+TRD(MUCH)+TOF for all tracks
   DrawEfficiency("rec_qa_global_efficiency_all", "hSts3DNormGlobalpx_All",
         "hHalfGlobal3DNormGlobalpx_All", "hGlobal3Dpx_All", "", sname, hgname, gname, "", "");

   // Draw global tracking efficiency STS+TRD(MUCH)+TOF for signal tracks
   DrawEfficiency("rec_qa_global_efficiency_signal", "hSts3DNormGlobalpx_"+cat,
         "hHalfGlobal3DNormGlobalpx_"+cat, "hGlobal3Dpx_"+cat, "", sname, hgname, gname, "", "");

   // Draw half global tracking efficiency STS+TRD(MUCH) for all tracks
   DrawEfficiency("rec_qa_half_global_efficiency_all", "hSts3DNormHalfGlobalpx_All",
         "hHalfGlobal3Dpx_"+cat, "", "", sname, hgname, "", "", "");

   // Draw half global tracking efficiency STS+TRD(MUCH) for signal tracks
   DrawEfficiency("rec_qa_half_global_efficiency_signal", "hSts3DNormHalfGlobalpx_"+cat,
         "hHalfGlobal3Dpx_"+cat, "", "", sname, hgname, "", "", "");

   // Draw efficiency for STS
   DrawEfficiency("rec_qa_sts_efficiency", "hSts3Dpx_All",
         "hSts3Dpx_"+cat, "", "", "STS: all", "STS: " + signal, "", "", "");

   if (fIsTrd || fIsMuch) {
      // Draw efficiency for TRD(MUCH)
      DrawEfficiency("rec_qa_rec_efficiency", "hRec3Dpx_All",
         "hRec3Dpx_"+cat, "", "", rname + ": all", rname + ": " + signal, "", "", "");
   }

   if (fIsTof) {
      // Draw efficiency for TOF
      DrawEfficiency("rec_qa_tof_efficiency", "hTof3Dpx_All",
         "hTof3Dpx_"+cat, "", "", "TOF: all", "TOF: " + signal, "", "", "");
   }

   if (fIsRich) {
      // Draw efficiency for RICH for electron set
      DrawEfficiency("rec_qa_rich_efficiency_electrons_mom", "hRich3Dpx_El",
         "hRich3Dpx_ElRef", "", "", "RICH: electrons", "RICH: electrons ref", "", "", "");

      // Draw efficiency for RICH for electron set vs. rapidity
      DrawEfficiency("rec_qa_rich_efficiency_electrons_rapidity", "hRich3Dpy_El",
         "hRich3Dpy_ElRef", "", "", "RICH: electrons", "RICH: electrons ref", "", "", "");

      // Draw efficiency for RICH for electron set vs. pt
      DrawEfficiency("rec_qa_rich_efficiency_electrons_pt", "hRich3Dpz_El",
         "hRich3Dpz_ElRef", "", "", "RICH: electrons", "RICH: electrons ref", "", "", "");

      // Draw efficiency vs. nof hits in ring for RICH for electron set
      DrawEfficiency("rec_qa_rich_efficiency_vs_nofhits_electrons", "hRichNh_El",
         "", "", "", "RICH: electrons", "", "", "", "");

      // Draw efficiency for STS+RICH for electron set
      DrawEfficiency("rec_qa_sts_rich_efficiency_electrons", "hSts3DNormStsRichpx_El",
         "hStsRich3Dpx_El", "", "", "STS", "STS+RICH", "", "", "");

      // Draw efficiency for STS+RICH No matching for electron set
      DrawEfficiency("rec_qa_sts_rich_no_matching_efficiency_electrons", "hSts3DNormStsRichpx_El",
         "hStsRichNoMatching3Dpx_El", "", "", "STS", "STS+RICH (No Matching)", "", "", "");

      // Draw efficiency for STS+RICH+TRD for electron set
      DrawEfficiency("rec_qa_sts_rich_trd_efficiency_electrons", "hSts3DNormStsRichTrdpx_El",
         "hStsRich3DNormStsRichTrdpx_El", "hStsRichTrd3Dpx_El", "",
         "STS", "STS+RICH", "STS+RICH+TRD", "", "");

      // Draw efficiency for STS+RICH+TRD+TOF for electron set
      DrawEfficiency("rec_qa_sts_rich_trd_tof_efficiency_electrons", "hSts3DNormStsRichTrdTofpx_El",
         "hStsRich3DNormStsRichTrdTofpx_El", "hStsRichTrd3DNormStsRichTrdTofpx_El", "hStsRichTrdTof3Dpx_El",
         "STS", "STS+RICH", "STS+RICH+TRD", "STS+RICH+TRD+TOF", "");

//      TCanvas* canvas = new TCanvas("rec_qa_sts_rich_trd_tof_momentum_electrons", "rec_qa_sts_rich_trd_tof_momentum_electrons", 600, 500);
//      canvas->SetGrid();
//      DrawHist1D(H1("hSts3DNormStsRichTrdTof_El_Rec"), H1("hStsRich3DNormStsRichTrdTof_El_Rec"),
//         H1("hStsRichTrd3DNormStsRichTrdTof_El_Rec"), H1("hStsRichTrdTof3D_El_Rec"),
//         "Efficiency", "Momentum [GeV/c]", "Efficiency", "STS", "STS+RICH",
//         "STS+RICH+TRD", "STS+RICH+TRD+TOF", kLitLinearScale, kLitLinearScale, true, 0.3,0.3,0.85,0.6);

      DrawEfficiency("rec_qa_sts_rich_trd_tof_electron_identification", "hStsRich3DElIdNormStsRichTrdTofpx_ElId",
         "hStsRichTrd3DElIdNormStsRichTrdTofpx_ElId", "hStsRichTrdTof3DElIdpx_ElId", "",
         "RICH", "RICH+TRD", "RICH+TRD+TOF", "", "");

      DrawEfficiency("rec_qa_sts_rich_trd_tof_pion_suppression", "hStsRich3DElIdNormStsRichTrdTofpx_PiSupp",
         "hStsRichTrd3DElIdNormStsRichTrdTofpx_PiSupp", "hStsRichTrdTof3DElIdpx_PiSupp", "",
         "RICH", "RICH+TRD", "RICH+TRD+TOF", "", "pisupp");
    }

   if (fIsRich) {
      // Draw detector acceptance
      //DrawEfficiency("rec_qa_sts_rich_trd_tof_detector_acceptance", "hStsDetAcc3Dpx_El",
      //      "hStsRichDetAcc3Dpx_El", "hStsRichTrdDetAcc3Dpx_El", "hStsRichTrdTofDetAcc3Dpx_El",
      //     "STS", "STS+RICH", "STS+RICH+TRD", "STS+RICH+TRD+TOF", "");

   }
}

void CbmLitTrackingQaDraw::DrawEfficiency(
      const string& canvasName,
      const string& hist1,
      const string& hist2,
      const string& hist3,
      const string& hist4,
      const string& name1,
      const string& name2,
      const string& name3,
      const string& name4,
      const string& opt)
{

   TCanvas* canvas = new TCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
   canvas->SetGrid();
   canvas->cd();

   TH1* h1 = NULL, *h2 = NULL, *h3 = NULL, *h4 = NULL;

   if (hist1 != "") h1 = H(hist1+"_Eff");
   if (hist2 != "") h2 = H(hist2+"_Eff");
   if (hist3 != "") h3 = H(hist3+"_Eff");
   if (hist4 != "") h4 = H(hist4+"_Eff");

   float eff1, eff2, eff3, eff4;

   if (hist1 != "") eff1 = CalcEfficiency(H(hist1+"_Rec"), H(hist1+"_Acc"), opt);
   if (hist2 != "") eff2 = CalcEfficiency(H(hist2+"_Rec"), H(hist2+"_Acc"), opt);
   if (hist3 != "") eff3 = CalcEfficiency(H(hist3+"_Rec"), H(hist3+"_Acc"), opt);
   if (hist4 != "") eff4 = CalcEfficiency(H(hist4+"_Rec"), H(hist4+"_Acc"), opt);

   string hname1, hname2, hname3, hname4;
   if (hist1 != "") hname1 = name1 + "(" + lit::NumberToString<float>(eff1, 1) + ")";
   if (hist2 != "") hname2 = name2 + "(" + lit::NumberToString<float>(eff2, 1) + ")";
   if (hist3 != "") hname3 = name3 + "(" + lit::NumberToString<float>(eff3, 1) + ")";
   if (hist4 != "") hname4 = name4 + "(" + lit::NumberToString<float>(eff4, 1) + ")";

   if (fRebin > 1) {
      if (hist1 != "") h1->Rebin(fRebin);
      if (hist2 != "") h2->Rebin(fRebin);
      if (hist3 != "") h3->Rebin(fRebin);
      if (hist4 != "") h4->Rebin(fRebin);

      if (hist1 != "") h1->Scale(1./fRebin);
      if (hist2 != "") h2->Scale(1./fRebin);
      if (hist3 != "") h3->Scale(1./fRebin);
      if (hist4 != "") h4->Scale(1./fRebin);
   }

   if (opt != "pisupp"){
      h1->SetMinimum(0.);
      h1->SetMaximum(100.);
   }

   if (h1 != NULL && h2 != NULL && h3 != NULL && h4 != NULL) {
      DrawHist1D(h1, h2, h3, h4, hname1, hname2, hname3, hname4,
            kLitLinear, kLitLinear, true, 0.3,0.3,0.85,0.6);
      DrawMeanEfficiencyLines(h1, eff1, eff2, eff3, eff4);

   } else if (h1 != NULL && h2 != NULL && h3 != NULL && h4 == NULL) {
      DrawHist1D(h1, h2, h3, NULL, hname1, hname2, hname3, "",
            kLitLinear, kLitLinear, true, 0.3,0.3,0.85,0.6);
      DrawMeanEfficiencyLines(h1, eff1, eff2, eff3);

   } else if (h1 != NULL && h2 != NULL && h3 == NULL && h4 == NULL){
      DrawHist1D(h1, h2, NULL, NULL, hname1, hname2, "", "",
            kLitLinear, kLitLinear, true, 0.3,0.3,0.85,0.6);
      DrawMeanEfficiencyLines(h1, eff1, eff2);

   } else if (h1 != NULL && h2 == NULL && h3 == NULL && h4 == NULL){
      DrawHist1D(h1, NULL, NULL, NULL, hname1, "", "", "",
            kLitLinear, kLitLinear, true, 0.3,0.3,0.85,0.6);
      DrawMeanEfficiencyLines(h1, eff1);
   }

   if (opt == "pisupp") gPad->SetLogy(true);
   if (fOutputDir != "") lit::SaveCanvasAsImage(canvas, fOutputDir);
}

float CbmLitTrackingQaDraw::CalcEfficiency(
   TH1* histRec,
   TH1* histAcc,
   const string& opt)
{
   if (histAcc->GetEntries() == 0 || histRec->GetEntries() == 0) {
      return 0.;
   } else {
      if (opt != "pisupp") return 100.*float(histRec->GetEntries()) / float(histAcc->GetEntries());
      else return float(histAcc->GetEntries()) / float(histRec->GetEntries());
   }
}

void CbmLitTrackingQaDraw::DrawMeanEfficiencyLines(
   TH1* h,
   float eff1,
   float eff2,
   float eff3,
   float eff4)
{
   float minY = h->GetXaxis()->GetXmin();
   float maxY = h->GetXaxis()->GetXmax();

   TLine* line1 = new TLine(minY, eff1, maxY, eff1);
   line1->SetLineWidth(1);
   line1->SetLineColor(LIT_COLOR1);
   line1->Draw();

   if (eff2 != -1){
      TLine* line2 = new TLine(minY, eff2, maxY, eff2);
      line2->SetLineWidth(1);
      line2->SetLineColor(LIT_COLOR2);
      line2->Draw();

      if(eff3 != -1){
         TLine* line3 = new TLine(minY, eff3, maxY, eff3);
         line3->SetLineWidth(1);
         line3->SetLineColor(LIT_COLOR3);
         line3->Draw();

         if (eff4 != -1){
            TLine* line4 = new TLine(minY, eff4, maxY, eff4);
            line4->SetLineWidth(1);
            line4->SetLineColor(LIT_COLOR4);
            line4->Draw();
         }
      }
   }
}

void CbmLitTrackingQaDraw::DrawMcEfficiencyGraph()
{
 /*  boost::property_tree::ptree pt = PrintPTree();

   TH1F* hAcc = new TH1F("hMcAccEfficiencyGraph", "hMcAccEfficiencyGraph;Detector;Efficiency [%]", 4, 0, 4);
   hAcc->SetMaximum(100.);
   hAcc->SetMinimum(0.);
   hAcc->SetBinContent(1, pt.get("hStsDetAccEl.detAccAcc.eff", 0.0));
   hAcc->GetXaxis()->SetBinLabel(1, "STS");
   hAcc->SetBinContent(2, pt.get("hStsRichDetAccEl.detAccAcc.eff", 0.0));
   hAcc->GetXaxis()->SetBinLabel(2, "RICH");
   hAcc->SetBinContent(3, pt.get("hStsRichTrdDetAccEl.detAccAcc.eff", 0.0));
   hAcc->GetXaxis()->SetBinLabel(3, "TRD");
   hAcc->SetBinContent(4, pt.get("hStsRichTrdTofDetAccEl.detAccAcc.eff", 0.0));
   hAcc->GetXaxis()->SetBinLabel(4, "TOF");

   TH1F* hRec = new TH1F("hMcRecEfficiencyGraph", "hMcRecEfficiencyGraph", 4, 0, 4);
   hRec->SetBinContent(1, pt.get("hStsDetAccEl.detAccRec.eff", 0.0));
   //hRec->SetBinLabel(1, "STS");
   hRec->SetBinContent(2, pt.get("hStsRichDetAccEl.detAccRec.eff", 0.0));
   //hRec->SetBinLabel(1, "RICH");
   hRec->SetBinContent(3, pt.get("hStsRichTrdDetAccEl.detAccRec.eff", 0.0));
   //hRec->SetBinLabel(1, "TRD");
   hRec->SetBinContent(4, pt.get("hStsRichTrdTofDetAccEl.detAccRec.eff", 0.0));
   //hRec->SetBinLabel(1, "TOF");

   TCanvas* canvas = new TCanvas("mc_acc_rec_graph", "mc_acc_rec_graph", 600, 500);
   DrawHist1D(hAcc, hRec, NULL, NULL, "", "Detector", "Efficiency [%]", "ACC/MC", "REC/MC", "", "",
              kLitLinearScale, kLitLinearScale, true, 0.6,0.1,0.9,0.4);
   gPad->SetGridx(true);
   gPad->SetGridy(true);*/
}


void CbmLitTrackingQaDraw::DrawHitsHistos()
{
   if (fIsMvd) { DrawHitsHistos("rec_qa_mvd_hits", "hMvdTrackHits"); }
   if (fIsSts) { DrawHitsHistos("rec_qa_sts_hits", "hStsTrackHits"); }
   if (fIsTrd) { DrawHitsHistos("rec_qa_trd_hits", "hTrdTrackHits"); }
   if (fIsMuch) { DrawHitsHistos("rec_qa_much_hits", "hMuchTrackHits"); }
   if (fIsRich) { DrawHitsHistos("rec_qa_rich_ring_hits", "hRichRingHits"); }
}

void CbmLitTrackingQaDraw::DrawHitsHistos(
   const string& canvasName,
   const string& hist)
{
   TCanvas* c = new TCanvas(canvasName.c_str(), canvasName.c_str(), 1200, 600);
   c->Divide(2,1);
   c->SetGrid();

   c->cd(1);
   DrawHist1D(H1(hist+"_All"), H1(hist+"_True"), H1(hist+"_Fake"), NULL,
              "all: " + lit::NumberToString<float>(H1(hist+"_All")->GetMean(), 1),
              "true: " + lit::NumberToString<float>(H1(hist+"_True")->GetMean(), 1),
              "fake: " + lit::NumberToString<float>(H1(hist+"_Fake")->GetMean(), 1),  "",
              kLitLinear, kLitLog, true, 0.25,0.99,0.55,0.75);

   c->cd(2);
   DrawHist1D(H1(hist+"_TrueOverAll"), H1(hist+"_FakeOverAll"), NULL, NULL,
              "true/all: " + lit::NumberToString<float>(H1(hist+"_TrueOverAll")->GetMean()),
              "fake/all: " + lit::NumberToString<float>(H1(hist+"_FakeOverAll")->GetMean()),
              "", "", kLitLinear, kLitLog, true, 0.25,0.99,0.55,0.75);

   if (fOutputDir != "") lit::SaveCanvasAsImage(c, fOutputDir);
}

void CbmLitTrackingQaDraw::DrawHitsStationHisto(
      const std::string& name,
      TH1F* hist)
{
   TCanvas* canvas = new TCanvas(name.c_str(), name.c_str(), 1200, 600);
   DrawHist1D(hist, kLitLinear, kLitLinear, "HIST TEXT0");
   if (fOutputDir != "") lit::SaveCanvasAsImage(canvas, fOutputDir);
}

void CbmLitTrackingQaDraw::DrawHitsStationHistos()
{
   if (fIsMvd) {DrawHitsStationHisto("rec_qa_mvd_hits_station", H1("hMvdNofHitsInStation"));}
   if (fIsSts) {DrawHitsStationHisto("rec_qa_sts_hits_station", H1("hStsNofHitsInStation"));}
   if (fIsTrd) {DrawHitsStationHisto("rec_qa_trd_hits_station", H1("hTrdNofHitsInStation"));}
   if (fIsMuch) {DrawHitsStationHisto("rec_qa_much_hits_station", H1("hMuchNofHitsInStation"));}
   if (fIsTof) {DrawHitsStationHisto("rec_qa_tof_hits_station", H1("hTofNofHitsInStation"));}
}

void CbmLitTrackingQaDraw::DrawStsTracksQaHistos()
{
   TCanvas* canvas1 = new TCanvas("rec_qa_sts_tracks_qa", "rec_qa_sts_tracks_qa", 900, 900);
   canvas1->Divide(2,2);
   canvas1->cd(1);
   H1("hStsChiprim")->Scale(1./H1("hStsChiprim")->Integral());
   DrawHist1D(H1("hStsChiprim"), kLitLinear, kLitLog);
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   canvas1->cd(2);
   TH1* projY = (TH1*)H2("hStsMomresVsMom")->ProjectionY()->Clone();
   projY->Scale(1./projY->Integral());
   DrawHist1D(projY, kLitLinear, kLitLog);
   gPad->SetGridx(true);
   gPad->SetGridy(true);

//   canvas1->cd(3);
//   DrawHist2D(fhStsMomresVsMom, "P [GeV/c]", "dP [%]", "Counter",
//         kLitLinearScale, kLitLinearScale, kLitLinearScale, "COLZ");
//   gPad->SetGridx(true);
//   gPad->SetGridy(true);

   canvas1->cd(3);
   H2("hStsMomresVsMom")->FitSlicesY();
   TH1* momslice = (TH1*) gDirectory->Get("hStsMomresVsMom_2");
   momslice->GetXaxis()->SetTitle("P [GeV/c]");
   momslice->GetYaxis()->SetTitle("dP/P, #sigma [%]");
   DrawHist1D(momslice, kLitLinear, kLitLinear);
   gPad->SetGridx(true);
   gPad->SetGridy(true);
   if (fOutputDir != "") lit::SaveCanvasAsImage(canvas1, fOutputDir);

   canvas1->cd(4);
   Int_t nBins = H2("hStsMomresVsMom")->GetNbinsX();
   TH1D* momResRms = (TH1D*)H2("hStsMomresVsMom")->ProjectionX()->Clone();
   for (Int_t i = 1; i < nBins; i++){
      TH1* projY = (TH1*)H2("hStsMomresVsMom")->ProjectionY("_py", i, i);
      float rms = projY->GetRMS();
      momResRms->SetBinContent(i, rms);
      momResRms->SetBinError(i, momslice->GetBinError(i));
   }
   momResRms->GetXaxis()->SetTitle("P [GeV/c]");
   momResRms->GetYaxis()->SetTitle("dP/P, RMS [%]");
   DrawHist1D(momResRms, kLitLinear, kLitLinear, "P");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   TCanvas* canvas2 = new TCanvas("rec_qa_track_length", "rec_qa_track_length", 500, 500);
   H1("hTrackLength")->Scale(1./H1("hTrackLength")->Integral());
   DrawHist1D(H1("hTrackLength"), kLitLinear, kLitLinear);
   gPad->SetGridx(true);
   gPad->SetGridy(true);

}

void CbmLitTrackingQaDraw::DrawMCMomVsAngle()
{
   TCanvas* canvas1 = new TCanvas("rec_qa_mc_mom_vs_angle", "rec_qa_mc_mom_vs_angle", 1200, 600);
   canvas1->Divide(2,1);
   canvas1->cd(1);
   DrawHist2D(H2("hMCMomVsAngle_All"), kLitLinear, kLitLinear, kLitLinear, "COLZ");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   canvas1->cd(2);
   DrawHist2D(H2("hMCMomVsAngle_El"), kLitLinear, kLitLinear, kLitLinear, "COLZ");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   if (fOutputDir != "") lit::SaveCanvasAsImage(canvas1, fOutputDir);
}
