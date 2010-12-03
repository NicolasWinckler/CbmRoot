#include "../../../littrack/utils/CbmLitDrawHist.cxx"
#include "../../../littrack/utils/CbmLitUtils.cxx"

void draw(){
    TFile *file = new TFile("/lustre/cbm/user/ebelolap/oct10/urqmd_rho0/25gev/70_field/real/mytask.analysis00.0000.root");
    gStyle->SetHistLineWidth(3);
	//SetStyles();
    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1,0);

    TCanvas *c1 = new TCanvas("c1","c1",1200,900);
    c1->Divide(4,3);
    c1->cd(1);
    fh_mc_signal_pty->Draw("COLZ");
    c1->cd(2);
    fh_acc_signal_pty->Draw("COLZ");
    c1->cd(3);    
    fh_acc_signal_pty_eff->Draw("COLZ");
    c1->cd(4);
    fh_reco_signal_pty->Draw("COLZ");
    c1->cd(5);
    fh_rich_id_signal_pty->Draw("COLZ");
    c1->cd(6);
    fh_trd_id_signal_pty->Draw("COLZ");
    c1->cd(7);
    fh_tof_id_signal_pty->Draw("COLZ");
    c1->cd(8);
    fh_ptcut_signal_pty->Draw("COLZ");
    c1->cd(9);
    fh_anglecut_signal_pty->Draw("COLZ");

    

    TCanvas *c2 = new TCanvas("c2", "c2", 1200, 400);
    c2->Divide(3,1);
    c2->cd(1);
    fh_mc_signal_mom->SetLineColor(kRed);
    fh_mc_signal_mom->Draw();
    fh_acc_signal_mom->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c2->cd(2);
    fh_acc_signal_mom_eff->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c2->cd(3);
    fh_rec_mc_mom_signal->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

   

    TCanvas *c3 = new TCanvas("c3", "c3", 500, 500);
    fh_mc_mother_pdg->Draw();
    fh_acc_mother_pdg->SetLineColor(kRed);
    fh_acc_mother_pdg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

    TCanvas *c4 = new TCanvas("c4", "c4", 1200, 600);
    c4->Divide(4,2);
    c4->cd(1);
     fh_chi2_prim_signal->SetLineColor(kRed);  
    fh_chi2_prim_bg->SetLineColor(kBlue);
    fh_chi2_prim_signal->Draw();
    fh_chi2_prim_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c4->cd(2);
    fh_pt_signal->SetLineColor(kRed);
    fh_pt_bg->SetLineColor(kBlue);
    fh_pt_signal->Draw();
    fh_pt_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c4->cd(3);
    fh_position_signal->SetLineColor(kRed);
    fh_position_bg->SetLineColor(kBlue);
    fh_position_signal->Draw();
    fh_position_bg->Draw("same");
    c4->cd(4);
    fh_angle_signal->SetLineColor(kRed);
    fh_angle_bg->SetLineColor(kBlue);
    fh_angle_signal->Draw();
    fh_angle_bg->Draw("same");
    


    
    TCanvas *c5 = new TCanvas("c5","c5", 1200, 400);
    c5->Divide(3,1);
    c5->cd(1);
    fh_mc_vertex_gamma_xz->Draw("COLZ");
    c5->cd(2);
    fh_mc_vertex_gamma_yz->Draw("COLZ");
    c5->cd(3);
    fh_mc_vertex_gamma_xy->Draw("COLZ");

    TCanvas *c6 = new TCanvas("c6", "c6", 1200, 600);
    c6->Divide(4,2);
    c6->cd(1); 
    fh_acc_signal_minv->SetLineColor(kRed);
    fh_mc_signal_minv->Draw();
    fh_acc_signal_minv->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

    c6->cd(2);
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

    c6->cd(3);
    fh_ptcut_signal_minv->SetLineColor(kMagenta);
    fh_anglecut_signal_minv->SetLineColor(kViolet+10);
    fh_ptcut_signal_minv->Draw();
    fh_anglecut_signal_minv->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

    c6->cd(4);
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

    c6->cd(5);
    fh_ptcut_bg_minv->SetLineColor(kMagenta);
    fh_anglecut_bg_minv->SetLineColor(kViolet+10);
   fh_ptcut_bg_minv->Draw();
    fh_anglecut_bg_minv->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

}


