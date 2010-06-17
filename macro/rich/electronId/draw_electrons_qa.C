/*  Description: This macro draw the histograms which
    were create with CbmRichRingQa class.

    Author : Simeon Lebedev
    E-mail : S.Lebedev@gsi.de
*/

TH1D* divide_hist(TH1D* h1, TH1D* h2)
{
	h1->Sumw2();
	h2->Sumw2();
	TH1D* h3 = new TH1D("h3","Efficiency;momentum, GeV/c; efficiency, %",30,0,15);
	h3->Divide(h1,h2,1,1,"B");
	return h3;
}

void draw_diff_el_pi(TH1D* h1, TH1D* h2)
{
	if (h1->GetMaximum() < h2->GetMaximum()) h1->SetMaximum(h2->GetMaximum()*1.05);

	h1->SetLineWidth(2.);
	h2->SetLineWidth(2.);
	h2->SetLineStyle(7);

	h1->Draw();
	h2->Draw("same");
}

void draw_electrons_qa(){
       // TFile *file = new TFile("/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.qa.root");
	TFile *file = new TFile( "/d/cbm02/andrey/test_electrons/elid.qa.0000.root");
   gROOT->SetStyle("Plain");
    gStyle->SetPalette(1,0);
   // gStyle->SetOptStat(0000);

	gStyle->SetHistLineWidth(3);
	gROOT->ForceStyle();
	//gStyle->SetOptStat(0);
	//gStyle->SetOptFit(1);

	TCanvas *c1 = new TCanvas("c1","c1",500,500);
	TH1D* accEff1 = divide_hist(fhAccRings, fhMCRings);
	accEff1->SetTitle("Efficiencies for primary electrons normilized to MC rings");
	accEff1->SetLineColor(kViolet+2);
	accEff1->SetMarkerStyle(24);
	accEff1->SetMarkerSize(2);
	accEff1->SetMarkerColor(kViolet+2);
	accEff1->Draw();
	accEff1->SetMinimum(0.);
    TH1D* accEff2 = divide_hist(fhAccRichTrdGlobal, fhMCRings);
    accEff2->SetLineColor(kRed);
    accEff2->SetMarkerStyle(25);
    accEff2->SetMarkerSize(2);
    accEff2->SetMarkerColor(kRed);
    accEff2->Draw("same");
    TH1D* accEff3 = divide_hist(fhAccRichTrdTofGlobal, fhMCRings);
    accEff3->SetLineColor(kGreen+2);
    accEff3->SetMarkerStyle(26);
    accEff3->SetMarkerSize(2);
    accEff3->SetMarkerColor(kGreen+2);
    accEff3->Draw("same");

    gPad->SetGridx(true);
    gPad->SetGridy(true);

    TLegend* leg2 = new TLegend(0.5,0.1, 0.99, 0.4);
    char accEffTxt[100];
    sprintf(accEffTxt,"accepted RICH rings (%.2f\%)", (Double_t)fhAccRings->GetEntries()/fhMCRings->GetEntries()*100);
    leg2->AddEntry(accEff1, accEffTxt);
    sprintf(accEffTxt,"accepted RICH rings+TRD (%.2f\%)", (Double_t)fhAccRichTrdGlobal->GetEntries()/fhMCRings->GetEntries()*100);
    leg2->AddEntry(accEff2, accEffTxt);
    sprintf(accEffTxt,"accepted RICH rings+TRD+TOF (%.2f\%)", (Double_t)fhAccRichTrdTofGlobal->GetEntries()/fhMCRings->GetEntries()*100);
    leg2->AddEntry(accEff3,accEffTxt);
    leg2->Draw();


    TCanvas *c2 = new TCanvas("c2","c2",500,500);
    TH1D* ringFindEff = divide_hist(fhTrueFoundRings, fhAccRings);
    ringFindEff->SetTitle("Efficiencies for primary electrons normilized to acc. rings");
    ringFindEff->SetLineColor(kViolet+2);
    ringFindEff->SetMarkerStyle(24);
    ringFindEff->SetMarkerSize(2);
    ringFindEff->SetMarkerColor(kViolet+2);
    ringFindEff->Draw();
    ringFindEff->SetMinimum(0.);
    TH1D* stsRichEff = divide_hist(fhTrueMatchStsRichGlobal, fhAccRings);
    stsRichEff->SetLineColor(kRed);
    stsRichEff->SetMarkerStyle(25);
    stsRichEff->SetMarkerSize(2);
    stsRichEff->SetMarkerColor(kRed);
    stsRichEff->Draw("same");
    TH1D* stsRichTrdEff = divide_hist(fhTrueMatchStsRichTrdGlobal, fhAccRings);
    stsRichTrdEff->SetLineColor(kGreen+2);
    stsRichTrdEff->SetMarkerStyle(26);
    stsRichTrdEff->SetMarkerSize(2);
    stsRichTrdEff->SetMarkerColor(kGreen+2);
    stsRichTrdEff->Draw("same");
    TH1D* stsRichTrdTofEff = divide_hist(fhTrueMatchStsRichTrdTofGlobal, fhAccRings);
    stsRichTrdTofEff->SetLineColor(kOrange-3);
    stsRichTrdTofEff->SetMarkerStyle(27);
    stsRichTrdTofEff->SetMarkerSize(2);
    stsRichTrdTofEff->SetMarkerColor(kOrange-3);
    stsRichTrdTofEff->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    TLegend* leg1 = new TLegend(0.5,0.1, 0.99, 0.4);
    char effTxt[100];
    sprintf(effTxt,"ring finding (%.2f\%)", (Double_t)fhTrueFoundRings->GetEntries()/fhAccRings->GetEntries()*100);
    leg1->AddEntry(ringFindEff, effTxt);
    sprintf(effTxt,"STS+RICH match (%.2f\%)", (Double_t)fhTrueMatchStsRichGlobal->GetEntries()/fhAccRings->GetEntries()*100);
    leg1->AddEntry(stsRichEff, effTxt);
    sprintf(effTxt,"STS+RICH+TRD match (%.2f\%)", (Double_t)fhTrueMatchStsRichTrdGlobal->GetEntries()/fhAccRings->GetEntries()*100);
    leg1->AddEntry(stsRichTrdEff,effTxt);
    sprintf(effTxt,"STS+RICH+TRD+TOF match (%.2f\%)", (Double_t)fhTrueMatchStsRichTrdTofGlobal->GetEntries()/fhAccRings->GetEntries()*100);
    leg1->AddEntry(stsRichTrdTofEff,effTxt);
    leg1->Draw();

    TCanvas *c3 = new TCanvas("c3","c3",800,800);
    c3->Divide(2,2);
    c3->cd(1);
    TH1D* eff1 = divide_hist(fhTrueFoundRings, fhAccRings);
    eff1->SetTitle("ring finding efficiency");
    eff1->Draw();
    sprintf(effTxt,"Mean eff. = (%.2f\%)", (Double_t)fhTrueFoundRings->GetEntries()/fhAccRings->GetEntries()*100);
    TText* t1 = new TText(3, 0.7, effTxt);
    t1->Draw();
    eff1->SetMinimum(0.);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c3->cd(2);
    TH1D* eff2 = divide_hist(fhTrueMatchStsRichGlobal, fhTrueFoundRings);
    eff2->SetTitle("STS+RICH matching efficiency");
    eff2->Draw();
    eff2->SetMinimum(0.);
    sprintf(effTxt,"Mean eff. = (%.2f\%)", (Double_t)fhTrueMatchStsRichGlobal->GetEntries()/fhTrueFoundRings->GetEntries()*100);
    TText *t2 = new TText(3, 0.8, effTxt);
    t2->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c3->cd(3);
    TH1D* eff3 = divide_hist(fhTrueMatchStsRichTrdGlobal, fhTrueMatchStsRichGlobal);
    eff3->SetTitle("STS+RICH+TRD matching efficiency");
    eff3->Draw();
    sprintf(effTxt,"Mean eff. = (%.2f\%)", (Double_t)fhTrueMatchStsRichTrdGlobal->GetEntries()/fhTrueMatchStsRichGlobal->GetEntries()*100);
    TText *t3 = new TText(3, 0.4, effTxt);
    t3->Draw();
    eff3->SetMinimum(0.);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c3->cd(4);
    TH1D* eff4 = divide_hist(fhTrueMatchStsRichTrdTofGlobal,fhTrueMatchStsRichTrdGlobal );
    eff4->SetTitle("STS+RICH+TRD+TOF matching efficiency");
    eff4->Draw();
    sprintf(effTxt,"Mean eff. = (%.2f\%)", (Double_t)fhTrueMatchStsRichTrdTofGlobal->GetEntries()/fhTrueMatchStsRichTrdGlobal->GetEntries()*100);
    TText *t4 = new TText(3, 0.4, effTxt);
    t4->Draw();
    eff4->SetMinimum(0.);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

////
    TCanvas* c4 = new TCanvas("c4","c4",900,900);
    c4->Divide(3,3);
    c4->cd(1);
	draw_diff_el_pi(fhAaxisEl, fhAaxisPi);
    c4->cd(2);
	draw_diff_el_pi(fhBAxisEl, fhBAxisPi);
    c4->cd(3);
	draw_diff_el_pi(fhAaxisCorEl, fhAaxisCorPi);
    c4->cd(4);
	draw_diff_el_pi(fhBAxisCorEl, fhBAxisCorPi);
    c4->cd(5);
	draw_diff_el_pi(fhDistEl, fhDistPi);
    c4->cd(6);
	draw_diff_el_pi(fhNofHitsEl, fhNofHitsPi);
    c4->cd(7);
	draw_diff_el_pi(fhChi2El,fhChi2Pi);
    c4->cd(8);
	draw_diff_el_pi(fhRadPosEl,fhRadPosPi);

    TCanvas* c5 = new TCanvas("c5","c5",600,900);
    c5->Divide(2,3);
    c5->cd(1);
	fhAaxisVsMomEl->Draw("COLZ");
    c5->cd(2);
	fhAaxisVsMomPi->Draw("COLZ");
    c5->cd(3);
	fhBAxisVsMomEl->Draw("COLZ");
    c5->cd(4);
	fhBAxisVsMomPi->Draw("COLZ");
    c5->cd(5);
	fhPhiVsRadAngEl->Draw("COLZ");
    c5->cd(6);
	fhPhiVsRadAngPi->Draw("COLZ");

    TCanvas* c6 = new TCanvas("c6","c6",500,500);
	draw_diff_el_pi(fhTrdAnnEl,fhTrdAnnPi);

	TCanvas* c7 = new TCanvas("c7","c7",800,400);
	c7->Divide(2,1);
	c7->cd(1);
    ringFindEff->Draw();
    TH1D* elidEffRich = divide_hist(fhTrueIdRich, fhAccRings);
    elidEffRich->SetLineColor(kRed);
    elidEffRich->SetMarkerStyle(25);
    elidEffRich->SetMarkerSize(2);
    elidEffRich->SetMarkerColor(kRed);
    elidEffRich->Draw("same");

    TH1D* elidEffRichTrd = divide_hist(fhTrueIdRichTrd, fhAccRings);
    elidEffRichTrd->SetLineColor(kGreen+2);
    elidEffRichTrd->SetMarkerStyle(26);
    elidEffRichTrd->SetMarkerSize(2);
    elidEffRichTrd->SetMarkerColor(kGreen+2);
    elidEffRichTrd->Draw("same");

    TH1D* elidEffRichTrdTof = divide_hist(fhTrueIdRichTrdTof, fhAccRings);
    elidEffRichTrdTof->SetLineColor(kOrange-3);
    elidEffRichTrdTof->SetMarkerStyle(27);
    elidEffRichTrdTof->SetMarkerSize(1.2);
    elidEffRichTrdTof->SetMarkerColor(kOrange-3);
    elidEffRichTrdTof->Draw("same");

    TLegend* elidLeg1 = new TLegend(0.5,0.1, 0.99, 0.4);
    char effTxt[100];
    sprintf(effTxt,"ring finding (%.2f\%)", (Double_t)fhTrueFoundRings->GetEntries()/fhAccRings->GetEntries()*100);
    elidLeg1->AddEntry(ringFindEff, effTxt);
    sprintf(effTxt,"RICH el id (%.2f\%)", (Double_t)fhTrueIdRich->GetEntries()/fhAccRings->GetEntries()*100);
    elidLeg1->AddEntry(stsRichEff, effTxt);
    sprintf(effTxt,"RICH+TRD el id (%.2f\%)", (Double_t)fhTrueIdRichTrd->GetEntries()/fhAccRings->GetEntries()*100);
    elidLeg1->AddEntry(stsRichTrdEff,effTxt);
    sprintf(effTxt,"RICH+TRD+TOF el id (%.2f\%)", (Double_t)fhTrueIdRichTrdTof->GetEntries()/fhAccRings->GetEntries()*100);
    elidLeg1->AddEntry(stsRichTrdTofEff, effTxt);
    elidLeg1->Draw();
    gPad->SetGridy(true);
    gPad->SetGridx(true);

	c7->cd(2);
    TH1D* piSuprRich = divide_hist(fhAccPi,fhPiasElRich);
    piSuprRich->SetTitle("Pion supression");
    piSuprRich->GetYaxis()->SetTitle("pion supression");
    piSuprRich->SetLineColor(kRed);
    piSuprRich->SetMarkerStyle(25);
    piSuprRich->SetMarkerSize(2);
    piSuprRich->SetMarkerColor(kRed);
    piSuprRich->Draw();
    piSuprRich->SetMinimum(5);
    piSuprRich->SetMaximum(1e5);

    TH1D* piSuprRichTrd = divide_hist(fhAccPi, fhPiasElRichTrd);
    piSuprRichTrd->SetLineColor(kGreen+2);
    piSuprRichTrd->SetMarkerStyle(26);
    piSuprRichTrd->SetMarkerSize(2);
    piSuprRichTrd->SetMarkerColor(kGreen+2);
    piSuprRichTrd->Draw("same");

    TH1D* piSuprRichTrdTof = divide_hist(fhAccPi, fhPiasElRichTrdTof);
    piSuprRichTrdTof->SetLineColor(kOrange-3);
    piSuprRichTrdTof->SetMarkerStyle(27);
    piSuprRichTrdTof->SetMarkerSize(1.2);
    piSuprRichTrdTof->SetMarkerColor(kOrange-3);
    piSuprRichTrdTof->Draw("same");

    for (int i = 1; i < 30; i++){
    	if ( fabs(piSuprRichTrd->GetBinContent(i)) < 0.00000001) piSuprRichTrd->SetBinContent(i, 0.0000001);
    	if ( fabs(piSuprRichTrdTof->GetBinContent(i)) < 0.00000001) piSuprRichTrdTof->SetBinContent(i, 0.0000001);
    }
    TLegend* elidLeg2 = new TLegend(0.5,0.1, 0.99, 0.4);
    char effTxt[100];
    sprintf(effTxt,"RICH (%.1e)", (Double_t)fhAccPi->GetEntries()/fhPiasElRich->GetEntries());
    elidLeg2->AddEntry(piSuprRich, effTxt);
    sprintf(effTxt,"RICH+TRD (%.1e)", (Double_t)fhAccPi->GetEntries()/fhPiasElRichTrd->GetEntries());
    elidLeg2->AddEntry(piSuprRichTrd, effTxt);
    sprintf(effTxt,"RICH+TRD+TOF (%.1e)", (Double_t)fhAccPi->GetEntries()/fhPiasElRichTrdTof->GetEntries());
    elidLeg2->AddEntry(piSuprRichTrdTof, effTxt);
    elidLeg2->Draw();
    gPad->SetLogy(true);
    gPad->SetGridy(true);
    gPad->SetGridx(true);
}

