TString system  = "auau";  
TString beam    = "25gev";  
TString trigger = "centr";
TString particle = "omega";
  
//TString dir = "/d/cbm02/andrey/events/much/10stations/new/";
//TString dir = "/d/cbm02/andrey/events/much/10stations/10mu/mu_urqmd/";
//TString fileName = dir + beam + "/" 
//                   + particle + "/much.tracks." + system + "." + beam + "." 
//                   + particle + "." + trigger + ".root";
  
//TFile *file = new TFile(fileName); 

TString dir  = "/d/cbm02/andrey/events/trd/standard/e_urqmd";
TFile *file = new TFile(dir + "/trd.reco.auau.25gev.e.centr.root"); 


Int_t lsGhost = 2;
Int_t lsTrue = 1;
Int_t width = 3;
Int_t size = 3;
Int_t colorGhost = 2;
Int_t colorTrue = 4;

void draw_robust_ana()
{
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/draw_hist.C");
	
	gStyle->SetOptStat("");
	gStyle->SetOptFit(0);
	gStyle->SetOptTitle(0);
	gStyle->SetPalette(1, 0);
	
	draw_nof_hits();
	draw_err_fits();
}

void draw_nof_hits()
{
  TCanvas *c1 = new TCanvas("parameters2D","parameters2D", 1200, 1000);
  c1->Divide(2,2);

  c1->cd(1); 
  hStationNofHits->GetXaxis()->SetTitle("plane id");
  hStationNofHits->GetYaxis()->SetTitle("nof hits");
  hStationNofHits->GetZaxis()->SetTitle("counter");
  hStationNofHits->Draw("LEGO");
  
  c1->cd(2); 
  hIterChi2->GetXaxis()->SetTitle("iteration");
  hIterChi2->GetYaxis()->SetTitle("chi2/ndf");
  hIterChi2->GetZaxis()->SetTitle("counter");
  hIterChi2->Draw("LEGO");
  
  c1->cd(3); 
  hIterWeight->GetXaxis()->SetTitle("iteration");
  hIterWeight->GetYaxis()->SetTitle("weight");
  hIterWeight->GetZaxis()->SetTitle("counter");
  hIterWeight->Draw("LEGO");  
}

void draw_err_fits()
{
	TCanvas *c1 = new TCanvas("errfit","errfit", 1200, 1000);

	c1->Divide(2,2);
	c1->cd(1); 
	draw_hist_1D(hEFIter, "iteration", "counter", kBlack, 2, 1, 1, kDot, false, true, "");
	c1->cd(2); 
	draw_hist_1D(hEFNofHits, "iteration", "counter", kBlack, 2, 1, 1, kDot, false, true, "");
	c1->cd(3); 
	hEFIterNofHits->GetXaxis()->SetTitle("iteration");
	hEFIterNofHits->GetYaxis()->SetTitle("chi2/ndf");
	hEFIterNofHits->GetZaxis()->SetTitle("counter");
	hEFIterNofHits->Draw("LEGO");
}

