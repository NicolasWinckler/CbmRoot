TString dir1  = "/home/d/andrey/events/trd/monolithic/10e/e_urqmd/";
TFile *file1 = new TFile(dir1 + "trd.tracks.branch.0000.root");
TString dir2  = "/home/d/andrey/events/trd/segmented/10e/e_urqmd/";
TFile *file2 = new TFile(dir2 + "trd.tracks.branch.0000.root");

Int_t lineWidth = 3;
Int_t markerSize = 3;

void draw_trd_rec_qa()
{
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/draw_hist.C");

	gStyle->SetOptStat("");
	gStyle->SetOptFit(0);
	gStyle->SetOptTitle(0);

	draw_eff();
}

void draw_eff()
{
	TCanvas *c1 = new TCanvas("momentum efficiency","momentum efficiency",1200,1000);
	c1->SetGrid();
	TH1* momhist1 = (TH1*)file1->Get("hMomEffAll");
	TH1* momhist2 = (TH1*)file1->Get("hMomEffElectrons");
	TH1* momhist3 = (TH1*)file2->Get("hMomEffAll");
	TH1* momhist4 = (TH1*)file2->Get("hMomEffElectrons");
	draw_eff(momhist1, momhist2, momhist3, momhist4);

	TCanvas *c2 = new TCanvas("np efficiency","np efficiency",1200,1000);
	c2->SetGrid();
	TH1* nphist1 = (TH1*)file1->Get("hNpEffAll");
	TH1* nphist2 = (TH1*)file1->Get("hNpEffElectron");
	TH1* nphist3 = (TH1*)file2->Get("hNpEffAll");
	TH1* nphist4 = (TH1*)file2->Get("hNpEffElectron");
	draw_eff(nphist1, nphist2, nphist3, nphist4);
}

void draw_eff(
		TH1* hist1,
		TH1* hist2,
		TH1* hist3,
		TH1* hist4)
{
  hist1->SetMaximum(1);
  hist1->SetMinimum(0);
  draw_hist_1D(hist1, "Momentum, GeV/c", "Efficiency",
		  kBlue, lineWidth, 1, markerSize, kCircle, false, false, "");
  hist1->GetXaxis()->SetLabelSize(0.055);
  hist1->GetYaxis()->SetLabelSize(0.055);

  draw_hist_1D(hist2, "Momentum, GeV/c", "Efficiency",
  		  kRed, lineWidth, 1, markerSize, kFullCircle, false, false, "SAME");

  draw_hist_1D(hist3, "Momentum, GeV/c", "Efficiency",
  		  kOrange+2, lineWidth, 1, markerSize, kOpenSquare, false, false, "SAME");
  draw_hist_1D(hist4, "Momentum, GeV/c", "Efficiency",
  		  kGreen+4, lineWidth, 1, markerSize, kFullSquare, false, false, "SAME");

  TLegend* l1 = new TLegend(0.5,0.7,0.9,0.9);
  l1->SetHeader("Efficiency");
  l1->AddEntry(hist1,"all tracks (monolithic TRD)","lp");
  l1->AddEntry(hist2,"electron tracks (monolithic TRD)","lp");
  l1->AddEntry(hist3,"all tracks (segmented TRD)","lp");
  l1->AddEntry(hist4,"electron tracks (segmented TRD)","lp");
  l1->Draw();
}
