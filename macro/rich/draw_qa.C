/*  Description: This macro draws the histograms which
    were created by CbmRichRingQa class.

    Author : Semen Lebedev
    E-mail : S.Lebedev@gsi.de
*/

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

void draw_diff_true_fake(TH1D* h1, TH1D* h2, char* xTitle)
{
	if (h1->GetMaximum() < h2->GetMaximum()) h1->SetMaximum(h2->GetMaximum()*1.05);

	h1->SetLineWidth(2.);
	h2->SetLineWidth(2.);
	h1->SetLineStyle(9);
	h2->SetLineStyle(2);
    h1->GetXaxis()->SetTitle(xTitle);
    h1->GetYaxis()->SetTitle("Entries");

	h1->Draw();
	h2->Draw("same");
}

void draw_qa(){
	TFile *file = new TFile("/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.recorich.root");

	TDirectory *current = gDirectory;
	current->cd("RichRingQaHist");
   // rich->cd();
    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1,0);

	gStyle->SetHistLineWidth(3);
	gROOT->ForceStyle();
	//gStyle->SetOptStat(0);
	//gStyle->SetOptFit(1);

    TCanvas *c1 = new TCanvas("CbmRichQa1","c1",1200,1000);
    c1->Divide(4,4);
    c1->cd(1);
    draw_diff_true_fake(fh_TrueElNofHits, fh_FakeNofHits, "Number of Hits");

    c1->cd(2);
    draw_diff_true_fake(fh_TrueElDistance, fh_FakeDistance, "ring-track distance, [cm]");

    c1->cd(3);
    draw_diff_true_fake(fh_TrueElAngle, fh_FakeAngle, "Angle, [rad]");

    c1->cd(4);
    draw_diff_true_fake(fh_TrueElNofHitsOnRing, fh_FakeNofHitsOnRing,"# hits on ring");

    c1->cd(5);
    draw_diff_true_fake(fh_TrueElRadPos, fh_FakeRadPos, "Radial posotion, [cm]");

    c1->cd(6);
    draw_diff_true_fake(fh_TrueElChi2, fh_FakeChi2, "chi2");

    c1->cd(7);
    draw_diff_true_fake(fh_TrueElRadius, fh_FakeRadius, "radius, [cm]");

    c1->cd(8);
    draw_diff_true_fake(fh_TrueElA, fh_FakeA, "A axis, [cm]");

    c1->cd(9);
    draw_diff_true_fake(fh_TrueElB, fh_FakeB, "B axis, [cm]");

    c1->cd(10);
    draw_diff_true_fake(fh_TrueElPhi, fh_FakePhi, "Entries");

    c1->cd(11);
    draw_diff_true_fake(fh_TrueElStsMom, fh_FakeStsMom, "mom, [Gev/c]");

    c1->cd(12);
    fh_TrueElPhiVsRadAngle->GetXaxis()->SetTitle("phi, [rad]");
    fh_TrueElPhiVsRadAngle->GetYaxis()->SetTitle("rad angle, [rad]");
    fh_TrueElPhiVsRadAngle->Draw("COLZ");

    c1->cd(13);
    fh_FakePhiVsRadAngle->GetXaxis()->SetTitle("phi, [rad]");
    fh_FakePhiVsRadAngle->GetYaxis()->SetTitle("rad angle, [rad]");
    fh_FakePhiVsRadAngle->Draw("COLZ");

    c1->cd(14);
    fh_TrueElRadiusVsMom->Draw("COLZ");

    c1->cd(15);
    fh_FakeRadiusVsMom->Draw("COLZ");

    char effTxt[200];
	TCanvas *c3 = new TCanvas("CbmRichQa3","c3",1200,1000);
	TH1D* th1 = divide_hist(fh_TrueFoundElRingsProjHitCutMom, fh_MCElRingsProjHitCutMom,
			"Efficiency, electrons vs momentum;momentum, GeV;efficiency, %", 30, 0, 15);
	th1->Draw();
	th1->SetMinimum(0.05);
    sprintf(effTxt,"Mean eff. = (%.2f\%)", (Double_t)fh_TrueFoundElRingsProjHitCutMom->GetEntries()/
    		fh_MCElRingsProjHitCutMom->GetEntries()*100);
    TText *t1 = new TText(3, 0.4, effTxt);
    t1->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);


	TCanvas *c4 = new TCanvas("CbmRichQa4","c4",1200,1000);
	TH1D* th2 = divide_hist(fh_TrueFoundElRingsProjHitCutRadPos, fh_MCElRingsProjHitCutRadPos,
			"Efficiency, electrons vs RadialPosition;radial position, cm;efficiency, %", 20, 0, 150);
	th2->Draw();
	th2->SetMinimum(0.05);
    sprintf(effTxt,"Mean eff. = (%.2f\%)", (Double_t)fh_TrueFoundElRingsProjHitCutRadPos->GetEntries()/
    		fh_MCElRingsProjHitCutRadPos->GetEntries()*100);
    TText *t2 = new TText(3, 0.4, effTxt);
    t2->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    TCanvas *c5 = new TCanvas("CbmRichQa5","c5",1200,1000);
	TH1D* th3 = divide_hist(fh_TrueMatchElMom, fh_TrueFoundElRingsProjHitCutMom,
			"Efficiency, ring-track matching vs momentum;momentum, GeV;efficiency, %", 30, 0, 15);
    th3->Draw();
    sprintf(effTxt,"Mean eff. = (%.2f\%)", (Double_t)fh_TrueMatchElMom->GetEntries()/
    		fh_TrueFoundElRingsProjHitCutMom->GetEntries()*100);
    TText *t3 = new TText(3, 0.4, effTxt);
    t3->Draw();
	th3->SetMinimum(0.05);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    TCanvas *c7 = new TCanvas("CbmRichQa7","c7",1200,1000);
	TH1D* thNofHits = divide_hist(fh_TrueFoundElRingsProjHitCutNofHits, fh_MCElRingsProjHitCutNofHits,
			"Efficiency, electrons vs NofHits;Nof Hits; efficiency, %", 20, 0, 40);
	thNofHits->Draw();

    TCanvas *c8 = new TCanvas("CbmRichQa8","c8",1200,1000);
	TH1D* thBoverA = divide_hist(fh_TrueFoundElRingsProjHitCutBoverA, fh_MCElRingsProjHitCutBoverA,
			"Efficiency, electrons vs B/A;B/A; efficiency, %", 20, 0, 1);
	thBoverA->Draw();

    cout << "El. eff = " << (Double_t)fh_TrueFoundElRingsProjHitCutMom->GetEntries()/
                            (Double_t)fh_MCElRingsProjHitCutMom->GetEntries() << endl;

    cout << "Matching eff = " << (Double_t)fh_TrueMatchElMom->GetEntries()/
                                (Double_t)fh_TrueFoundElRingsProjHitCutMom->GetEntries() << endl;

    cout << "Nof Fakes = " << fh_FakePhi->GetEntries()<< endl;
}


