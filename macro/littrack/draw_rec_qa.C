#include "../../littrack/utils/CbmLitDrawHist.h"
#include "../../littrack/utils/CbmLitDrawHist.cxx"
#include "../../littrack/utils/CbmLitUtils.cxx"

const TString dir  = "/home/d/andrey/phd/much1_10mu_urqmd/";
const TFile *file1 = new TFile(dir + "global.tracks.branch.0000.root");
const TFile *file2 = new TFile(dir + "global.tracks.nn.0000.root");
const TFile *file3 = new TFile(dir + "global.tracks.weight.0000.root");

void draw_rec_qa()
{
	SetStyles();

	DrawEffMethods();
}

void DrawEffMethods()
{
	TCanvas *canvas = new TCanvas("rec_qa_eff_methods_signal","rec_qa_eff_methods_signal",1200,1000);
	canvas->SetGrid();
	TH1* eff1 = (TH1*)file1->Get("hRecMomEffMuon");
	TH1* eff2 = (TH1*)file2->Get("hRecMomEffMuon");
	TH1* eff3 = (TH1*)file3->Get("hRecMomEffMuon");

	TH1* acc1 = (TH1*)file1->Get("hRecMomAccMuon");
	TH1* acc2 = (TH1*)file2->Get("hRecMomAccMuon");
	TH1* acc3 = (TH1*)file3->Get("hRecMomAccMuon");

	TH1* rec1 = (TH1*)file1->Get("hRecMomRecMuon");
	TH1* rec2 = (TH1*)file2->Get("hRecMomRecMuon");
	TH1* rec3 = (TH1*)file3->Get("hRecMomRecMuon");

	std::string hname1, hname2, hname3;
	hname1 = "branch (" + CalcEfficiency(rec1, acc1) + ")";
	hname2 = "nn (" + CalcEfficiency(rec2, acc2) + ")";
	hname3 = "weight (" + CalcEfficiency(rec3, acc3) + ")";

	canvas->cd();
	eff1->SetMinimum(0.);
	eff1->SetMaximum(1.);
	DrawHist1D(eff1, eff2, eff3,
			"Efficiency", "Momentum [GeV/c]", "Efficiency", hname1, hname2, hname3,
			false, false, true, 0.3,0.3,0.85,0.6);
	SaveCanvasAsImage(canvas, "./test/");
}

std::string CalcEfficiency(
		TH1* histRec,
		TH1* histAcc)
{
	if (histAcc->GetEntries() == 0) return "0.0";
	else {
		Double_t eff = Double_t(histRec->GetEntries()) / Double_t(histAcc->GetEntries());
		std::stringstream ss;
		ss.precision(3);
		ss << eff;
		return ss.str();
	}
}

