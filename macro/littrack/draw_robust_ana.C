TString dir = "/home/d/andrey/events/newmuch/standard/10mu/mu_urqmd/";
TFile *file = new TFile(dir + "much.robust.ana.root");

void draw_robust_ana()
{
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/draw_hist.C");

	gStyle->SetOptStat("");
	gStyle->SetOptFit(0);
	gStyle->SetOptTitle(0);
	gStyle->SetPalette(1, 0);

	DrawTrackParameters();
}

void DrawTrackParameters()
{
	TCanvas *c1 = new TCanvas("track parameters","track parameters", 1200, 1000);
	c1->Divide(3,2);

	c1->cd(1);
	hPlaneNofHits->GetXaxis()->SetTitle("plane id");
	hPlaneNofHits->GetYaxis()->SetTitle("nof hits");
	hPlaneNofHits->GetZaxis()->SetTitle("counter");
	gPad->SetLogz();
	hPlaneNofHits->Draw("LEGO");

	c1->cd(2);
	hLastPlaneIdNofHits->GetXaxis()->SetTitle("last plane id");
	hLastPlaneIdNofHits->GetYaxis()->SetTitle("nof hits");
	hLastPlaneIdNofHits->GetZaxis()->SetTitle("counter");
	gPad->SetLogz();
	hLastPlaneIdNofHits->Draw("LEGO");

	c1->cd(3);
	draw_hist_1D(hNofTrueHitsInTrack, "nof hits", "counter", kBlue, 2, 3, 1, kDot, false, true, "");
	draw_hist_1D(hNofHitsInTrack, "nof hits", "counter", kBlack, 2, 1, 1, kDot, false, true, "SAME");
	draw_hist_1D(hNofFalseHitsInTrack, "nof hits", "counter", kGreen, 2, 5, 1, kDot, false, true, "SAME");
	TLegend* l1 = new TLegend(0.5,0.7,0.9,0.9);
	l1->SetHeader("Number of hits");
	l1->AddEntry(hNofHitsInTrack,"total nof hits","lp");
	l1->AddEntry(hNofTrueHitsInTrack,"nof true hits","lp");
	l1->AddEntry(hNofFalseHitsInTrack,"nof false hits","lp");
	l1->Draw();

	c1->cd(4);
	draw_hist_1D(hLastPlaneIdInTrack, "last plane id", "counter", kBlack, 2, 1, 1, kDot, false, true, "");

	c1->cd(5);
	draw_hist_1D(hNofHitsInPlane, "nof hits in plane", "counter", kBlack, 2, 1, 1, kDot, false, true, "");

	c1->cd(6);
}

