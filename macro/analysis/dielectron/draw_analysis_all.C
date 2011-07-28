/** draw_analysis_all.C
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2010
 * @version 1.0
 **/

//#include "../../../littrack/cbm/utils/CbmLitDrawHist.cxx"

//CUT EFFICIENCY
TH1D* sAll_chi_prim;
TH1D* sAll_gammacut;
TH1D* sAll_stcut;
TH1D* sAll_ttcut;
TH1D* sAll_ptcut;
TH1D* bg_chi_prim;
TH1D* bg_gammacut;
TH1D* bg_stcut;
TH1D* bg_ttcut;
TH1D* bg_ptcut;

void scaleAllHistogramms(TFile* file, Int_t nofEvents)
{
    cout << "Scale all histogramms:" << endl;
    TDirectory * dir = gDirectory;
    TIter nextkey( dir->GetListOfKeys());
    TKey *key;

    Int_t count = 0;
    while (key = (TKey*) nextkey()) {
        TObject* obj = key->ReadObj();

        if (obj->IsA()->InheritsFrom (TH1::Class())) {
            TH1* h = (TH1*) obj;
            cout << h->GetName() << endl;
            TH1* h1 = (TH1*)file->Get(h->GetName());
            h1->Scale(1./(Double_t)nofEvents);
            count++;
        }
    }
    cout << count << " histogramms were scaled." << endl;
}

void draw_minv(TH1* srho1, TH1* somega1, TH1* sphi1, TH1* bg1, TH1* eta, TH1* pi0, TH1* omegaDalitz)
{

    TH1D* sRho = (TH1D*)srho1->Clone();
    sRho->SetFillColor(kMagenta-3);
    sRho->SetLineColor(kBlack);

    TH1D* sOmega = (TH1D*)somega1->Clone();
    sOmega->SetFillColor(kOrange+7);
    sOmega->SetLineColor(kBlack);

    TH1D* sPhi = (TH1D*)sphi1->Clone();
    sPhi->SetFillColor(kRed+1);
    sPhi->SetLineColor(kBlack);

    TH1D* bg = (TH1D*)bg1->Clone();
    bg->SetFillColor(kYellow-10);
    bg->SetLineColor(kBlack);

    TH1D* sEta = (TH1D*)eta->Clone();
    sEta->SetFillColor(kAzure+2);
    sEta->SetLineColor(kBlack);

    TH1D* sPi0 = (TH1D*)pi0->Clone();
    sPi0->SetFillColor(kGreen-3);
    sPi0->SetLineColor(kBlack);

    TH1D* sOmegaDalitz = (TH1D*)omegaDalitz->Clone();
    sOmegaDalitz->SetFillColor(kCyan+2);
    sOmegaDalitz->SetLineColor(kBlack);

    TH1D* sbg = (TH1D*)bg->Clone();
    sbg->SetFillColor(kBlack);
    sbg->SetLineColor(kBlack);
    sbg->Add(sRho);
    sbg->Add(sOmega);
    sbg->Add(sPhi);
    sbg->Add(sEta);
    sbg->Add(sPi0);
    sbg->Add(sOmegaDalitz);
    sbg->SetMinimum(1e-8);
    sbg->SetMaximum(1e-1);

    sbg->Rebin(10);
	bg->Rebin(10);
	sPi0->Rebin(10);
	sEta->Rebin(10);
	sOmegaDalitz->Rebin(10);
	sOmega->Rebin(10);
	sRho->Rebin(10);
	sPhi->Rebin(10);


    sbg->Draw();
    bg->Draw("same");
    sPi0->Draw("same");
    sEta->Draw("same");
    sOmegaDalitz->Draw("same");
    sOmega->Draw("same");
    sRho->Draw("same");
    sPhi->Draw("same");

    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
}

TH1D* add_signals(TH1* rho, TH1* omega, TH1* phi, TH1* eta, TH1* pi0, TH1* omegaD)
{
    TH1D* sAll = (TH1D*)rho->Clone();
    sAll->Add(omega);
    sAll->Add(phi);
    sAll->Add(eta);
    sAll->Add(pi0);
    sAll->Add(omegaD);
    return sAll;
}

void calc_cut_eff(Double_t min, Double_t max)
{
    Int_t x1bin = sAll_chi_prim->FindBin(min);
    Int_t x2bin = sAll_chi_prim->FindBin(max);

//signal
    Double_t sChiprim  = sAll_chi_prim->Integral(x1bin, x2bin);
    Double_t sGamma  = sAll_gammacut->Integral(x1bin, x2bin);
    Double_t sStcut  = sAll_stcut->Integral(x1bin, x2bin);
    Double_t sTtcut  = sAll_ttcut->Integral(x1bin, x2bin);
    Double_t sPt = sAll_ptcut->Integral(x1bin, x2bin);
    //	Double_t sAngle = fh_anglecut_signal_minv->Integral(x1bin, x2bin);
    //    Double_t sApcut  = fh_apmcut_signal_minv->Integral(x1bin, x2bin);

// BG
    Double_t bgChiprim = bg_chi_prim->Integral(x1bin, x2bin);
    Double_t bgGamma = bg_gammacut->Integral(x1bin, x2bin);
    Double_t bgStcut = bg_stcut->Integral(x1bin, x2bin);
    Double_t bgTtcut = bg_ttcut->Integral(x1bin, x2bin);
    Double_t bgPt = bg_ptcut->Integral(x1bin, x2bin);
//    Double_t bgAngle = fh_anglecut_bg_minv->Integral(x1bin, x2bin, "width");
 //   Double_t bgApcut = fh_apmcut_bg_minv->Integral(x1bin, x2bin, "width");

    Double_t x[5] = {0.5, 1.5, 2.5, 3.5, 4.5};
    Int_t n = 5;
    Double_t y1[5] = {100. *(sChiprim / sChiprim),
                      100. *(sGamma / sChiprim),
		      100. *(sStcut / sChiprim),
		      100. *(sTtcut / sChiprim),
		      100. *(sPt / sChiprim)
 //					  100. *(sAngle / sTof)
                      //100. *(sApcut / sTof)
    };

    Double_t y2[5] = {100 * (bgChiprim / bgChiprim),
                      100 * (bgGamma / bgChiprim),
		      100 * (bgStcut / bgChiprim),
		      100 * (bgTtcut / bgChiprim),
		      100 * (bgPt / bgChiprim)
//					  100 * (bgAngle / bgTof)
                     // 100 * (bgApcut / bgTof)
    };
    char txt[100];
    sprintf(txt,"c-%.2f-%.2f",min,max);
    TCanvas* can = new TCanvas (txt, txt, 158,28,600,600);
    TGraph* grS = new TGraph(n, x, y1);
    TGraph* grBg = new TGraph(n, x, y2);
    grS->SetLineWidth(3.);
    grBg->SetLineWidth(3.);
    grS->SetLineColor(kRed);
    grBg->SetLineColor(kBlue);
    grS->SetMarkerStyle(29);
    grBg->SetMarkerStyle(29);
    grS->SetMarkerColor(kRed);
    grBg->SetMarkerColor(kBlue);

    grBg->GetYaxis()->SetTitle("efficiency, %");
    grBg->GetXaxis()->SetTitle("cuts");
    grBg->Draw("APL");
    grS->Draw("PL");
    gPad->SetLogy(true);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    TLegend* leg = new TLegend(0.65,0.6,0.9, 0.9);
    leg->AddEntry(grS, "signal", "l");
    leg->AddEntry(grBg, "BG", "l");
    leg->Draw();
}

void draw_analysis_all()
{
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();

	gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
	cbmlibs();


	Int_t fNofEvents;

    TFile *fileRho0 = new TFile("/lustre/cbm/user/ebelolap/oct10/urqmd_rho0/25gev/70_field/delta/mytask.analysis.delta.all.root");
    fNofEvents = fh_event_number->GetEntries();
	scaleAllHistogramms(fileRho0, fNofEvents);

    TH1D* rho_chi_prim = (TH1D*)fh_chi_prim_signal_minv->Clone();
    TH1D* rho_gammacut = (TH1D*)fh_gammacut_signal_minv->Clone();
    TH1D* rho_stcut = (TH1D*)fh_stcut_signal_minv->Clone();
    TH1D* rho_ttcut = (TH1D*)fh_ttcut_signal_minv->Clone();
    TH1D* rho_ptcut = (TH1D*)fh_ptcut_signal_minv->Clone();

    bg_chi_prim = (TH1D*)fh_chi_prim_bg_minv->Clone();
    bg_gammacut = (TH1D*)fh_gammacut_bg_minv->Clone();
    bg_stcut = (TH1D*)fh_stcut_bg_minv->Clone();
    bg_ttcut = (TH1D*)fh_ttcut_bg_minv->Clone();
    bg_ptcut = (TH1D*)fh_ptcut_bg_minv->Clone();

    TH1D* eta_chi_prim = (TH1D*)fh_chi_prim_eta_minv->Clone();
    TH1D* eta_gammacut = (TH1D*)fh_gammacut_eta_minv->Clone();
    TH1D* eta_stcut = (TH1D*)fh_stcut_eta_minv->Clone();
    TH1D* eta_ttcut = (TH1D*)fh_ttcut_eta_minv->Clone();
    TH1D* eta_ptcut = (TH1D*)fh_ptcut_eta_minv->Clone();

    TH1D* pi0_chi_prim = (TH1D*)fh_chi_prim_pi0_minv->Clone();
    TH1D* pi0_gammacut = (TH1D*)fh_gammacut_pi0_minv->Clone();
    TH1D* pi0_stcut = (TH1D*)fh_stcut_pi0_minv->Clone();
    TH1D* pi0_ttcut = (TH1D*)fh_ttcut_pi0_minv->Clone();
    TH1D* pi0_ptcut = (TH1D*)fh_ptcut_pi0_minv->Clone();

    TFile *fileOmega = new TFile("/lustre/cbm/user/ebelolap/oct10/urqmd_omega/25gev/70_field/delta/mytask.analysis.delta.all.root");
    fNofEvents = fh_event_number->GetEntries();
	scaleAllHistogramms(fileOmega, fNofEvents);
    TH1D* omega_chi_prim = (TH1D*)fh_chi_prim_signal_minv->Clone();
    TH1D* omega_gammacut = (TH1D*)fh_gammacut_signal_minv->Clone();
    TH1D* omega_stcut = (TH1D*)fh_stcut_signal_minv->Clone();
    TH1D* omega_ttcut = (TH1D*)fh_ttcut_signal_minv->Clone();
    TH1D* omega_ptcut = (TH1D*)fh_ptcut_signal_minv->Clone();

    TFile *filePhi = new TFile("/lustre/cbm/user/ebelolap/oct10/urqmd_phi/25gev/70_field/delta/mytask.analysis.delta.all.root");
    fNofEvents = fh_event_number->GetEntries();
	scaleAllHistogramms(filePhi, fNofEvents);
    TH1D* phi_chi_prim = (TH1D*)fh_chi_prim_signal_minv->Clone();
    TH1D* phi_gammacut = (TH1D*)fh_gammacut_signal_minv->Clone();
    TH1D* phi_stcut = (TH1D*)fh_stcut_signal_minv->Clone();
    TH1D* phi_ttcut = (TH1D*)fh_ttcut_signal_minv->Clone();
    TH1D* phi_ptcut = (TH1D*)fh_ptcut_signal_minv->Clone();

    TFile *fileOmegaDalitz = new TFile("/lustre/cbm/user/ebelolap/oct10/urqmd_omega_dalitz/25gev/70_field/delta/mytask.analysis.delta.all.root");
    fNofEvents = fh_event_number->GetEntries();
	scaleAllHistogramms(fileOmegaDalitz, fNofEvents);
    TH1D* omegaD_chi_prim = (TH1D*)fh_chi_prim_signal_minv->Clone();
    TH1D* omegaD_gammacut = (TH1D*)fh_gammacut_signal_minv->Clone();
    TH1D* omegaD_stcut = (TH1D*)fh_stcut_signal_minv->Clone();
    TH1D* omegaD_ttcut = (TH1D*)fh_ttcut_signal_minv->Clone();
    TH1D* omegaD_ptcut = (TH1D*)fh_ptcut_signal_minv->Clone();

    gStyle->SetHistLineWidth(3);
	//SetStyles();
    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1,0);


//INVARIANT MASS DISTRIBUTION
    TCanvas *c10 = new TCanvas("c10-minv", "c10-minv", 1200, 1200);
    //c10->Divide(3,2);
    //c10->cd(1);
   // draw_minv(rho_chi_prim, omega_chi_prim, phi_chi_prim, bg_chi_prim, eta_chi_prim, pi0_chi_prim, omegaD_chi_prim);
   // c10->cd(2);
   // draw_minv(rho_gammacut, omega_gammacut, phi_gammacut, bg_gammacut, eta_gammacut, pi0_gammacut, omegaD_gammacut);
   // c10->cd(3);
   // draw_minv(rho_stcut, omega_stcut, phi_stcut, bg_stcut, eta_stcut, pi0_stcut, omegaD_stcut);
   // c10->cd(4);
   // draw_minv(rho_ttcut, omega_ttcut, phi_ttcut, bg_ttcut, eta_ttcut, pi0_ttcut, omegaD_ttcut);
  //  c10->cd(5);
    draw_minv(rho_ptcut, omega_ptcut, phi_ptcut, bg_ptcut, eta_ptcut, pi0_ptcut, omegaD_ptcut);

//CUT EFFICIENCY
    sAll_chi_prim = add_signals(rho_chi_prim, omega_chi_prim, phi_chi_prim, eta_chi_prim, pi0_chi_prim, omegaD_chi_prim);
    sAll_gammacut = add_signals(rho_gammacut, omega_gammacut, phi_gammacut, eta_gammacut, pi0_gammacut, omegaD_gammacut);
    sAll_stcut = add_signals(rho_stcut, omega_stcut, phi_stcut, eta_stcut, pi0_stcut, omegaD_stcut);
    sAll_ttcut = add_signals(rho_ttcut, omega_ttcut, phi_ttcut, eta_ttcut, pi0_ttcut, omegaD_ttcut);
    sAll_ptcut = add_signals(rho_ptcut, omega_ptcut, phi_ptcut, eta_ptcut, pi0_ptcut, omegaD_ptcut);

    calc_cut_eff(0.0, 0.2);
    calc_cut_eff(0.2, 0.6);
    calc_cut_eff(0.6, 1.0);




   /* TH1D* s = (TH1D*)sRho->Clone();
    s->Add(sOmega);
    s->Add(sPhi);
    s->Add(eta);
    s->Add(pi0);
    s->Add(sOmegaDalitz);
   // TCanvas *c10 = new TCanvas("c10-minv", "c10-minv", 1200, 600);
   // s->Draw();
  //  bg->Draw("same");
    cout <<1./( s->Integral(s->FindBin(0.), s->FindBin(0.2))/bg->Integral(s->FindBin(0.), s->FindBin(0.2))) << endl;
    cout << 1./(s->Integral(s->FindBin(0.2), s->FindBin(0.6))/bg->Integral(s->FindBin(0.2), s->FindBin(0.6))) << endl;
    cout << 1./(s->Integral(s->FindBin(0.6), s->FindBin(1.))/bg->Integral(s->FindBin(0.6), s->FindBin(1.))) << endl;
     */

}
