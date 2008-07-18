

draw_eloss_ana()
{

  TString engine = "geant3/";
  TString setup = "1-10gev_20cm/";
  
  TString dir = "/d/cbm02/andrey/events/eloss/" + engine + setup;
  TString fileName = dir + "eloss.ana.root";
  
  TString outDir = "./" + engine + setup;
  
  TFile *file = new TFile(fileName); 
  
  TCanvas *c1 = new TCanvas("energy loss","c1",1200,1000);
  c1->Divide(4, 2);
  c1->SetGrid();
    
  gStyle->SetOptFit(0001);
  
  Double_t lwidth = 2.;
  
  c1->cd(1);
  fh_dp_mc->SetLineColor(4);
  fh_dp_mc->SetMarkerColor(4);
  fh_dp_mc->SetLineWidth(lwidth);
  fh_dp_mc->GetXaxis()->SetTitle("mc_mom_in - mc_mom_out, GeV");
  fh_dp_mc->GetYaxis()->SetTitle("counter");
  fh_dp_mc->Fit("landau");
  fh_dp_mc->Draw();
  
  c1->cd(2);
  fh_eloss_bb->SetLineColor(4);
  fh_eloss_bb->SetMarkerColor(4);
  fh_eloss_bb->SetLineWidth(lwidth);
  fh_eloss_bb->GetXaxis()->SetTitle("eloss_bb, GeV");
  fh_eloss_bb->GetYaxis()->SetTitle("counter");
  fh_eloss_bb->Draw();
  
  c1->cd(3);
  fh_eloss_vl->SetLineColor(4);
  fh_eloss_vl->SetMarkerColor(4);
  fh_eloss_vl->SetLineWidth(lwidth);
  fh_eloss_vl->GetXaxis()->SetTitle("eloss_vl, GeV");
  fh_eloss_vl->GetYaxis()->SetTitle("counter");
  fh_eloss_vl->Draw();
  
  c1->cd(4);
  fh_qp_err->SetLineColor(4);
  fh_qp_err->SetMarkerColor(4);
  fh_qp_err->SetLineWidth(lwidth);
  fh_qp_err->GetXaxis()->SetTitle("qp_out - mc_qp_out, GeV");
  fh_qp_err->GetYaxis()->SetTitle("counter");
  fh_qp_err->Draw();
  
  c1->cd(5);
  fh_qp_err_calc->SetLineColor(4);
  fh_qp_err_calc->SetMarkerColor(4);
  fh_qp_err_calc->SetLineWidth(lwidth);
  fh_qp_err_calc->GetXaxis()->SetTitle("qp_err_calc, GeV");
  fh_qp_err_calc->GetYaxis()->SetTitle("counter");
  //fh_qp_err_calc->Fit("landau");
  fh_qp_err_calc->Draw();
  
  c1->cd(6);
  fh_eloss_err->SetLineColor(4);
  fh_eloss_err->SetMarkerColor(4);
  fh_eloss_err->SetLineWidth(lwidth);
  fh_eloss_err->GetXaxis()->SetTitle("bb_eloss - mc_dp, GeV");
  fh_eloss_err->GetYaxis()->SetTitle("counter");
  fh_eloss_err->Draw();
  
  c1->cd(7);
  fh_eloss_err_calc->SetLineColor(4);
  fh_eloss_err_calc->SetMarkerColor(4);
  fh_eloss_err_calc->SetLineWidth(lwidth);
  fh_eloss_err_calc->GetXaxis()->SetTitle("eloss_err_calc, GeV");
  fh_eloss_err_calc->GetYaxis()->SetTitle("counter");
  fh_eloss_err_calc->Draw();
  
  c1->cd(8);
  fh_pull_qp->SetLineColor(4);
  fh_pull_qp->SetMarkerColor(4);
  fh_pull_qp->SetLineWidth(lwidth);
  fh_pull_qp->GetXaxis()->SetTitle("pull_qp");
  fh_pull_qp->GetYaxis()->SetTitle("counter");
  fh_pull_qp->Fit("gaus");
  fh_pull_qp->Draw();
  
  c1->SaveAs(TString(outDir + "eloss.gif"));
  
  
  gStyle->SetPalette(1,0);
  
  TCanvas *c2 = new TCanvas("momentum","c2",1200,1000);
  c2->Divide(4, 2);
  c2->SetGrid();

  c2->cd(1);
  fhm_dp_mc->GetXaxis()->SetTitle("mc_mom_in - mc_mom_out, GeV");
  fhm_dp_mc->GetYaxis()->SetTitle("momentum, GeV");
  fhm_dp_mc->GetZaxis()->SetTitle("counter");
  fhm_dp_mc->Draw("COLZ");
  
  c2->cd(2);
  fhm_eloss_bb->GetXaxis()->SetTitle("eloss_bb, GeV");
  fhm_eloss_bb->GetYaxis()->SetTitle("momentum, GeV");
  fhm_eloss_bb->GetZaxis()->SetTitle("counter");
  fhm_eloss_bb->Draw("COLZ");
  
  c2->cd(3);
  fhm_eloss_vl->GetXaxis()->SetTitle("eloss_vl, GeV");
  fhm_eloss_vl->GetYaxis()->SetTitle("momentum, GeV");
  fhm_eloss_vl->GetZaxis()->SetTitle("counter");
  fhm_eloss_vl->Draw("COLZ");
  
  c2->cd(4);
  fhm_qp_err->GetXaxis()->SetTitle("qp_out - mc_qp_out, GeV");
  fhm_qp_err->GetYaxis()->SetTitle("momentum, GeV");
  fhm_qp_err->GetZaxis()->SetTitle("counter");
  fhm_qp_err->Draw("COLZ");
  
  c2->cd(5);
  fhm_qp_err_calc->GetXaxis()->SetTitle("qp_err_calc, GeV");
  fhm_qp_err_calc->GetYaxis()->SetTitle("momentum, GeV");
  fhm_qp_err_calc->GetZaxis()->SetTitle("counter");
  fhm_qp_err_calc->Draw("COLZ");
  
  c2->cd(6);
  fhm_eloss_err->GetXaxis()->SetTitle("bb_eloss - mc_dp, GeV");
  fhm_eloss_err->GetYaxis()->SetTitle("momentum, GeV");
  fhm_eloss_err->GetZaxis()->SetTitle("counter");
  fhm_eloss_err->Draw("COLZ");
  
  c2->cd(7);
  fhm_eloss_err_calc->GetXaxis()->SetTitle("eloss_err_calc, GeV");
  fhm_eloss_err_calc->GetYaxis()->SetTitle("momentum, GeV");
  fhm_eloss_err_calc->GetZaxis()->SetTitle("counter");
  fhm_eloss_err_calc->Draw("COLZ");
  
  c2->cd(8);
  fhm_pull_qp->GetXaxis()->SetTitle("pull_qp");
  fhm_pull_qp->GetYaxis()->SetTitle("momentum, GeV");
  fhm_pull_qp->GetZaxis()->SetTitle("counter");
  fhm_pull_qp->Draw("COLZ");
  
  c2->SaveAs(TString(outDir + "eloss_mom.gif"));
  
  TCanvas *c3 = new TCanvas("teta","c3",1200,1000);
  c3->Divide(2, 2);
 
  c3->cd(1);
  fh_teta_calc->SetLineColor(4);
  fh_teta_calc->SetMarkerColor(4);
  fh_teta_calc->SetLineWidth(lwidth);
  fh_teta_calc->GetXaxis()->SetTitle("teta_calc");
  fh_teta_calc->GetYaxis()->SetTitle("counter");
  fh_teta_calc->Draw();
  
  c3->cd(2);
  fhm_teta_calc->GetXaxis()->SetTitle("teta_calc");
  fhm_teta_calc->GetYaxis()->SetTitle("momentum, GeV");
  fhm_teta_calc->GetZaxis()->SetTitle("counter");
  fhm_teta_calc->Draw("COLZ");
 
  c3->cd(3);
  fh_teta_mc->SetLineColor(4);
  fh_teta_mc->SetMarkerColor(4);
  fh_teta_mc->SetLineWidth(lwidth);
  fh_teta_mc->GetXaxis()->SetTitle("teta_mc");
  fh_teta_mc->GetYaxis()->SetTitle("counter");
  fh_teta_mc->Draw();
  
  c3->cd(4);
  fhm_teta_mc->GetXaxis()->SetTitle("teta_mc");
  fhm_teta_mc->GetYaxis()->SetTitle("momentum, GeV");
  fhm_teta_mc->GetZaxis()->SetTitle("counter");
  fhm_teta_mc->Draw("COLZ");
  
  c3->SaveAs(TString(outDir + "teta.gif"));
  
//  c2->cd(1);
//  fh_pout_mc->SetLineColor(4);
//  fh_pout_mc->SetMarkerColor(4);
//  fh_pout_mc->SetLineWidth(lwidth);
//  fh_pout_mc->GetXaxis()->SetTitle("mc_mom_out, GeV");
//  fh_pout_mc->GetYaxis()->SetTitle("counter");
//  fh_pout_mc->Draw();
//  
//  c2->cd(2);
//  fh_pout_calc->SetLineColor(4);
//  fh_pout_calc->SetMarkerColor(4);
//  fh_pout_calc->SetLineWidth(lwidth);
//  fh_pout_calc->GetXaxis()->SetTitle("mc_mom_in - mc_eloss, GeV");
//  fh_pout_calc->GetYaxis()->SetTitle("counter");
//  fh_pout_calc->Draw();
//  
//  c2->cd(3);
//  fh_pout_calc_e->SetLineColor(4);
//  fh_pout_calc_e->SetMarkerColor(4);
//  fh_pout_calc_e->SetLineWidth(lwidth);
//  fh_pout_calc_e->GetXaxis()->SetTitle("mc_energy_in - mc_eloss, GeV");
//  fh_pout_calc_e->GetYaxis()->SetTitle("counter");
//  fh_pout_calc_e->Draw();
  
//  TLegend* l1 = new TLegend(0.1,0.91,0.9,0.99);
//  l1->SetHeader("Chi2/NDF");
//  l1->SetFillColor(0);
//  l1->SetLineColor(4);
//  l1->SetTextColor(4);
//  l1->AddEntry(hChi2Muons,"primary muons","l");  
//  l1->AddEntry(hChi2Other,"other","l"); 
//  l1->Draw();
  
}

 
  
  