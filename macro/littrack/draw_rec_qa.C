TString dir  = "/home/d/andrey/events/newmuch/large/10mu/mu/";
TFile *file = new TFile(dir + "much.tracks.root");

Int_t lsGhost = 2;
Int_t lsTrue = 1;
Int_t width = 3;
Int_t size = 3;
Int_t colorGhost = 2;
Int_t colorTrue = 4;

void draw_rec_qa()
{
//	gStyle->SetOptStat("");
//	gStyle->SetOptFit(0);
//	gStyle->SetOptTitle(0);

	draw_eff();
    draw_params();
    draw_params_2D();
}

void draw_eff()
{
  TCanvas *c1 = new TCanvas("efficiency","efficiency",1200,1000);
  c1->SetGrid();
  hMomEffAll->Scale(100);
  hMomEffAll->SetLineColor(4);
  hMomEffAll->SetMarkerColor(4);
  hMomEffAll->SetLineWidth(4.);
  hMomEffAll->SetMarkerStyle(4);
  hMomEffAll->SetMarkerSize(3.0);
  hMomEffAll->GetXaxis()->SetTitle("Momentum, GeV/c");
  hMomEffAll->GetYaxis()->SetTitle("Efficiency, %");
  hMomEffAll->SetMaximum(100);
  hMomEffAll->SetMinimum(0);

  hMomEffMuons->Scale(100);
  hMomEffMuons->SetLineColor(2);
  hMomEffMuons->SetMarkerColor(2);
  hMomEffMuons->SetLineWidth(4.);
  hMomEffMuons->SetMarkerStyle(8);
  hMomEffMuons->SetMarkerSize(3.0);

  hMomEffElectrons->Scale(100);
  hMomEffElectrons->SetLineColor(2);
  hMomEffElectrons->SetMarkerColor(2);
  hMomEffElectrons->SetLineWidth(4.);
  hMomEffElectrons->SetMarkerStyle(8);
  hMomEffElectrons->SetMarkerSize(3.0);

  hMomEffAll->Draw();
  hMomEffMuons->Draw("SAME");
//  hMomEffElectrons->Draw("SAME");

  TLegend* l1 = new TLegend(0.5,0.7,0.9,0.9);
  l1->SetHeader("Efficiency");
  l1->AddEntry(hMomEffAll,"all tracks","lp");
  l1->AddEntry(hMomEffMuons,"muon tracks","lp");
//  l1->AddEntry(hMomEffElectrons,"electron tracks","lp");
  l1->Draw();
}

void draw_params()
{
  TCanvas *c1 = new TCanvas("parameters","parameters", 1200, 1000);
  c1->Divide(3,2);
  c1->SetGrid();

  c1->cd(1);
  draw_param_hist(hChi2True, hChi2Ghost, "Chi2/NDF", "Counter", "Chi2/NDF");
  c1->cd(2);
  draw_param_hist(hMomTrue, hMomGhost, "Momentum", "Counter", "Momentum, GeV/c");
  c1->cd(3);
  draw_param_hist(hNofHitsTrue, hNofHitsGhost, "Number of hits", "Counter", "Number of hits");
  c1->cd(4);
  draw_param_hist(hRadialPosTrue, hRadialPosGhost, "Radial position", "Counter", "Radial position, cm");
  c1->cd(5);
  draw_param_hist(hLastPlaneIdTrue, hLastPlaneIdGhost, "Last plane id", "Counter", "Last plane id");
  c1->cd(6);
  draw_nof_hits();
}

void draw_params_2D()
{
	gStyle->SetPalette(1, 0);

  Int_t lsGhost = 2;
  Int_t lsTrue = 1;
  Int_t width = 3;
  Int_t size = 3;
  Int_t colorGhost = 2;
  Int_t colorTrue = 4;

  Double_t max;

  TCanvas *c1 = new TCanvas("parameters2D","parameters2D", 1200, 1000);
  c1->Divide(2,1);

  c1->cd(1);
  c1->SetGrid();

  hMomChi2True->GetXaxis()->SetTitle("momentum, GeV");
  hMomChi2True->GetYaxis()->SetTitle("chi2");
  hMomChi2True->GetZaxis()->SetTitle("counter");
  hMomChi2True->SetMarkerColor(colorTrue);
  hMomChi2True->SetLineColor(colorTrue);
  hMomChi2True->SetLineWidth(width);
  hMomChi2True->SetMarkerSize(size);
  hMomChi2True->Draw("COLZ");

  c1->cd(2);
  hMomChi2Ghost->GetXaxis()->SetTitle("momentum, GeV");
  hMomChi2Ghost->GetYaxis()->SetTitle("chi2");
  hMomChi2Ghost->GetZaxis()->SetTitle("counter");
  hMomChi2Ghost->SetMarkerColor(colorGhost);
  hMomChi2Ghost->SetLineColor(colorGhost);
  hMomChi2Ghost->SetLineWidth(width);
  hMomChi2Ghost->SetMarkerSize(size);
  hMomChi2Ghost->Draw("COLZ");
}

void draw_nof_hits()
{
  hNofHits->SetLineColor(1);
  hNofHits->SetMarkerColor(1);
  hNofHits->SetLineWidth(width);
  hNofHits->SetLineStyle(3);
  hNofHits->GetXaxis()->SetTitle("nof hits");
  hNofHits->GetYaxis()->SetTitle("counter");

  hNofGoodHits->SetLineColor(colorTrue);
  hNofGoodHits->SetMarkerColor(colorTrue);
  hNofGoodHits->SetLineWidth(width);
  hNofGoodHits->SetLineStyle(lsTrue);

  hNofBadHits->SetLineColor(colorGhost);
  hNofBadHits->SetMarkerColor(colorGhost);
  hNofBadHits->SetLineWidth(width);
  hNofBadHits->SetLineStyle(lsGhost);

  gPad->SetLogy();
  hNofGoodHits->Draw();
  hNofHits->Draw("SAME");
  hNofBadHits->Draw("SAME");

  TLegend* l1 = new TLegend(0.5,0.7,0.9,0.9);
  l1->SetHeader("nof hits");
  l1->AddEntry(hNofHits,"nof hits","l");
  l1->AddEntry(hNofGoodHits,"nof good hits","l");
  l1->AddEntry(hNofBadHits,"nof bad hits","l");
  l1->Draw();
}

void draw_param_hist(
		TH1* h1,
		TH1* h2,
		const std::string& header,
		const std::string& titleX,
		const std::string& titleY)
{
  h1->SetLineColor(colorTrue);
  h1->SetMarkerColor(colorTrue);
  h1->SetLineWidth(width);
  h1->SetLineStyle(lsTrue);
  h1->GetXaxis()->SetTitle(titleX.c_str());
  h1->GetYaxis()->SetTitle(titleY.c_str());

  h2->SetLineColor(colorGhost);
  h2->SetMarkerColor(colorGhost);
  h2->SetLineWidth(width);
  h2->SetLineStyle(lsGhost);

  Double_t max = (h1->GetMaximum() > h2->GetMaximum()) ? h1->GetMaximum() : h2->GetMaximum();
  h1->SetMaximum(max);
  gPad->SetLogy();
  h1->Draw();
  h2->Draw("SAME");

  TLegend* l1 = new TLegend(0.5,0.7,0.9,0.9);
  l1->SetHeader(header.c_str());
  l1->AddEntry(h1,"true tracks","l");
  l1->AddEntry(h2,"ghost tracks","l");
  l1->Draw();
}

