/** draw_global_rec_qa.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 3.0
 * Macro draws histograms produced by CbmLitReconstructionQa class.
 **/
#include "../../littrack/utils/CbmLitDrawHist.h"

#include <string>
#include <sstream>
#include <ostream>
#include <iomanip>

//Input directory.
const TString dir = "/d/cbm02/andrey/stdtrd_10pi/";
//Input reconstruction file.
const TFile *file = new TFile(dir + "global.tracks.0000.root");

// Input MC file. Used to automotize some text drawings.
const TString parFile = dir + "param.0000.root";

//Some drawing settings.
const Int_t lineWidth = 2;
const Int_t markerSize = 1;
const Double_t labelSize = 0.055;

// histogram types
const Int_t ACC=0; // accepted tracks histogram
const Int_t REC=1; // reconstructed tracks histogram
const Int_t EFF=2; // efficiency histogram
// track categories
const Int_t ALL=0; // all tracks
const Int_t REF=1; // reference tracks
const Int_t PRIM=2; // primary tracks
const Int_t SEC=3; // secondary tracks
const Int_t MU=4; // primary muon tracks
const Int_t EL=5; // primary electron tracks

const Int_t NTYPES = 3; // ACC, REC, EFF
const Int_t NCATS = 6; // ALL, REF, PRIM, SEC, MU, EL

//histograms
TH1F* histStsMom[NCATS][NTYPES]; // STS tracking efficiency in  dependence on momentum
TH1F* histHalfGlobalMom[NCATS][NTYPES]; // STS+TRD(MUCH) tracking efficiency in  dependence on momentum
TH1F* histGlobalMom[NCATS][NTYPES]; // STS+TRD(MUCH)+TOF tracking efficiency in  dependence on momentum
TH1F* histRecMom[NCATS][NTYPES]; // TRD(MUCH) tracking efficiency in dependence on momentum
TH1F* histTofMom[NCATS][NTYPES]; // TOF hit to track matching efficiency in dependence on momentum

TH1F* histStsTrackHits[5];
TH1F* histTrdTrackHits[5];
TH1F* histMuchTrackHits[5];

void draw_global_rec_qa()
{
	gSystem->Load("/home/soft/tbb22_004oss/libtbb");

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/style.C");
	style();

	gStyle->SetOptStat("");
	gStyle->SetOptFit(0);
	gStyle->SetOptTitle(0);

	read_histos();

	draw_eff();
	draw_hits();
}

void read_histos()
{
	std::string cat[] = { "All", "Ref", "Prim", "Sec", "Muon", "Electron"};
	std::string type[] = { "Acc", "Rec", "Eff" };
	for (Int_t i = 0; i < NCATS; i++) {
		for (Int_t j = 0; j < NTYPES; j++) {
			histStsMom[i][j] = (TH1F*) file->Get(std::string("hStsMom" + type[j] + cat[i]).c_str());
			histHalfGlobalMom[i][j] = (TH1F*) file->Get(std::string("hHalfGlobalMom" + type[j] + cat[i]).c_str());
			histGlobalMom[i][j] = (TH1F*) file->Get(std::string("hGlobalMom" + type[j] + cat[i]).c_str());
			histRecMom[i][j] = (TH1F*) file->Get(std::string("hRecMom" + type[j] + cat[i]).c_str());
			histTofMom[i][j] = (TH1F*) file->Get(std::string("hTofMom" + type[j] + cat[i]).c_str());
		}
	}

	std::string hittype[] = { "All", "True", "Fake", "TrueOverAll", "FakeOverAll" };
	for (Int_t i = 0; i < 5; i++) {
		histStsTrackHits[i] = (TH1F*) file->Get(std::string("hStsTrackHits" + hittype[i]).c_str());
		histTrdTrackHits[i] = (TH1F*) file->Get(std::string("hTrdTrackHits" + hittype[i]).c_str());
		histMuchTrackHits[i] = (TH1F*) file->Get(std::string("hMuchTrackHits" + hittype[i]).c_str());
	}
}

void draw_eff()
{
	TCanvas *c1 = new TCanvas("global_tracking_efficiency","global_tracking_efficiency", 1200, 1000);
	c1->Divide(2,2);
	c1->SetGrid();

	std::string sname("STS");
	std::string rname;
	if (IsMuch(parFile) && !IsTrd(parFile)) rname = "MUCH";
	else if (IsTrd(parFile) && !IsMuch(parFile)) rname = "TRD";
	else if (IsTrd(parFile) && IsMuch(parFile)) rname = "MUCH+TRD";
	std::string hgname(sname + "+" + rname);
	std::string gname = hgname += "+TOF";

	std::string signal;
	if (IsMuch(parFile)) signal = "muons"; else signal = "electrons";

	std::string hname1(sname), hname2(hgname), hname3(gname);
	hname1 += "(" + eff(histStsMom[ALL][REC], histStsMom[ALL][ACC]) + ")";
	hname2 += "(" + eff(histHalfGlobalMom[ALL][REC], histHalfGlobalMom[ALL][ACC]) + ")";
	hname3 += "(" + eff(histGlobalMom[ALL][REC], histGlobalMom[ALL][ACC]) + ")";
	c1->cd(1);
	draw_eff(histStsMom[ALL][EFF], histHalfGlobalMom[ALL][EFF], histGlobalMom[ALL][EFF], hname1, hname2, hname3);

	Int_t cat;
	if (IsMuch(parFile)) cat = MU; else cat = EL;
	std::string hname1(sname), hname2(hgname), hname3(gname);
	hname1 += "(" + eff(histStsMom[cat][REC], histStsMom[cat][ACC])+ ")";
	hname2 += "(" + eff(histHalfGlobalMom[cat][REC], histHalfGlobalMom[cat][ACC]) + ")";
	hname3 += "(" + eff(histGlobalMom[cat][REC], histGlobalMom[cat][ACC]) + ")";
	c1->cd(2);
	draw_eff(histStsMom[cat][EFF], histHalfGlobalMom[cat][EFF], histGlobalMom[cat][EFF], hname1, hname2, hname3);


	std::string hname1(rname + ": all"), hname2(rname + ": " + signal);
	c1->cd(3);
	hname1 += "(" + eff(histRecMom[ALL][REC], histRecMom[ALL][ACC])+ ")";
	hname2 += "(" + eff(histRecMom[cat][REC], histRecMom[cat][ACC]) + ")";
	draw_eff(histRecMom[ALL][EFF], histRecMom[cat][EFF], NULL, hname1, hname2, "");

	std::string hname1("TOF: all"), hname2("TOF: " + signal);
	hname1 += "(" + eff(histTofMom[ALL][REC], histTofMom[ALL][ACC])+ ")";
	hname2 += "(" + eff(histTofMom[cat][REC], histTofMom[cat][ACC]) + ")";
	c1->cd(4);
	draw_eff(histTofMom[ALL][EFF], histTofMom[cat][EFF], NULL, hname1, hname2, "");

	stringstream oss1;
	oss1 << c1->GetTitle() << ".eps";
	c1->SaveAs(TString(oss1.str().c_str()));

	stringstream oss3;
	oss3 << c1->GetTitle() << ".gif";
	c1->SaveAs(TString(oss3.str().c_str()));
}

void draw_eff(
		TH1* hist1,
		TH1* hist2,
		TH1* hist3,
		const std::string& hist1label,
		const std::string& hist2label,
		const std::string& hist3label)
{
	if (hist1 != NULL) {
		hist1->SetMaximum(1);
		hist1->SetMinimum(0);
		hist1->GetXaxis()->SetRange(0, 28);
		DrawHist1D(hist1, "Momentum [GeV/c]", "Efficiency",
			  kBlue, lineWidth, 1, markerSize, kCircle, false, false, "");
	}

	if (hist2 != NULL) DrawHist1D(hist2, "Momentum [GeV/c]", "Efficiency",
		  kRed, lineWidth, 1, markerSize, kStar, false, false, "SAME");

	if (hist3 != NULL) DrawHist1D(hist3, "Momentum [GeV/c]", "Efficiency",
		  kGreen + 3, lineWidth, 1, markerSize, kFullSquare, false, false, "SAME");

	TLegend* l1 = new TLegend(0.3,0.3,0.85,0.6);
	l1->SetFillColor(kWhite);
	l1->SetHeader("Efficiency");
	if (hist1 != NULL) l1->AddEntry(hist1,hist1label.c_str(),"lp");
	if (hist2 != NULL) l1->AddEntry(hist2,hist2label.c_str(),"lp");
	if (hist3 != NULL) l1->AddEntry(hist3,hist3label.c_str(),"lp");
	l1->Draw();
}

std::string eff(
		TH1* histRec,
		TH1* histAcc)
{
	Double_t eff;
	if (histAcc->GetEntries() == 0) eff = 0.;
	else eff = Double_t(histRec->GetEntries()) / Double_t(histAcc->GetEntries());
	std::stringstream ss;
	ss.precision(3);
	ss << eff;
	return ss.str();
}

std::string num_to_string(Double_t num)
{
	std::stringstream ss;
	ss.precision(5);
	ss << num;
	return ss.str();
}

void draw_hits()
{
	TCanvas *c_sts_hits = new TCanvas("c_sts_hits","c_sts_hits", 1200, 600);
	c_sts_hits->Divide(2,1);
	c_sts_hits->SetGrid();
	draw_hits(c_sts_hits, histStsTrackHits);

	if (IsTrd(parFile)){
	   TCanvas *c_trd_hits = new TCanvas("c_trd_hits","c_trd_hits", 1200, 600);
	   c_trd_hits->Divide(2,1);
	   c_trd_hits->SetGrid();
       draw_hits(c_trd_hits, histTrdTrackHits);
   }

   if (IsMuch(parFile)) {
	   TCanvas *c_much_hits = new TCanvas("c_much_hits","c_much_hits", 1200, 600);
	   c_much_hits->Divide(2,1);
	   c_much_hits->SetGrid();
       draw_hits(c_much_hits, histMuchTrackHits);
   }
}

void draw_hits(
		TCanvas* c,
		TH1F** hists)
{
   c->cd(1);
   DrawHist1D(hists[0], "Number of hits", "Counter",
   			  kBlue, lineWidth, 1, markerSize, kCircle, false, true, "");
   DrawHist1D(hists[1], "Number of hits", "Counter",
		      kRed, lineWidth, 1, markerSize, kStar, false, true, "SAME");
   DrawHist1D(hists[2], "Number of hits", "Counter",
		      kGreen + 3, lineWidth, 1, markerSize, kFullSquare, false, true, "SAME");

   TLegend* l1 = new TLegend(0.25,0.99,0.55,0.75);
   l1->SetFillColor(kWhite);
   l1->SetHeader(std::string("Nof hits " + num_to_string(hists[0]->GetEntries())).c_str());
   l1->AddEntry(hists[0], std::string("all: " + num_to_string(hists[0]->GetMean())).c_str(), "lp");
   l1->AddEntry(hists[1], std::string("true: " + num_to_string(hists[1]->GetMean())).c_str(), "lp");
   l1->AddEntry(hists[2], std::string("fake: " + num_to_string(hists[2]->GetMean())).c_str(), "lp");
   l1->Draw();


   c->cd(2);
   DrawHist1D(hists[3], "Ratio", "Counter",
   			  kBlue, lineWidth, 1, markerSize, kCircle, false, true, "");
   DrawHist1D(hists[4], "Ratio", "Counter",
		      kRed, lineWidth, 1, markerSize, kStar, false, true, "SAME");
   TLegend* l2 = new TLegend(0.25,0.99,0.55,0.75);
   l2->SetFillColor(kWhite);
   l2->SetHeader(std::string("Ratio " + num_to_string(hists[0]->GetEntries())).c_str());
   l2->AddEntry(hists[3], std::string("true/all: " + num_to_string(hists[3]->GetMean())).c_str(), "lp");
   l2->AddEntry(hists[4], std::string("fake/all: " + num_to_string(hists[4]->GetMean())).c_str(), "lp");
   l2->Draw();

	stringstream oss1;
	oss1 << c->GetTitle() << ".eps";
	c->SaveAs(TString(oss1.str().c_str()));

	stringstream oss3;
	oss3 << c->GetTitle() << ".gif";
	c->SaveAs(TString(oss3.str().c_str()));
}

