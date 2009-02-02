#include "TPolyLine.h"
void much_draw_sectors(){
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
  cbmrootlibs();

  TString dir = "/home/d/andrey/events/newmuch/large/10mu/mu_urqmd/";
  TFile* f = new TFile(dir + "much.digi.root");

  f->ls();
  TObjArray* stations = (TObjArray*) f->Get("stations");
  for (Int_t st=0;st<stations->GetEntriesFast();st++){
    TCanvas* c1 = new TCanvas(Form("station%i",st+1),Form("station%i",st+1),800,800);
    c1->Range(-230,-230,230,230);
    CbmMuchStation* station = (CbmMuchStation*) stations->At(st);
    CbmMuchLayer* layer = station->GetLayer(0);
    for (Int_t s=1;s>=0;s--){
      CbmMuchLayerSide* side = layer->GetSide(s);
      for (Int_t m=0;m<side->GetNModules();m++) {
        CbmMuchModule* module = side->GetModule(m);
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
