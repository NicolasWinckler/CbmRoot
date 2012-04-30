/**
 * \file CbmLitTrackingQaDraw.cxx
 * \brief Draw histograms for tracking QA.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitTrackingQaDraw.h"
#include "CbmLitTrackingQaHistCreator.h"
#include "qa/base/CbmLitHistManager.h"

#include "TCanvas.h"
#include "TLine.h"
#include "TDirectory.h"

#include "cbm/qa/draw/CbmLitDrawHist.h"
#include "utils/CbmLitUtils.h"

#include <boost/assign/list_of.hpp>

#include <iostream>
#include <string>

using boost::assign::list_of;
using lit::NumberToString;
using lit::FindAndReplace;
using lit::Split;

CbmLitTrackingQaDraw::CbmLitTrackingQaDraw():
   fHM(),
//   fRebin(1),
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

   SetStyles();
   DrawEfficiencyHistos();
   DrawHitsHistos();
//   DrawStsTracksQaHistos();
//   DrawMcEfficiencyGraph();
//   DrawMCMomVsAngle();
}

void CbmLitTrackingQaDraw::DrawEfficiencyHistos()
{
	// Draw global tracking efficiency
	vector<string> globalTrackVariants = CbmLitTrackingQaHistCreator::GlobalTrackVariants();
	for (UInt_t i = 0; i < globalTrackVariants.size(); i++) {
		string variant = globalTrackVariants[i];
		DrawEfficiency("tracking_qa_global_tracking_efficiency_" + variant + "_p", "hte_Sts.*_" + variant + "_All_Eff_p");
		//DrawEfficiency("tracking_qa_global_tracking_efficiency_" + variant + "_pt", "hte_Sts.*_" + variant + "_All_Eff_pt");
		//DrawEfficiency("tracking_qa_global_tracking_efficiency_" + variant + "_y", "hte_Sts.*_" + variant + "_All_Eff_y");
	}

	// Draw local tracking efficiency
	vector<string> localTrackVariants = list_of("Sts")("Trd")("Much")("Rich")("Tof");
	for (UInt_t i = 0; i < localTrackVariants.size(); i++) {
		string variant = localTrackVariants[i];
		string re = (variant == "Sts") ? "hte_Sts_Sts_(All|Muon|Electron)_Eff_p" : "hte_" + variant + "_.*_(All|Muon|Electron)_Eff_p";
		DrawEfficiency("tracking_qa_local_tracking_efficiency_" + variant + "_p", re);
		//DrawEfficiency("tracking_qa_local_tracking_efficiency_" + variant + "_pt", "hte_" + variant + "_.*" + variant + ".*_(All|Electron)_Eff_pt");
		//DrawEfficiency("tracking_qa_local_tracking_efficiency_" + variant + "_y", "hte_" + variant + "_.*" + variant + ".*_(All|Electron)_Eff_y");
	}
}

void CbmLitTrackingQaDraw::DrawEfficiency(
      const string& canvasName,
      const string& histNamePattern)
{
	vector<TH1*> histos = fHM->H1Vector(histNamePattern);
	if (histos.size() == 0) return;

	TCanvas* canvas = new TCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
	canvas->SetGrid();
	canvas->cd();

	Int_t nofHistos = histos.size();
	vector<string> labels(nofHistos);
	vector<Double_t> efficiencies(nofHistos);
	for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
		string name = histos[iHist]->GetName();
		efficiencies[iHist] = CalcEfficiency(fHM->H1(FindAndReplace(name, "_Eff_", "_Rec_")), fHM->H1(FindAndReplace(name, "_Eff_", "_Acc_")), 100.);
		vector<string> split = Split(name, '_');
		labels[iHist] = split[1] + ":" + split[3] + "(" + NumberToString<Double_t>(efficiencies[iHist], 1) + ")";
	}

	DrawH1(histos, labels, kLitLinear, kLitLinear, true, 0.3, 0.3, 0.85, 0.6, "PE1");
	DrawMeanEfficiencyLines(histos, efficiencies);
	if (fOutputDir != "") lit::SaveCanvasAsImage(canvas, fOutputDir);
}

Double_t CbmLitTrackingQaDraw::CalcEfficiency(
   TH1* histRec,
   TH1* histAcc,
   Double_t scale)
{
   if (histAcc->GetEntries() == 0 || histRec->GetEntries() == 0) {
      return 0.;
   } else {
      return scale * Double_t(histRec->GetEntries()) / Double_t(histAcc->GetEntries());
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

//void CbmLitTrackingQaDraw::DrawMcEfficiencyGraph()
//{
// /*  boost::property_tree::ptree pt = PrintPTree();
//
//   TH1* hAcc = new TH1("hMcAccEfficiencyGraph", "hMcAccEfficiencyGraph;Detector;Efficiency [%]", 4, 0, 4);
//   hAcc->SetMaximum(100.);
//   hAcc->SetMinimum(0.);
//   hAcc->SetBinContent(1, pt.get("hStsDetAccEl.detAccAcc.eff", 0.0));
//   hAcc->GetXaxis()->SetBinLabel(1, "STS");
//   hAcc->SetBinContent(2, pt.get("hStsRichDetAccEl.detAccAcc.eff", 0.0));
//   hAcc->GetXaxis()->SetBinLabel(2, "RICH");
//   hAcc->SetBinContent(3, pt.get("hStsRichTrdDetAccEl.detAccAcc.eff", 0.0));
//   hAcc->GetXaxis()->SetBinLabel(3, "TRD");
//   hAcc->SetBinContent(4, pt.get("hStsRichTrdTofDetAccEl.detAccAcc.eff", 0.0));
//   hAcc->GetXaxis()->SetBinLabel(4, "TOF");
//
//   TH1* hRec = new TH1("hMcRecEfficiencyGraph", "hMcRecEfficiencyGraph", 4, 0, 4);
//   hRec->SetBinContent(1, pt.get("hStsDetAccEl.detAccRec.eff", 0.0));
//   //hRec->SetBinLabel(1, "STS");
//   hRec->SetBinContent(2, pt.get("hStsRichDetAccEl.detAccRec.eff", 0.0));
//   //hRec->SetBinLabel(1, "RICH");
//   hRec->SetBinContent(3, pt.get("hStsRichTrdDetAccEl.detAccRec.eff", 0.0));
//   //hRec->SetBinLabel(1, "TRD");
//   hRec->SetBinContent(4, pt.get("hStsRichTrdTofDetAccEl.detAccRec.eff", 0.0));
//   //hRec->SetBinLabel(1, "TOF");
//
//   TCanvas* canvas = new TCanvas("mc_acc_rec_graph", "mc_acc_rec_graph", 600, 500);
//   DrawHist1D(hAcc, hRec, NULL, NULL, "", "Detector", "Efficiency [%]", "ACC/MC", "REC/MC", "", "",
//              kLitLinearScale, kLitLinearScale, true, 0.6,0.1,0.9,0.4);
//   gPad->SetGridx(true);
//   gPad->SetGridy(true);*/
//}


void CbmLitTrackingQaDraw::DrawHitsHistos()
{
   DrawHitsHistos("tracking_qa_mvd_hits", "hth_Mvd_TrackHits");
   DrawHitsHistos("tracking_qa_sts_hits", "hth_Sts_TrackHits");
   DrawHitsHistos("tracking_qa_trd_hits", "hth_Trd_TrackHits");
   DrawHitsHistos("tracking_qa_much_hits", "hth_Much_TrackHits");
   DrawHitsHistos("tracking_qa_rich_hits", "hth_Rich_TrackHits");
}

void CbmLitTrackingQaDraw::DrawHitsHistos(
   const string& canvasName,
   const string& hist)
{
   if (!(fHM->Exists(hist + "_All") && fHM->Exists(hist + "_True") &&
         fHM->Exists(hist + "_Fake") && fHM->Exists(hist + "_TrueOverAll") &&
         fHM->Exists(hist + "_FakeOverAll"))) return;

   TCanvas* canvas = new TCanvas(canvasName.c_str(), canvasName.c_str(), 1200, 600);
   canvas->Divide(2,1);
   canvas->SetGrid();

   canvas->cd(1);
   TH1* hAll = fHM->H1(hist + "_All");
   TH1* hTrue = fHM->H1(hist + "_True");
   TH1* hFake = fHM->H1(hist + "_Fake");
   DrawH1(list_of(hAll)(hTrue)(hFake),
      list_of("all: " + NumberToString<Double_t>(hAll->GetMean(), 1))
             ("true: " + NumberToString<Double_t>(hTrue->GetMean(), 1))
             ("fake: " + NumberToString<Double_t>(hFake->GetMean(), 1)),
              kLitLinear, kLitLog, true, 0.25, 0.99, 0.55, 0.75);

   canvas->cd(2);
   TH1* hTrueOverAll = fHM->H1(hist + "_TrueOverAll");
   TH1* hFakeOverAll = fHM->H1(hist + "_FakeOverAll");
   DrawH1(list_of(hTrueOverAll)(hFakeOverAll),
      list_of("true/all: " + NumberToString<Double_t>(hTrueOverAll->GetMean()))
             ("fake/all: " + NumberToString<Double_t>(hFakeOverAll->GetMean())),
             kLitLinear, kLitLog, true, 0.25, 0.99, 0.55, 0.75);

   if (fOutputDir != "") lit::SaveCanvasAsImage(canvas, fOutputDir);
}

//void CbmLitTrackingQaDraw::DrawStsTracksQaHistos()
//{
//   TCanvas* canvas1 = new TCanvas("tracking_qa_sts_tracks_qa", "tracking_qa_sts_tracks_qa", 900, 900);
//   canvas1->Divide(2,2);
//   canvas1->cd(1);
//   TH1* hChiprim = fHM->H1("hStsChiprim");
//   hChiprim->Scale(1. / hChiprim->Integral());
//   DrawH1(hChiprim, kLitLinear, kLitLog);
//   gPad->SetGridx(true);
//   gPad->SetGridy(true);
//
//   canvas1->cd(2);
//   TH1* projY = (TH1*)fHM->H2F("hStsMomresVsMom")->ProjectionY();
//   projY->Scale(1./projY->Integral());
//   DrawH1(projY, kLitLinear, kLitLog);
//   gPad->SetGridx(true);
//   gPad->SetGridy(true);
//
////   canvas1->cd(3);
////   DrawHist2D(fhStsMomresVsMom, "P [GeV/c]", "dP [%]", "Counter",
////         kLitLinearScale, kLitLinearScale, kLitLinearScale, "COLZ");
////   gPad->SetGridx(true);
////   gPad->SetGridy(true);
//
//   canvas1->cd(3);
//   fHM->H2F("hStsMomresVsMom")->FitSlicesY();
//   TH1* momslice = (TH1*) gDirectory->Get("hStsMomresVsMom_2");
//   momslice->GetXaxis()->SetTitle("P [GeV/c]");
//   momslice->GetYaxis()->SetTitle("dP/P, #sigma [%]");
//   DrawH1(momslice, kLitLinear, kLitLinear);
//   gPad->SetGridx(true);
//   gPad->SetGridy(true);
//   if (fOutputDir != "") lit::SaveCanvasAsImage(canvas1, fOutputDir);
//
//   canvas1->cd(4);
//   TH2F* hMomres = fHM->H2F("hStsMomresVsMom");
//   Int_t nBins = hMomres->GetNbinsX();
//   TH1D* momResRms = (TH1D*)hMomres->ProjectionX();
//   for (Int_t i = 1; i < nBins; i++){
//      TH1* projY = (TH1*)hMomres->ProjectionY("_py", i, i);
//      Double_t rms = projY->GetRMS();
//      momResRms->SetBinContent(i, rms);
//      momResRms->SetBinError(i, momslice->GetBinError(i));
//   }
//   momResRms->GetXaxis()->SetTitle("P [GeV/c]");
//   momResRms->GetYaxis()->SetTitle("dP/P, RMS [%]");
//   DrawH1(momResRms, kLitLinear, kLitLinear, "P");
//   gPad->SetGridx(true);
//   gPad->SetGridy(true);
//
//   TCanvas* canvas2 = new TCanvas("tracking_qa_track_length", "tracking_qa_track_length", 500, 500);
//   TH1* hLength = fHM->H1("hTrackLength");
//   hLength->Scale(1. / hLength->Integral());
//   DrawH1(hLength, kLitLinear, kLitLinear);
//   gPad->SetGridx(true);
//   gPad->SetGridy(true);
//}
//
//void CbmLitTrackingQaDraw::DrawMCMomVsAngle()
//{
//   if (!(fHM->Exists("hMCMomVsAngle_All") && fHM->Exists("hMCMomVsAngle_El"))) return;
//   TCanvas* canvas1 = new TCanvas("tracking_qa_mc_mom_vs_angle", "tracking_qa_mc_mom_vs_angle", 1200, 600);
//   canvas1->Divide(2,1);
//   canvas1->cd(1);
//   DrawH2(fHM->H2F("hMCMomVsAngle_All"), kLitLinear, kLitLinear, kLitLinear, "COLZ");
//   gPad->SetGridx(true);
//   gPad->SetGridy(true);
//
//   canvas1->cd(2);
//   DrawH2(fHM->H2F("hMCMomVsAngle_El"), kLitLinear, kLitLinear, kLitLinear, "COLZ");
//   gPad->SetGridx(true);
//   gPad->SetGridy(true);
//
//   if (fOutputDir != "") lit::SaveCanvasAsImage(canvas1, fOutputDir);
//}
