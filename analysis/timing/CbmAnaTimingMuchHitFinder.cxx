/** CbmAnaTimingMuchHitFinder.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2011-01-10
 **/

#include "CbmAnaTimingMuchHitFinder.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairMCPoint.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchSector.h"
#include "CbmMuchPad.h"
#include "CbmMCEpoch.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "TClonesArray.h"
#include "TH3D.h"
#include "TH1D.h"
#include "TVector3.h"
#include "TCanvas.h"
#include "TArc.h"

// -----   Default constructor   -------------------------------------------
CbmAnaTimingMuchHitFinder::CbmAnaTimingMuchHitFinder():
  fEvent(0),fDigiFile("much_digi.root"),fHistoName("histo.root"),
  fGeoScheme(CbmMuchGeoScheme::Instance())
{}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmAnaTimingMuchHitFinder::CbmAnaTimingMuchHitFinder(const char* name, TString digiFile, TString histoName):
  FairTask(name),fEvent(0),fDigiFile(digiFile),fHistoName(histoName),
  fGeoScheme(CbmMuchGeoScheme::Instance())
{}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmAnaTimingMuchHitFinder::~CbmAnaTimingMuchHitFinder(){}
// -------------------------------------------------------------------------

// -----  SetParContainers -------------------------------------------------
void CbmAnaTimingMuchHitFinder::SetParContainers(){
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  rtdb->getContainer("FairBaseParSet");
  rtdb->getContainer("CbmGeoPassivePar");
  rtdb->getContainer("CbmGeoStsPar");
  rtdb->getContainer("CbmGeoMuchPar");
  rtdb->getContainer("CbmFieldPar");
}
// -------------------------------------------------------------------------


// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmAnaTimingMuchHitFinder::Init(){
  FairRootManager* fManager = FairRootManager::Instance();
  // Initialize GeoScheme
  TFile* oldfile=gFile;
  TFile* file=new TFile(fDigiFile);
  TObjArray* stations = (TObjArray*) file->Get("stations");
  file->Close();
  file->Delete();
  gFile=oldfile;
  fGeoScheme->Init(stations);
  
  fMuchDigis       = (TClonesArray*) fManager->GetObject("MuchDigi");
  fMuchDigiMatches = (TClonesArray*) fManager->GetObject("MuchDigiMatch");
  fMuchClusters    = (TClonesArray*) fManager->GetObject("MuchCluster");
  fMcEpoch         = (CbmMCEpoch*)   fManager->GetObject("MCEpoch.");
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaTimingMuchHitFinder::Exec(Option_t* opt){
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaTimingMuchHitFinder::Finish(){
  TFile* f = new TFile(fHistoName.Data(),"RECREATE");
//  fDetEventTime->Write();
  f->Close();

  CbmMuchModuleGem* module = (CbmMuchModuleGem*) fGeoScheme->GetModule(0,0,0,4);
  Double_t r    = module->GetCutRadius();
  TVector3 size = module->GetSize();
  TVector3 pos  = module->GetPosition();
  printf("module:");
  printf(" r=%.1f",r);
  printf(" size=(%.1f,%.1f,%.1f)",size.x(),size.y(),size.z());
  printf(" pos=(%.1f,%.1f,%.1f)",pos.x(),pos.y(),pos.z());
  printf("\n");
  
  TCanvas* c1 = new TCanvas("c1","c1",1000,1000);
  gPad->Range(pos.x()-size.x()/2.,pos.y()-size.y()/2.,pos.x()+size.x()/2.,pos.y()+size.y()/2.);

  module->SetFillColor(kYellow);
  module->SetFillStyle(3001);
  module->Draw("f");
  module->DrawModule(kYellow);
  for (Int_t i=0;i<module->GetNSectors();i++){
    CbmMuchSector* sector = module->GetSector(i);
    printf("Sector:%i %f %f\n",i,sector->GetPosition().x(),sector->GetPosition().y());
//    sector->Draw("f");
//    sector->Draw();
    for (Int_t p=0;p<sector->GetNChannels();p++){
      CbmMuchPad* pad = sector->GetPad(p);
      pad->Draw("f");
      if (i==38) pad->SetLineColor(kRed);
      pad->Draw();
    }
    sector->SetLineWidth(2);
    sector->SetLineColor(kGreen+3);
    if (i==38) {
      sector->SetLineColor(kRed);
      sector->SetLineWidth(4);
    }
    sector->Draw();
  }

  TArc* cMin = new TArc(0,0,r,0,360);
  cMin->SetFillColor(10);
  cMin->Draw("f");
}
// -------------------------------------------------------------------------

ClassImp(CbmAnaTimingMuchHitFinder);
