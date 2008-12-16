TString engine = "geant4/";
TString setup = "10gev10X0iron/";
TString dir = "/d/cbm02/andrey/events/eloss/" + engine + setup;
TString fileName = dir + "eloss.ana.root";
TString outDir = "./" + engine + setup;

void draw_eloss_ana()
{
	gStyle->SetOptStat("");
	gStyle->SetOptFit(0);
	gStyle->SetOptTitle(0);
	//gStyle->SetOptFit(0001);

	TFile *file = new TFile(fileName); 
	
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/draw_hist.C");
  
	Double_t minEloss = 0.;
	Double_t maxEloss = 10.0;
	Double_t minMom = 0.1;
	Double_t maxMom = 35.;
 	
	Double_t lwidth = 2.;
	Double_t msize = 2.;
	
  
	TCanvas *c1 = new TCanvas("energy loss","c1",1200,1000);
	c1->Divide(2, 2);
	c1->SetGrid();
    
	c1->cd(1);
	draw_hist_1D(fh_eloss_mc, "energy loss, GeV", "counter", 
			  kBlue, lwidth, 1, msize, kDot, true, true, "");
	fh_eloss_mc->Fit("landau");
	
	draw_hist_1D(fh_eloss_lit, "energy loss, GeV", "counter", 
			  kRed, lwidth, 3, msize, kDot, true, true, "SAME");
//  
//  fh_eloss_bb->SetLineColor(2);
//  fh_eloss_bb->SetLineStyle(3);
//  fh_eloss_bb->SetLineWidth(lwidth);
//  fh_eloss_bb->Draw("SAME");
//  
//  fh_eloss_bb_simple->SetLineColor(8);
//  fh_eloss_bb_simple->SetLineStyle(7);
//  fh_eloss_bb_simple->SetLineWidth(lwidth);
//  fh_eloss_bb_simple->Draw("SAME");
//  
//  fh_eloss_mpv->SetLineColor(12);
//  fh_eloss_mpv->SetLineStyle(4);
//  fh_eloss_mpv->SetLineWidth(lwidth);
//  fh_eloss_mpv->Draw("SAME");
  
  Double_t max1 = fh_eloss_mc->GetMaximum();
  Double_t max2 = fh_eloss_lit->GetMaximum();
  fh_eloss_mc->SetMaximum((max1 > max2)? max1 : max2);
    
  TLegend* l1_1 = new TLegend(0.4,0.5,0.99,0.9);
  l1_1->SetHeader("Energy Losses");
  l1_1->AddEntry(fh_eloss_mc,"TGeant4","l");
  l1_1->AddEntry(fh_eloss_lit,"Lit","l");
//  l1_1->AddEntry(fh_eloss_bb,"eloss_bb","l");
//  l1_1->AddEntry(fh_eloss_bb_simple,"eloss_bb_simple","l");
//  l1_1->AddEntry(fh_eloss_mpv,"eloss_mpv","l");
  l1_1->Draw();
  gPad->SetLogx();
  gPad->SetLogy();

  
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
  
  TLegend* l1_2 = new TLegend(0.4,0.5,0.99,0.9);
  l1_2->SetHeader("q/p errors");
  l1_2->AddEntry(fh_eloss_mc,"qp_out - mc_qp_out","l");
  l1_2->AddEntry(fh_eloss_bb,"qp_err_calc","l");
  l1_2->Draw();
  
  
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
    
  fhm_eloss_lit->SetMarkerColor(2);
  fhm_eloss_lit->SetLineColor(2);
  fhm_eloss_lit->SetLineWidth(lwidth);
  fhm_eloss_lit->Draw("SCATSAME");
//  
//  fhm_eloss_bb->SetMarkerColor(2);
//  fhm_eloss_bb->SetLineColor(2);
//  fhm_eloss_bb->SetLineWidth(lwidth);
//  fhm_eloss_bb->Draw("SCATSAME");
//      
//  fhm_eloss_bb_simple->SetMarkerColor(8);
//  fhm_eloss_bb_simple->SetLineColor(8);
//  fhm_eloss_bb_simple->SetLineWidth(lwidth);
//  fhm_eloss_bb_simple->Draw("SCATSAME");
//  
//  fhm_eloss_mpv->SetMarkerColor(12);
//  fhm_eloss_mpv->SetLineColor(12);
//  fhm_eloss_mpv->SetLineWidth(lwidth);
//  fhm_eloss_mpv->Draw("SCATSAME");
  
  TLegend* l2_1 = new TLegend(0.4,0.5,0.99,0.9);
  l2_1->SetHeader("Energy Loss vs. momentum");
  l2_1->AddEntry(fhm_eloss_mc,"TGeant4","l");
  l2_1->AddEntry(fhm_eloss_lit,"Lit","l");
//  l2_1->AddEntry(fhm_eloss_bb,"eloss_bb","l");
//  l2_1->AddEntry(fhm_eloss_bb_simple,"eloss_bb_simple","l");
//  l2_1->AddEntry(fhm_eloss_mpv,"eloss_mpv","l");
  l2_1->Draw();
  
  gPad->SetLogx();
  gPad->SetLogy();
  
  c2->cd(2);
  TProfile* profile1 = fhm_eloss_mc->ProfileX();
  TProfile* profile2 = fhm_eloss_lit->ProfileX();
//  TProfile* profile2 = fhm_eloss_bb->ProfileX();
//  TProfile* profile3 = fhm_eloss_bb_simple->ProfileX();
//  TProfile* profile4 = fhm_eloss_mpv->ProfileX();
  profile1->SetMinimum(0.01);
  profile1->SetMaximum(maxEloss);
  profile1->SetLineWidth(lwidth);
  profile2->SetLineWidth(lwidth);
  
//  profile2->SetLineWidth(lwidth);
//  profile3->SetLineWidth(lwidth);
//  profile4->SetLineWidth(lwidth);
  profile1->Draw();
  profile2->Draw("SAME");
//  profile3->Draw("SAME");
//  profile4->Draw("SAME");  
  
  TLegend* l2_2 = new TLegend(0.4,0.5,0.99,0.9);
  l2_2->SetHeader("Mean energy loss vs. momentum");
  l2_2->AddEntry(profile1,"TGeant4","l");
  l2_2->AddEntry(profile2,"Lit","l");
//  l2_2->AddEntry(fhm_eloss_bb,"eloss_bb","l");
//  l2_2->AddEntry(fhm_eloss_bb_simple,"eloss_bb_simple","l");
//  l2_2->AddEntry(fhm_eloss_mpv,"eloss_mpv","l");
   l2_2->Draw();
    
 gPad->SetLogx();
 gPad->SetLogy(); 
 
  
  c2->cd(3);
  fhm_qp_err_mc->GetXaxis()->SetTitle("momentum, GeV/c");
  fhm_qp_err_mc->GetYaxis()->SetTitle("q/p error, GeV/c");
  fhm_qp_err_mc->GetZaxis()->SetTitle("counter");
  fhm_qp_err_mc->SetMarkerColor(2);
  fhm_qp_err_mc->SetLineColor(2);
  fhm_qp_err_mc->Draw("SCAT");

  fhm_qp_err_calc->SetMarkerColor(4);
  fhm_qp_err_calc->SetLineColor(4);
  fhm_qp_err_calc->Draw("SCATSAME");
  
  TLegend* l2_3 = new TLegend(0.4,0.5,0.99,0.9);
  l2_3->SetHeader("qp errors vs momentum");
  l2_3->AddEntry(fhm_qp_err_mc,"residual qp","l");
  l2_3->AddEntry(fhm_qp_err_calc,"calc error","l");
  l2_3->Draw();
  
  c2->cd(4);
  fhm_pull_qp->GetXaxis()->SetTitle("momentum, GeV/c");
  fhm_pull_qp->GetYaxis()->SetTitle("pull_qp");
  fhm_pull_qp->GetZaxis()->SetTitle("counter");
  fhm_pull_qp->Draw("COLZ");
  
  c2->SaveAs(TString(outDir + "eloss_mom.gif"));
  

  gStyle->SetPalette(1,0);
  TCanvas *c3 = new TCanvas("theta","c3",1200,1000);
  c3->Divide(1, 1);
  //c3->SetGrid();

  c3->cd(1);
  fhm_theta_mc->GetXaxis()->SetTitle("momentum, GeV/c");
  fhm_theta_mc->GetYaxis()->SetTitle("theta, rad");
  fhm_theta_mc->GetZaxis()->SetTitle("counter");
  fhm_theta_mc->SetMarkerColor(4);
  fhm_theta_mc->SetLineColor(4);
  fhm_theta_mc->SetLineWidth(lwidth);
  fhm_theta_mc->Draw("SCAT");
    
  fhm_theta_calc->SetMarkerColor(2);
  fhm_theta_calc->SetLineColor(2);
  fhm_theta_calc->SetLineWidth(lwidth);
  fhm_theta_calc->Draw("SCATSAME");
  
  TProfile* profile_theta_mc = fhm_theta_mc->ProfileX();
  profile_theta_mc->SetMarkerColor(kGreen+3);
  profile_theta_mc->SetLineColor(kGreen+3);
  profile_theta_mc->SetLineWidth(lwidth);
  profile_theta_mc->Draw("SAME");
  
  TProfile* profile_theta_calc = fhm_theta_calc->ProfileX();
  profile_theta_calc->SetMarkerColor(kBlack);
  profile_theta_calc->SetLineColor(kBlack);
  profile_theta_calc->SetLineWidth(lwidth);
  profile_theta_calc->Draw("SAME");
  
  TLegend* l_theta = new TLegend(0.4,0.5,0.99,0.9);
  l_theta->SetHeader("Theta vs. momentum");
  l_theta->AddEntry(fhm_theta_mc,"MC","l");
  l_theta->AddEntry(fhm_theta_calc,"Calculated","l");
  l_theta->AddEntry(profile_theta_mc,"MC (mean)","l");
  l_theta->AddEntry(profile_theta_calc,"Calculated (mean)","l");
  l_theta->Draw();
  
  c3->cd(2);
  TProfile* projectionY_theta_mc = fhm_theta_mc->ProjectionY("_pfx", 10, 12);
  projectionY_theta_mc->SetMarkerColor(kGreen+3);
  projectionY_theta_mc->SetLineColor(kGreen+3);
  projectionY_theta_mc->SetLineWidth(lwidth);
  projectionY_theta_mc->Draw("");
  
  
  c3->cd(1);
  draw_hist_1D(fh_theta_mc, "projected scattering angle, rad", "counter", 
		  kBlue, lwidth, 1, msize, kDot, false, true, "");
  fh_theta_mc->Fit("gaus");
  //draw_hist_1D(fh_theta_calc, "projected scattering angle, rad", "counter", 
  //		  kRed, lwidth, 1, msize, kDot, false, true, "SAME");
 
  stringstream oss1;
  oss1.precision(3);
  oss1 << "Geant4, #sigma = " << fh_theta_mc->GetFunction("gaus")->GetParameter(2);
  TLegend* l1 = new TLegend(0.5,0.7,0.9,0.9);
  l1->AddEntry(NULL,oss1.str().c_str(),"");  
  l1->AddEntry(NULL,"Lit, #sigma = 0.0047",""); 
  l1->Draw();
  
  TLegend* l2 = new TLegend(0.5,0.5,0.9,0.9);
  l2->SetHeader("#splitline{Projected scattering angle;}{10 X_{0} of iron, p=10 Gev/c}");
  l2->Draw();
  
  c3->cd(4);
	
}

  