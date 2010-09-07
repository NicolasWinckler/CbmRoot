/*  Description: This macro draws the histograms which
    were created by CbmRichRingQa class.

    Author : Semen Lebedev
    E-mail : S.Lebedev@gsi.de
*/
#include "../../littrack/utils/CbmLitDrawHist.cxx"
#include "../../littrack/utils/CbmLitUtils.cxx"
TH1D* divide_hist(TH1D* h1, TH1D* h2, char* histTitle, Int_t nofBins, Double_t min, Double_t max)
{
    h1->SetBins(nofBins, min, max);
    h2->SetBins(nofBins, min, max);
	h1->Sumw2();
	h2->Sumw2();
	TH1D* h3 = new TH1D("h3",histTitle ,nofBins, min, max);
	h3->Divide(h1,h2,1,1,"B");
	return h3;
}

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
       // TFile *file = new TFile("/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.recorich.root");
        TFile *file = new TFile("/d/cbm02/slebedev/rich/JUL09/test_electrons/rich.reco.0000.root");
	TDirectory *current = gDirectory;
        current->cd("RichRingQaHist");

   // gROOT->SetStyle("Plain");
   // gStyle->SetPalette(1,0);

     //   gStyle->SetHistLineWidth(3);
     //   gROOT->ForceStyle();
	//gStyle->SetOptStat(0);
	//gStyle->SetOptFit(1);
	SetStyles();

    TCanvas *c1 = new TCanvas("c1","c1",400,600);
    c1->Divide(2,3);
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

    char effTxt[200];
	TCanvas *c2 = new TCanvas("c2","c2",500,500);
	TH1D* th1 = divide_hist(fh_RecElRingsMom, fh_AccElRingsMom,
			"Ring finding efficiency vs. momentum;momentum, GeV/c;efficiency, %", 30, 0, 15);
	th1->Draw();
	th1->SetMinimum(0.0001);
    sprintf(effTxt,"Mean eff. = (%.2f\%)", (Double_t)fh_RecElRingsMom->GetEntries()/
    		fh_AccElRingsMom->GetEntries()*100.);
    TText *t1 = new TText(3, 0.4, effTxt);
	t1->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);


	TCanvas *c3 = new TCanvas("c3","c3",500,500);
	TH1D* th2 = divide_hist(fh_RecElRingsRadPos, fh_AccElRingsRadPos,
			"Ring finding efficiency vs. radial position;radial position, cm;efficiency, %", 20, 0, 150);
	th2->Draw();
	th2->SetMinimum(0.0001);
    sprintf(effTxt,"Mean eff. = (%.2f\%)", (Double_t)fh_RecElRingsRadPos->GetEntries()/
    		fh_AccElRingsRadPos->GetEntries()*100.);
    TText *t2 = new TText(3, 0.4, effTxt);
    t2->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    TCanvas *c4 = new TCanvas("c4","c4",500,500);
	TH1D* th3 = divide_hist(fh_TrueMatchElMom, fh_RecElRingsMom,
			"Ring-track matching efficiency vs. momentum;momentum, GeV/c;efficiency, %", 30, 0, 15);
    th3->Draw();
    sprintf(effTxt,"Mean eff. = (%.2f\%)", (Double_t)fh_TrueMatchElMom->GetEntries()/
    		fh_RecElRingsMom->GetEntries()*100.);
    TText *t3 = new TText(3, 0.4, effTxt);
    t3->Draw();
	th3->SetMinimum(0.05);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    TCanvas *c5 = new TCanvas("c5","c5",500,500);
	TH1D* thNofHits = divide_hist(fh_RecElRingsNofHits, fh_AccElRingsNofHits,
			"Ring finding efficiency vs. nof hits;nof hits; efficiency, %", 20, 0, 40);
	thNofHits->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    TCanvas *c6 = new TCanvas("c6","c6",500,500);
	TH1D* thBoverA = divide_hist(fh_RecElRingsBoverA, fh_AccElRingsBoverA,
			"Ring finding efficiency vs. B/A;B/A; efficiency, %", 20, 0, 1);
	thBoverA->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    TCanvas *c7 = new TCanvas("c7","c7",500,500);
    fh_HitsXY->Draw("COLZ");


    cout << "Electron ring finding efficiency = " << (Double_t)fh_RecElRingsMom->GetEntries()/
                            (Double_t)fh_AccElRingsMom->GetEntries() << endl;

    cout << "Matching efficiency = " << (Double_t)fh_TrueMatchElMom->GetEntries()/
                                (Double_t)fh_RecElRingsMom->GetEntries() << endl;

    cout << "Nof Fakes = " << fh_FakeNofHits->GetEntries()<< endl;
}


