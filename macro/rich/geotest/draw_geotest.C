/*  Description: This macro draws the histograms for easy and fast
 * test of the different RICH geometries.
 * Histograms are created using CbmRichGeoTest class.

    Author : Elena Belolaptikova
*/


void draw_geo_test() {

	gStyle->SetPalette(1,0);


	TFile* fcompact = new TFile ("/d/cbm02/slebedev/ebelolap/compact/reco.19.root");


	TCanvas* c1 = new TCanvas();
	c1->Divide(3,3);
	c1->cd(1);
	fhAaxisEl->Draw();

	c1->cd(2);
	fhBaxisEl->Draw();

	c1->cd(3);
	fhBoverAEl->Draw();

	c1->cd(4);
	fhBoverAXYEl->Divide(fhXYEl);
	fhBoverAXYEl->Draw("COLZ");

	c1->cd(5);
	fhXYEl->Draw("COLZ");

	c1->cd(6);
	fhNofHitsEl->Draw();

	c1->cd(7);
	fhBaxisVsMomEl->Draw("COLZ");

	c1->cd(8);
	fhAaxisVsMomEl->Draw("COLZ");

	c1->cd(9);
	fhNofPointsEl->Draw();

	TCanvas* c2 = new TCanvas();
	c2->Divide(3,3);
	c2->cd(1);
        fhNofHitsEvent->Draw();

	c2->cd(2);
	fhNofPointsEvent->Draw();

	c2->cd(3);
	fhXYHits->Draw("COLZ");

	c2->cd(4);
	fhXYPoints->Draw("COLZ");

	//TCanvas* c3 = new TCanvas();
	//c3->Divide(2,1);
	c2->cd(5);
	fhXYHitsOneEvent->Draw();

	c2->cd(6);
	fhXYPointsOneEvent->Draw();

	c2->cd(7);
	//TCanvas* c4 = new TCanvas();
	fhMCEl->Sumw2();
	fhAccEl->Sumw2();
	TH1D* hAccEff = new TH1D("hAccEff", "hAccEff", 30, 0, 15);
	hAccEff->Divide(fhAccEl, fhMCEl, 1,1, "B");
	hAccEff->SetLineWidth(3);
	hAccEff->Draw();
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	char txt[200];
	sprintf(txt, "Mean efficiency = %.2f", (Double_t)fhAccEl->GetEntries()/fhMCEl->GetEntries() * 100.);
	TText *text = new TText(4, 0.5, txt);
	text->Draw();
}
