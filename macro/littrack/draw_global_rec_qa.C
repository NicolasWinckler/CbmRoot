TString dir  = "/home/d/andrey/test/trunk/global_mu_urqmd/";
TFile *file = new TFile(dir + "global.tracks.0000.root");

Int_t lineWidth = 2;
Int_t markerSize = 2;
Double_t labelSize = 0.055;

void draw_global_rec_qa()
{
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/draw_hist.C");

	gStyle->SetOptStat("");
	gStyle->SetOptFit(0);
	gStyle->SetOptTitle(0);

	draw_eff();
}

void draw_eff()
{
	TCanvas *c1 = new TCanvas("Global tracking efficiency","Global tracking efficiency", 1200, 1000);
	c1->Divide(2,2);
	c1->SetGrid();

	std::string cat[] = { "All", "Ref", "Prim", "Sec", "Muon", "Electron"};
	std::string type[] = { "Acc", "Rec", "Eff" };

	TH1F* hist1 = (TH1F*) file->Get("hStsMomEffAll");
	TH1F* hist2 = (TH1F*) file->Get("hHalfGlobalMomEffAll");
	TH1F* hist3 = (TH1F*) file->Get("hGlobalMomEffAll");
	c1->cd(1);
	draw_eff(hist1, hist2, hist3, "STS", "STS+MUCH", "STS+MUCH+TOF");

	TH1F* hist1 = (TH1F*) file->Get("hStsMomEffMuon");
	TH1F* hist2 = (TH1F*) file->Get("hHalfGlobalMomEffMuon");
	TH1F* hist3 = (TH1F*) file->Get("hGlobalMomEffMuon");
	c1->cd(2);
	draw_eff(hist1, hist2, hist3, "STS", "STS+MUCH", "STS+MUCH+TOF");

	TH1F* hist1 = (TH1F*) file->Get("hRecMomEffAll");
	TH1F* hist2 = (TH1F*) file->Get("hRecMomEffMuon");
	c1->cd(3);
	draw_eff(hist1, hist2, NULL, "MUCH: all tracks", "MUCH: muon tracks", "");

	TH1F* hist1 = (TH1F*) file->Get("hTofMomEffAll");
	TH1F* hist2 = (TH1F*) file->Get("hTofMomEffMuon");
	c1->cd(4);
	draw_eff(hist1, hist2, NULL, "TOF: all tracks", "TOF: muon tracks", "");
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
		draw_hist_1D(hist1, "Momentum, GeV/c", "Efficiency",
			  kBlue, lineWidth, 1, markerSize, kCircle, false, false, "");
		hist1->GetXaxis()->SetLabelSize(labelSize);
		hist1->GetYaxis()->SetLabelSize(labelSize);
	}

	if (hist2 != NULL) draw_hist_1D(hist2, "Momentum, GeV/c", "Efficiency",
		  kRed, lineWidth, 1, markerSize, kFullDotLarge, false, false, "SAME");

	if (hist3 != NULL) draw_hist_1D(hist3, "Momentum, GeV/c", "Efficiency",
		  kGreen + 3, lineWidth, 1, markerSize, kFullSquare, false, false, "SAME");

	TLegend* l1 = new TLegend(0.5,0.7,0.9,0.9);
	l1->SetHeader("Efficiency");
	if (hist1 != NULL) l1->AddEntry(hist1,hist1label.c_str(),"lp");
	if (hist2 != NULL) l1->AddEntry(hist2,hist2label.c_str(),"lp");
	if (hist3 != NULL) l1->AddEntry(hist3,hist3label.c_str(),"lp");
	l1->Draw();
}
