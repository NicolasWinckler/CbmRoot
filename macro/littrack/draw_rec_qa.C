TString dir  = "/d/cbm02/andrey/events/much/compact10/10mu/mu/";
TFile *file = new TFile(dir + "much.tracks.root");

Int_t lineWidth = 3;
Int_t markerSize = 3;

Int_t lineStyleGhost = 2;
Int_t lineStyleTrue = 1;
Int_t colorGhost = 2;
Int_t colorTrue = 4;

void draw_rec_qa()
{
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/draw_hist.C");

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

  hMomEffAll->SetMaximum(1);
  hMomEffAll->SetMinimum(0);
  draw_hist_1D(hMomEffAll, "Momentum, GeV/c", "Efficiency",
		  kBlue, lineWidth, 1, markerSize, kCircle, false, false, "");

  draw_hist_1D(hMomEffMuons, "Momentum, GeV/c", "Efficiency",
  		  kRed, lineWidth, 1, markerSize, kFullDotLarge, false, false, "SAME");

//  draw_hist_1D(hMomEffElectrons, "Momentum, GeV/c", "Efficiency",
//    		  kRed, lineWidth, 1, markerSize, kFullDotLarge, false, false, "SAME");

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
  c1->SetGrid();

  c1->cd(1);
  hMomChi2True->GetXaxis()->SetTitle("momentum, GeV");
  hMomChi2True->GetYaxis()->SetTitle("chi2");
  hMomChi2True->GetZaxis()->SetTitle("counter");
  hMomChi2True->SetMarkerColor(colorTrue);
  hMomChi2True->SetLineColor(colorTrue);
  hMomChi2True->SetLineWidth(lineWidth);
  hMomChi2True->SetMarkerSize(markerSize);
  hMomChi2True->Draw("COLZ");

  c1->cd(2);
  hMomChi2Ghost->GetXaxis()->SetTitle("momentum, GeV");
  hMomChi2Ghost->GetYaxis()->SetTitle("chi2");
  hMomChi2Ghost->GetZaxis()->SetTitle("counter");
  hMomChi2Ghost->SetMarkerColor(colorGhost);
  hMomChi2Ghost->SetLineColor(colorGhost);
  hMomChi2Ghost->SetLineWidth(lineWidth);
  hMomChi2Ghost->SetMarkerSize(markerSize);
  hMomChi2Ghost->Draw("COLZ");
}

void draw_nof_hits()
{
	draw_hist_1D(hNofHits, "nof hits", "counter",
			  kBlack, lineWidth, 3, markerSize, kDot, false, true, "");

	draw_hist_1D(hNofGoodHits, "nof hits", "counter",
			colorTrue, lineWidth, lineStyleTrue, markerSize, kDot, false, true, "SAME");

	draw_hist_1D(hNofBadHits, "nof hits", "counter",
			colorGhost, lineWidth, lineStyleGhost, markerSize, kDot, false, true, "SAME");

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
	draw_hist_1D(h1, titleX.c_str(), titleY.c_str(),
			  colorTrue, lineWidth, lineStyleTrue, markerSize, kDot, false, true, "");
	draw_hist_1D(h2, titleX.c_str(), titleY.c_str(),
			  colorGhost, lineWidth, lineStyleGhost, markerSize, kDot, false, true, "SAME");

	Double_t max = (h1->GetMaximum() > h2->GetMaximum()) ? h1->GetMaximum() : h2->GetMaximum();
  	h1->SetMaximum(max);

  	TLegend* l1 = new TLegend(0.5,0.7,0.9,0.9);
  	l1->SetHeader(header.c_str());
  	l1->AddEntry(h1,"true tracks","l");
  	l1->AddEntry(h2,"ghost tracks","l");
  	l1->Draw();
}

