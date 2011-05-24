/** CbmRichRingQa.cxx
 * Description: This macro draws the histograms which were created by CbmRichRingQa class.
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2005
 * @version 3.0
 **/

void draw_diff_true_fake(TH1D* h1, TH1D* h2)
{
	if (h1->GetMaximum() < h2->GetMaximum()) h1->SetMaximum(h2->GetMaximum()*1.05);

	h1->SetLineWidth(2.);
	h2->SetLineWidth(2.);
	h1->SetLineStyle(9);
	h2->SetLineStyle(2);

	h1->Draw();
	h2->Draw("same");
   gPad->SetGridx(true);
   gPad->SetGridy(true);
}

void draw_qa(){
   TFile *file = new TFile("/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.recorich.root");
	TDirectory *current = gDirectory;
   current->cd("RichRingQaHist");

   // gROOT->SetStyle("Plain");
   // gStyle->SetPalette(1,0);

    gStyle->SetHistLineWidth(3);
     //   gROOT->ForceStyle();
	gStyle->SetOptStat(0);
	//gStyle->SetOptFit(1);

    TCanvas *c1 = new TCanvas("c1","c1",900,600);
    c1->Divide(3,2);
    c1->cd(1);
    draw_diff_true_fake(fh_TrueElNofHits, fh_FakeNofHits);

    c1->cd(2);
    draw_diff_true_fake(fh_TrueElAngle, fh_FakeAngle);

    c1->cd(3);
    draw_diff_true_fake(fh_TrueElNofHitsOnRing, fh_FakeNofHitsOnRing);

    c1->cd(4);
    draw_diff_true_fake(fh_TrueElRadPos, fh_FakeRadPos);

    c1->cd(5);
    draw_diff_true_fake(fh_TrueElChi2, fh_FakeChi2);

    c1->cd(6);
    draw_diff_true_fake(fh_TrueElRadius, fh_FakeRadius);
}


