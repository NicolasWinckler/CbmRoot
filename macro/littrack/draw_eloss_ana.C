
draw_eloss_ana()
{

  TString engine = "geant3/";
  TString setup = "1-10gev_20cm/";
  
  TString dir = "/d/cbm02/andrey/events/eloss/" + engine + setup;
  TString fileName = dir + "eloss.ana.root";
  
  TString outDir = "./" + engine + setup;
  
  TFile *file = new TFile(fileName); 
  
  Int_t nBins = 200;
  Int_t nBinsMom = 100;
  Double_t minEloss = 0.;
  Double_t maxEloss = 1.;
  Double_t minMom = 0.1;
  Double_t maxMom = 10.;
  Double_t minErr = -0.25;
  Double_t maxErr = 0.25;
  
  
  TCanvas *c1 = new TCanvas("energy loss","c1",1200,1000);
  c1->Divide(2, 2);
  c1->SetGrid();
    
  gStyle->SetOptFit(0001);
  
  Double_t lwidth = 2.;
  
  c1->cd(1);
  fh_eloss_mc->SetLineColor(4);
  fh_eloss_mc->SetLineWidth(lwidth);
  fh_eloss_mc->GetXaxis()->SetTitle("energy loss, GeV");
  fh_eloss_mc->GetYaxis()->SetTitle("counter");
  fh_eloss_mc->Fit("landau");
  fh_eloss_mc->Draw();
  
  fh_eloss_bb->SetLineColor(2);
  fh_eloss_bb->SetLineStyle(3);
  fh_eloss_bb->SetLineWidth(lwidth);
  fh_eloss_bb->Draw("SAME");
  
  fh_eloss_bb_simple->SetLineColor(8);
  fh_eloss_bb_simple->SetLineStyle(7);
  fh_eloss_bb_simple->SetLineWidth(lwidth);
  fh_eloss_bb_simple->Draw("SAME");
  
  fh_eloss_mpv->SetLineColor(12);
  fh_eloss_mpv->SetLineStyle(4);
  fh_eloss_mpv->SetLineWidth(lwidth);
  fh_eloss_mpv->Draw("SAME");
  
  Double_t max1 = fh_eloss_mc->GetMaximum();
  Double_t max2 = fh_eloss_bb->GetMaximum();
  Double_t max3 = fh_eloss_bb_simple->GetMaximum();
  Double_t max = (max1 > max2)? max1 : max2;
  max = (max > max3)? max : max3;
  fh_eloss_mc->SetMaximum(max);
    
  TLegend* l1 = new TLegend(0.4,0.5,0.99,0.9);
  l1->SetHeader("Energy Losses");
  l1->AddEntry(fh_eloss_mc,"mc_mom_in - mc_mom_out","l");
  l1->AddEntry(fh_eloss_bb,"eloss_bb","l");
  l1->AddEntry(fh_eloss_bb_simple,"eloss_bb_simple","l");
  l1->AddEntry(fh_eloss_mpv,"eloss_mpv","l");
  l1->Draw();
  
  
  c1->cd(2);
  fh_qp_err_mc->SetLineColor(4);
  fh_qp_err_mc->SetLineStyle(1);
  fh_qp_err_mc->SetLineWidth(lwidth);
  fh_qp_err_mc->GetXaxis()->SetTitle("q/p error, GeV");
  fh_qp_err_mc->GetYaxis()->SetTitle("counter");
  fh_qp_err_mc->Draw();
  
  fh_qp_err_calc->SetLineColor(2);
  fh_qp_err_calc->SetLineStyle(8);
  fh_qp_err_calc->SetLineWidth(lwidth);
  fh_qp_err_calc->Draw("SAME");
  
  Double_t max1 = fh_qp_err_mc->GetMaximum();
  Double_t max2 = fh_qp_err_calc->GetMaximum();
  Double_t max = (max1 > max2)? max1 : max2;
  fh_qp_err_mc->SetMaximum(max);
  
  TLegend* l2 = new TLegend(0.4,0.5,0.99,0.9);
  l2->SetHeader("q/p errors");
  l2->AddEntry(fh_eloss_mc,"qp_out - mc_qp_out","l");
  l2->AddEntry(fh_eloss_bb,"qp_err_calc","l");
  l2->Draw();
  
  
  c1->cd(3);
  fh_pull_qp->SetLineColor(4);
  fh_pull_qp->SetMarkerColor(4);
  fh_pull_qp->SetLineWidth(lwidth);
  fh_pull_qp->GetXaxis()->SetTitle("pull_qp");
  fh_pull_qp->GetYaxis()->SetTitle("counter");
//  fh_pull_qp->Fit("gaus");
  fh_pull_qp->Draw();
  
  c1->SaveAs(TString(outDir + "eloss.gif"));
  
  
  gStyle->SetPalette(1,0);
  
  TCanvas *c2 = new TCanvas("momentum","c2",1200,1000);
  c2->Divide(2, 2);
  c2->SetGrid();

  c2->cd(1);
  fhm_eloss_mc->GetXaxis()->SetTitle("momentum, GeV");
  fhm_eloss_mc->GetYaxis()->SetTitle("energy loss, GeV");
  fhm_eloss_mc->GetZaxis()->SetTitle("counter");
  fhm_eloss_mc->SetMarkerColor(4);
  fhm_eloss_mc->SetLineColor(4);
  fhm_eloss_mc->SetLineWidth(lwidth);
  fhm_eloss_mc->Draw("SCAT");
    
  fhm_eloss_bb->SetMarkerColor(2);
  fhm_eloss_bb->SetLineColor(2);
  fhm_eloss_bb->SetLineWidth(lwidth);
  fhm_eloss_bb->Draw("SCATSAME");
      
  fhm_eloss_bb_simple->SetMarkerColor(8);
  fhm_eloss_bb_simple->SetLineColor(8);
  fhm_eloss_bb_simple->SetLineWidth(lwidth);
  fhm_eloss_bb_simple->Draw("SCATSAME");
  
  fhm_eloss_mpv->SetMarkerColor(12);
  fhm_eloss_mpv->SetLineColor(12);
  fhm_eloss_mpv->SetLineWidth(lwidth);
  fhm_eloss_mpv->Draw("SCATSAME");
  
  TLegend* l3 = new TLegend(0.4,0.5,0.99,0.9);
  l3->SetHeader("Energy Loss vs. momentum");
  l3->AddEntry(fhm_eloss_mc,"mc_mom_in - mc_mom_out","l");
  l3->AddEntry(fhm_eloss_bb,"eloss_bb","l");
  l3->AddEntry(fhm_eloss_bb_simple,"eloss_bb_simple","l");
  l3->AddEntry(fhm_eloss_mpv,"eloss_mpv","l");
  l3->Draw();
  
  c2->cd(2);
  TProfile* profile1 = fhm_eloss_mc->ProfileX();
  TProfile* profile2 = fhm_eloss_bb->ProfileX();
  TProfile* profile3 = fhm_eloss_bb_simple->ProfileX();
  TProfile* profile4 = fhm_eloss_mpv->ProfileX();
  profile1->SetMinimum(minEloss);
  profile1->SetMaximum(maxEloss);
  profile1->SetLineWidth(lwidth);
  profile2->SetLineWidth(lwidth);
  profile3->SetLineWidth(lwidth);
  profile4->SetLineWidth(lwidth);
  profile1->Draw();
  profile2->Draw("SAME");
  profile3->Draw("SAME");
  profile4->Draw("SAME");
  
  c2->cd(3);
  fhm_qp_err_mc->GetXaxis()->SetTitle("momentum, GeV");
  fhm_qp_err_mc->GetYaxis()->SetTitle("q/p error, GeV");
  fhm_qp_err_mc->GetZaxis()->SetTitle("counter");
  fhm_qp_err_mc->SetMarkerColor(2);
  fhm_qp_err_mc->SetLineColor(2);
  fhm_qp_err_mc->Draw("SCAT");

  fhm_qp_err_calc->SetMarkerColor(4);
  fhm_qp_err_calc->SetLineColor(4);
  fhm_qp_err_calc->Draw("SCATSAME");
  
  TLegend* l4 = new TLegend(0.4,0.5,0.99,0.9);
  l4->SetHeader("qp errors vs momentum");
  l4->AddEntry(fhm_qp_err_mc,"residual qp","l");
  l4->AddEntry(fhm_qp_err_calc,"calc error","l");
  l4->Draw();
  
  c2->cd(4);
  fhm_pull_qp->GetXaxis()->SetTitle("momentum, GeV");
  fhm_pull_qp->GetYaxis()->SetTitle("pull_qp");
  fhm_pull_qp->GetZaxis()->SetTitle("counter");
  fhm_pull_qp->Draw("COLZ");
  
  c2->SaveAs(TString(outDir + "eloss_mom.gif"));
  
}
  
  