#include "../../../littrack/utils/CbmLitDrawHist.cxx"
#include "../../../littrack/utils/CbmLitUtils.cxx"

void draw(){
    TFile *file = new TFile("/lustre/cbm/user/ebelolap/oct10/rho0/25gev/100_field/analysis.dilep.0000.root");
    gStyle->SetHistLineWidth(3);
	//SetStyles();
    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1,0);

    TCanvas *c1 = new TCanvas("c1","c1",600,600);
    fh_mc_signal_pty->Draw("COLZ");
}


