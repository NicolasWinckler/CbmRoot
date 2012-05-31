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
  //Clear Maps !!!
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

  return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdOccupancy::Exec(Option_t * option)
{
  TStopwatch timer;
  timer.Start();
  Int_t nEntries = fDigis->GetEntries();
  Int_t digiCounter = 0;
  TString title;
  fLayerDummy = new TH2I("LayerDummy","",1200,-600,600,1000,-500,500);
  fLayerDummy->SetXTitle("x-coordinate [cm]");
  fLayerDummy->SetYTitle("y-coordinate [cm]");
  fDigiChargeSpectrum = new TH1I("DigiChargeSpectrum","DigiChargeSpectrum",1e6,0,1e-3);
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
    if (digi->GetCharge() > 1e-6) {
      if (fModuleOccupancyMap.find(moduleId) == fModuleOccupancyMap.end()){
	title.Form("Module_%i",moduleId);
	fModuleOccupancyMap[moduleId] = new TH2F(title,title,nCols,-0.5,nCols-0.5,nRows,-0.5,nRows-0.5);
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
    b->SetLineColor(1);
    b->SetFillColor(15);
    fLayerOccupancyMap[fModuleMap[fModuleOccupancyMapIt->first]->Station*10+fModuleMap[fModuleOccupancyMapIt->first]->Layer]->cd();
    b->Draw("same");
    title.Form("%.2f%%",Float_t(fModuleOccupancyMapIt->second->Integral()) / Float_t(fModuleOccupancyMapIt->second->GetNbinsX() * fModuleOccupancyMapIt->second->GetNbinsY()) * 100.);
    TPaveText *occupancy = new TPaveText(
					 fModuleMap[fModuleOccupancyMapIt->first]->ModulePositionX - fModuleMap[fModuleOccupancyMapIt->first]->ModuleSizeX,
					 fModuleMap[fModuleOccupancyMapIt->first]->ModulePositionY - fModuleMap[fModuleOccupancyMapIt->first]->ModuleSizeY,
					 fModuleMap[fModuleOccupancyMapIt->first]->ModulePositionX + fModuleMap[fModuleOccupancyMapIt->first]->ModuleSizeX,
					 fModuleMap[fModuleOccupancyMapIt->first]->ModulePositionY + fModuleMap[fModuleOccupancyMapIt->first]->ModuleSizeY
					 );
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
  //TFile *outFile = new TFile("data/CbmTrdOccupancy.root","RECREATE","output of CbmTrdOccupancy");
  //outFile->Cd();
  gDirectory->pwd();
  fDigiChargeSpectrum->Write("", TObject::kOverwrite);
  for (fLayerOccupancyMapIt = fLayerOccupancyMap.begin();
       fLayerOccupancyMapIt != fLayerOccupancyMap.end(); ++fLayerOccupancyMapIt) {
    fLayerOccupancyMapIt->second->Write("", TObject::kOverwrite);
  }
  for (fModuleOccupancyMapIt = fModuleOccupancyMap.begin();
       fModuleOccupancyMapIt != fModuleOccupancyMap.end(); ++fModuleOccupancyMapIt) {
    title.Form("Occupancy: %.2f%%",Float_t(fModuleOccupancyMapIt->second->Integral()) / Float_t(fModuleOccupancyMapIt->second->GetNbinsX() * fModuleOccupancyMapIt->second->GetNbinsY()) * 100.);
    fModuleOccupancyMapIt->second->SetTitle(title);
    fModuleOccupancyMapIt->second->Write("", TObject::kOverwrite);
  }


  //outFile->Close();
}
  ClassImp(CbmTrdOccupancy)
