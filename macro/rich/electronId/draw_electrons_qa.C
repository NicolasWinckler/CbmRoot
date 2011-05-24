/** draw_electrons_qa.C
 * Description: This macro draw the histograms which were create with CbmRichRingQa class.
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2008
 * @version 2.0
 **/

void draw_diff_el_pi(TH1D* h1, TH1D* h2)
{
	if (h1->GetMaximum() < h2->GetMaximum()) h1->SetMaximum(h2->GetMaximum()*1.05);

	h1->SetLineWidth(2.);
	h2->SetLineWidth(2.);
	h2->SetLineStyle(7);

	h1->Draw();
	h2->Draw("same");
	gPad->SetGridx(true);
	gPad->SetGridy(true);
}

void draw_electrons_qa()
{
	TFile *file = new TFile("/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.reco.root");
   gROOT->SetStyle("Plain");
   gStyle->SetPalette(1,0);
   gStyle->SetOptStat(0000);
	gStyle->SetHistLineWidth(3);
	gROOT->ForceStyle();

   TCanvas* c1 = new TCanvas("c1", "c1", 1000, 500);
	c1->Divide(4, 2);
   c1->cd(1);
   draw_diff_el_pi(fhAaxisEl, fhAaxisPi);
   c1->cd(2);
   draw_diff_el_pi(fhBAxisEl, fhBAxisPi);
	c1->cd(3);
	draw_diff_el_pi(fhAaxisCorEl, fhAaxisCorPi);
	c1->cd(4);
	draw_diff_el_pi(fhBAxisCorEl, fhBAxisCorPi);
	c1->cd(5);
	draw_diff_el_pi(fhDistEl, fhDistPi);
   c1->cd(6);
   draw_diff_el_pi(fhNofHitsEl, fhNofHitsPi);
   c1->cd(7);
   draw_diff_el_pi(fhChi2El, fhChi2Pi);
   c1->cd(8);
   draw_diff_el_pi(fhRadPosEl, fhRadPosPi);

	TCanvas* c2 = new TCanvas("c2", "c2", 900, 600);
	c2->Divide(3, 2);
	c2->cd(1);
	fhAaxisVsMomEl->Draw("COLZ");
	c2->cd(2);
	fhAaxisVsMomPi->Draw("COLZ");
	c2->cd(3);
	fhBAxisVsMomEl->Draw("COLZ");
	c2->cd(4);
	fhBAxisVsMomPi->Draw("COLZ");
	c2->cd(5);
	fhPhiVsRadAngEl->Draw("COLZ");
	c2->cd(6);
	fhPhiVsRadAngPi->Draw("COLZ");
}

