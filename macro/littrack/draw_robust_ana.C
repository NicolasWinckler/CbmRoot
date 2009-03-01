TString dir = "/home/d/andrey/events/newmuch/standard/10mu/mu_urqmd/";
TFile *file = new TFile(dir + "much.robust.ana.root");

void draw_robust_ana()
{
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/draw_hist.C");

	gStyle->SetOptStat("");
	gStyle->SetOptFit(0);
	gStyle->SetOptTitle(0);
	gStyle->SetPalette(1, 0);

	DrawTrackParameters1D(0);
	DrawTrackParameters1D(1);
//	DrawTrackParameters2D(0);
//	DrawTrackParameters2D(1);
}

void DrawTrackParameters1D(
		Int_t id)
{
	std::stringstream cname;
	cname << "track parameters 1D " << id;
	TCanvas *c1 = new TCanvas(cname.str().c_str(),cname.str().c_str(), 1200, 1000);
	c1->Divide(3,2);

	std::string names[] = {
			"hNofHitsInPlane", "hTrueHitsRatioInTrack",
			"hNofHitsInTrack", "hNofTrueHitsInTrack", "hNofFalseHitsInTrack",
			"hNofHitsInTrueTrack", "hNofHitsInGhostTrack",
			"hTrackLastPlaneId", "hTrueTrackLastPlaneId", "hGhostTrackLastPlaneId",
			"hTrackChiSq", "hTrueTrackChiSq", "hGhostTrackChiSq"};

	Int_t nofPar = 13;
	TH1* hist[13];
	for (int i = 0; i < nofPar; i++) {
		stringstream ss;
		ss << names[i] << "_" << id;
		hist[i] = (TH1*) file->Get(ss.str().c_str());
	}

	c1->cd(1);
	draw_hist_1D(hist[2], "nof hits", "counter", kBlue, 2, 3, 1, kDot, false, true, "");
	draw_hist_1D(hist[3], "nof hits", "counter", kBlack, 2, 1, 1, kDot, false, true, "SAME");
	draw_hist_1D(hist[4], "nof hits", "counter", kGreen, 2, 5, 1, kDot, false, true, "SAME");
	TLegend* l1 = new TLegend(0.5,0.7,0.9,0.9);
	l1->SetHeader("Number of hits");
	l1->AddEntry(hist[2],"total nof hits","lp");
	l1->AddEntry(hist[3],"nof true hits","lp");
	l1->AddEntry(hist[4],"nof false hits","lp");
	l1->Draw();

	c1->cd(2);
//	draw_hist_1D(hist[2], "nof hits", "counter", kBlue, 2, 3, 1, kDot, false, true, "");
	draw_hist_1D(hist[5], "nof hits", "counter", kBlack, 2, 1, 1, kDot, false, true, "");
	draw_hist_1D(hist[6], "nof hits", "counter", kGreen, 2, 5, 1, kDot, false, true, "SAME");
	TLegend* l2 = new TLegend(0.5,0.7,0.9,0.9);
	l2->SetHeader("Number of hits");
//	l2->AddEntry(hist[2],"nof hits","lp");
	l2->AddEntry(hist[5],"nof hits in true track","lp");
	l2->AddEntry(hist[6],"nof hits in ghost track","lp");
	l2->Draw();

	c1->cd(3);
	draw_hist_1D(hist[7], "last plane id", "counter", kBlue, 2, 3, 1, kDot, false, true, "");
	draw_hist_1D(hist[8], "last plane id", "counter", kBlack, 2, 1, 1, kDot, false, true, "SAME");
	draw_hist_1D(hist[9], "last plane id", "counter", kGreen, 2, 5, 1, kDot, false, true, "SAME");
	TLegend* l3 = new TLegend(0.5,0.7,0.9,0.9);
	l3->SetHeader("last plane id of the track");
	l3->AddEntry(hist[7],"all tracks","lp");
	l3->AddEntry(hist[8],"true tracks","lp");
	l3->AddEntry(hist[9],"ghost tracks","lp");
	l3->Draw();

	c1->cd(4);
	draw_hist_1D(hist[10], "chi square", "counter", kBlue, 2, 3, 1, kDot, false, true, "");
	draw_hist_1D(hist[11], "chi square", "counter", kBlack, 2, 1, 1, kDot, false, true, "SAME");
	draw_hist_1D(hist[12], "chi square", "counter", kGreen, 2, 5, 1, kDot, false, true, "SAME");
	TLegend* l2 = new TLegend(0.5,0.7,0.9,0.9);
	l2->SetHeader("chi square of the track");
	l2->AddEntry(hist[10],"all tracks","lp");
	l2->AddEntry(hist[11],"true tracks","lp");
	l2->AddEntry(hist[12],"ghost tracks","lp");
	l2->Draw();

	c1->cd(5);
	draw_hist_1D(hist[0], "nof hits in plane", "counter", kBlack, 2, 1, 1, kDot, false, true, "");

	c1->cd(6);
	draw_hist_1D(hist[1], "true hits ratio in track", "counter", kBlack, 2, 1, 1, kDot, false, true, "");

}

void DrawTrackParameters2D(
		Int_t id)
{
	std::stringstream cname;
	cname << "track parameters 2D " << id;
	TCanvas *c1 = new TCanvas(cname.str().c_str(),cname.str().c_str(), 1200, 1000);
	c1->Divide(3,2);

	std::string names[] = { "hPlaneNofHits", "hLastPlaneIdNofHits"};
	Int_t nofPar = 2;
	TH1* hist[2];
	for (int i = 0; i < nofPar; i++) {
		stringstream ss;
		ss << names[i] << "_" << id;
		hist[i] = (TH1*) file->Get(ss.str().c_str());
	}

	c1->cd(1);
	draw_hist_2D((TH2*)hist[0], "plane id", "nof hits", "counter", false, false, true, "COLZ");

	c1->cd(2);
	draw_hist_2D((TH2*)hist[1], "last plane id", "nof hits", "counter", false, false, true, "COLZ");
}
