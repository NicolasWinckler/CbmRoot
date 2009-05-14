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
      c1->Range(-240,-240,240,240);
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
   }//stations
}
