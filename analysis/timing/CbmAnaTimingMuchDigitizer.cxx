/** CbmAnaTimingMuchDigitizer.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2011-01-06
 **/

#include "CbmAnaTimingMuchDigitizer.h"
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
CbmAnaTimingMuchDigitizer::CbmAnaTimingMuchDigitizer():
  fEvent(0),fDigiFile("much_digi.root"),fHistoName("histo.root"),
  fGeoScheme(CbmMuchGeoScheme::Instance())
{}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmAnaTimingMuchDigitizer::CbmAnaTimingMuchDigitizer(const char* name, TString digiFile, TString histoName):
  FairTask(name),fEvent(0),fDigiFile(digiFile),fHistoName(histoName),
  fGeoScheme(CbmMuchGeoScheme::Instance())
{}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmAnaTimingMuchDigitizer::~CbmAnaTimingMuchDigitizer(){}
// -------------------------------------------------------------------------

// -----  SetParContainers -------------------------------------------------
void CbmAnaTimingMuchDigitizer::SetParContainers(){
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
InitStatus CbmAnaTimingMuchDigitizer::Init(){
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
  fMcEpoch         = (CbmMCEpoch*)   fManager->GetObject("MCEpoch.");
  
//  fMCTracks = new TClonesArray("CbmMCTrack");
//  fMcChain->SetBranchAddress("MCTrack",&fMCTracks);

  CbmMuchModuleGem* module = (CbmMuchModuleGem*) fGeoScheme->GetModule(0,0,0,4);
  Double_t r    = module->GetCutRadius();
  TVector3 size = module->GetSize();
  TVector3 pos  = module->GetPosition();
  fhPointXYT = new TH3D("hPointXYT","", 128,pos.x()-size.x()/2.,pos.x()+size.x()/2.,
                                        128,pos.y()-size.y()/2.,pos.y()+size.y()/2.,
                                       1000,                  0,10000);
  fhDigiXYT = new TH3D("hDigiXYT","", 128,pos.x()-size.x()/2.,pos.x()+size.x()/2.,
                                        128,pos.y()-size.y()/2.,pos.y()+size.y()/2.,
                                       1000,                  0,10000);
  CbmMuchSector* sector = module->GetSector(38); 
  TVector3 l = sector->GetSize();
  TVector3 p = sector->GetPosition();
  for (Int_t i=0;i<sector->GetNChannels();i++){
    CbmMuchPad* pad = sector->GetPad(i);
    printf("pad %i %f %f\n",i,pad->GetX0(),pad->GetY0());
  }
  
  fhSectorPointXYT = new TH3D("hSectorPointXYT","", 8,p[0]-l[0]/2,p[0]+l[0]/2.,
                                                   16,p[1]-l[1]/2,p[1]+l[1]/2,
                                                   1000,0,10000);
  fhSectorDigiXYT = new TH3D("hSectorDigiXYT","", 8,p[0]-l[0]/2,p[0]+l[0]/2.,
                                                  16,p[1]-l[1]/2,p[1]+l[1]/2,
                                                  1000,0,10000);
  
  fhChannelHits    = new TH1D("hChannelHits","",128,0,128);
  fhChannelHitDist = new TH1D("hChannelHitDist","",20,0,20);
  
  fhChannelT = new TH1D("hChannelT","",10000,0,10000);
  fhPointT   = new TH1D("hPointT","",10000,0,10000);
  fhModuleT = new TH1D("hModuleT","",10000,0,10000);
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaTimingMuchDigitizer::Exec(Option_t* opt){
  fMuchPoints = fMcEpoch->GetPoints(kMUCH);
  
  Int_t nMuchPoints = fMuchPoints ? fMuchPoints->GetEntriesFast() : 0;
  Int_t nMuchDigis  = fMuchDigis  ? fMuchDigis->GetEntriesFast() : 0;
  if (fVerbose>-1) printf(" Event: %4i",fEvent++);
  if (fVerbose>0) printf(" nMuchPoints: %4i",nMuchPoints);
  if (fVerbose>0) printf(" nMuchDigis: %4i",nMuchDigis);
  if (fVerbose>-1) printf("\n");
   
  CbmMuchSector* sector = ((CbmMuchModuleGem*) fGeoScheme->GetModule(0,0,0,4))->GetSector(38);     
  TVector3 l = sector->GetSize();
  TVector3 p = sector->GetPosition();

  for (Int_t i=0;i<nMuchPoints;i++){
    FairMCPoint* point = (FairMCPoint*) fMuchPoints->At(i);
//    if (fVerbose>2) printf("  Much point: %i time=%f",i,point->GetTime());
//    if (fVerbose>2) printf("Detector Id: %i",point->GetDetectorID());
//    if (fVerbose>2) printf("\n");
    Double_t x = point->GetX();
    Double_t y = point->GetY();
    Double_t t = point->GetTime();
    if (fGeoScheme->GetModuleIndex(point->GetDetectorID())!=4) continue;
    fhPointXYT->Fill(x,y,t);
    if (x>p[0]-l[0]/2 && x <p[0]+l[0]/2 && y>p[1]-l[1]/2 && y<p[1]+l[1]/2)  fhSectorPointXYT->Fill(x,y,t);
  }

  for (Int_t i=0;i<nMuchDigis;i++) {
    CbmMuchDigi* digi = (CbmMuchDigi*) fMuchDigis->At(i);
    Int_t    detId     = digi->GetDetectorId();
    Long64_t channelId = digi->GetChannelId();
    Double_t time      = digi->GetTime();
    Double_t dead_time = digi->GetDeadTime();
    UInt_t   adcCharge = digi->GetADCCharge(); 
    if (fGeoScheme->GetStationIndex(detId)!=0) continue;
    if (fGeoScheme->GetLayerIndex(detId)!=0) continue;
    if (fGeoScheme->GetLayerSideIndex(detId)!=0) continue;
    if (fGeoScheme->GetModuleIndex(detId)!=4) continue;
    fhModuleT->Fill(time);
    CbmMuchModuleGem* module = (CbmMuchModuleGem*) fGeoScheme->GetModuleByDetId(detId);
    Int_t iSector  = module->GetSectorIndex(channelId);
    Int_t iChannel = module->GetChannelIndex(channelId);
    CbmMuchPad* pad = module->GetPad(channelId);
    Double_t x = pad->GetX0();
    Double_t y = pad->GetY0();
    fhDigiXYT->Fill(x,y,time,digi->GetADCCharge());
    if (iSector!=38) continue;
    fhSectorDigiXYT->Fill(x,y,time,digi->GetADCCharge());
    fhChannelHits->Fill(iChannel);
    if (iChannel!=21) continue;
    printf("time=%f dead_time=%f\n",time,dead_time);
    for (Double_t t=time;t<time+dead_time;t++){
      fhChannelT->Fill(t);
    }
    CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fMuchDigiMatches->At(i);
    for (Int_t ipoint=0;ipoint<match->GetNPoints();ipoint++){
      FairMCPoint* point = (FairMCPoint*) fMuchPoints->At(match->GetRefIndex(ipoint));
      Double_t point_time   = point->GetTime();
      Double_t point_charge = match->GetCharge(ipoint);
//      fhPointT->Fill(point_time,point_charge);
      fhPointT->Fill(point_time);
    }
    
  }
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaTimingMuchDigitizer::Finish(){
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

  TCanvas* c2 = new TCanvas("c2","c2",1000,1000);
  fhPointXYT->SetFillColor(kBlue);
  fhPointXYT->SetLineColor(kRed);
  fhPointXYT->Draw("BOX");

  TCanvas* c3 = new TCanvas("c3","c3",1000,1000);
  fhSectorPointXYT->SetFillColor(kBlue);
  fhSectorPointXYT->SetLineColor(kRed);
  fhSectorPointXYT->Draw("BOX");

  TCanvas* c4 = new TCanvas("c4","c4",1000,1000);
  fhDigiXYT->SetFillColor(kBlue);
  fhDigiXYT->SetLineColor(kRed);
  fhDigiXYT->Draw("BOX");
  
  TCanvas* c5 = new TCanvas("c5","c5",1000,1000);
  fhSectorDigiXYT->SetFillColor(kBlue);
  fhSectorDigiXYT->SetLineColor(kRed);
  fhSectorDigiXYT->Draw("BOX");

  TCanvas* c6 = new TCanvas("c6","c6",1600,1000);
  c6->Divide(1,3);
  c6->cd(1);
  fhChannelT->SetFillColor(kBlue);
  fhChannelT->SetLineColor(kRed);
  fhChannelT->Draw();
  c6->cd(2);
  fhPointT->SetLineColor(kRed);
  fhPointT->Draw();
  c6->cd(3);
  fhModuleT->SetLineColor(kBlue);
  fhModuleT->Draw();
  
  TCanvas* c7 = new TCanvas("c7","c7",1600,400);
  fhChannelHits->SetFillColor(kBlue);
  fhChannelHits->SetLineColor(kRed);
  fhChannelHits->Draw();
  
  for (Int_t i=0;i<128;i++){
    fhChannelHitDist->Fill(fhChannelHits->GetBinContent(i)); 
  }

  TCanvas* c8 = new TCanvas("c8","c8",800,400);
  fhChannelHitDist->SetStats(kTRUE);
  fhChannelHitDist->SetFillColor(kBlue);
  fhChannelHitDist->SetLineColor(kRed);
  fhChannelHitDist->Draw();
}
// -------------------------------------------------------------------------

ClassImp(CbmAnaTimingMuchDigitizer);
