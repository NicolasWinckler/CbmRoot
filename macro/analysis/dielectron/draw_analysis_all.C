/** draw_analysis_all.C
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2010
 * @version 1.0
 **/

//#include "../../../littrack/cbm/utils/CbmLitDrawHist.cxx"

//CUT EFFICIENCY
TH1D* sAll_el_id;
TH1D* sAll_gammacut;
TH1D* sAll_dstscut;
TH1D* sAll_dsts2cut;
TH1D* sAll_stcut;
TH1D* sAll_ttcut;
TH1D* sAll_ptcut;
TH1D* bg_el_id;
TH1D* bg_gammacut;
TH1D* bg_dstscut;
TH1D* bg_dsts2cut;
TH1D* bg_stcut;
TH1D* bg_ttcut;
TH1D* bg_ptcut;

Double_t lineW = 5.;

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
   sbg->SetMaximum(1e-2);

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
    Int_t x1bin = sAll_el_id->FindBin(min);
    Int_t x2bin = sAll_el_id->FindBin(max);

    //signal
    Double_t sElid = sAll_el_id->Integral(x1bin, x2bin);
    Double_t sGamma = sAll_gammacut->Integral(x1bin, x2bin);
    Double_t sDsts = sAll_dstscut->Integral(x1bin, x2bin);
    Double_t sDsts2 = sAll_dsts2cut->Integral(x1bin, x2bin);
    Double_t sSt = sAll_stcut->Integral(x1bin, x2bin);
    Double_t sTt = sAll_ttcut->Integral(x1bin, x2bin);
    Double_t sPt = sAll_ptcut->Integral(x1bin, x2bin);

    // BG
    Double_t bgElid = bg_el_id->Integral(x1bin, x2bin);
    Double_t bgGamma = bg_gammacut->Integral(x1bin, x2bin);
    Double_t bgDsts = bg_dstscut->Integral(x1bin, x2bin);
    Double_t bgDsts2 = bg_dsts2cut->Integral(x1bin, x2bin);
    Double_t bgSt = bg_stcut->Integral(x1bin, x2bin);
    Double_t bgTt = bg_ttcut->Integral(x1bin, x2bin);
    Double_t bgPt = bg_ptcut->Integral(x1bin, x2bin);

    Int_t n = 7;
    Double_t y1[7] = {100. *(sElid / sElid),
                      100. *(sGamma / sElid),
                      100. *(sDsts / sElid),
                      100. *(sDsts2 / sElid),
                      100. *(sSt / sElid),
                      100. *(sTt / sElid),
                      100. *(sPt / sElid)
    };

    Double_t y2[7] = {100. * (bgElid / bgElid),
                      100. * (bgGamma / bgElid),
                      100. * (bgDsts / bgElid),
                      100. * (bgDsts2 / bgElid),
                      100. * (bgSt / bgElid),
                      100. * (bgTt / bgElid),
                      100. * (bgPt / bgElid)
    };

    TH1D* grS = new TH1D("grS", "grS;cuts;efficiency [%]", n, 0, n);
    TH1D* grBg = new TH1D("grBg", "grBg;cuts;efficiency [%]", n, 0, n);

    char* yLabels[7] = {"ID", "m_{#gamma}", "dsts", "dsts2", "ST", "TT", "P_{t}"};
    grBg->GetXaxis()->SetLabelSize(0.06);
    for (Int_t i = 1; i <= n; i++){
       grBg->GetXaxis()->SetBinLabel(i,yLabels[i-1]);
       grBg->SetBinContent(i,y2[i-1]);
       grS->SetBinContent(i,y1[i-1]);
    }

    char txt2[100];
    sprintf(txt2,"cut_eff_%.2f_%.2f",min,max);
    TCanvas* can = new TCanvas(txt2, txt2, 158,28,500,500);
    grS->SetLineWidth(lineW);
    grBg->SetLineWidth(lineW);
    grS->SetLineColor(kRed);
    grBg->SetLineColor(kBlue);
    grBg->SetMinimum(1);
    grBg->SetMaximum(105);
    grBg->Draw("");
    grS->Draw("same");
    //gPad->SetLogy(true);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    TLegend* leg = new TLegend(0.75,0.85,1.0, 1.0);
    leg->AddEntry(grS, "signal", "l");
    leg->AddEntry(grBg, "BG", "l");
    leg->Draw();

    char txt[100];
    sprintf(txt,"%.2f<M [GeV/c^2]<%.2f",min,max);
    TText *t = new TText(0.1, 110, txt);
    t->Draw();
}



TH1D* calc_s_bg(Double_t min, Double_t max)
{
   Int_t bin1 = sAll_el_id->FindBin(min);
   Int_t bin2 = sAll_el_id->FindBin(max);
   Int_t n = 7;
   Double_t y[7] = {(sAll_el_id->Integral(bin1, bin2)/bg_el_id->Integral(bin1, bin2) ),
                   (sAll_gammacut->Integral(bin1, bin2)/bg_gammacut->Integral(bin1, bin2) ),
                   (sAll_dstscut->Integral(bin1, bin2)/bg_dstscut->Integral(bin1, bin2) ),
                   (sAll_dsts2cut->Integral(bin1, bin2)/bg_dsts2cut->Integral(bin1, bin2) ),
                   (sAll_stcut->Integral(bin1, bin2)/bg_stcut->Integral(bin1, bin2) ),
                   (sAll_ttcut->Integral(bin1, bin2)/bg_ttcut->Integral(bin1, bin2) ),
                   (sAll_ptcut->Integral(bin1, bin2)/bg_ptcut->Integral(bin1, bin2) )
                   };

   TH1D* gr_s_bg = new TH1D("gr_s_bg", "gr_s_bg;cuts;S/BG", n, 0, n);
   char* yLabels[7] = {"ID", "m_{#gamma}", "dsts", "dsts2", "ST", "TT", "P_{t}"};
   gr_s_bg->GetXaxis()->SetLabelSize(0.06);
   for (Int_t i = 1; i <= n; i++){
      gr_s_bg->GetXaxis()->SetBinLabel(i,yLabels[i-1]);
      gr_s_bg->SetBinContent(i,y[i-1]);
   }
   return gr_s_bg;
}

void draw_s_bg()
{
   TH1D* h_00_02 = calc_s_bg(0.0, 0.2);
   TH1D* h_02_06 = calc_s_bg(0.2, 0.6);
   TH1D* h_06_12 = calc_s_bg(0.6, 1.2);

   TCanvas* can = new TCanvas("s_bg_regions", "s_bg_regions", 500, 500);
   h_00_02->SetLineWidth(lineW);
   h_02_06->SetLineWidth(lineW);
   h_06_12->SetLineWidth(lineW);
   h_00_02->SetLineColor(kRed);
   h_02_06->SetLineColor(kBlue);
   h_06_12->SetLineColor(kBlack);
   h_00_02->SetMinimum(1e-3);
   h_00_02->SetMaximum(2);
   h_00_02->Draw("");
   h_02_06->Draw("same");
   h_06_12->Draw("same");

   gPad->SetLogy(true);
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   TLegend* leg = new TLegend(0.50,0.8,1.0, 1.0);
   leg->AddEntry(h_00_02, "0.0<M [GeV/c^{2}]<0.2", "l");
   leg->AddEntry(h_02_06, "0.2<M [GeV/c^{2}]<0.6", "l");
   leg->AddEntry(h_06_12, "0.6<M [GeV/c^{2}]<1.2", "l");
   leg->Draw();
}

void draw_s_bg_signals()
{
    Int_t n = 7;
    Double_t y_rho0[7] = { 1./246., 1./207., 1./201., 1./187., 1./156., 1./134., 1./111.};
    Double_t y_omega[7] = { 0.151, 0.181, 0.191, 0.190, 0.232,0.256, 0.294 };
    Double_t y_phi[7] = {0.112, 0.111,0.110, 0.112,0.154,0.180,0.178 };

    TH1D* gr_s_bg_rho0 = new TH1D("gr_s_bg_rho0", "gr_s_bg;cuts;S/BG", n, 0, n);
    TH1D* gr_s_bg_omega = new TH1D("gr_s_bg_omega", "gr_s_bg;cuts;S/BG", n, 0, n);
    TH1D* gr_s_bg_phi = new TH1D("gr_s_bg_phi", "gr_s_bg;cuts;S/BG", n, 0, n);

    char* yLabels[7] = {"ID", "m_{#gamma}", "dsts", "dsts2", "ST", "TT", "P_{t}"};
    gr_s_bg_rho0->GetXaxis()->SetLabelSize(0.06);
    for (Int_t i = 1; i <= n; i++){
       gr_s_bg_rho0->GetXaxis()->SetBinLabel(i,yLabels[i-1]);
       gr_s_bg_rho0->SetBinContent(i,y_rho0[i-1]);
       gr_s_bg_omega->SetBinContent(i,y_omega[i-1]);
       gr_s_bg_phi->SetBinContent(i,y_phi[i-1]);
    }

    TCanvas* can = new TCanvas("s_bg_signals", "s_bg_signals", 500, 500);
    gr_s_bg_rho0->SetLineWidth(lineW);
    gr_s_bg_omega->SetLineWidth(lineW);
    gr_s_bg_phi->SetLineWidth(lineW);
    gr_s_bg_rho0->SetLineColor(kRed);
    gr_s_bg_omega->SetLineColor(kBlue);
    gr_s_bg_phi->SetLineColor(kBlack);
    gr_s_bg_rho0->SetMinimum(0.002);
    gr_s_bg_rho0->SetMaximum(1.);
    gr_s_bg_rho0->Draw("");
    gr_s_bg_omega->Draw("same");
    gr_s_bg_phi->Draw("same");

    gPad->SetLogy(true);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    TLegend* leg = new TLegend(0.70,0.8,1.0, 1.0);
    leg->AddEntry(gr_s_bg_rho0, "#rho^{0}", "l");
    leg->AddEntry(gr_s_bg_omega, "#omega", "l");
    leg->AddEntry(gr_s_bg_phi, "#phi", "l");
    leg->Draw();
}

void draw_analysis_all()
{
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();

   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();

   TString fnRho0 = "/lustre/cbm/user/ebelolap/aug11/25gev/70field/nomvd/rho0/analysis.all.root";
   TString fnOmega = "/lustre/cbm/user/ebelolap/aug11/25gev/70field/nomvd/omega/analysis.all.root";
   TString fnPhi = "/lustre/cbm/user/ebelolap/aug11/25gev/70field/nomvd/phi/analysis.all.root";
   TString fnOmegaD = "/lustre/cbm/user/ebelolap/aug11/25gev/70field/nomvd/omegadalitz/analysis.all.root";

   TFile *fileRho0 = new TFile(fnRho0);
   Int_t nofEvents = fh_event_number->GetEntries();
   scaleAllHistogramms(fileRho0, nofEvents);
   TH1D* rho_el_id = (TH1D*)fh_el_id_signal_minv->Clone();
   TH1D* rho_gammacut = (TH1D*)fh_gammacut_signal_minv->Clone();
   TH1D* rho_dstscut = (TH1D*)fh_dstscut_signal_minv->Clone();
   TH1D* rho_dsts2cut = (TH1D*)fh_dsts2cut_signal_minv->Clone();
   TH1D* rho_stcut = (TH1D*)fh_stcut_signal_minv->Clone();
   TH1D* rho_ttcut = (TH1D*)fh_ttcut_signal_minv->Clone();
   TH1D* rho_ptcut = (TH1D*)fh_ptcut_signal_minv->Clone();

   bg_el_id= (TH1D*)fh_el_id_bg_minv->Clone();
   bg_gammacut = (TH1D*)fh_gammacut_bg_minv->Clone();
   bg_dstscut = (TH1D*)fh_dstscut_bg_minv->Clone();
   bg_dsts2cut = (TH1D*)fh_dsts2cut_bg_minv->Clone();
   bg_stcut = (TH1D*)fh_stcut_bg_minv->Clone();
   bg_ttcut = (TH1D*)fh_ttcut_bg_minv->Clone();
   bg_ptcut = (TH1D*)fh_ptcut_bg_minv->Clone();

   TH1D* eta_el_id = (TH1D*)fh_el_id_eta_minv->Clone();
   TH1D* eta_gammacut = (TH1D*)fh_gammacut_eta_minv->Clone();
   TH1D* eta_dstscut = (TH1D*)fh_dstscut_eta_minv->Clone();
   TH1D* eta_dsts2cut = (TH1D*)fh_dsts2cut_eta_minv->Clone();
   TH1D* eta_stcut = (TH1D*)fh_stcut_eta_minv->Clone();
   TH1D* eta_ttcut = (TH1D*)fh_ttcut_eta_minv->Clone();
   TH1D* eta_ptcut = (TH1D*)fh_ptcut_eta_minv->Clone();

   TH1D* pi0_el_id = (TH1D*)fh_el_id_pi0_minv->Clone();
   TH1D* pi0_gammacut = (TH1D*)fh_gammacut_pi0_minv->Clone();
   TH1D* pi0_dstscut = (TH1D*)fh_dstscut_pi0_minv->Clone();
   TH1D* pi0_dsts2cut = (TH1D*)fh_dsts2cut_pi0_minv->Clone();
   TH1D* pi0_stcut = (TH1D*)fh_stcut_pi0_minv->Clone();
   TH1D* pi0_ttcut = (TH1D*)fh_ttcut_pi0_minv->Clone();
   TH1D* pi0_ptcut = (TH1D*)fh_ptcut_pi0_minv->Clone();

   TFile *fileOmega = new TFile(fnOmega);
   Int_t nofEvents = fh_event_number->GetEntries();
   scaleAllHistogramms(fileOmega, nofEvents);
   TH1D* omega_el_id = (TH1D*)fh_el_id_signal_minv->Clone();
   TH1D* omega_gammacut = (TH1D*)fh_gammacut_signal_minv->Clone();
   TH1D* omega_dstscut = (TH1D*)fh_dstscut_signal_minv->Clone();
   TH1D* omega_dsts2cut = (TH1D*)fh_dsts2cut_signal_minv->Clone();
   TH1D* omega_stcut = (TH1D*)fh_stcut_signal_minv->Clone();
   TH1D* omega_ttcut = (TH1D*)fh_ttcut_signal_minv->Clone();
   TH1D* omega_ptcut = (TH1D*)fh_ptcut_signal_minv->Clone();

   TFile *filePhi = new TFile(fnPhi);
   Int_t nofEvents = fh_event_number->GetEntries();
   scaleAllHistogramms(filePhi, nofEvents);
   TH1D* phi_el_id = (TH1D*)fh_el_id_signal_minv->Clone();
   TH1D* phi_gammacut = (TH1D*)fh_gammacut_signal_minv->Clone();
   TH1D* phi_dstscut = (TH1D*)fh_dstscut_signal_minv->Clone();
   TH1D* phi_dsts2cut = (TH1D*)fh_dsts2cut_signal_minv->Clone();
   TH1D* phi_stcut = (TH1D*)fh_stcut_signal_minv->Clone();
   TH1D* phi_ttcut = (TH1D*)fh_ttcut_signal_minv->Clone();
   TH1D* phi_ptcut = (TH1D*)fh_ptcut_signal_minv->Clone();

   TFile *fileOmegaDalitz = new TFile(fnOmegaD);
   Int_t nofEvents = fh_event_number->GetEntries();
   scaleAllHistogramms(fileOmegaDalitz, nofEvents);
   TH1D* omegaD_el_id = (TH1D*)fh_el_id_signal_minv->Clone();
   TH1D* omegaD_gammacut = (TH1D*)fh_gammacut_signal_minv->Clone();
   TH1D* omegaD_dstscut = (TH1D*)fh_dstscut_signal_minv->Clone();
   TH1D* omegaD_dsts2cut = (TH1D*)fh_dsts2cut_signal_minv->Clone();
   TH1D* omegaD_stcut = (TH1D*)fh_stcut_signal_minv->Clone();
   TH1D* omegaD_ttcut = (TH1D*)fh_ttcut_signal_minv->Clone();
   TH1D* omegaD_ptcut = (TH1D*)fh_ptcut_signal_minv->Clone();

   gStyle->SetHistLineWidth(3);
   gROOT->SetStyle("Plain");
   gStyle->SetPalette(1,0);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);

   //INVARIANT MASS DISTRIBUTION
   TCanvas *c10 = new TCanvas("minv_all", "minv_all", 1000, 500);
   c10->Divide(2,1);
   c10->cd(1);
   draw_minv(rho_ttcut, omega_ttcut, phi_ttcut, bg_ttcut, eta_ttcut, pi0_ttcut, omegaD_ttcut);
   c10->cd(2);
   draw_minv(rho_ptcut, omega_ptcut, phi_ptcut, bg_ptcut, eta_ptcut, pi0_ptcut, omegaD_ptcut);

   //CUT EFFICIENCY
   sAll_el_id = add_signals(rho_el_id, omega_el_id, phi_el_id, eta_el_id, pi0_el_id, omegaD_el_id);
   sAll_gammacut = add_signals(rho_gammacut, omega_gammacut, phi_gammacut, eta_gammacut, pi0_gammacut, omegaD_gammacut);
   sAll_dstscut = add_signals(rho_dstscut, omega_dstscut, phi_dstscut, eta_dstscut, pi0_dstscut, omegaD_dstscut);
   sAll_dsts2cut = add_signals(rho_dsts2cut, omega_dsts2cut, phi_dsts2cut, eta_dsts2cut, pi0_dsts2cut, omegaD_dsts2cut);
   sAll_stcut = add_signals(rho_stcut, omega_stcut, phi_stcut, eta_stcut, pi0_stcut, omegaD_stcut);
   sAll_ttcut = add_signals(rho_ttcut, omega_ttcut, phi_ttcut, eta_ttcut, pi0_ttcut, omegaD_ttcut);
   sAll_ptcut = add_signals(rho_ptcut, omega_ptcut, phi_ptcut, eta_ptcut, pi0_ptcut, omegaD_ptcut);

   calc_cut_eff(0.0, 0.2);
   calc_cut_eff(0.2, 0.6);
   calc_cut_eff(0.6, 1.2);

   draw_s_bg();

   //SET VALUES MANUALY!!!
   draw_s_bg_signals();
}
