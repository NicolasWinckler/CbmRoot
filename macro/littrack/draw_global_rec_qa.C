/** draw_global_rec_qa.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 3.0
 * Macro draws histograms produced by CbmLitReconstructionQa class.
 **/
#include <string>
#include <sstream>
#include <ostream>
#include <iomanip>

//Input directory.
const TString dir = "/home/d/andrey/test/trunk/global_e_urqmd/";
//Input reconstruction file.
const TFile *file = new TFile(dir + "global.tracks.branch.0000.root");

// Input MC file. Used to automotize some text drawings.
const TString mcFile = dir + "mc.0000.root";

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

void draw_global_rec_qa()
{
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/draw_hist.C");
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/style.C");
	style();

	gStyle->SetOptStat("");
	gStyle->SetOptFit(0);
	gStyle->SetOptTitle(0);

	read_histos();

	draw_eff();
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
}

void draw_eff()
{
	TCanvas *c1 = new TCanvas("Global tracking efficiency","Global tracking efficiency", 1200, 1000);
	c1->Divide(2,2);
	c1->SetGrid();

	std::string sname("STS");
	std::string rname;
	if (IsMuch(mcFile) && !IsTrd(mcFile)) rname = "MUCH";
	else if (IsTrd(mcFile) && !IsMuch(mcFile)) rname = "TRD";
	else if (IsTrd(mcFile) && IsMuch(mcFile)) rname = "MUCH+TRD";
	std::string hgname(sname + "+" + rname);
	std::string gname = hgname += "+TOF";

	std::string signal;
	if (IsMuch(mcFile)) signal = "muons"; else signal = "electrons";

	std::string hname1(sname), hname2(hgname), hname3(gname);
	hname1 += "(" + eff(histStsMom[ALL][REC], histStsMom[ALL][ACC]) + ")";
	hname2 += "(" + eff(histHalfGlobalMom[ALL][REC], histHalfGlobalMom[ALL][ACC]) + ")";
	hname3 += "(" + eff(histGlobalMom[ALL][REC], histGlobalMom[ALL][ACC]) + ")";
	c1->cd(1);
	draw_eff(histStsMom[ALL][EFF], histHalfGlobalMom[ALL][EFF], histGlobalMom[ALL][EFF], hname1, hname2, hname3);

	Int_t cat;
	if (IsMuch(mcFile)) cat = MU; else cat = EL;
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
		draw_hist_1D(hist1, "Momentum [GeV/c]", "Efficiency",
			  kBlue, lineWidth, 1, markerSize, kCircle, false, false, "");
		hist1->GetXaxis()->SetLabelSize(labelSize);
		hist1->GetYaxis()->SetLabelSize(labelSize);
	}

	if (hist2 != NULL) draw_hist_1D(hist2, "Momentum [GeV/c]", "Efficiency",
		  kRed, lineWidth, 1, markerSize, kStar, false, false, "SAME");

	if (hist3 != NULL) draw_hist_1D(hist3, "Momentum [GeV/c]", "Efficiency",
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
	Double_t eff = histRec->GetEntries() / histAcc->GetEntries();
	std::stringstream ss;
	ss.precision(3);
	ss << eff;
	return ss.str();
}
