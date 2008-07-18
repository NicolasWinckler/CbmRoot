draw_much_rec_qa()
{

  TString system  = "auau";  
  TString beam    = "25gev";  
  TString trigger = "centr";
  TString particle = "mu";
  
  //TString dir = "/d/cbm02/andrey/events/much/10stations/new/";
  TString dir = "/d/cbm02/andrey/events/much/10stations/10mu/mu/";
  TString fileName = dir + beam + "/" 
                    + particle + "/much.tracks." + system + "." + beam + "." 
                    + particle + "." + trigger + ".root";
  
  TFile *file = new TFile(fileName); 
  
  TCanvas *c1 = new TCanvas("efficiency all","c1",1200,1000);
  c1->SetGrid();
  hMomEffAll->Scale(100);
  hMomEffAll->SetLineColor(4);
  hMomEffAll->SetMarkerColor(4);
  hMomEffAll->SetLineWidth(4.);
  //hMomEffAll->SetMarkerStyle(2);
  //hMomEffAll->SetMarkerSize(3.0);
  hMomEffAll->GetXaxis()->SetTitle("Momentum, GeV");
  hMomEffAll->GetYaxis()->SetTitle("Efficiency, %");
  hMomEffAll->SetMaximum(100);
  hMomEffAll->SetMinimum(0);
  hMomEffAll->Draw();
  
  TCanvas *c2 = new TCanvas("efficiency mu","c2",1200,1000);
  c2->SetGrid();
  hMomEffMuons->Scale(100);
  hMomEffMuons->SetLineColor(4);
  hMomEffMuons->SetMarkerColor(4);
  hMomEffMuons->SetLineWidth(4.);
  //hMomEffAll->SetMarkerStyle(2);
  //hMomEffAll->SetMarkerSize(3.0);
  hMomEffMuons->GetXaxis()->SetTitle("Momentum, GeV");
  hMomEffMuons->GetYaxis()->SetTitle("Efficiency, %");
  hMomEffMuons->SetMaximum(100);
  hMomEffMuons->SetMinimum(0);
  hMomEffMuons->Draw();
  
  
//  TCanvas *c2 = new TCanvas("Chi2","c2",1200,1000);
//  c2->SetGrid();
//
//  hChi2Muons->SetLineColor(4);
//  hChi2Muons->SetMarkerColor(4);
//  hChi2Muons->SetLineWidth(4.);
//  hChi2Muons->GetXaxis()->SetTitle("Chi2/NDF");
//  hChi2Muons->GetYaxis()->SetTitle("Counter");
//
//  hChi2Other->SetLineColor(2);
//  hChi2Other->SetMarkerColor(2);
//  hChi2Other->SetLineWidth(4.);
// 
//  
//  hChi2Muons->Draw();
//  hChi2Other->Draw("SAME");
//  
//  TLegend* l1 = new TLegend(0.1,0.91,0.9,0.99);
//  l1->SetHeader("Chi2/NDF");
//  l1->SetFillColor(0);
//  l1->SetLineColor(4);
//  l1->SetTextColor(4);
//  l1->AddEntry(hChi2Muons,"primary muons","l");  
//  l1->AddEntry(hChi2Other,"other","l"); 
//  //hMomEffAll->SetLineColor(4);
//  
//  l1->Draw();
  
}

 
  
  