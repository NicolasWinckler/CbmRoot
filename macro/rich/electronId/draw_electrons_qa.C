/*  Description: This macro draw the histograms which
    were create with CbmRichRingQa class.

    Author : Simeon Lebedev
    E-mail : S.Lebedev@gsi.de
*/
#include "../../../littrack/utils/CbmLitDrawHist.cxx"
#include "../../../littrack/utils/CbmLitUtils.cxx"
#include "../../../rich/utils/CbmRichDrawElectronsQa.cxx"
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
	SetStyles();
    // TFile *file = new TFile("/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.qa.root");
      //  TFile *file = new TFile( "/d/cbm02/andrey/test_electrons/elid.qa.0000.root");
	TFile *file = new TFile("/d/cbm02/slebedev/rich/JUL09/test_electrons/elid.qa.0000.root");
   // gROOT->SetStyle("Plain");
   // gStyle->SetPalette(1,0);
   // gStyle->SetOptStat(0000);

	//gStyle->SetHistLineWidth(3);
	//gROOT->ForceStyle();

// get all histos from file
	TH1D* fhAccRings = (TH1D*)file->Get("fhAccRings");
	TH1D* fhMCRings = (TH1D*)file->Get("fhMCRings");
	TH1D* fhAccRichTrdGlobal = (TH1D*)file->Get("fhAccRichTrdGlobal");
    TH1D* fhAccRichTrdGlobal = (TH1D*)file->Get("fhAccRichTrdTofGlobal");
	TH1D* fhTrueFoundRings = (TH1D*)file->Get("fhTrueFoundRings");
	TH1D* fhTrueMatchStsRichGlobal = (TH1D*)file->Get("fhTrueMatchStsRichGlobal");
	TH1D* fhTrueMatchStsRichTrdGlobal = (TH1D*)file->Get("fhTrueMatchStsRichTrdGlobal");
	TH1D* fhTrueMatchStsRichTrdTofGlobal = (TH1D*)file->Get("fhTrueMatchStsRichTrdTofGlobal");
	TH1D* fhTrueIdRich = (TH1D*)file->Get("fhTrueIdRich");
	TH1D* fhTrueIdRichTrd = (TH1D*)file->Get("fhTrueIdRichTrd");
	TH1D* fhTrueIdRichTrdTof = (TH1D*)file->Get("fhTrueIdRichTrdTof");
	TH1D* fhAccPi = (TH1D*)file->Get("fhAccPi");
	TH1D* fhPiasElRich = (TH1D*)file->Get("fhPiasElRich");
	TH1D* fhPiasElRichTrd = (TH1D*)file->Get("fhPiasElRichTrd");
	TH1D* fhPiasElRichTrdTof = (TH1D*)file->Get("fhPiasElRichTrdTof");

       // DrawAcc(fhMCRings, fhAccRings, fhAccRichTrdGlobal, fhAccRichTrdTofGlobal);
       // DrawMatchingEff(fhAccRings, fhTrueFoundRings, fhTrueMatchStsRichGlobal,
      //  		fhTrueMatchStsRichTrdGlobal,fhTrueMatchStsRichTrdTofGlobal);
      //  DrawMatchingEff2(fhAccRings, fhTrueFoundRings, fhTrueMatchStsRichGlobal,
      //  		fhTrueMatchStsRichTrdGlobal, fhTrueMatchStsRichTrdTofGlobal);
      //  DrawElidEff(fhAccRings, fhTrueFoundRings, fhTrueIdRich,
      //  		fhTrueIdRichTrd, fhTrueIdRichTrdTof,
      //  		fhAccPi, fhPiasElRich, fhPiasElRichTrd, fhPiasElRichTrdTof);

////Draw difference electron-pion
   // TCanvas* c6 = new TCanvas("c6","c6",500,500);
   //     draw_diff_el_pi(fhTrdAnnEl,fhTrdAnnPi);

    TCanvas* c4 = new TCanvas("c4", "c4", 750, 250);
	c4->Divide(3, 1);
       // c4->cd(1);
       // draw_diff_el_pi(fhAaxisEl, fhAaxisPi);
      //  c4->cd(2);
      //  draw_diff_el_pi(fhBAxisEl, fhBAxisPi);
	c4->cd(1);
	draw_diff_el_pi(fhAaxisCorEl, fhAaxisCorPi);
	TLine* l1= new TLine(5.6,0,5.6,1);
        l1->SetLineWidth(4);
	l1->Draw();
	TLine* l2= new TLine(4.4,0,4.4,1);
        l2->SetLineWidth(4);
        l2->Draw();
	c4->cd(2);
	draw_diff_el_pi(fhBAxisCorEl, fhBAxisCorPi);
	TLine* l3= new TLine(5.5,0,5.5,1);
        l3->SetLineWidth(4);
	l3->Draw();
	TLine* l4= new TLine(4.,0,4.,1);
        l4->SetLineWidth(4);
        l4->Draw();
	c4->cd(3);
	draw_diff_el_pi(fhDistEl, fhDistPi);
	TLine* l5= new TLine(1,0,1,1);
        l5->SetLineWidth(4);
        l5->Draw();
       // c4->cd(4);
       // draw_diff_el_pi(fhNofHitsEl, fhNofHitsPi);
       // c4->cd(5);
       // draw_diff_el_pi(fhChi2El, fhChi2Pi);
       // c4->cd(8);
       // draw_diff_el_pi(fhRadPosEl, fhRadPosPi);

        return;
	TCanvas* c5 = new TCanvas("c5", "c5", 600, 900);
	c5->Divide(2, 3);
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
}

