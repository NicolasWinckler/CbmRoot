#include "TPolyLine.h"

/**
 * Draws sector layout after segmentation.
 *
 * @param digiFile  Input file containing segmentation parameters
 */
void much_draw_sectors(TString digiFile = ""){

  TString inputdir = gSystem->Getenv("VMCWORKDIR");
  if (digiFile == "") {
    digiFile = inputdir + "/macro/much/data/much_digi.root";
  }

  // ----  Load libraries   -------------------------------------------------
  cout << endl << "=== much_draw_sectors.C : Loading libraries ..." << endl;
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libSts");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libMuch");
  // -----------------------------------------------------------------------

  TFile* f = new TFile(digiFile.Data());
  f->ls();
  TObjArray* stations = (TObjArray*) f->Get("stations");
  for (Int_t st=0;st<stations->GetEntriesFast();st++){
    TCanvas* c1 = new TCanvas(Form("station%i",st+1),Form("station%i",st+1),800,800);
    c1->SetFillColor(0);
    c1->Range(-230,-230,230,230);//(-27,-2,0,25);
    CbmMuchStation* station = (CbmMuchStation*) stations->At(st);
    CbmMuchLayer* layer = station->GetLayer(0);
    for (Int_t s=1;s>=0;s--){
      CbmMuchLayerSide* side = layer->GetSide(s);
      for (Int_t m=0;m<side->GetNModules();m++) {
        CbmMuchModuleGem* module = (CbmMuchModuleGem*)side->GetModule(m);
        module->SetFillStyle(0);
        module->Draw();
        for (Int_t j=0;j<module->GetNSectors();j++){
          CbmMuchSector* sector = module->GetSector(j);
          // Reject incomplete sectors by size
          Double_t secLx = sector->GetSize()[0];
          Double_t secLy = sector->GetSize()[1];
          Double_t less = TMath::Min(secLx, secLy);
          Double_t more = TMath::Max(secLx, secLy);
          Int_t nReal = Int_t((more+1e-5)/less);
          Int_t nPow = Int_t(TMath::Log2(nReal) + 1e-2);
          Double_t more1 = less*TMath::Power(2,nPow);
          if(TMath::Abs(more1-more ) > 1e-5 || sector->GetNChannels() < module->GetNSectorChannels())
            sector->SetFillColor(kRed);
          else
            sector->SetFillColor(s ? kOrange : kYellow);
          sector->Draw("f");
          sector->Draw();
        } // sectors
      } // modules
    } // sides
    TArc* arc1 = new TArc(0.,0.,station->GetRmin());
    arc1->SetFillColor(10);
    arc1->Draw("f");
    TArc* arc2 = new TArc(0.,0.,station->GetRmax());
    arc2->SetFillStyle(0);
    arc2->Draw();

    //      TArc* arc3 = new TArc(0.,0.,13.990);
    //      arc3->SetLineColor(kBlue);
    //      arc3->SetLineWidth(2);
    //      arc3->SetFillStyle(0);
    //      arc3->Draw();
    //      TArc* arc4 = new TArc(0.,0.,19.390);
    //      arc4->SetLineColor(kBlue);
    //      arc4->SetLineWidth(2);
    //      arc4->SetFillStyle(0);
    //      arc4->Draw();
    //      TArc* arc5 = new TArc(0.,0.,24.410);
    //      arc5->SetLineColor(kBlue);
    //      arc5->SetLineWidth(2);
    //      arc5->SetFillStyle(0);
    //      arc5->Draw();
    //      TArc* arc6 = new TArc(0.,0.,31.510);
    //      arc6->SetLineColor(kBlue);
    //      arc6->SetLineWidth(2);
    //      arc6->SetFillStyle(0);
    //      arc6->Draw();
    //      TArc* arc7 = new TArc(0.,0.,64.760);
    //      arc7->SetLineColor(kBlue);
    //      arc7->SetLineWidth(2);
    //      arc7->SetFillStyle(0);
    //      arc7->Draw();
  }//stations
}
