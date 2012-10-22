/**
 * \file CbmLitTrackingQaDraw.cxx
 * \brief Draw histograms for tracking QA.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitTrackingQaDraw.h"
#include "CbmLitTrackingQaHistCreator.h"
#include "CbmHistManager.h"

#include "TCanvas.h"
#include "TLine.h"
#include "TDirectory.h"

#include "CbmDrawHist.h"
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
      CbmHistManager* histManager,
      const std::string& outputDir)
{
   assert(histManager != NULL);
   fHM = histManager;
   fOutputDir = outputDir;

   SetDefaultDrawStyle();
   DrawEfficiencyHistos();
   DrawYPtHistos();
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

	// Draw local accapted and reconstructed tracks vs number of points
	fHM->ShrinkEmptyBinsByPattern("hte_.+_.+_.+_.+_Np");
	vector<string> accRecTracks = list_of("Sts")("Trd")("Much")("Tof");
	for (UInt_t i = 0; i < accRecTracks.size(); i++) {
		string variant = accRecTracks[i];
		string re = (variant == "Sts") ? "hte_Sts_Sts_(All|Muon|Electron)_(Acc|Rec)_Np" : "hte_" + variant + "_.*_(All|Muon|Electron)_(Acc|Rec)_Np";
		DrawAccAndRec("tracking_qa_local_acc_and_rec_" + variant + "_Np", re);
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

	DrawH1(histos, labels, kLinear, kLinear, true, 0.3, 0.3, 0.85, 0.6, "PE1");
	DrawMeanEfficiencyLines(histos, efficiencies);
	lit::SaveCanvasAsImage(canvas, fOutputDir);
}

void CbmLitTrackingQaDraw::DrawYPtHistos()
{
   // Draw global tracking efficiency
   vector<string> globalTrackVariants = CbmLitTrackingQaHistCreator::GlobalTrackVariants();
   for (UInt_t i = 0; i < globalTrackVariants.size(); i++) {
      string variant = globalTrackVariants[i];
      DrawYPt("tracking_qa_" + variant + "_ypt", "hte_" + variant + "_" + variant + "_All_Eff_YPt");
   }
}

void CbmLitTrackingQaDraw::DrawYPt(
      const string& canvasName,
      const string& effHistName)
{
   string accHistName = FindAndReplace(effHistName, "_Eff_", "_Acc_");
   string recHistName = FindAndReplace(effHistName, "_Eff_", "_Rec_");

   if (!(fHM->Exists(effHistName) && fHM->Exists(accHistName) && fHM->Exists(recHistName))) return;

   TCanvas* canvas = new TCanvas(canvasName.c_str(), canvasName.c_str(), 1800, 600);
   canvas->Divide(3, 1);
   canvas->SetGrid();

   TH2* accHist = (TH2*)fHM->H1(accHistName);
   TH2* recHist = (TH2*)fHM->H1(recHistName);
   TH2* effHist = (TH2*)fHM->H1(effHistName);

   canvas->cd(1);
   DrawH2(accHist);

   canvas->cd(2);
   DrawH2(recHist);

   canvas->cd(3);
   DrawH2(effHist);

   lit::SaveCanvasAsImage(canvas, fOutputDir);
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

void CbmLitTrackingQaDraw::DrawAccAndRec(
      const string& canvasName,
      const string& histNamePattern)
{
	vector<TH1*> histos = fHM->H1Vector(histNamePattern);
	if (histos.size() == 0) return;

	TCanvas* canvas = new TCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
	canvas->SetGrid();
	canvas->cd();

	Int_t nofEvents = fHM->H1("hen_EventNo_TrackingQa")->GetEntries();
	Int_t nofHistos = histos.size();
	vector<string> labels(nofHistos);
	for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
		TH1* hist = histos[iHist];
		hist->Scale(1./nofEvents);
		string name = hist->GetName();
		vector<string> split = Split(name, '_');
		labels[iHist] = split[4] + ":" + split[3] + "(" + NumberToString<Double_t>(hist->GetEntries() / nofEvents, 1) + ")";
	}

	DrawH1(histos, labels, kLinear, kLog, true, 0.2, 0.75, 0.5, 0.99);
	lit::SaveCanvasAsImage(canvas, fOutputDir);
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
   fHM->ShrinkEmptyBinsByPattern("hth_.*(_All|_True|_Fake)");
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
              kLinear, kLog, true, 0.25, 0.99, 0.55, 0.75);

   canvas->cd(2);
   TH1* hTrueOverAll = fHM->H1(hist + "_TrueOverAll");
   TH1* hFakeOverAll = fHM->H1(hist + "_FakeOverAll");
   DrawH1(list_of(hTrueOverAll)(hFakeOverAll),
      list_of("true/all: " + NumberToString<Double_t>(hTrueOverAll->GetMean()))
             ("fake/all: " + NumberToString<Double_t>(hFakeOverAll->GetMean())),
             kLinear, kLog, true, 0.25, 0.99, 0.55, 0.75);

   lit::SaveCanvasAsImage(canvas, fOutputDir);
}

//void CbmLitTrackingQaDraw::DrawStsTracksQaHistos()
//{
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
