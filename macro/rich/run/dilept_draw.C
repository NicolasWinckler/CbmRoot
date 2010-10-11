#include "TH1.h"
#include "TGraph.h"
#define MPI0 0.1349766
#define MOMEGA 0.781940
#define MRHO0 0.769900
#define MPHI 1.019413


void setOPT_canvas(TCanvas *canvas)
{
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);    // X errors of the data points set to be 0
    gStyle->SetLineStyleString(22,"80 18 12 18 12 12"); // special style for the line
    gStyle->SetEndErrorSize(5);   // define end width of error bars
    gStyle->SetCanvasColor(10);
    gStyle->SetPadColor(10);
    gStyle->SetPalette(1);
    canvas->SetLeftMargin(0.15);
    canvas->SetRightMargin(0.045);
    canvas->SetTopMargin(0.074);
    canvas->SetBottomMargin(0.165);
    canvas->ToggleEventStatus();
    canvas->Range(-194.483,-10.3682,1041.38,-2.08469);
    canvas->SetFillColor(0);
    canvas->SetBorderMode(0);
    canvas->SetBorderSize(0);
    canvas->SetTickx();
    canvas->SetTicky();
    canvas->SetFrameLineWidth(2);
    canvas->SetFrameBorderMode(0);
    canvas->SetFrameBorderSize(0);
}


void Histo(TH2* h, const char *xTitle, const char *yTitle)
{
    h->GetXaxis()->SetTitle(xTitle);
    h->GetYaxis()->SetTitle(yTitle);
    h->GetXaxis()->SetTitleSize(0.05);
    h->GetYaxis()->SetTitleSize(0.05);
    h->GetXaxis()->SetTitleFont(42);
    h->GetYaxis()->SetTitleFont(42);
    h->GetXaxis()->SetTitleOffset(1.1);
    h->GetYaxis()->SetTitleOffset(1.2);
    h->GetXaxis()->SetLabelFont(42);
    h->GetYaxis()->SetLabelFont(42);
    h->GetZaxis()->SetLabelFont(42);
    h->GetXaxis()->SetLabelSize(0.05);
    h->GetYaxis()->SetLabelSize(0.05);
    h->GetZaxis()->SetLabelSize(0.03);
    h->SetTitle();
}


void Histo(TH1* h, const char *xTitle, const char *yTitle)
{
    h->GetXaxis()->SetTitle(xTitle);
    h->GetYaxis()->SetTitle(yTitle);
    h->GetXaxis()->SetTitleSize(0.05);
    h->GetYaxis()->SetTitleSize(0.05);
    h->GetXaxis()->SetTitleFont(42);
    h->GetYaxis()->SetTitleFont(42);
    h->GetXaxis()->SetTitleOffset(1.1);
    h->GetYaxis()->SetTitleOffset(1.5);
    h->GetXaxis()->SetLabelFont(42);
    h->GetYaxis()->SetLabelFont(42);
    h->GetXaxis()->SetLabelSize(0.05);
    h->GetYaxis()->SetLabelSize(0.05);
    h->SetTitle();
}


void Histo(TPaveText *p)
{
  p->SetFillColor(10);
  p->SetBorderSize(-1);
  p->SetTextFont(42);
}


void Histo(TPaveLabel *p)
{
  p->SetFillColor(10);
  p->SetBorderSize(-1);
  p->SetTextFont(42);
}



void Histo(TLegend *p)
{
  p->SetFillColor(10);
  p->SetBorderSize(-1);
  p->SetTextFont(42);
}
//------------------------------------------------------------------------------

TString dir_id2   = "/u/ebelolap/SEP09/trunk/macro/dilept_rec/analysis/auau/centr/8gev/draw/";
TString file_rho0_25gev = dir_id2 + "urqmd_rho0_ee.auau.25gev.centr.70k.spectra.root";
//TString file_rho0_10gev = dir_id2 + "urqmd_rho0_ee.auau.10gev.centr.all.spectra.root";

TString file_rho0_10gev = "/lustre/cbm/user/slebedev/all/analysis.all.root";
TString file_all = dir_id2 + "10gev.auau.centr.all.spectra.root";


//============================================================================
//============================================================================
void plot_minv_ee()
{
    TFile *file_tot = new TFile(file_rho0_10gev);

    TH1D *fh_pi0cut_bg_minv    = (TH1D*) file_tot->Get("h_gcut_minv");
    TH1D *fh_pi0cut_gamma_minv = (TH1D*) file_tot->Get("h_gcut_gamma_minv");
    TH1D *fh_pi0cut_pi0_minv   = (TH1D*) file_tot->Get("h_gcut_pi0_minv");
    TH1D *fh_pi0cut_eta_minv   = (TH1D*) file_tot->Get("h_gcut_eta_minv");
    TH1D *fh_pi0cut_rho0_ee_minv = (TH1D*) file_tot->Get("h_gcut_omega_minv");

    TH1D *fh_all = new TH1D("h_all", "", 2000, 0., 2.);
    fh_all->Add(fh_pi0cut_bg_minv);
    fh_all->Add(fh_pi0cut_gamma_minv);
    fh_all->Add(fh_pi0cut_pi0_minv);
    fh_all->Add(fh_pi0cut_eta_minv);
    fh_all->Add(fh_pi0cut_rho0_ee_minv);

    // ---   Normalize histograms   ---
    Int_t nBin = 20;
    fh_pi0cut_bg_minv->Rebin(nBin);
    fh_pi0cut_gamma_minv->Rebin(nBin);
    fh_pi0cut_pi0_minv->Rebin(nBin);
    fh_pi0cut_eta_minv->Rebin(nBin);
    fh_pi0cut_rho0_ee_minv->Rebin(nBin);
    fh_all->Rebin(nBin);

    Double_t binsize = 1 * 2./ (2000./(Double_t)nBin);
    fh_pi0cut_bg_minv->Scale(1./ binsize);
    fh_pi0cut_gamma_minv->Scale(1./ binsize);
    fh_pi0cut_pi0_minv->Scale(1./ binsize);
    fh_pi0cut_eta_minv->Scale(1./ binsize);
    fh_pi0cut_rho0_ee_minv->Scale(1./ binsize);
    fh_all->Scale(1./ binsize);
    // ----------------------------------

    Histo(fh_pi0cut_bg_minv,"M_{ee} [GeV/c^{2}]","dN/dM_{ee} / [GeV/c^{2}]");
    fh_pi0cut_bg_minv->GetYaxis()->SetRangeUser(1e-6, 10);
//    fh_pi0cut_bg_minv->GetYaxis()->SetRangeUser(1e-6, 1e-1);
    fh_pi0cut_bg_minv->GetXaxis()->SetRangeUser(0, 2.);

    fh_all->SetLineColor(1);
    fh_all->SetFillColor(1);
    fh_all->SetLineWidth(2);

    fh_pi0cut_bg_minv->SetLineColor(15);
    fh_pi0cut_bg_minv->SetFillColor(15);

    TH1F *pi0   = fh_pi0cut_pi0_minv->Clone();
    TH1F *gamma = fh_pi0cut_gamma_minv->Clone();
    TH1F *eta   = fh_pi0cut_eta_minv->Clone();
    TH1F *rho0  = fh_pi0cut_rho0_ee_minv->Clone();

    gamma->SetLineColor(1);
    pi0->SetLineColor(1);
    eta->SetLineColor(1);
    rho0->SetLineColor(9);

    gamma->SetLineWidth(2);
    pi0->SetLineWidth(2);
    eta->SetLineWidth(2);
    rho0->SetLineWidth(2);

    fh_pi0cut_bg_minv->SetLineColor(1);
    fh_pi0cut_bg_minv->SetFillColor(19);
    fh_pi0cut_bg_minv->SetLineWidth(2);

    fh_pi0cut_gamma_minv->SetLineColor(1);
    fh_pi0cut_gamma_minv->SetLineWidth(2);

    fh_pi0cut_pi0_minv->SetLineColor(1);
    fh_pi0cut_pi0_minv->SetLineWidth(2);
    fh_pi0cut_pi0_minv->SetFillColor(2);

    fh_pi0cut_eta_minv->SetLineColor(1);
    fh_pi0cut_eta_minv->SetLineWidth(2);
    fh_pi0cut_eta_minv->SetFillColor(4);

    fh_pi0cut_rho0_ee_minv->SetLineColor(9);
    fh_pi0cut_rho0_ee_minv->SetLineWidth(2);
    fh_pi0cut_rho0_ee_minv->SetFillColor(9);

    TCanvas* can = new TCanvas ("can", "", 158,28,900,918);
    setOPT_canvas(can);

    gPad->SetLogy();
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetGridx();
    gPad->SetGridy();
    fh_pi0cut_bg_minv->Draw();

    fh_all->Draw("same");
    fh_pi0cut_bg_minv->Draw("same");
    fh_pi0cut_pi0_minv->Draw("same");
    fh_pi0cut_eta_minv->Draw("same");
    fh_pi0cut_rho0_ee_minv->Draw("same");
    gamma->Draw("same");
    rho0->Draw("same");
}


void eff_cut_app()
{
    TFile *file10gev = new TFile(file_rho0_10gev);
    // BG
    TH1D *fh_rich_id_cut_minv = (TH1D*) file10gev->Get("h_rich_id_minv");
    TH1D *fh_g_cut_minv = (TH1D*) file10gev->Get("h_gcut_minv");
    TH1D *fh_dist_cut_minv = (TH1D*) file10gev->Get("h_dcut_minv");
    TH1D *fh_theta_mom_cut_minv = (TH1D*) file10gev->Get("h_tpcut_minv");
    TH1D *fh_pt_cut_minv = (TH1D*) file10gev->Get("h_ptcut_minv");
    TH1D *fh_tcut_minv = (TH1D*) file10gev->Get("h_tcut_minv");
    TH1D *fh_pi0cut_minv = (TH1D*) file10gev->Get("h_pi0cut_minv");

//signal
    TH1D *fh_rich_id_omega_minv = (TH1D*) file10gev->Get("h_rich_id_omega_minv");
    TH1D *fh_rich_id_eta_minv = (TH1D*) file10gev->Get("h_rich_id_eta_minv");
    TH1D *fh_rich_id_pi0_minv = (TH1D*) file10gev->Get("h_rich_id_pi0_minv");

//omega
 
TH1D *fh_gcut_omega_minv = (TH1D*) file10gev->Get("h_gcut_omega_minv");
TH1D *fh_dcut_omega_minv = (TH1D*) file10gev->Get("h_dcut_omega_minv");
TH1D *fh_tpcut_omega_minv = (TH1D*) file10gev->Get("h_tpcut_omega_minv");
TH1D *fh_ptcut_omega_minv = (TH1D*) file10gev->Get("h_ptcut_omega_minv");
TH1D *fh_tcut_omega_minv = (TH1D*) file10gev->Get("h_tcut_omega_minv");
TH1D *fh_pi0cut_omega_minv = (TH1D*) file10gev->Get("h_pi0cut_omega_minv");

//eta

TH1D *fh_gcut_eta_minv = (TH1D*) file10gev->Get("h_gcut_eta_minv");
TH1D *fh_dcut_eta_minv = (TH1D*) file10gev->Get("h_dcut_eta_minv");
TH1D *fh_tpcut_eta_minv = (TH1D*) file10gev->Get("h_tpcut_eta_minv");
TH1D *fh_ptcut_eta_minv = (TH1D*) file10gev->Get("h_ptcut_eta_minv");
TH1D *fh_tcut_eta_minv = (TH1D*) file10gev->Get("h_tcut_eta_minv");
TH1D *fh_pi0cut_eta_minv = (TH1D*) file10gev->Get("h_pi0cut_eta_minv");

//pi0

TH1D *fh_gcut_pi0_minv = (TH1D*) file10gev->Get("h_gcut_pi0_minv");
TH1D *fh_dcut_pi0_minv = (TH1D*) file10gev->Get("h_dcut_pi0_minv");
TH1D *fh_tpcut_pi0_minv = (TH1D*) file10gev->Get("h_tpcut_pi0_minv");
TH1D *fh_ptcut_pi0_minv = (TH1D*) file10gev->Get("h_ptcut_pi0_minv");
TH1D *fh_tcut_pi0_minv = (TH1D*) file10gev->Get("h_tcut_pi0_minv");
TH1D *fh_pi0cut_pi0_minv = (TH1D*) file10gev->Get("h_pi0cut_pi0_minv");

//add
TH1D *fh_signal = new TH1D("h_signal", "", 2000, 0., 2.);
fh_signal->Add(fh_rich_id_omega_minv);
fh_signal->Add(fh_rich_id_eta_minv);
fh_signal->Add(fh_rich_id_pi0_minv);

TH1D *fh_signal_gcut = new TH1D("h_signal_gcut", "", 2000, 0., 2.);
fh_signal_gcut->Add(fh_gcut_omega_minv);
fh_signal_gcut->Add(fh_gcut_eta_minv);
fh_signal_gcut->Add(fh_gcut_pi0_minv);

TH1D *fh_signal_dcut = new TH1D("h_signal_dcut", "", 2000, 0., 2.);
fh_signal_dcut->Add(fh_dcut_omega_minv);
fh_signal_dcut->Add(fh_dcut_eta_minv);
fh_signal_dcut->Add(fh_dcut_pi0_minv);

TH1D *fh_signal_tpcut = new TH1D("h_signal_tpcut", "", 2000, 0., 2.);
fh_signal_tpcut->Add(fh_tpcut_omega_minv);
fh_signal_tpcut->Add(fh_tpcut_eta_minv);
fh_signal_tpcut->Add(fh_tpcut_pi0_minv);

TH1D *fh_signal_ptcut = new TH1D("h_signal_ptcut", "", 2000, 0., 2.);
fh_signal_ptcut->Add(fh_ptcut_omega_minv);
fh_signal_ptcut->Add(fh_ptcut_eta_minv);
fh_signal_ptcut->Add(fh_ptcut_pi0_minv);

TH1D *fh_signal_tcut = new TH1D("h_signal_tcut", "", 2000, 0., 2.);
fh_signal_tcut->Add(fh_tcut_omega_minv);
fh_signal_tcut->Add(fh_tcut_eta_minv);
fh_signal_tcut->Add(fh_tcut_pi0_minv);

TH1D *fh_signal_pi0cut = new TH1D("h_signal_pi0cut", "", 2000, 0., 2.);
fh_signal_pi0cut->Add(fh_pi0cut_omega_minv);
fh_signal_pi0cut->Add(fh_pi0cut_eta_minv);
fh_signal_pi0cut->Add(fh_pi0cut_pi0_minv);

    Int_t nBin = 20;
    fh_rich_id_cut_minv->Rebin(nBin);
    fh_g_cut_minv->Rebin(nBin);
    fh_dist_cut_minv->Rebin(nBin);
    fh_theta_mom_cut_minv->Rebin(nBin);
    fh_pt_cut_minv->Rebin(nBin);
    fh_tcut_minv->Rebin(nBin);
    fh_pi0cut_minv->Rebin(nBin);

fh_signal->Rebin(nBin);
fh_signal_gcut->Rebin(nBin);
fh_signal_dcut->Rebin(nBin);
fh_signal_tpcut->Rebin(nBin);
fh_signal_ptcut->Rebin(nBin);
fh_signal_tcut->Rebin(nBin);
fh_signal_pi0cut->Rebin(nBin);


//------------ 0 < x < 0.2
//BG
Double_t Sum_rich1  = 0;
Double_t Sum_gcut1  = 0;
Double_t Sum_dsts1  = 0;
Double_t Sum_theta1 = 0;
Double_t Sum_pt1    = 0;
Double_t Sum_tcut1  = 0;
Double_t Sum_pi01   = 0;

//signal
Double_t sum_signal1        = 0;
Double_t sum_signal_gcut1   = 0;
Double_t sum_signal_dcut1   = 0;
Double_t sum_signal_tpcut1  = 0;
Double_t sum_signal_ptcut1  = 0;
Double_t sum_signal_tcut1   = 0;
Double_t sum_signal_pi0cut1 = 0;

//------------ 0.2 < x < 0.6

Double_t Sum_rich2  = 0;
Double_t Sum_gcut2  = 0;
Double_t Sum_dsts2  = 0;
Double_t Sum_theta2 = 0;
Double_t Sum_pt2    = 0;
Double_t Sum_tcut2  = 0;
Double_t Sum_pi02   = 0;

//signal
Double_t sum_signal2        = 0;
Double_t sum_signal_gcut2   = 0;
Double_t sum_signal_dcut2   = 0;
Double_t sum_signal_tpcut2  = 0;
Double_t sum_signal_ptcut2  = 0;
Double_t sum_signal_tcut2   = 0;
Double_t sum_signal_pi0cut2 = 0;


//------------ 0.6 < x < 1.2

Double_t Sum_rich3  = 0;
Double_t Sum_gcut3  = 0;
Double_t Sum_dsts3  = 0;
Double_t Sum_theta3 = 0;
Double_t Sum_pt3    = 0;
Double_t Sum_tcut3  = 0;
Double_t Sum_pi03   = 0;

//signal
Double_t sum_signal3        = 0;
Double_t sum_signal_gcut3   = 0;
Double_t sum_signal_dcut3   = 0;
Double_t sum_signal_tpcut3  = 0;
Double_t sum_signal_ptcut3  = 0;
Double_t sum_signal_tcut3   = 0;
Double_t sum_signal_pi0cut3 = 0;

//-------------------

Double_t x1 = 0;
Double_t x2 = 0.2;
Double_t x3 = 0.6;
Double_t x4 = 1.2;



Int_t Nbins = fh_rich_id_cut_minv->GetNbinsX();

for (Int_t i =1; i <= Nbins; i++) {

	Int_t binCenter = fh_rich_id_cut_minv->GetBinCenter(i);
	if (binCenter < x1 || binCenter > x2) continue;
		{
		Sum_rich1 += (fh_rich_id_cut_minv->GetBinWidth(i)) * (fh_rich_id_cut_minv->GetBinContent(i));
		Sum_gcut1 += (fh_g_cut_minv->GetBinWidth(i)) * (fh_g_cut_minv->GetBinContent(i));
		Sum_dsts1 += (fh_dist_cut_minv->GetBinWidth(i)) * (fh_dist_cut_minv->GetBinContent(i));
		Sum_theta1 += (fh_theta_mom_cut_minv->GetBinWidth(i)) * (fh_theta_mom_cut_minv->GetBinContent(i));
		Sum_pt1 += (fh_pt_cut_minv->GetBinWidth(i)) * (fh_pt_cut_minv->GetBinContent(i));
		Sum_tcut1 += (fh_tcut_minv->GetBinWidth(i)) * (fh_tcut_minv->GetBinContent(i));
		Sum_pi01 += (fh_pi0cut_minv->GetBinWidth(i)) * (fh_pi0cut_minv->GetBinContent(i));

		sum_signal1 += (fh_signal->GetBinWidth(i)) * (fh_signal->GetBinContent(i));
		sum_signal_gcut1 += (fh_signal_gcut->GetBinWidth(i)) * (fh_signal_gcut->GetBinContent(i));
		sum_signal_dcut1 += (fh_signal_dcut->GetBinWidth(i)) * (fh_signal_dcut->GetBinContent(i));
		sum_signal_tpcut1 += (fh_signal_tpcut->GetBinWidth(i)) * (fh_signal_tpcut->GetBinContent(i));
		sum_signal_ptcut1 += (fh_signal_ptcut->GetBinWidth(i)) * (fh_signal_ptcut->GetBinContent(i));
		sum_signal_tcut1 += (fh_signal_tcut->GetBinWidth(i)) * (fh_signal_tcut->GetBinContent(i));
		sum_signal_pi0cut1 += (fh_signal_pi0cut->GetBinWidth(i)) * (fh_signal_pi0cut->GetBinContent(i));
		}
	
	if (binCenter < x2 || binCenter > x3) continue;
		{
		Sum_rich2 += (fh_rich_id_cut_minv->GetBinWidth(i)) * (fh_rich_id_cut_minv->GetBinContent(i));
		Sum_gcut2 += (fh_g_cut_minv->GetBinWidth(i)) * (fh_g_cut_minv->GetBinContent(i));
		Sum_dsts2 += (fh_dist_cut_minv->GetBinWidth(i)) * (fh_dist_cut_minv->GetBinContent(i));
		Sum_theta2 += (fh_theta_mom_cut_minv->GetBinWidth(i)) * (fh_theta_mom_cut_minv->GetBinContent(i));
		Sum_pt2 += (fh_pt_cut_minv->GetBinWidth(i)) * (fh_pt_cut_minv->GetBinContent(i));
		Sum_tcut2 += (fh_tcut_minv->GetBinWidth(i)) * (fh_tcut_minv->GetBinContent(i));
		Sum_pi02 += (fh_pi0cut_minv->GetBinWidth(i)) * (fh_pi0cut_minv->GetBinContent(i));

		sum_signal2 += (fh_signal->GetBinWidth(i)) * (fh_signal->GetBinContent(i));
		sum_signal_gcut2 += (fh_signal_gcut->GetBinWidth(i)) * (fh_signal_gcut->GetBinContent(i));
		sum_signal_dcut2 += (fh_signal_dcut->GetBinWidth(i)) * (fh_signal_dcut->GetBinContent(i));
		sum_signal_tpcut2 += (fh_signal_tpcut->GetBinWidth(i)) * (fh_signal_tpcut->GetBinContent(i));
		sum_signal_ptcut2 += (fh_signal_ptcut->GetBinWidth(i)) * (fh_signal_ptcut->GetBinContent(i));
		sum_signal_tcut2 += (fh_signal_tcut->GetBinWidth(i)) * (fh_signal_tcut->GetBinContent(i));
		sum_signal_pi0cut2 += (fh_signal_pi0cut->GetBinWidth(i)) * (fh_signal_pi0cut->GetBinContent(i));
		}
	
	if (binCenter < x3 || binCenter > x4) continue;
		{
		Sum_rich3 += (fh_rich_id_cut_minv->GetBinWidth(i)) * (fh_rich_id_cut_minv->GetBinContent(i));
		Sum_gcut3 += (fh_g_cut_minv->GetBinWidth(i)) * (fh_g_cut_minv->GetBinContent(i));
		Sum_dsts3 += (fh_dist_cut_minv->GetBinWidth(i)) * (fh_dist_cut_minv->GetBinContent(i));
		Sum_theta3 += (fh_theta_mom_cut_minv->GetBinWidth(i)) * (fh_theta_mom_cut_minv->GetBinContent(i));
		Sum_pt3 += (fh_pt_cut_minv->GetBinWidth(i)) * (fh_pt_cut_minv->GetBinContent(i));
		Sum_tcut3 += (fh_tcut_minv->GetBinWidth(i)) * (fh_tcut_minv->GetBinContent(i));
		Sum_pi03 += (fh_pi0cut_minv->GetBinWidth(i)) * (fh_pi0cut_minv->GetBinContent(i));

		sum_signal3 += (fh_signal->GetBinWidth(i)) * (fh_signal->GetBinContent(i));
		sum_signal_gcut3 += (fh_signal_gcut->GetBinWidth(i)) * (fh_signal_gcut->GetBinContent(i));
		sum_signal_dcut3 += (fh_signal_dcut->GetBinWidth(i)) * (fh_signal_dcut->GetBinContent(i));
		sum_signal_tpcut3 += (fh_signal_tpcut->GetBinWidth(i)) * (fh_signal_tpcut->GetBinContent(i));
		sum_signal_ptcut3 += (fh_signal_ptcut->GetBinWidth(i)) * (fh_signal_ptcut->GetBinContent(i));
		sum_signal_tcut3 += (fh_signal_tcut->GetBinWidth(i)) * (fh_signal_tcut->GetBinContent(i));
		sum_signal_pi0cut3 += (fh_signal_pi0cut->GetBinWidth(i)) * (fh_signal_pi0cut->GetBinContent(i));
		}
}


//------------ 0 < x < 0.2
	if (Sum_rich1 == 0) Sum_rich1 = 100000;
	Double_t eff_rich1 = Sum_rich1 / Sum_rich1;
	Double_t eff_gcut1 = Sum_gcut1 / Sum_rich1;
	Double_t eff_dsts1 = Sum_dsts1 / Sum_rich1;
	Double_t eff_theta1 = Sum_theta1 / Sum_rich1;
	Double_t eff_pt1 = Sum_pt1 / Sum_rich1;
	Double_t eff_tcut1 = Sum_tcut1 / Sum_rich1;
	Double_t eff_pi01 = Sum_pi01 / Sum_rich1;

//-----------signal
	if (sum_signal1 == 0) sum_signal1 = 100000;
	Double_t eff_signal1 = sum_signal1 / sum_signal1;
	Double_t eff_signal_gcut1 = sum_signal_gcut1 / sum_signal1;
	Double_t eff_signal_dcut1 = sum_signal_dcut1 / sum_signal1;
	Double_t eff_signal_tpcut1 = sum_signal_tpcut1 / sum_signal1;
	Double_t eff_signal_ptcut1 = sum_signal_ptcut1 / sum_signal1;
	Double_t eff_signal_tcut1 =  sum_signal_tcut1 / sum_signal1;
	Double_t eff_signal_pi0cut1 = sum_signal_pi0cut1 / sum_signal1;


//------------ 0.2 < x < 0.6	
	if (Sum_rich2 == 0) Sum_rich2 = 100000;
	Double_t eff_rich2 = Sum_rich2 / Sum_rich2;
	Double_t eff_gcut2 = Sum_gcut2 / Sum_rich2;
	Double_t eff_dsts2 = Sum_dsts2 / Sum_rich2;
	Double_t eff_theta2 = Sum_theta2 / Sum_rich2;
	Double_t eff_pt2 = Sum_pt2 / Sum_rich2;
	Double_t eff_tcut2 = Sum_tcut2 / Sum_rich2;
	Double_t eff_pi02 = Sum_pi02 / Sum_rich2;


//------------signal
	if (sum_signal2 == 0) sum_signal2 = 100000;
	Double_t eff_signal2 = sum_signal2 / sum_signal2;
	Double_t eff_signal_gcut2 = sum_signal_gcut2 / sum_signal2;
	Double_t eff_signal_dcut2 = sum_signal_dcut2 / sum_signal2;
	Double_t eff_signal_tpcut2 = sum_signal_tpcut2 / sum_signal2;
	Double_t eff_signal_ptcut2 = sum_signal_ptcut2 / sum_signal2;
	Double_t eff_signal_tcut2 =  sum_signal_tcut2 / sum_signal2;
	Double_t eff_signal_pi0cut2 = sum_signal_pi0cut2 / sum_signal2;
	
//------------ 0.6 < x < 1.2	
	if (Sum_rich3 == 0) Sum_rich3 = 100000;
	Double_t eff_rich3 = Sum_rich3 / Sum_rich3;
	Double_t eff_gcut3 = Sum_gcut3 / Sum_rich3;
	Double_t eff_dsts3 = Sum_dsts3 / Sum_rich3;
	Double_t eff_theta3 = Sum_theta3 / Sum_rich3;
	Double_t eff_pt3 = Sum_pt3 / Sum_rich3;
	Double_t eff_tcut3 = Sum_tcut3 / Sum_rich3;
	Double_t eff_pi03 = Sum_pi03 / Sum_rich3;

//---------------signal
	if (sum_signal3 == 0) sum_signal3 = 100000;
	Double_t eff_signal3 = sum_signal3 / sum_signal3;
	Double_t eff_signal_gcut3 = sum_signal_gcut3 / sum_signal3;
	Double_t eff_signal_dcut3 = sum_signal_dcut3 / sum_signal3;
	Double_t eff_signal_tpcut3 = sum_signal_tpcut3 / sum_signal3;
	Double_t eff_signal_ptcut3 = sum_signal_ptcut3 / sum_signal3;
	Double_t eff_signal_tcut3 =  sum_signal_tcut3 / sum_signal3;
	Double_t eff_signal_pi0cut3 = sum_signal_pi0cut3 / sum_signal3;
	

//BG
	Float_t Yaxis1[7];
	Yaxis1[0] = eff_rich1;
	Yaxis1[1] = eff_gcut1;
	Yaxis1[2] = eff_dsts1;
	Yaxis1[3] = eff_theta1;
	Yaxis1[4] = eff_pt1;
	Yaxis1[5] = eff_tcut1;
	Yaxis1[6] = eff_pi01;

//signal

	Float_t yAxis1[7];
	yAxis1[0] = eff_signal1;
	yAxis1[1] = eff_signal_gcut1;	
	yAxis1[2] = eff_signal_dcut1;
	yAxis1[3] = eff_signal_tpcut1;
	yAxis1[4] = eff_signal_ptcut1;
	yAxis1[5] = eff_signal_tcut1;
	yAxis1[6] = eff_signal_pi0cut1;

//BG
	Float_t Yaxis2[7]; 
	Yaxis2[0] = eff_rich2;
	Yaxis2[1] = eff_gcut2;
	Yaxis2[2] = eff_dsts2;
	Yaxis2[3] = eff_theta2;
	Yaxis2[4] = eff_pt2;
	Yaxis2[5] = eff_tcut2;
	Yaxis2[6] = eff_pi02;

//signal
	Float_t yAxis2[7];
	yAxis2[0] = eff_signal2;
	yAxis2[1] = eff_signal_gcut2;	
	yAxis2[2] = eff_signal_dcut2;
	yAxis2[3] = eff_signal_tpcut2;
	yAxis2[4] = eff_signal_ptcut2;
	yAxis2[5] = eff_signal_tcut2;
	yAxis2[6] = eff_signal_pi0cut2;


//BG	
	Float_t Yaxis3[7]; 
	Yaxis3[0] = eff_rich3;
	Yaxis3[1] = eff_gcut3;
	Yaxis3[2] = eff_dsts3;
	Yaxis3[3] = eff_theta3;
	Yaxis3[4] = eff_pt3;
	Yaxis3[5] = eff_tcut3;
	Yaxis3[6] = eff_pi03;

//signal
	Float_t yAxis3[7];
	yAxis3[0] = eff_signal3;
	yAxis3[1] = eff_signal_gcut3;	
	yAxis3[2] = eff_signal_dcut3;
	yAxis3[3] = eff_signal_tpcut3;
	yAxis3[4] = eff_signal_ptcut3;
	yAxis3[5] = eff_signal_tcut3;
	yAxis3[6] = eff_signal_pi0cut3;



	//TString s[] = {"ID", "m_{#gamma}", "d,p" , "#theta, p" , "p_{t}" , "#theta_{1,2}", "pi_{0}"};
	Float_t x[7] = {0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5};	
	Int_t n = 7;

	TCanvas* can = new TCanvas ("can1", "", 158,28,900,918);
    	setOPT_canvas(can);
	TGraph* gr1 = new TGraph(n, x, Yaxis1);
	TGraph* gr1_signal = new TGraph(n, x, yAxis1);
	gr1->SetLineColor(4);
	gr1_signal->SetLineColor(kRed);
	gr1->SetMarkerColor(4);
	gr1_signal->SetMarkerColor(kRed);
	gr1->Draw("AL*");
	gr1_signal->Draw("same");


	TCanvas* can2 = new TCanvas ("can2", "", 158,28,900,918);
    	setOPT_canvas(can2);
	TGraph* gr2 = new TGraph(n, x, Yaxis2);
	TGraph* gr2_signal = new TGraph(n, x, yAxis2);
	gr2->SetLineColor(4);
	gr2_signal->SetLineColor(kRed);
	gr2->SetMarkerColor(4);
	gr2_signal->SetMarkerColor(kRed);
	gr2->Draw("AL*");
	gr2_signal->Draw("same");

	TCanvas* can3 = new TCanvas ("can3", "", 158,28,900,918);
    	setOPT_canvas(can3);
	TGraph* gr3 = new TGraph(n, x, Yaxis3);
	TGraph* gr3_signal = new TGraph(n, x, yAxis3);
	gr3->SetLineColor(4);
	gr3_signal->SetLineColor(kRed);
	gr3->SetMarkerColor(4);
	gr3_signal->SetMarkerColor(kRed);
	gr3->Draw("AL*");
	gr3_signal->Draw("same");
	

}



void plot_minv_bg_10_25()
{
    TFile *file25gev = new TFile(file_rho0_25gev);
    TFile *file10gev = new TFile(file_all);

    TH1D *fh_pi0cut_bg_minv_25gev    = (TH1D*) file25gev->Get("h_rich_id_minv");
    TH1D *fh_pi0cut_bg_minv_10gev    = (TH1D*) file10gev->Get("h_rich_id_minv");



    TH1D *fh_all = new TH1D("h_all", "", 2000, 0., 2.);
  //  fh_all->Add(fh_pi0cut_bg_minv);
 //   fh_all->Add(fh_pi0cut_gamma_minv);
//    fh_all->Add(fh_pi0cut_pi0_minv);
//    fh_all->Add(fh_pi0cut_eta_minv);
    //    fh_all->Add(fh_pi0cut_rho0_ee_minv);
    fh_all->Add(fh_pi0cut_bg_minv_10gev);
    fh_all->Add(fh_pi0cut_bg_minv_25gev);

    // ---   Normalize histograms   ---
    Int_t nBin = 20;
  //  fh_pi0cut_bg_minv->Rebin(nBin);
  //  fh_pi0cut_gamma_minv->Rebin(nBin);
  //  fh_pi0cut_pi0_minv->Rebin(nBin);
  //  fh_pi0cut_eta_minv->Rebin(nBin);
  //  fh_pi0cut_rho0_ee_minv->Rebin(nBin);
    fh_all->Rebin(nBin);
    fh_pi0cut_bg_minv_25gev->Rebin(nBin);
    fh_pi0cut_bg_minv_10gev->Rebin(nBin);

    Double_t Sum = 0;
    Double_t Sum1 = 0;
    Int_t numX = fh_pi0cut_bg_minv_25gev->GetNbinsX();
    Int_t numX1 = fh_pi0cut_bg_minv_10gev->GetNbinsX();
    for(Int_t i = 1; i <= numX; i++) {
	Sum = Sum + (fh_pi0cut_bg_minv_25gev->GetBinWidth(i)) * (fh_pi0cut_bg_minv_25gev->GetBinContent(i));
    }
for(Int_t i1 = 1; i1 <= numX1; i1++) {
	Sum1 = Sum1 + (fh_pi0cut_bg_minv_10gev->GetBinWidth(i1)) * (fh_pi0cut_bg_minv_10gev->GetBinContent(i1));
    }
cout << "S_for_25gev = "<< Sum << endl;
    cout << "S_for_10gev = "<< Sum1 << endl;
    Double_t Sum_Sum1 = 0;
    //Sum_Sum1 = Sum/Sum1;
    cout << "Otnoshenie = " << Sum_Sum1 <<endl;

    Double_t binsize = 1 * 2./ (2000./(Double_t)nBin);
 //   fh_pi0cut_bg_minv->Scale(1./ binsize);
 //   fh_pi0cut_gamma_minv->Scale(1./ binsize);
 //   fh_pi0cut_pi0_minv->Scale(1./ binsize);
 //   fh_pi0cut_eta_minv->Scale(1./ binsize);
//    fh_pi0cut_rho0_ee_minv->Scale(1./ binsize);
    fh_all->Scale(1./ binsize);
    fh_pi0cut_bg_minv_25gev->Scale(1./ binsize);
    fh_pi0cut_bg_minv_10gev->Scale(1./ binsize);


    // ----------------------------------

    Histo(fh_pi0cut_bg_minv_25gev,"M_{ee} [GeV/c^{2}]","dN/dM_{ee} / [GeV/c^{2}]");
    fh_pi0cut_bg_minv_25gev->GetYaxis()->SetRangeUser(1e-6, 10);
//    fh_pi0cut_bg_minv->GetYaxis()->SetRangeUser(1e-6, 1e-1);
    fh_pi0cut_bg_minv_25gev->GetXaxis()->SetRangeUser(0, 2.);

    fh_all->SetLineColor(15);

    fh_all->SetLineWidth(2);

    fh_pi0cut_bg_minv_25gev->SetLineColor(1);
    fh_pi0cut_bg_minv_10gev->SetLineColor(4);
    fh_pi0cut_bg_minv_25gev->SetLineWidth(3);
    fh_pi0cut_bg_minv_10gev->SetLineWidth(3);
   



    TCanvas* can = new TCanvas ("can", "", 158,28,900,918);
    setOPT_canvas(can);

    gPad->SetLogy();
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetGridx();
    gPad->SetGridy();
    fh_pi0cut_bg_minv_25gev->Draw();
 //   fh_all->Draw("same");
    fh_pi0cut_bg_minv_10gev->Draw("same");
 //   fh_pi0cut_bg_minv->Draw("same");
 //   fh_pi0cut_pi0_minv->Draw("same");
 //   fh_pi0cut_eta_minv->Draw("same");
 //   fh_pi0cut_rho0_ee_minv->Draw("same");
 //   gamma->Draw("same");
 //   rho0->Draw("same");
}

///////////////////////////////////////////////////////////////////////////////////

void plot_minv_rho0_10_25()
{
    TFile *file25gev = new TFile(file_rho0_25gev);
    TFile *file10gev = new TFile(file_all);

    TH1D *fh_rich_id_omega_minv_25gev    = (TH1D*) file25gev->Get("h_rich_id_omega_minv");
    TH1D *fh_rich_id_omega_minv_10gev    = (TH1D*) file10gev->Get("h_rich_id_omega_minv");
    TH1D *fh_pi0cut_bg_minv_25gev    = (TH1D*) file25gev->Get("h_rich_id_minv");
    TH1D *fh_pi0cut_bg_minv_10gev    = (TH1D*) file10gev->Get("h_rich_id_minv");
    

//Double_t signalBg10 = (fh_rich_id_omega_minv_10gev->GetEntries() / fh_pi0cut_bg_minv_10gev->GetEntries());
//Double_t signalBg25 = (fh_rich_id_omega_minv_25gev->GetEntries() / fh_pi0cut_bg_minv_25gev->GetEntries());

//cout << "signal to background ratio for 10 = " << signalBg10 << endl;
//cout << "signal to background ratio for 25 = " << signalBg25 << endl;
    TH1D *fh_all = new TH1D("h_all", "", 2000, 0., 2.);

//    fh_all->Add(fh_rich_id_omega_minv_10gev);
//    fh_all->Add(fh_rich_id_omega_minv_25gev);

    // ---   Normalize histograms   ---
    Int_t nBin = 20;

    fh_all->Rebin(nBin);
    fh_rich_id_omega_minv_25gev->Rebin(nBin);
  //  fh_rich_id_omega_minv_10gev->Rebin(nBin);


    Double_t binsize = 1 * 2./ (2000./(Double_t)nBin);

    fh_all->Scale(1./ binsize);
    fh_rich_id_omega_minv_25gev->Scale(1./ binsize);
   // fh_rich_id_omega_minv_10gev->Scale(1./ binsize);

 /*   Double_t Sum = 0;
    Double_t Sum1 = 0;
    Int_t numX = fh_rich_id_omega_minv_25gev->GetNbinsX();
    Int_t numX1 = fh_rich_id_omega_minv_10gev->GetNbinsX();
    for(Int_t i = 1; i <= numX; i++) {
	Sum = Sum + (fh_rich_id_omega_minv_25gev->GetBinWidth(i)) * (fh_rich_id_omega_minv_25gev->GetBinContent(i));
    }
for(Int_t i1 = 1; i1 <= numX1; i1++) {
	Sum1 = Sum1 + (fh_rich_id_omega_minv_10gev->GetBinWidth(i1)) * (fh_rich_id_omega_minv_10gev->GetBinContent(i1));
    }
cout << "S_for_25gev = "<< Sum << endl;
    cout << "S_for_10gev = "<< Sum1 << endl;
    Double_t Sum_Sum1 = 0;
    Sum_Sum1 = Sum/Sum1;
    cout << "Otnoshenie = " << Sum_Sum1 <<endl;
*/
    // ----------------------------------

    Histo(fh_rich_id_omega_minv_10gev,"M_{ee} [GeV/c^{2}]","dN/dM_{ee} / [GeV/c^{2}]");
    fh_rich_id_omega_minv_10gev->GetYaxis()->SetRangeUser(1e-6, 10);
//    fh_pi0cut_bg_minv->GetYaxis()->SetRangeUser(1e-6, 1e-1);
    fh_rich_id_omega_minv_10gev->GetXaxis()->SetRangeUser(0, 2.);

    fh_all->SetLineColor(15);
//    fh_all->SetFillColor(1);
    fh_all->SetLineWidth(2);

    fh_rich_id_omega_minv_25gev->SetLineColor(1);
 //   fh_rich_id_omega_minv_25gev->SetFillColor(15);

    fh_rich_id_omega_minv_10gev->SetLineColor(4);
 //   fh_rich_id_omega_minv_10gev->SetFillColor(4);
    fh_rich_id_omega_minv_25gev->SetLineWidth(3);
    fh_rich_id_omega_minv_10gev->SetLineWidth(3);


    TCanvas* can = new TCanvas ("can", "", 158,28,900,918);
    setOPT_canvas(can);

    gPad->SetLogy();
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetGridx();
    gPad->SetGridy();
  //  fh_rich_id_omega_minv_25gev->Draw();
  //  fh_all->Draw("same");
    fh_rich_id_omega_minv_10gev->Draw();

}








//============================================================================
//============================================================================
// Begin STS + RICH + TRD + TOF plots
//============================================================================
//============================================================================
void plot_tof_m2_p()
{
    TFile *file = new TFile(file_rho0_10gev);
    TH2D *fh_tof = (TH2D*) file->Get("h_tof_m2_mom");
    Histo(fh_tof, "p_{lab} [GeV/c]", "m^{2} [GeV/c^{2}]^{2}");

    TCanvas* can = new TCanvas ("can", "", 158,28,900,918);
    setOPT_canvas(can);
    gPad->SetLogz();
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.13);
    fh_tof->Draw("colz");
}




//============================================================================
//============================================================================
// Begin momentum, pt-y, theta, mass plots for mc_all, mc_rec + id
//============================================================================
//============================================================================
void plot_lept_momentum_mc_id()
{
    TFile *file = new TFile(file_all);

    TH1D* fh_mc_all_mom = (TH1D*) file->Get("h_mc_all_lept_mom");
    TH1D* fh_mc_rec_mom = (TH1D*) file->Get("h_mc_rec_lept_mom");
    TH1D* fh_rich_id_mom = (TH1D*) file->Get("h_rich_id_lept_mom");

    cout << "mc rec/ mc all : " << fh_mc_rec_mom->GetEntries()/(Double_t)fh_mc_all_mom->GetEntries() << endl;
    cout << "id    / mc all : " << fh_rich_id_mom->GetEntries()/(Double_t)fh_mc_rec_mom->GetEntries() << endl;

    // ---   Normalize gistograms   ---
    Int_t cBin = 2;
    fh_mc_all_mom->Rebin(cBin);
    fh_mc_rec_mom->Rebin(cBin);
    fh_rich_id_mom->Rebin(cBin);

    Double_t binsize = 10./ (1000./(Double_t)cBin);
    fh_mc_all_mom->Scale(1./ binsize);
    fh_mc_rec_mom->Scale(1./ binsize);
    fh_rich_id_mom->Scale(1./ binsize);
    // -----------------------------------

    Histo(fh_mc_all_mom, "p_{lab} [GeV/c]", "dN/dp_{lab}  1/[GeV/c]");

    fh_mc_all_mom->GetXaxis()->SetRangeUser(0., 2.);
    fh_mc_all_mom->GetYaxis()->SetRangeUser(5e-2, 4e1);

    fh_mc_all_mom->SetLineColor(1);
    fh_mc_rec_mom->SetLineColor(2);
    fh_rich_id_mom->SetLineColor(4);

    fh_mc_all_mom->SetLineWidth(3);
    fh_mc_rec_mom->SetLineWidth(3);
    fh_rich_id_mom->SetLineWidth(3);

    TCanvas* can = new TCanvas ("can", "", 158,28,900,918);
    setOPT_canvas(can);
    gPad->SetLogy();
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    fh_mc_all_mom->Draw();
    fh_mc_rec_mom->Draw("same");
    fh_rich_id_mom->Draw("same");
    can->cd();
}
//============================================================================
//============================================================================
void plot_signal_momentum_mc_id()
{
    TFile *file = new TFile(file_all);

    TH1D* fh_mc_all_mom = (TH1D*) file->Get("h_mc_all_omega_mom");
    TH1D* fh_mc_rec_mom = (TH1D*) file->Get("h_mc_rec_omega_mom");
    TH1D* fh_rich_id_mom = (TH1D*) file->Get("h_rich_id_omega_mom");

    cout << "mc rec/ mc all : " << fh_mc_rec_mom->GetEntries()/(Double_t)fh_mc_all_mom->GetEntries() << endl;
    cout << "id    / mc all : " << fh_rich_id_mom->GetEntries()/(Double_t)fh_mc_rec_mom->GetEntries() << endl;

    // ---   Normalize gistograms   ---
    Int_t cBin = 10;
    fh_mc_all_mom->Rebin(cBin);
    fh_mc_rec_mom->Rebin(cBin);
    fh_rich_id_mom->Rebin(cBin);

    Double_t binsize = 10./ (1000./(Double_t)cBin);
    fh_mc_all_mom->Scale(1./ binsize);
    fh_mc_rec_mom->Scale(1./ binsize);
    fh_rich_id_mom->Scale(1./ binsize);
    // -----------------------------------

    Histo(fh_mc_all_mom, "p_{lab} [GeV/c]", "dN/dp_{lab}  1/[GeV/c]");
    fh_mc_all_mom->GetXaxis()->SetRangeUser(0., 10.);
    fh_mc_all_mom->GetYaxis()->SetRangeUser(1e-6., 1e-3);

    fh_mc_all_mom->SetLineColor(1);
    fh_mc_rec_mom->SetLineColor(2);
    fh_rich_id_mom->SetLineColor(4);

    fh_mc_all_mom->SetLineWidth(3);
    fh_mc_rec_mom->SetLineWidth(3);
    fh_rich_id_mom->SetLineWidth(3);

    TCanvas* can = new TCanvas ("can", "", 158,28,900,918);
    setOPT_canvas(can);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetLogy();
    fh_mc_all_mom->Draw();
    fh_mc_rec_mom->Draw("same");
    fh_rich_id_mom->Draw("same");
    can->cd();
}
//-------------------------------------------------
void plot_signal_momentum_10gev_25gev()
{
    TFile *file1 = new TFile(file_all);
    TFile *file2 = new TFile(file_rho0_25gev);

    TH1D* fh_mc_all_mom1 = (TH1D*) file1->Get("h_mc_all_omega_mom");
    TH1D* fh_mc_all_mom2 = (TH1D*) file2->Get("h_mc_all_omega_mom");

    // ---   Normalize gistograms   ---
    Int_t cBin = 10;
    fh_mc_all_mom1->Rebin(cBin);
    fh_mc_all_mom2->Rebin(cBin);

    Double_t binsize = 10./ (1000./(Double_t)cBin);
    fh_mc_all_mom1->Scale(1./ binsize);
    fh_mc_all_mom2->Scale(1./ binsize);
    // -----------------------------------

    Histo(fh_mc_all_mom1, "p_{lab} [GeV/c]", "dN/dp_{lab}  1/[GeV/c]");
    fh_mc_all_mom1->GetXaxis()->SetRangeUser(0., 10.);
    fh_mc_all_mom1->GetYaxis()->SetRangeUser(1e-6., 1e-3);

    fh_mc_all_mom1->SetLineColor(1);
    fh_mc_all_mom2->SetLineColor(2);

    fh_mc_all_mom1->SetLineWidth(2);
    fh_mc_all_mom2->SetLineWidth(2);

    TCanvas* can = new TCanvas ("can", "", 158,28,900,918);
    setOPT_canvas(can);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetLogy();
    fh_mc_all_mom1->Draw();
    fh_mc_all_mom2->Draw("same");
    can->cd();
}
//============================================================================
void plot_pty_acc_pty_all_rho0()
{
    TFile *file = new TFile(file_rho0_10gev);
    TH2D *fh_all = (TH2D*) file->Get("h_mc_all_omega_pty");
    TH2D *fh_rec = (TH2D*) file->Get("h_mc_rec_omega_pty");
    TH2D *fh_rec_all = new TH2D("fh_rec_all", "", 40, 0., 4., 20, 0., 2.);
    fh_rec_all->Divide(fh_rec, fh_all);
 //   TH2D *fh_r = (TH2D*) file->Get("h_rich_id_omega_pty");
 //   TH2D *fh_r = (TH2D*) file->Get("h_pi0cut_omega_pty");
    Histo(fh_rec_all, "Rapidity", "p_{t} [GeV/c]");

    TCanvas* can = new TCanvas ("can", "", 158,28,900,918);
    setOPT_canvas(can);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.15);
    fh_rec_all->Draw("colz");
    can->cd();
}

//=============================================================================
void dist_closest_mom()
{
    TFile *file = new TFile(file_rho0_10gev);
    TH2D *fh_bg = (TH2D*) file->Get("h_dsts_mom_bg");
//    TH2D *fh_omega = (TH2D*) file->Get("h_dsts_mom_omega");

    Histo(fh_bg, "d_{MVD} [cm]", "p_{lab} [GeV/c]");

    TCanvas* can = new TCanvas ("can", "", 158,28,900,918);
    setOPT_canvas(can);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.15);
    fh_bg->Draw("colz");
    can->cd();
}

//----------------------------------------------------------------------------

void theta_vs_sqrt()
{
    TFile *file = new TFile(file_rho0_10gev);
    TH2D *fh_bg = (TH2D*) file->Get("h_dcut_theta_mom_gamma");
  //  TH2D *fh_omega = (TH2D*) file->Get("h_dcut_theta_mom_omega");

    Histo(fh_bg, "#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c]" , " #theta_{e^{#pm},rec} [deg]");

    TCanvas* can = new TCanvas ("can", "", 158,28,900,918);
    setOPT_canvas(can);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.15);
    fh_bg->Draw("colz");
    can->cd();
}

//===========================================================================



//============================================================================
void plot_pty_rho0()
{
    TFile *file = new TFile(file_rho0_10gev);
//    TH2D *fh_r = (TH2D*) file->Get("h_mc_rec_omega_pty");
   TH2D *fh_r = (TH2D*) file->Get("h_rich_id_omega_pty");
 //   TH2D *fh_r = (TH2D*) file->Get("h_pi0cut_omega_pty");
    Histo(fh_r, "Rapidity", "p_{t} [GeV/c]");

    TCanvas* can = new TCanvas ("can", "", 158,28,900,918);
    setOPT_canvas(can);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.15);
    fh_r->Draw("colz");
    can->cd();
}
//============================================================================
//============================================================================




//============================================================================
//============================================================================
void plot_pt_pid()
{
    TFile *file_rho0 = new TFile(file_all);

    TH1D* fh_signal = (TH1D*) file_rho0->Get("h_tpcut_pt_lept_omega");
    TH1D* fh_bg    = (TH1D*) file_rho0->Get("h_tpcut_pt_lept_bg");

    // --- Normalize gistograms   ---
    Int_t cBin = 1;
    fh_signal->Rebin(cBin);
    fh_bg->Rebin(cBin);

    Double_t binsize = 10./ (1000./(Double_t)cBin);
    fh_signal->Scale(2000./ binsize);
    fh_bg->Scale(1./ binsize);
    // --------------------------------

    fh_bg->SetLineColor(4);
    fh_bg->SetLineWidth(2);

    fh_signal->SetLineColor(2);
    fh_signal->SetLineWidth(2);

    Histo(fh_bg, "p_{t} [GeV/c]", "dN/dp_{t} 1/[GeV/c]");
    fh_bg->GetXaxis()->SetRangeUser(0., 1.4);
    fh_bg->GetYaxis()->SetRangeUser(0., 1.4);

    TCanvas* can = new TCanvas ("can", "", 158,28,900,918);
    setOPT_canvas(can);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);

    fh_bg->Draw();
    fh_signal->Draw("same");

//    TLegend *leg1 = new TLegend(0.761, 0.648, 0.933, 0.879);
//    Histo(leg1);
//    leg1->AddEntry(fh_bg,    "  e_{bg}", "l");
//    leg1->AddEntry(fh_signal,"  e_{#omega}", "l");
//    leg1->Draw();

//    TArrow* arrow1 = new TArrow(0.2, 2e-3, 0.2, 3.);
//    arrow1->SetLineColor(1);
//    arrow1->SetArrowSize(0.02);
//    arrow1->SetLineWidth(2);
//    arrow1->Draw("<-");
    can->cd();
}
//============================================================================
//============================================================================
// Begin mass, efficiency, detection plots
//============================================================================
//============================================================================
void plot_phys_bg_cocktail()
{
    TFile *file = new TFile(file_all);

    TH1D *all = (TH1D*) file->Get("h_pi0cut_mc_minv");
    TH1D *pp = (TH1D*) file->Get("h_pi0cut_pi0_pi0_minv");
    TH1D *pg = (TH1D*) file->Get("h_pi0cut_pi0_gamma_minv");
    TH1D *gg = (TH1D*) file->Get("h_pi0cut_gamma_gamma_minv");
    TH1D *eg = (TH1D*) file->Get("h_pi0cut_eta_gamma_minv");
    TH1D *ep = (TH1D*) file->Get("h_pi0cut_eta_pi0_minv");
    TH1D *ee = (TH1D*) file->Get("h_pi0cut_eta_eta_minv");
/*
    // small ckeck
    pp->Add(pg);
    pp->Add(gg);
    pp->Add(eg);
    pp->Add(ep);
    pp->Add(ee);
    all->Rebin(50);
    pp->Rebin(50);
    pp->SetLineColor(2);
    all->Draw();
    pp->Draw("same");
    break;
*/


/*
    TH1D *pp = (TH1D*) file->Get("h_rich_id_pi0_pi0_minv");
    TH1D *pg = (TH1D*) file->Get("h_rich_id_pi0_gamma_minv");
    TH1D *gg = (TH1D*) file->Get("h_rich_id_gamma_gamma_minv");
    TH1D *eg = (TH1D*) file->Get("h_rich_id_eta_gamma_minv");
    TH1D *ep = (TH1D*) file->Get("h_rich_id_eta_pi0_minv");
    TH1D *ee = (TH1D*) file->Get("h_rich_id_eta_eta_minv");
*/

    // ---   Normalize histograms   ---
    Int_t nBin = 120;
    pp->Rebin(nBin);
    pg->Rebin(nBin);
    gg->Rebin(nBin);
    eg->Rebin(nBin);
    ep->Rebin(nBin);
    ee->Rebin(nBin);

    Double_t binsize = 2./ (2000./(Double_t)nBin);
    pp->Scale(1./ binsize);
    pg->Scale(1./ binsize);
    gg->Scale(1./ binsize);
    eg->Scale(1./ binsize);
    ep->Scale(1./ binsize);
    ee->Scale(1./ binsize);
    // ----------------------------------
    Histo(pp,"M_{ee} [GeV/c^{2}]","dN/dM_{ee} 1/[GeV/c^{2}]");
    pp->GetYaxis()->SetRangeUser(1e-4, 5e-1);
    pp->GetXaxis()->SetRangeUser(0, 2.);
    pp->SetLineWidth(2);
    pp->SetLineColor(2);
    pg->SetLineWidth(2);
    pg->SetLineColor(4);
    gg->SetLineWidth(2);
    gg->SetLineColor(1);
    eg->SetLineWidth(2);
    eg->SetLineColor(8);
    ep->SetLineWidth(2);
    ep->SetLineColor(6);
    ee->SetLineWidth(2);
    ee->SetLineColor(9);

    TCanvas* can = new TCanvas ("can", "", 158,28,900,918);
    setOPT_canvas(can);
    gPad->SetLogy();
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    pp->Draw();
    pg->Draw("same");
    gg->Draw("same");
    eg->Draw("same");
    ep->Draw("same");
//  ee->Draw("same");

    TLegend *leg = new TLegend(0.675, 0.564, 0.934, 0.885);
    Histo(leg);
    leg->AddEntry(pp,"e_{#pi^{0}} + e_{#pi^{0}}","l");
    leg->AddEntry(pg,"e_{#pi^{0}} + e_{#gamma}","l");
    leg->AddEntry(gg,"e_{#gamma} + e_{#gamma}","l");
    leg->AddEntry(eg,"e_{#eta} + e_{#gamma}","l");
    leg->AddEntry(ep,"e_{#eta} + e_{#pi^{0}}","l");
    //  leg->AddEntry(ee,"e_{#eta}" + "e_{#eta}","l");
    leg->Draw();
    can->cd();

}
