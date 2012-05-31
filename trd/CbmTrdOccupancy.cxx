#include "CbmTrdOccupancy.h"
#include "CbmTrdDigiPar.h"
#include "CbmTrdDigi.h"
#include "CbmTrdModule.h"
#include "CbmTrdGeoHandler.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TMath.h"
#include "TF1.h"
#include "TH2.h"
#include "TH1.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TStopwatch.h"
#include "TBox.h"
#include "TPaveText.h"


#include <iostream>
#include <cmath>
using std::cout;
using std::cin;
using std::endl;
using std::pair;
using std::fabs;

CbmTrdOccupancy::CbmTrdOccupancy()
  : FairTask("TrdOccupancy"),
    fDigis(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fGeoHandler(new CbmTrdGeoHandler()),
    fModuleMap(),
    fModuleMapIt(),
    fModuleOccupancyMemoryMap(),
    fModuleOccupancyMemoryMapIt(),
    fModuleOccupancyMap(),
    fModuleOccupancyMapIt(),
    fDigiChargeSpectrum(NULL)
{
}
CbmTrdOccupancy::CbmTrdOccupancy(const char *name, const char *title)
  : FairTask(name),
    fDigis(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fGeoHandler(new CbmTrdGeoHandler()),
    fModuleMap(),
    fModuleMapIt(),
    fModuleOccupancyMemoryMap(),
    fModuleOccupancyMemoryMapIt(),
    fModuleOccupancyMap(),
    fModuleOccupancyMapIt(),
    fDigiChargeSpectrum(NULL)
{
}
CbmTrdOccupancy::~CbmTrdOccupancy()
{
  FairRootManager *ioman =FairRootManager::Instance();
  ioman->Write();
  fDigis->Delete();
  delete fDigis;
  if(fDigiPar)
    delete fDigiPar;
  if(fModuleInfo)
    delete fModuleInfo;
  for (fModuleMapIt = fModuleMap.begin();
       fModuleMapIt != fModuleMap.end(); ++fModuleMapIt) {
    delete fModuleMapIt->second;
  } 
  for (fLayerOccupancyMapIt = fLayerOccupancyMap.begin();
       fLayerOccupancyMapIt != fLayerOccupancyMap.end(); ++fLayerOccupancyMapIt) {
    delete fLayerOccupancyMapIt->second;
  }
  for (fModuleOccupancyMapIt = fModuleOccupancyMap.begin();
       fModuleOccupancyMapIt != fModuleOccupancyMap.end(); ++fModuleOccupancyMapIt) {
    delete fModuleOccupancyMapIt->second;
  } 
  for (fModuleOccupancyMemoryMapIt = fModuleOccupancyMemoryMap.begin();
       fModuleOccupancyMemoryMapIt != fModuleOccupancyMemoryMap.end(); ++fModuleOccupancyMemoryMapIt) {
    delete fModuleOccupancyMemoryMapIt->second;
  }
  delete fDigiChargeSpectrum;
  delete fLayerDummy;
}
void CbmTrdOccupancy::SetParContainers()
{
    cout<<" * CbmTrdCbmTrdOccupancy * :: SetParContainers() "<<endl;
    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();
    fDigiPar = (CbmTrdDigiPar*)
               (rtdb->getContainer("CbmTrdDigiPar"));
}
// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdOccupancy::ReInit()
{
  cout<<" * CbmTrdClusterizer * :: ReInit() "<<endl;
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();  
  fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar")); 
   fLayerDummy = new TH2I("LayerDummy","",1200,-600,600,1000,-500,500);
  fLayerDummy->SetXTitle("x-coordinate [cm]");
  fLayerDummy->SetYTitle("y-coordinate [cm]");
fDigiChargeSpectrum = new TH1I("DigiChargeSpectrum","DigiChargeSpectrum",1e6,0,1e-3);
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdOccupancy::Init()
{
  cout<<" * CbmTrdOccupancy * :: Init() "<<endl;
  FairRootManager *ioman = FairRootManager::Instance();  
  fDigis =(TClonesArray *)  ioman->GetObject("TrdDigi");
  if ( ! fDigis ) {
    cout << "-W CbmTrdHitProducerDigi::Init: No TrdDigi array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
  }
  fGeoHandler->Init();
  fLayerDummy = new TH2I("LayerDummy","",1200,-600,600,1000,-500,500);
  fLayerDummy->SetXTitle("x-coordinate [cm]");
  fLayerDummy->SetYTitle("y-coordinate [cm]");
  fDigiChargeSpectrum = new TH1I("DigiChargeSpectrum","DigiChargeSpectrum",1e6,0,1e-3);
  return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdOccupancy::Exec(Option_t * option)
{
  //TFile *outFile = new TFile("data/CbmTrdOccupancy.root","UPDATE","output of CbmTrdOccupancy");
  TStopwatch timer;
  timer.Start();
  Int_t nEntries = fDigis->GetEntries();
  Int_t digiCounter = 0;
  TString title;
  /*
  fLayerDummy = new TH2I("LayerDummy","",1200,-600,600,1000,-500,500);
  fLayerDummy->SetXTitle("x-coordinate [cm]");
  fLayerDummy->SetYTitle("y-coordinate [cm]");
  //fDigiChargeSpectrum = (TH1I*)outFile->Get("DigiChargeSpectrum");
  //if (!fDigiChargeSpectrum)
  fDigiChargeSpectrum = new TH1I("DigiChargeSpectrum","DigiChargeSpectrum",1e6,0,1e-3);
  */
  for (Int_t iDigi=0; iDigi < nEntries; iDigi++ ) {
    CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(iDigi);
    
    Int_t moduleId = digi->GetDetId();
    Int_t Station  = fGeoHandler->GetStation(moduleId);
    Int_t Layer    = fGeoHandler->GetLayer(moduleId);
    Int_t combiId = 10 * Station + Layer;
    fModuleInfo = fDigiPar->GetModule(moduleId);
    Int_t nRows = fModuleInfo->GetnRow();
    Int_t nCols = fModuleInfo->GetnCol();
    fDigiChargeSpectrum->Fill(digi->GetCharge());
    if (digi->GetCharge() > 1e-7) {
      if (fModuleOccupancyMap.find(moduleId) == fModuleOccupancyMap.end()){
	title.Form("Module_%i",moduleId);
	//fModuleOccupancyMap[moduleId] = (TH2F*)outFile->Get(title);
	//if (!fModuleOccupancyMap[moduleId])
	fModuleOccupancyMap[moduleId] = new TH2F(title,title,nCols,-0.5,nCols-0.5,nRows,-0.5,nRows-0.5);
	//fModuleOccupancyMap[moduleId]->Reset();
	title.Form("M_%i",moduleId);
	fModuleOccupancyMemoryMap[moduleId] = new TH1F(title,title,10000,0,100);
	fModuleMap[moduleId] = new OccupancyModule();
	fModuleMap[moduleId]->Station = Station;
	fModuleMap[moduleId]->Layer = Layer;
	fModuleMap[moduleId]->ModuleSizeX = fModuleInfo->GetSizex();
	fModuleMap[moduleId]->ModuleSizeY = fModuleInfo->GetSizey();
	fModuleMap[moduleId]->ModulePositionX = fModuleInfo->GetX();
	fModuleMap[moduleId]->ModulePositionY = fModuleInfo->GetY();
	fModuleMap[moduleId]->moduleId = moduleId;
      }
      if (fLayerOccupancyMap.find(combiId) == fLayerOccupancyMap.end()){
	title.Form("Station_%i__Layer_%i",Station,Layer);
	//fLayerOccupancyMap[combiId] = new TH2F(title,title,1200,-600,600,1000,-500,500);
	fLayerOccupancyMap[combiId] = new TCanvas(title,title,800,800);
	fLayerOccupancyMap[combiId]->cd();
	fLayerDummy->DrawCopy("");
    
      }
      fModuleOccupancyMap[moduleId]->Fill(digi->GetCol(), digi->GetRow());
      //fModuleOccupancyMap[moduleId]->SetBinContent(digi->GetCol()+1, digi->GetRow()+1, 1);
    }
  }
  for (fModuleOccupancyMapIt = fModuleOccupancyMap.begin();
       fModuleOccupancyMapIt != fModuleOccupancyMap.end(); ++fModuleOccupancyMapIt) {
    TBox *b = new TBox(
		       fModuleMap[fModuleOccupancyMapIt->first]->ModulePositionX - fModuleMap[fModuleOccupancyMapIt->first]->ModuleSizeX,
		       fModuleMap[fModuleOccupancyMapIt->first]->ModulePositionY - fModuleMap[fModuleOccupancyMapIt->first]->ModuleSizeY,
		       fModuleMap[fModuleOccupancyMapIt->first]->ModulePositionX + fModuleMap[fModuleOccupancyMapIt->first]->ModuleSizeX,
		       fModuleMap[fModuleOccupancyMapIt->first]->ModulePositionY + fModuleMap[fModuleOccupancyMapIt->first]->ModuleSizeY
		       );
   
    fLayerOccupancyMap[fModuleMap[fModuleOccupancyMapIt->first]->Station*10+fModuleMap[fModuleOccupancyMapIt->first]->Layer]->cd();
    Float_t occ = Float_t(fModuleOccupancyMapIt->second->Integral()) / Float_t(fModuleOccupancyMapIt->second->GetNbinsX() * fModuleOccupancyMapIt->second->GetNbinsY()) * 100.;
    fModuleOccupancyMemoryMap[fModuleOccupancyMapIt->first]->Fill(occ);
    occ = fModuleOccupancyMemoryMap[fModuleOccupancyMapIt->first]->GetMean(1);
    title.Form("%.2f%%",occ);
    TPaveText *occupancy = new TPaveText(
					 fModuleMap[fModuleOccupancyMapIt->first]->ModulePositionX - fModuleMap[fModuleOccupancyMapIt->first]->ModuleSizeX,
					 fModuleMap[fModuleOccupancyMapIt->first]->ModulePositionY - fModuleMap[fModuleOccupancyMapIt->first]->ModuleSizeY,
					 fModuleMap[fModuleOccupancyMapIt->first]->ModulePositionX + fModuleMap[fModuleOccupancyMapIt->first]->ModuleSizeX,
					 fModuleMap[fModuleOccupancyMapIt->first]->ModulePositionY + fModuleMap[fModuleOccupancyMapIt->first]->ModuleSizeY
					 );
    b->SetLineColor(1);
    if (occ > 0 && occ <= 5)
      b->SetFillColor(kViolet);
    if (occ > 5 && occ <= 10)
      b->SetFillColor(kBlue);
    if (occ > 10 && occ <= 15)
      b->SetFillColor(kCyan);
    if (occ > 15 && occ <= 20)
      b->SetFillColor(kGreen);
    if (occ > 20 && occ <= 25)
      b->SetFillColor(kYellow);
    if (occ > 25 && occ <= 30)
      b->SetFillColor(kOrange);
    if (occ > 30)
      b->SetFillColor(kRed);

    b->Draw("same");
    occupancy->SetFillStyle(0);
    occupancy->AddText(title);
    occupancy->Draw("same");
  }

  SaveHistos2File();
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  printf("\n\n******************** Reading Test  **********************\n");
  printf("   RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  printf("*********************************************************\n\n");
}
 // ---- FinishTask-----------------------------------------------------
void CbmTrdOccupancy::FinishEvent()
{
  fDigis->Delete();
}

  // ---- Register ------------------------------------------------------
void CbmTrdOccupancy::Register()
{
  //FairRootManager::Instance()->Register("TrdDigi","Trd Digi", fDigiCollection, kTRUE);
  //FairRootManager::Instance()->Register("TrdDigiMatch","Trd Digi Match", fDigiMatchCollection, kTRUE);
}
  // --------------------------------------------------------------------
void CbmTrdOccupancy::SaveHistos2File() 
{
  TString title;
  /*
    TFile *outFile = new TFile("data/CbmTrdOccupancy.root","UPDATE","output of CbmTrdOccupancy");
    outFile->cd();
  */
  gDirectory->pwd();
  fDigiChargeSpectrum->Write("", TObject::kOverwrite);
  for (fLayerOccupancyMapIt = fLayerOccupancyMap.begin();
       fLayerOccupancyMapIt != fLayerOccupancyMap.end(); ++fLayerOccupancyMapIt) {
    fLayerOccupancyMapIt->second->Write("", TObject::kOverwrite);
  }
  /*
  if (!gDirectory->Cd("Module2D")) 
    gDirectory->mkdir("Module2D");
  gDirectory->Cd("Module2D");
  for (fModuleOccupancyMapIt = fModuleOccupancyMap.begin();
       fModuleOccupancyMapIt != fModuleOccupancyMap.end(); ++fModuleOccupancyMapIt) {
    title.Form("Occupancy: %.2f%%",Float_t(fModuleOccupancyMapIt->second->Integral()) / Float_t(fModuleOccupancyMapIt->second->GetNbinsX() * fModuleOccupancyMapIt->second->GetNbinsY()) * 100.);
    fModuleOccupancyMapIt->second->SetTitle(title);
    fModuleOccupancyMapIt->second->Write("", TObject::kOverwrite);
  }
  gDirectory->Cd("..");
  */
  if (!gDirectory->Cd("Module1D")) 
    gDirectory->mkdir("Module1D");
  gDirectory->Cd("Module1D");
  for (fModuleOccupancyMemoryMapIt = fModuleOccupancyMemoryMap.begin();
       fModuleOccupancyMemoryMapIt != fModuleOccupancyMemoryMap.end(); ++fModuleOccupancyMemoryMapIt) {
    fModuleOccupancyMemoryMapIt->second->Write("", TObject::kOverwrite);
  }
  gDirectory->Cd("..");
  //outFile->Close();
}
  ClassImp(CbmTrdOccupancy)
