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

CbmLitTrackingQaDraw::CbmLitTrackingQaDraw():
   fHM(),
   fRebin(1),
   fDet(),
   fOutputDir("")
{

}

CbmLitTrackingQaDraw::~CbmLitTrackingQaDraw()
{

}

void CbmLitTrackingQaDraw::Draw(
      CbmLitHistManager* histManager,
      const std::string& outputDir)
{
   assert(histManager != NULL);
   fHM = histManager;
   fOutputDir = outputDir;

   fDet.DetermineSetup();

   SetStyles();
   DrawEfficiencyHistos();
   DrawHitsHistos();
   DrawStsTracksQaHistos();
   DrawMcEfficiencyGraph();
   DrawMCMomVsAngle();
}

void CbmLitTrackingQaDraw::DrawEfficiencyHistos()
{
   //return;
   string sname("STS");
   string rname;
   if (fDet.GetDet(kMUCH) && !fDet.GetDet(kTRD)) { rname = "MUCH"; }
   else if (fDet.GetDet(kTRD) && !fDet.GetDet(kMUCH)) { rname = "TRD"; }
   else if (fDet.GetDet(kTRD) && fDet.GetDet(kMUCH)) { rname = "MUCH+TRD"; }
   string hgname(sname + "+" + rname);
   string gname = hgname + "+TOF";

   string signal = fDet.GetDet(kMUCH) ? "muons" : "electrons";
   string cat = fDet.GetDet(kMUCH) ? "Mu" : "El";

   // Draw global tracking efficiency STS+TRD(MUCH)+TOF for all tracks
   DrawEfficiency("rec_qa_global_efficiency_all", "hSts3DNormGlobal_All",
         "hHalfGlobal3DNormGlobal_All", "hGlobal3D_All", "", sname, hgname, gname, "", "", "_px");

   // Draw global tracking efficiency STS+TRD(MUCH)+TOF for signal tracks
   DrawEfficiency("rec_qa_global_efficiency_signal", "hSts3DNormGlobal_" + cat,
         "hHalfGlobal3DNormGlobal_" + cat, "hGlobal3D_" + cat, "", sname, hgname, gname, "", "", "_px");

   // Draw half global tracking efficiency STS+TRD(MUCH) for all tracks
   DrawEfficiency("rec_qa_half_global_efficiency_all", "hSts3DNormHalfGlobal_All",
         "hHalfGlobal3D_" + cat, "", "", sname, hgname, "", "", "", "_px");

   // Draw half global tracking efficiency STS+TRD(MUCH) for signal tracks
   DrawEfficiency("rec_qa_half_global_efficiency_signal", "hSts3DNormHalfGlobal_" + cat,
         "hHalfGlobal3D_" + cat, "", "", sname, hgname, "", "", "", "_px");

   // Draw efficiency for STS
   DrawEfficiency("rec_qa_sts_efficiency", "hSts3D_All", "hSts3D_" + cat,
         "", "", "STS: all", "STS: " + signal, "", "", "", "_px");

   if (fDet.GetDet(kTRD) || fDet.GetDet(kMUCH)) {
      // Draw efficiency for TRD(MUCH)
      DrawEfficiency("rec_qa_rec_efficiency", "hRec3D_All", "hRec3D_" + cat,
            "", "", rname + ": all", rname + ": " + signal, "", "", "", "_px");
   }

   if (fDet.GetDet(kTOF)) {
      // Draw efficiency for TOF
      DrawEfficiency("rec_qa_tof_efficiency", "hTof3D_All",
         "hTof3D_" + cat, "", "", "TOF: all", "TOF: " + signal, "", "", "", "_px");
   }

   if (fDet.GetDet(kRICH)) {
      // Draw efficiency for RICH for electron set
      DrawEfficiency("rec_qa_rich_efficiency_electrons_mom", "hRich3D_El",
         "hRich3D_ElRef", "", "", "RICH: electrons", "RICH: electrons ref", "", "", "", "_px");

      // Draw efficiency for RICH for electron set vs. rapidity
      DrawEfficiency("rec_qa_rich_efficiency_electrons_rapidity", "hRich3D_El",
         "hRich3D_ElRef", "", "", "RICH: electrons", "RICH: electrons ref", "", "", "", "_py");

      // Draw efficiency for RICH for electron set vs. pt
      DrawEfficiency("rec_qa_rich_efficiency_electrons_pt", "hRich3D_El",
         "hRich3D_ElRef", "", "", "RICH: electrons", "RICH: electrons ref", "", "", "", "_pz");

      // Draw efficiency vs. nof hits in ring for RICH for electron set
      DrawEfficiency("rec_qa_rich_efficiency_vs_nofhits_electrons", "hRichNh_El",
         "", "", "", "RICH: electrons", "", "", "", "");

      // Draw efficiency vs. B/A for ellipse fitting for RICH for electron set
      DrawEfficiency("rec_qa_rich_efficiency_vs_boa_electrons", "hRichBoA_El",
         "", "", "", "RICH: electrons", "", "", "", "");

      // Draw efficiency vs. radial position of the ring for RICH for electron set
      DrawEfficiency("rec_qa_rich_efficiency_vs_radial_position_electrons", "hRichRadPos_El",
         "", "", "", "RICH: electrons", "", "", "", "");

      // Draw efficiency for STS+RICH for electron set
      DrawEfficiency("rec_qa_sts_rich_efficiency_electrons", "hSts3DNormStsRich_El",
         "hStsRich3D_El", "", "", "STS", "STS+RICH", "", "", "", "_px");

      // Draw efficiency for STS+RICH No matching for electron set
      DrawEfficiency("rec_qa_sts_rich_no_matching_efficiency_electrons", "hSts3DNormStsRich_El",
         "hStsRichNoMatching3D_El", "", "", "STS", "STS+RICH (No Matching)", "", "", "", "_px");

      // Draw efficiency for STS+RICH+TRD for electron set
      DrawEfficiency("rec_qa_sts_rich_trd_efficiency_electrons", "hSts3DNormStsRichTrd_El",
         "hStsRich3DNormStsRichTrd_El", "hStsRichTrd3D_El", "",
         "STS", "STS+RICH", "STS+RICH+TRD", "", "", "_px");

      // Draw efficiency for STS+RICH+TRD+TOF for electron set
      DrawEfficiency("rec_qa_sts_rich_trd_tof_efficiency_electrons", "hSts3DNormStsRichTrdTof_El",
         "hStsRich3DNormStsRichTrdTof_El", "hStsRichTrd3DNormStsRichTrdTof_El", "hStsRichTrdTof3D_El",
         "STS", "STS+RICH", "STS+RICH+TRD", "STS+RICH+TRD+TOF", "", "_px");

//      TCanvas* canvas = new TCanvas("rec_qa_sts_rich_trd_tof_momentum_electrons", "rec_qa_sts_rich_trd_tof_momentum_electrons", 600, 500);
//      canvas->SetGrid();
//      DrawHist1D(H1("hSts3DNormStsRichTrdTof_El_Rec"), H1("hStsRich3DNormStsRichTrdTof_El_Rec"),
//         H1("hStsRichTrd3DNormStsRichTrdTof_El_Rec"), H1("hStsRichTrdTof3D_El_Rec"),
//         "Efficiency", "Momentum [GeV/c]", "Efficiency", "STS", "STS+RICH",
//         "STS+RICH+TRD", "STS+RICH+TRD+TOF", kLitLinearScale, kLitLinearScale, true, 0.3,0.3,0.85,0.6);

      DrawEfficiency("rec_qa_sts_rich_trd_tof_electron_identification", "hStsRich3DElIdNormStsRichTrdTof_ElId",
         "hStsRichTrd3DElIdNormStsRichTrdTof_ElId", "hStsRichTrdTof3DElId_ElId", "",
         "RICH", "RICH+TRD", "RICH+TRD+TOF", "", "", "_px");

      DrawEfficiency("rec_qa_sts_rich_trd_tof_pion_suppression", "hStsRich3DElIdNormStsRichTrdTof_PiSupp",
         "hStsRichTrd3DElIdNormStsRichTrdTof_PiSupp", "hStsRichTrdTof3DElId_PiSupp", "",
         "RICH", "RICH+TRD", "RICH+TRD+TOF", "", "pisupp", "_px");
    }

   if (fDet.GetDet(kRICH)) {
      // Draw detector acceptance
      //DrawEfficiency("rec_qa_sts_rich_trd_tof_detector_acceptance", "hStsDetAcc3D_El_px",
      //      "hStsRichDetAcc3D_El_px", "hStsRichTrdDetAcc3D_El_px", "hStsRichTrdTofDetAcc3D_El_px",
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
      const string& opt,
      const string& proj)
{

   TCanvas* canvas = new TCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
   canvas->SetGrid();
   canvas->cd();

   TH1* h1 = (hist1 != "") ? fHM->H1(hist1 + "_Eff" + proj) : NULL;
   TH1* h2 = (hist2 != "") ? fHM->H1(hist2 + "_Eff" + proj) : NULL;
   TH1* h3 = (hist3 != "") ? fHM->H1(hist3 + "_Eff" + proj) : NULL;
   TH1* h4 = (hist4 != "") ? fHM->H1(hist4 + "_Eff" + proj) : NULL;

   float eff1 = (hist1 != "") ? CalcEfficiency(fHM->H1(hist1 + "_Rec" + proj), fHM->H1(hist1 + "_Acc" + proj), opt) : 0.;
   float eff2 = (hist2 != "") ? CalcEfficiency(fHM->H1(hist2 + "_Rec" + proj), fHM->H1(hist2 + "_Acc" + proj), opt) : 0.;
   float eff3 = (hist3 != "") ? CalcEfficiency(fHM->H1(hist3 + "_Rec" + proj), fHM->H1(hist3 + "_Acc" + proj), opt) : 0.;
   float eff4 = (hist4 != "") ? CalcEfficiency(fHM->H1(hist4 + "_Rec" + proj), fHM->H1(hist4 + "_Acc" + proj), opt) : 0.;

   string hname1 = (hist1 != "") ? name1 + "(" + lit::NumberToString<float>(eff1, 1) + ")" : name1;
   string hname2 = (hist2 != "") ? name2 + "(" + lit::NumberToString<float>(eff2, 1) + ")" : name2;
   string hname3 = (hist3 != "") ? name3 + "(" + lit::NumberToString<float>(eff3, 1) + ")" : name3;
   string hname4 = (hist4 != "") ? name4 + "(" + lit::NumberToString<float>(eff4, 1) + ")" : name4;

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
            kLitLinear, kLitLinear, true, 0.3, 0.3, 0.85, 0.6, "PE1");
      DrawMeanEfficiencyLines(h1, eff1, eff2, eff3, eff4);

   } else if (h1 != NULL && h2 != NULL && h3 != NULL && h4 == NULL) {
      DrawHist1D(h1, h2, h3, NULL, hname1, hname2, hname3, "",
            kLitLinear, kLitLinear, true, 0.3, 0.3, 0.85, 0.6, "PE1");
      DrawMeanEfficiencyLines(h1, eff1, eff2, eff3);

   } else if (h1 != NULL && h2 != NULL && h3 == NULL && h4 == NULL){
      DrawHist1D(h1, h2, NULL, NULL, hname1, hname2, "", "",
            kLitLinear, kLitLinear, true, 0.3, 0.3, 0.85, 0.6, "PE1");
      DrawMeanEfficiencyLines(h1, eff1, eff2);

   } else if (h1 != NULL && h2 == NULL && h3 == NULL && h4 == NULL){
      DrawHist1D(h1, NULL, NULL, NULL, hname1, "", "", "",
            kLitLinear, kLitLinear, true, 0.3, 0.3, 0.85, 0.6, "PE1");
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
   if (fDet.GetDet(kMVD)) { DrawHitsHistos("rec_qa_mvd_hits", "hMvdTrackHits"); }
   if (fDet.GetDet(kSTS)) { DrawHitsHistos("rec_qa_sts_hits", "hStsTrackHits"); }
   if (fDet.GetDet(kTRD)) { DrawHitsHistos("rec_qa_trd_hits", "hTrdTrackHits"); }
   if (fDet.GetDet(kMUCH)) { DrawHitsHistos("rec_qa_much_hits", "hMuchTrackHits"); }
   if (fDet.GetDet(kRICH)) { DrawHitsHistos("rec_qa_rich_ring_hits", "hRichRingHits"); }
}

void CbmLitTrackingQaDraw::DrawHitsHistos(
   const string& canvasName,
   const string& hist)
{
   TCanvas* c = new TCanvas(canvasName.c_str(), canvasName.c_str(), 1200, 600);
   c->Divide(2,1);
   c->SetGrid();

   c->cd(1);
   TH1F* hAll = fHM->H1F(hist + "_All");
   TH1F* hTrue = fHM->H1F(hist + "_True");
   TH1F* hFake = fHM->H1F(hist + "_Fake");
   DrawHist1D(hAll, hTrue, hFake, NULL,
              "all: " + lit::NumberToString<float>(hAll->GetMean(), 1),
              "true: " + lit::NumberToString<float>(hTrue->GetMean(), 1),
              "fake: " + lit::NumberToString<float>(hFake->GetMean(), 1),  "",
              kLitLinear, kLitLog, true, 0.25,0.99,0.55,0.75);

   c->cd(2);
   TH1F* hTrueOverAll = fHM->H1F(hist+"_TrueOverAll");
   TH1F* hFakeOverAll = fHM->H1F(hist+"_FakeOverAll");
   DrawHist1D(hTrueOverAll, hFakeOverAll, NULL, NULL,
              "true/all: " + lit::NumberToString<float>(hTrueOverAll->GetMean()),
              "fake/all: " + lit::NumberToString<float>(hFakeOverAll->GetMean()),
              "", "", kLitLinear, kLitLog, true, 0.25,0.99,0.55,0.75);

   if (fOutputDir != "") lit::SaveCanvasAsImage(c, fOutputDir);
}

void CbmLitTrackingQaDraw::DrawStsTracksQaHistos()
{
   TCanvas* canvas1 = new TCanvas("rec_qa_sts_tracks_qa", "rec_qa_sts_tracks_qa", 900, 900);
   canvas1->Divide(2,2);
   canvas1->cd(1);
   TH1F* hChiprim = fHM->H1F("hStsChiprim");
   hChiprim->Scale(1. / hChiprim->Integral());
   DrawHist1D(hChiprim, kLitLinear, kLitLog);
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   canvas1->cd(2);
   TH1* projY = (TH1*)fHM->H2F("hStsMomresVsMom")->ProjectionY();
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
   fHM->H2F("hStsMomresVsMom")->FitSlicesY();
   TH1* momslice = (TH1*) gDirectory->Get("hStsMomresVsMom_2");
   momslice->GetXaxis()->SetTitle("P [GeV/c]");
   momslice->GetYaxis()->SetTitle("dP/P, #sigma [%]");
   DrawHist1D(momslice, kLitLinear, kLitLinear);
   gPad->SetGridx(true);
   gPad->SetGridy(true);
   if (fOutputDir != "") lit::SaveCanvasAsImage(canvas1, fOutputDir);

   canvas1->cd(4);
   TH2F* hMomres = fHM->H2F("hStsMomresVsMom");
   Int_t nBins = hMomres->GetNbinsX();
   TH1D* momResRms = (TH1D*)hMomres->ProjectionX();
   for (Int_t i = 1; i < nBins; i++){
      TH1* projY = (TH1*)hMomres->ProjectionY("_py", i, i);
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
   TH1F* hLength = fHM->H1F("hTrackLength");
   hLength->Scale(1. / hLength->Integral());
   DrawHist1D(hLength, kLitLinear, kLitLinear);
   gPad->SetGridx(true);
   gPad->SetGridy(true);
}

void CbmLitTrackingQaDraw::DrawMCMomVsAngle()
{
   TCanvas* canvas1 = new TCanvas("rec_qa_mc_mom_vs_angle", "rec_qa_mc_mom_vs_angle", 1200, 600);
   canvas1->Divide(2,1);
   canvas1->cd(1);
   DrawHist2D(fHM->H2F("hMCMomVsAngle_All"), kLitLinear, kLitLinear, kLitLinear, "COLZ");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   canvas1->cd(2);
   DrawHist2D(fHM->H2F("hMCMomVsAngle_El"), kLitLinear, kLitLinear, kLitLinear, "COLZ");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   if (fOutputDir != "") lit::SaveCanvasAsImage(canvas1, fOutputDir);
}
