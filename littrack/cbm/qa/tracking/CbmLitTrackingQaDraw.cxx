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

#include <boost/assign/list_of.hpp>

#include <iostream>
#include <string>

using boost::assign::list_of;
using lit::NumberToString;

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
   DrawEfficiency("tracking_qa_global_efficiency_all", list_of("hSts3DNormGlobal_All")
         ("hHalfGlobal3DNormGlobal_All")("hGlobal3D_All"), list_of(sname)(hgname)(gname), "", "_px");

   // Draw global tracking efficiency STS+TRD(MUCH)+TOF for signal tracks
   DrawEfficiency("tracking_qa_global_efficiency_signal", list_of("hSts3DNormGlobal_" + cat)
         ("hHalfGlobal3DNormGlobal_" + cat)("hGlobal3D_" + cat), list_of(sname)(hgname)(gname), "", "_px");

   // Draw half global tracking efficiency STS+TRD(MUCH) for all tracks
   DrawEfficiency("tracking_qa_half_global_efficiency_all", list_of(string("hSts3DNormHalfGlobal_All"))
         ("hHalfGlobal3D_" + cat), list_of(sname)(hgname), "", "_px");

   // Draw half global tracking efficiency STS+TRD(MUCH) for signal tracks
   DrawEfficiency("tracking_qa_half_global_efficiency_signal", list_of("hSts3DNormHalfGlobal_" + cat)
         ("hHalfGlobal3D_" + cat), list_of(sname)(hgname), "", "_px");

   // Draw efficiency for STS
   DrawEfficiency("tracking_qa_sts_efficiency", list_of(string("hSts3D_All"))("hSts3D_" + cat),
         list_of(string("STS: all"))("STS: " + signal), "", "_px");

   if (fDet.GetDet(kTRD) || fDet.GetDet(kMUCH)) {
      // Draw efficiency for TRD(MUCH)
      DrawEfficiency("tracking_qa_rec_efficiency", list_of(string("hRec3D_All"))("hRec3D_" + cat),
            list_of(rname + ": all")(rname + ": " + signal), "", "_px");
   }

   if (fDet.GetDet(kTOF)) {
      // Draw efficiency for TOF
      DrawEfficiency("tracking_qa_tof_efficiency", list_of(string("hTof3D_All"))("hTof3D_" + cat),
            list_of(string("TOF: all"))("TOF: " + signal), "", "_px");
   }

   if (fDet.GetDet(kRICH)) {
      // Draw efficiency for RICH for electron set
      DrawEfficiency("tracking_qa_rich_efficiency_electrons_mom", list_of("hRich3D_El")
         ("hRich3D_ElRef"), list_of("RICH: electrons")("RICH: electrons ref"), "", "_px");

      // Draw efficiency for RICH for electron set vs. rapidity
      DrawEfficiency("tracking_qa_rich_efficiency_electrons_rapidity", list_of("hRich3D_El")
         ("hRich3D_ElRef"), list_of("RICH: electrons")("RICH: electrons ref"), "", "_py");

      // Draw efficiency for RICH for electron set vs. pt
      DrawEfficiency("tracking_qa_rich_efficiency_electrons_pt", list_of("hRich3D_El")
         ("hRich3D_ElRef"), list_of("RICH: electrons")("RICH: electrons ref"), "", "_pz");

      // Draw efficiency vs. nof hits in ring for RICH for electron set
      DrawEfficiency("tracking_qa_rich_efficiency_vs_nofhits_electrons",
            list_of("hRichNh_El"), list_of("RICH: electrons"));

      // Draw efficiency vs. B/A for ellipse fitting for RICH for electron set
      DrawEfficiency("tracking_qa_rich_efficiency_vs_boa_electrons",
            list_of("hRichBoA_El"), list_of("RICH: electrons"));

      // Draw efficiency vs. radial position of the ring for RICH for electron set
      DrawEfficiency("tracking_qa_rich_efficiency_vs_radial_position_electrons",
            list_of("hRichRadPos_El"), list_of("RICH: electrons"));

      // Draw efficiency for STS+RICH for electron set
      DrawEfficiency("tracking_qa_sts_rich_efficiency_electrons", list_of("hSts3DNormStsRich_El")
         ("hStsRich3D_El"), list_of("STS")("STS+RICH"), "", "_px");

      // Draw efficiency for STS+RICH No matching for electron set
      DrawEfficiency("tracking_qa_sts_rich_no_matching_efficiency_electrons", list_of("hSts3DNormStsRich_El")
         ("hStsRichNoMatching3D_El"), list_of("STS")("STS+RICH (No Matching)"), "", "_px");

      // Draw efficiency for STS+RICH+TRD for electron set
      DrawEfficiency("tracking_qa_sts_rich_trd_efficiency_electrons", list_of("hSts3DNormStsRichTrd_El")
         ("hStsRich3DNormStsRichTrd_El")("hStsRichTrd3D_El"), list_of("STS")("STS+RICH")("STS+RICH+TRD"), "", "_px");

      // Draw efficiency for STS+RICH+TRD+TOF for electron set
      DrawEfficiency("tracking_qa_sts_rich_trd_tof_efficiency_electrons", list_of("hSts3DNormStsRichTrdTof_El")
         ("hStsRich3DNormStsRichTrdTof_El")("hStsRichTrd3DNormStsRichTrdTof_El")("hStsRichTrdTof3D_El"),
         list_of("STS")("STS+RICH")("STS+RICH+TRD")("STS+RICH+TRD+TOF"), "", "_px");

//      TCanvas* canvas = new TCanvas("tracking_qa_sts_rich_trd_tof_momentum_electrons", "tracking_qa_sts_rich_trd_tof_momentum_electrons", 600, 500);
//      canvas->SetGrid();
//      DrawHist1D(H1("hSts3DNormStsRichTrdTof_El_Rec"), H1("hStsRich3DNormStsRichTrdTof_El_Rec"),
//         H1("hStsRichTrd3DNormStsRichTrdTof_El_Rec"), H1("hStsRichTrdTof3D_El_Rec"),
//         "Efficiency", "Momentum [GeV/c]", "Efficiency", "STS", "STS+RICH",
//         "STS+RICH+TRD", "STS+RICH+TRD+TOF", kLitLinearScale, kLitLinearScale, true, 0.3,0.3,0.85,0.6);

      DrawEfficiency("tracking_qa_sts_rich_trd_tof_electron_identification", list_of("hStsRich3DElIdNormStsRichTrdTof_ElId")
         ("hStsRichTrd3DElIdNormStsRichTrdTof_ElId")("hStsRichTrdTof3DElId_ElId"),
         list_of("RICH")("RICH+TRD")("RICH+TRD+TOF"), "", "_px");

      DrawEfficiency("tracking_qa_sts_rich_trd_tof_pion_suppression", list_of("hStsRich3DElIdNormStsRichTrdTof_PiSupp")
         ("hStsRichTrd3DElIdNormStsRichTrdTof_PiSupp")("hStsRichTrdTof3DElId_PiSupp"),
         list_of("RICH")("RICH+TRD")("RICH+TRD+TOF"), "pisupp", "_px");
    }

   if (fDet.GetDet(kRICH)) {
      // Draw detector acceptance
      //DrawEfficiency("tracking_qa_sts_rich_trd_tof_detector_acceptance", "hStsDetAcc3D_El_px",
      //      "hStsRichDetAcc3D_El_px", "hStsRichTrdDetAcc3D_El_px", "hStsRichTrdTofDetAcc3D_El_px",
      //     "STS", "STS+RICH", "STS+RICH+TRD", "STS+RICH+TRD+TOF", "");

   }
}

void CbmLitTrackingQaDraw::DrawEfficiency(
      const string& canvasName,
      const vector<string>& histNames,
      const vector<string>& histLabels,
      const string& opt,
      const string& proj)
{
   assert(histNames.size() != 0 && histLabels.size() == histNames.size());

   TCanvas* canvas = new TCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
   canvas->SetGrid();
   canvas->cd();

   Int_t nofHistos = histNames.size();
   vector<TH1*> histos(nofHistos);
   vector<string> labels(nofHistos);
   vector<Double_t> efficiencies(nofHistos);
   for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
      string name = histNames[iHist];
      histos[iHist] = fHM->H1(name + "_Eff" + proj);
      efficiencies[iHist] = CalcEfficiency(fHM->H1(name + "_Rec" + proj), fHM->H1(name + "_Acc" + proj), opt);
      labels[iHist] = histLabels[iHist] + "(" + NumberToString<Double_t>(efficiencies[iHist], 1) + ")";
      if (fRebin > 1) {
         histos[iHist]->Rebin(fRebin);
         histos[iHist]->Scale(1. / fRebin);
      }
   }

   if (opt != "pisupp"){
      histos[0]->SetMinimum(0.);
      histos[0]->SetMaximum(100.);
   }

   DrawH1(histos, labels, kLitLinear, kLitLinear, true, 0.3, 0.3, 0.85, 0.6, "PE1");
   DrawMeanEfficiencyLines(histos, efficiencies);

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
   const vector<TH1*>& histos,
   const vector<Double_t>& efficiencies)
{
   assert(histos.size() != 0 && efficiencies.size() == histos.size());

   Double_t minX = histos[0]->GetXaxis()->GetXmin();
   Double_t maxX = histos[0]->GetXaxis()->GetXmax();
   Int_t nofHistos = histos.size();
   for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
      TLine* line = new TLine(minX, efficiencies[iHist], maxX, efficiencies[iHist]);
      line->SetLineWidth(1);
      line->SetLineColor(histos[iHist]->GetLineColor());
      line->Draw();
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
   if (fDet.GetDet(kMVD)) { DrawHitsHistos("tracking_qa_mvd_hits", "hMvdTrackHits"); }
   if (fDet.GetDet(kSTS)) { DrawHitsHistos("tracking_qa_sts_hits", "hStsTrackHits"); }
   if (fDet.GetDet(kTRD)) { DrawHitsHistos("tracking_qa_trd_hits", "hTrdTrackHits"); }
   if (fDet.GetDet(kMUCH)) { DrawHitsHistos("tracking_qa_much_hits", "hMuchTrackHits"); }
   if (fDet.GetDet(kRICH)) { DrawHitsHistos("tracking_qa_rich_ring_hits", "hRichRingHits"); }
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
   DrawH1(list_of(hAll)(hTrue)(hFake),
      list_of("all: " + NumberToString<float>(hAll->GetMean(), 1))
             ("true: " + NumberToString<float>(hTrue->GetMean(), 1))
             ("fake: " + NumberToString<float>(hFake->GetMean(), 1)),
              kLitLinear, kLitLog, true, 0.25, 0.99, 0.55, 0.75);

   c->cd(2);
   TH1F* hTrueOverAll = fHM->H1F(hist+"_TrueOverAll");
   TH1F* hFakeOverAll = fHM->H1F(hist+"_FakeOverAll");
   DrawH1(list_of(hTrueOverAll)(hFakeOverAll),
      list_of("true/all: " + NumberToString<float>(hTrueOverAll->GetMean()))
             ("fake/all: " + NumberToString<float>(hFakeOverAll->GetMean())),
             kLitLinear, kLitLog, true, 0.25,0.99,0.55,0.75);

   if (fOutputDir != "") lit::SaveCanvasAsImage(c, fOutputDir);
}

void CbmLitTrackingQaDraw::DrawStsTracksQaHistos()
{
   TCanvas* canvas1 = new TCanvas("tracking_qa_sts_tracks_qa", "tracking_qa_sts_tracks_qa", 900, 900);
   canvas1->Divide(2,2);
   canvas1->cd(1);
   TH1F* hChiprim = fHM->H1F("hStsChiprim");
   hChiprim->Scale(1. / hChiprim->Integral());
   DrawH1(hChiprim, kLitLinear, kLitLog);
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   canvas1->cd(2);
   TH1* projY = (TH1*)fHM->H2F("hStsMomresVsMom")->ProjectionY();
   projY->Scale(1./projY->Integral());
   DrawH1(projY, kLitLinear, kLitLog);
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
   DrawH1(momslice, kLitLinear, kLitLinear);
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
   DrawH1(momResRms, kLitLinear, kLitLinear, "P");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   TCanvas* canvas2 = new TCanvas("tracking_qa_track_length", "tracking_qa_track_length", 500, 500);
   TH1F* hLength = fHM->H1F("hTrackLength");
   hLength->Scale(1. / hLength->Integral());
   DrawH1(hLength, kLitLinear, kLitLinear);
   gPad->SetGridx(true);
   gPad->SetGridy(true);
}

void CbmLitTrackingQaDraw::DrawMCMomVsAngle()
{
   TCanvas* canvas1 = new TCanvas("tracking_qa_mc_mom_vs_angle", "tracking_qa_mc_mom_vs_angle", 1200, 600);
   canvas1->Divide(2,1);
   canvas1->cd(1);
   DrawH2(fHM->H2F("hMCMomVsAngle_All"), kLitLinear, kLitLinear, kLitLinear, "COLZ");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   canvas1->cd(2);
   DrawH2(fHM->H2F("hMCMomVsAngle_El"), kLitLinear, kLitLinear, kLitLinear, "COLZ");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   if (fOutputDir != "") lit::SaveCanvasAsImage(canvas1, fOutputDir);
}
