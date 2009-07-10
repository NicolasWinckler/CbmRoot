const TString dir  = "/home/d/andrey/test/trunk/global_mu_urqmd/";
const TFile *file1 = new TFile(dir + "global.tracks.branch.simple.0000.root");
const TFile *file2 = new TFile(dir + "global.tracks.nn.simple.0000.root");
const TFile *file3 = new TFile(dir + "global.tracks.weight.simple.0000.root");

const Int_t lineWidth = 2;
const Int_t markerSize = 1;
const Double_t labelSize = 0.055;

void draw_much_rec_qa()
{
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/draw_hist.C");
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/style.C");
	style();

	gStyle->SetOptStat("");
	gStyle->SetOptFit(0);
	gStyle->SetOptTitle(0);

	draw_eff();
}

void draw_eff()
{
	TCanvas *c2 = new TCanvas("efficiency muons","efficiency muons",1200,1000);
	c2->SetGrid();
	TH1* muhisteff1 = (TH1*)file1->Get("hRecMomEffMuon");
	TH1* muhisteff2 = (TH1*)file2->Get("hRecMomEffMuon");
	TH1* muhisteff3 = (TH1*)file3->Get("hRecMomEffMuon");

	TH1* muhistacc1 = (TH1*)file1->Get("hRecMomAccMuon");
	TH1* muhistacc2 = (TH1*)file2->Get("hRecMomAccMuon");
	TH1* muhistacc3 = (TH1*)file3->Get("hRecMomAccMuon");

	TH1* muhistrec1 = (TH1*)file1->Get("hRecMomRecMuon");
	TH1* muhistrec2 = (TH1*)file2->Get("hRecMomRecMuon");
	TH1* muhistrec3 = (TH1*)file3->Get("hRecMomRecMuon");


	std::string hname1("branching (" + eff(muhistrec1, muhistacc1)+ ")");
	std::string hname2("nearest neighbor (" + eff(muhistrec2, muhistacc2) + ")");
	std::string hname3("weighting (" + eff(muhistrec3, muhistacc3) + ")");
	draw_eff(muhisteff1, muhisteff2, muhisteff3, hname1, hname2, hname3);
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
