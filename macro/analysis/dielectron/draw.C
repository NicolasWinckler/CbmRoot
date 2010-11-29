#include "../../../littrack/utils/CbmLitDrawHist.cxx"
#include "../../../littrack/utils/CbmLitUtils.cxx"

void draw(){
    TFile *file = new TFile("/lustre/cbm/user/ebelolap/oct10/urqmd_rho0/25gev/70_field/real/mytask.analysis00.0000.root");
    gStyle->SetHistLineWidth(3);
	//SetStyles();
    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1,0);

    TCanvas *c1 = new TCanvas("c1","c1",1200,400);
    c1->Divide(3,1);
    c1->cd(1);
    fh_mc_signal_pty->Draw("COLZ");
    c1->cd(2);
    fh_acc_signal_pty->Draw("COLZ");
    c1->cd(3);    
    fh_acc_signal_pty_eff->Draw("COLZ");

    TCanvas *c3 = new TCanvas("c3", "c3", 1200, 400);
    c3->Divide(3,1);
    c3->cd(1);
    fh_mc_signal_mom->SetLineColor(kRed);
    fh_mc_signal_mom->Draw();
    fh_acc_signal_mom->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c3->cd(2);
    fh_acc_signal_mom_eff->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c3->cd(3);
    fh_rec_mc_mom_signal->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

   

    TCanvas *c5 = new TCanvas("c5", "c5", 500, 500);
    fh_mc_mother_pdg->Draw();
    fh_acc_mother_pdg->SetLineColor(kRed);
    fh_acc_mother_pdg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

    TCanvas *c6 = new TCanvas("c6", "c6", 1000, 500);
    c6->Divide(2,1);
    c6->cd(1);
     fh_chi2_prim_signal->SetLineColor(kRed);  
    fh_chi2_prim_bg->SetLineColor(kBlue);
    fh_chi2_prim_signal->Draw();
    fh_chi2_prim_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c6->cd(2);
    fh_pt_signal->SetLineColor(kRed);
    fh_pt_bg->SetLineColor(kBlue);
    fh_pt_signal->Draw();
    fh_pt_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

    
    TCanvas *c7 = new TCanvas("c7","c7", 1200, 400);
    c7->Divide(3,1);
    c7->cd(1);
    fh_mc_vertex_gamma_xz->Draw("COLZ");
    c7->cd(2);
    fh_mc_vertex_gamma_yz->Draw("COLZ");
    c7->cd(3);
    fh_mc_vertex_gamma_xy->Draw("COLZ");

    TCanvas *c9 = new TCanvas("c9", "c9", 1200, 400);
    c9->Divide(3,1);
    c9->cd(1); 
    fh_acc_signal_minv->SetLineColor(kRed);
    fh_mc_signal_minv->Draw();
    fh_acc_signal_minv->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

    c9->cd(2);
    fh_rich_id_signal_minv->SetLineColor(kRed);
    fh_trd_id_signal_minv->SetLineColor(kBlue);
    fh_tof_id_signal_minv->SetLineColor(kGreen);
    fh_rec_signal_minv->Draw();
    fh_rich_id_signal_minv->Draw("same");
    fh_trd_id_signal_minv->Draw("same");
    fh_tof_id_signal_minv->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

    c9->cd(3);
    fh_rich_id_bg_minv->SetLineColor(kRed);
    fh_trd_id_bg_minv->SetLineColor(kBlue);
    fh_tof_id_bg_minv->SetLineColor(kGreen);
    fh_rec_bg_minv->Draw();
    fh_rich_id_bg_minv->Draw("same");
    fh_trd_id_bg_minv->Draw("same");
    fh_tof_id_bg_minv->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

    

}


