TString dir  = "/home/d/andrey/events/newmuch/standard/10mu/mu_urqmd/";
TFile *file1 = new TFile(dir + "much.tracks.branch.0000.root");
TFile *file2 = new TFile(dir + "much.tracks.nn.0000.root");
TFile *file3 = new TFile(dir + "much.tracks.robust.0000.root");

Int_t lineWidth = 3;
Int_t markerSize = 3;

void draw_much_rec_qa()
{
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/draw_hist.C");

	gStyle->SetOptStat("");
	gStyle->SetOptFit(0);
	gStyle->SetOptTitle(0);

	draw_eff();
}

void draw_eff()
{
	TCanvas *c1 = new TCanvas("efficiency all","efficiency all",1200,1000);
	c1->SetGrid();
	TH1* allhist1 = (TH1*)file1->Get("hMomEffAll");
	TH1* allhist2 = (TH1*)file2->Get("hMomEffAll");
	TH1* allhist3 = (TH1*)file3->Get("hMomEffAll");
	draw_eff(allhist1, allhist2, allhist3);

	TCanvas *c2 = new TCanvas("efficiency muons","efficiency muons",1200,1000);
	c2->SetGrid();
	TH1* muhist1 = (TH1*)file1->Get("hMomEffMuons");
	TH1* muhist2 = (TH1*)file2->Get("hMomEffMuons");
	TH1* muhist3 = (TH1*)file3->Get("hMomEffMuons");
	draw_eff(muhist1, muhist2, muhist3);
}

void draw_eff(
		TH1* hist1,
		TH1* hist2,
		TH1* hist3)
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
		  kGreen+4, lineWidth, 1, markerSize, kOpenSquare, false, false, "SAME");

  TLegend* l1 = new TLegend(0.5,0.7,0.9,0.9);
  l1->SetHeader("Efficiency");
  l1->AddEntry(hist1,"branching","lp");
  l1->AddEntry(hist2,"nearest neighbour","lp");
  l1->AddEntry(hist3,"weighted","lp");
  l1->Draw();
}
