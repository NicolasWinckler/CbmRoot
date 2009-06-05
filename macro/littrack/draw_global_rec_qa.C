/** draw_global_rec_qa.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 3.0
 * Macro for drawing histograms produced by CbmLitReconstructionQa class.
 **/

TFile *file = new TFile("/home/d/andrey/test/trunk/global_mu/global.tracks.0000.root");

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
TH1F* histStsMom[NCATS][NTYPES];
TH1F* histHalfGlobalMom[NCATS][NTYPES];
TH1F* histGlobalMom[NCATS][NTYPES];
TH1F* histRecMom[NCATS][NTYPES];
TH1F* histTofMom[NCATS][NTYPES];

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

	std::string hname1("STS"), hname2("STS+MUCH"), hname3("STS+MUCH+TOF");

	c1->cd(1);
	draw_eff(histStsMom[ALL][EFF], histHalfGlobalMom[ALL][EFF], histGlobalMom[ALL][EFF], hname1, hname2, hname3);

	c1->cd(2);
	draw_eff(histStsMom[MU][EFF], histHalfGlobalMom[MU][EFF], histGlobalMom[MU][EFF], hname1, hname2, hname3);

	c1->cd(3);
	draw_eff(histRecMom[ALL][EFF], histRecMom[MU][EFF], NULL, "MUCH: all tracks", "MUCH: muon tracks", "");

	c1->cd(4);
	draw_eff(histTofMom[ALL][EFF], histTofMom[MU][EFF], NULL, "TOF: all tracks", "TOF: muon tracks", "");
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
