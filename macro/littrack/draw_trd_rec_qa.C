{

   //TString recFileName = "/d/cbm02/andrey/events/trd/25gev/trd.444.reco.root";
   TString recFileName = "/d/cbm02/andrey/events/much/10stations/signal/25gev/omega/much.tracks.auau.25gev.omega.centr.root";
   //TString recFileName = "/d/cbm02/andrey/events/much/10stations/25gev/omega/much.tracks.auau.25gev.omega.centr.root";
   
   TFile *recFile = new TFile(recFileName);
 
   TCanvas *c1 = new TCanvas("CbmTrdEfficiency","c1",1200,1000);

   c1->SetGrid();

   TDirectory *dir = gDirectory;

   //dir->cd("TrdFinderQa");
   dir->cd("MuchFinderQa");
 
  // hMomEffAll->SetBins(5, 0, 10);
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
    
   gStyle->SetHistLineWidth(4);
   gROOT->ForceStyle(); 
      
   hMomEffAll->Draw();
  

   /* 
   TLegend* l1 = new TLegend(0.1,0.91,0.9,0.99);
   l1->SetHeader("TRD track finder efficiency");
   l1->SetFillColor(0);
   l1->SetLineColor(4);
   l1->SetTextColor(4);
   l1->AddEntry(hMomEffAll,"normalization to STS+TRD","l");  
   hMomEffAll->SetLineColor(4);
   
   l1->Draw();
   */



}


