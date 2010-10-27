#include "../../../littrack/utils/CbmLitDrawHist.cxx"
#include "../../../littrack/utils/CbmLitUtils.cxx"

void draw(){
    TFile *file = new TFile("/lustre/cbm/user/ebelolap/oct10/rho0/25gev/100_field/analysis.dilep.0000.root");
    gStyle->SetHistLineWidth(3);
	//SetStyles();
    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1,0);

    TCanvas *c1 = new TCanvas("c1","c1",1000,500);
    c1->Divide(2,1);
    c1->cd(1);
    fh_mc_signal_pty->Draw("COLZ");
    c1->cd(2);
    fh_acc_signal_pty->Draw("COLZ");

    TCanvas *c2 = new TCanvas("c2","c2",500,500);
    fh_acc_signal_pty_eff->Draw("COLZ");

    TCanvas *c3 = new TCanvas("c3", "c3", 1000, 500);
    c3->Divide(2,1);
    c3->cd(1);
    fh_mc_signal_mom->SetLineColor(kRed);
    fh_mc_signal_mom->Draw();
    fh_acc_signal_mom->Draw("same");
    c3->cd(2);
   // fh_acc_signal_mom_eff->SetLineColor(kGreen);
    fh_acc_signal_mom_eff->Draw();

    TCanvas *c4 = new TCanvas("c4", "c4", 500, 500);
    fh_mc_signal_minv->SetLineColor(kRed);
    fh_mc_signal_minv->Draw();
    fh_acc_signal_minv->Draw("same");
    

}


