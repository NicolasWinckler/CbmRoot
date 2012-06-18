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
    fDigiChargeSpectrum(NULL),
    fTriggerThreshold(1e-6),
    fNeigbourReadout(true),
    fGeo()
{
}
/*
CbmTrdOccupancy::CbmTrdOccupancy(const char *name, const char *title, const char *geo)
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
    fDigiChargeSpectrum(NULL),
    fTriggerThreshold(1.0e-6),
    fNeigbourReadout(true),
    fGeo(geo)
{
}
*/
CbmTrdOccupancy::CbmTrdOccupancy(const char *name, const char *title, const char *geo, Double_t triggerThreshold)
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
    fDigiChargeSpectrum(NULL),
    fTriggerThreshold(triggerThreshold),
    fNeigbourReadout(true),
    fGeo(geo)
{
}

CbmTrdOccupancy::~CbmTrdOccupancy()
{
  
  FairRootManager *ioman =FairRootManager::Instance();
  ioman->Write();
  //CreateLayerView();
  //SaveHistos2File();
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
    cout<<" * CbmTrdOccupancy * :: SetParContainers() "<<endl;
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
  fDigiChargeSpectrum = new TH1I("DigiChargeSpectrum","DigiChargeSpectrum",1e6,0,1.0e-3);
  for (fModuleOccupancyMapIt = fModuleOccupancyMap.begin();
       fModuleOccupancyMapIt != fModuleOccupancyMap.end(); ++fModuleOccupancyMapIt) {
    //delete fModuleOccupancyMapIt->second;
  } 
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
  fLayerDummy->GetXaxis()->SetLabelSize(0.02);
  fLayerDummy->GetYaxis()->SetLabelSize(0.02);
  fLayerDummy->GetZaxis()->SetLabelSize(0.02);
  fLayerDummy->GetXaxis()->SetTitleSize(0.02);
  fLayerDummy->GetXaxis()->SetTitleOffset(1.5);
  fLayerDummy->GetYaxis()->SetTitleSize(0.02);
  fLayerDummy->GetYaxis()->SetTitleOffset(2);
  fLayerDummy->GetZaxis()->SetTitleSize(0.02);
  fLayerDummy->GetZaxis()->SetTitleOffset(-2);

  fDigiChargeSpectrum = new TH1I("DigiChargeSpectrum","DigiChargeSpectrum",1e6,0,1.0e-3);
  for (fModuleOccupancyMapIt = fModuleOccupancyMap.begin();
       fModuleOccupancyMapIt != fModuleOccupancyMap.end(); ++fModuleOccupancyMapIt) {
    //delete fModuleOccupancyMapIt->second;
  } 
  return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdOccupancy::Exec(Option_t * option)
{
  printf("================CbmTrdOccupancy=====================\n");
  printf("TriggerThreshold: %.2E\n",fTriggerThreshold);
  printf("NeigbourReadout:%i\n",Int_t(fNeigbourReadout));
  Bool_t debug = false;
  //TFile *outFile = new TFile("data/CbmTrdOccupancy.root","UPDATE","output of CbmTrdOccupancy");
  TStopwatch timer;
  timer.Start();
  Int_t nEntries = fDigis->GetEntries();
  Int_t digiCounter(0), digiTriggerCounter(0);
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
    if (digi->GetCharge() > fTriggerThreshold) {
      digiTriggerCounter++;
      digiCounter++;
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
	title.Form("S%i_L%i",Station,Layer);
	//fLayerOccupancyMap[combiId] = new TH2F(title,title,1200,-600,600,1000,-500,500);
	fLayerOccupancyMap[combiId] = new TCanvas(title,title,1200,1000);
	fLayerOccupancyMap[combiId]->cd();
	fLayerDummy->DrawCopy("");
	title.Form("Station%i_Layer%i",Station,Layer);
	fLayerAverageOccupancyMap[combiId] = new TProfile(title,title,1e6,0,1.0e-3);
	fLayerAverageOccupancyMap[combiId]->SetYTitle("Average layer occupancy [%]");
	fLayerAverageOccupancyMap[combiId]->SetXTitle("Trigger threshold");
      }
      Int_t iCol(digi->GetCol()), iRow(digi->GetRow()), ixBin(iCol+1), iyBin(iRow+1);
      if (fModuleOccupancyMap[moduleId]->GetBinContent(ixBin,iyBin) == 0)
	fModuleOccupancyMap[moduleId]->Fill(iCol,iRow);
      if(fNeigbourReadout) {
	if (nCols > nRows) { // Neigbour readout only along the short pad size direction
	  if (ixBin > 1 && fModuleOccupancyMap[moduleId]->GetBinContent(ixBin-1,iyBin) == 0){//left
	    fModuleOccupancyMap[moduleId]->Fill(iCol-1,iRow);
	    digiCounter++;
	  }
	  if (ixBin < nCols && fModuleOccupancyMap[moduleId]->GetBinContent(ixBin+1,iyBin) == 0){//right
	    fModuleOccupancyMap[moduleId]->Fill(iCol+1,iRow);
	    digiCounter++;
	  }
	}
	else {
	  if (iyBin < nRows && fModuleOccupancyMap[moduleId]->GetBinContent(ixBin,iyBin+1) == 0){//upper
	    fModuleOccupancyMap[moduleId]->Fill(iCol,iRow+1);
	    digiCounter++;
	  }
	  if (iyBin > 1 && fModuleOccupancyMap[moduleId]->GetBinContent(ixBin,iyBin-1) == 0){//lower
	    fModuleOccupancyMap[moduleId]->Fill(iCol,iRow-1);
	    digiCounter++;
	  }
	}
      }
      //fModuleOccupancyMap[moduleId]->SetBinContent(digi->GetCol()+1, digi->GetRow()+1, 1);
    }
  }
  printf("%6i primary triggered digis\n%6i including neigbouring triggered digis\n", digiTriggerCounter, digiCounter);
  CopyEvent2MemoryMap();
  //CreateLayerView();
  //SaveHistos2File();
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
  for (fModuleOccupancyMapIt = fModuleOccupancyMap.begin();
       fModuleOccupancyMapIt != fModuleOccupancyMap.end(); ++fModuleOccupancyMapIt) {
    fModuleOccupancyMapIt->second->Reset();
  } 
}
void CbmTrdOccupancy::FinishTask()
{
  CreateLayerView();
  SaveHistos2File();

}
	  // ---- Register ------------------------------------------------------
void CbmTrdOccupancy::Register()
{
  //FairRootManager::Instance()->Register("TrdDigi","Trd Digi", fDigiCollection, kTRUE);
  //FairRootManager::Instance()->Register("TrdDigiMatch","Trd Digi Match", fDigiMatchCollection, kTRUE);
}
	  // --------------------------------------------------------------------
void CbmTrdOccupancy::CopyEvent2MemoryMap()
{
  for (fModuleOccupancyMapIt = fModuleOccupancyMap.begin();
       fModuleOccupancyMapIt != fModuleOccupancyMap.end(); ++fModuleOccupancyMapIt) {
    fModuleOccupancyMemoryMap[fModuleOccupancyMapIt->first]->Fill(Float_t(fModuleOccupancyMapIt->second->Integral()) / Float_t(fModuleOccupancyMapIt->second->GetNbinsX() * fModuleOccupancyMapIt->second->GetNbinsY()) * 100.);
    fLayerAverageOccupancyMap[10 * (fModuleMap[fModuleOccupancyMapIt->first]->Station) + (fModuleMap[fModuleOccupancyMapIt->first]->Layer)]->Fill(fTriggerThreshold, Float_t(fModuleOccupancyMapIt->second->Integral()) / Float_t(fModuleOccupancyMapIt->second->GetNbinsX() * fModuleOccupancyMapIt->second->GetNbinsY()) * 100.);
  }
}

void CbmTrdOccupancy::CreateLayerView()
{
  cout << "CreateLayerView" << endl;
  Bool_t debug = false;
  TString title;
  for (fModuleOccupancyMemoryMapIt = fModuleOccupancyMemoryMap.begin();
       fModuleOccupancyMemoryMapIt != fModuleOccupancyMemoryMap.end(); ++fModuleOccupancyMemoryMapIt) {
    /*
      TBox *b = new TBox(
      fModuleMap[fModuleOccupancyMapIt->first]->ModulePositionX - fModuleMap[fModuleOccupancyMapIt->first]->ModuleSizeX,
      fModuleMap[fModuleOccupancyMapIt->first]->ModulePositionY - fModuleMap[fModuleOccupancyMapIt->first]->ModuleSizeY,
      fModuleMap[fModuleOccupancyMapIt->first]->ModulePositionX + fModuleMap[fModuleOccupancyMapIt->first]->ModuleSizeX,
      fModuleMap[fModuleOccupancyMapIt->first]->ModulePositionY + fModuleMap[fModuleOccupancyMapIt->first]->ModuleSizeY
      );
    */
    /*
      Float_t occupancy = Float_t(fModuleOccupancyMapIt->second->Integral()) / Float_t(fModuleOccupancyMapIt->second->GetNbinsX() * fModuleOccupancyMapIt->second->GetNbinsY()) * 100.;
      if (debug)
      printf("M: %i O:%6.2f  ",fModuleOccupancyMapIt->first,occupancy);
      fModuleOccupancyMemoryMap[fModuleOccupancyMapIt->first]->Fill(occupancy);
    */
    Float_t occupancy = fModuleOccupancyMemoryMapIt->second->GetMean(1);
    Float_t occupancyE = fModuleOccupancyMemoryMapIt->second->GetRMS(1);
    if (debug)
      printf(" <O>:%6.2f  ",occupancy);
    title.Form("%.1f#pm%.1f%%",occupancy,occupancyE);
    if (debug)
      printf("%s\n",title.Data());
    TPaveText *text = new TPaveText(
				    fModuleMap[fModuleOccupancyMemoryMapIt->first]->ModulePositionX - fModuleMap[fModuleOccupancyMemoryMapIt->first]->ModuleSizeX,
				    fModuleMap[fModuleOccupancyMemoryMapIt->first]->ModulePositionY - fModuleMap[fModuleOccupancyMemoryMapIt->first]->ModuleSizeY,
				    fModuleMap[fModuleOccupancyMemoryMapIt->first]->ModulePositionX + fModuleMap[fModuleOccupancyMemoryMapIt->first]->ModuleSizeX,
				    fModuleMap[fModuleOccupancyMemoryMapIt->first]->ModulePositionY + fModuleMap[fModuleOccupancyMemoryMapIt->first]->ModuleSizeY
				    );
    text->SetFillStyle(1001);
    text->SetLineColor(1);
    Int_t maxCol = 4;
    if (occupancy > 0 && occupancy <= 5)
      text->SetFillColor(kViolet
			 //+Int_t((occupancy-0)/6.*maxCol)
			 );
    if (occupancy > 5 && occupancy <= 10){
      text->SetFillColor(kAzure
			 //+Int_t((occupancy-6)/6.*maxCol)
			 );
      text->SetTextColor(kWhite);
    }
    if (occupancy > 10 && occupancy <= 15)
      text->SetFillColor(kTeal
			 //+Int_t((occupancy-12)/6.*maxCol)
			 );
    if (occupancy > 15 && occupancy <= 20)
      text->SetFillColor(kSpring
			 //+Int_t((occupancy-18)/6.*maxCol)
			 );
    if (occupancy > 20 && occupancy <= 25)
      text->SetFillColor(kYellow
			 //+Int_t((occupancy-24)/6.*maxCol)
			 );
    if (occupancy > 25 && occupancy <= 30)
      text->SetFillColor(kOrange
			 //+Int_t((occupancy-24)/6.*maxCol)
			 );
    if (occupancy > 30)
      text->SetFillColor(kRed);
    fLayerOccupancyMap[fModuleMap[fModuleOccupancyMemoryMapIt->first]->Station*10+fModuleMap[fModuleOccupancyMemoryMapIt->first]->Layer]->cd();
    //b->Draw("same");
    //text->SetFillStyle(0);
    text->AddText(title);
    text->Draw("same");
  }
  gDirectory->pwd();
  if (!gDirectory->Cd("Occupancy")) 
    gDirectory->mkdir("Occupancy");
  gDirectory->Cd("Occupancy");
  gDirectory->pwd();
  title.Form("TH%.2E",fTriggerThreshold);
  if (!gDirectory->Cd(title)) 
    gDirectory->mkdir(title);
  gDirectory->Cd(title);
  gDirectory->pwd();
  for (fLayerOccupancyMapIt = fLayerOccupancyMap.begin();
       fLayerOccupancyMapIt != fLayerOccupancyMap.end(); ++fLayerOccupancyMapIt) {
    fLayerOccupancyMapIt->second->Write("", TObject::kOverwrite);
    title.Form("pics/Occupancy_%.2E_%s_%s.pdf", fTriggerThreshold, TString(fLayerOccupancyMapIt->second->GetTitle()).Data(), fGeo.Data());
    fLayerOccupancyMapIt->second->SaveAs(title);
    title.Form("pics/Occupancy_%.2E_%s_%s.png", fTriggerThreshold, TString(fLayerOccupancyMapIt->second->GetTitle()).Data(), fGeo.Data());
    fLayerOccupancyMapIt->second->SaveAs(title);
  }
  gDirectory->Cd("..");
  TCanvas *c = new TCanvas("c","c",800,600);
  Int_t counter = 0;
  for (fLayerAverageOccupancyMapIt = fLayerAverageOccupancyMap.begin();
       fLayerAverageOccupancyMapIt != fLayerAverageOccupancyMap.end(); ++fLayerAverageOccupancyMapIt) {
    fLayerAverageOccupancyMapIt->second->Write("", TObject::kOverwrite);
  
    counter++;
    fLayerAverageOccupancyMapIt->second->SetLineColor(counter);
    fLayerAverageOccupancyMapIt->second->SetMarkerColor(counter);
    fLayerAverageOccupancyMapIt->second->SetMarkerStyle(20);
    c->cd()->SetLogy(1);
    c->cd()->SetLogx(1);
    if (fLayerAverageOccupancyMapIt == fLayerAverageOccupancyMap.begin()){
      fLayerAverageOccupancyMapIt->second->GetXaxis()->SetRangeUser(1.0e-10,1.0e-3);
      fLayerAverageOccupancyMapIt->second->GetYaxis()->SetRangeUser(0.1,100);
      fLayerAverageOccupancyMapIt->second->DrawCopy();
    }
    else
      fLayerAverageOccupancyMapIt->second->DrawCopy("same");
  }
  title.Form("pics/Occupancy_%.2E_%s.pdf", fTriggerThreshold, fGeo.Data());
  c->SaveAs(title);
  title.Form("pics/Occupancy_%.2E_%s.png", fTriggerThreshold, fGeo.Data());
  c->SaveAs(title);
  gDirectory->Cd("..");
  c->Close();
}
void CbmTrdOccupancy::SetNeighbourReadout(Bool_t neighbourReadout){fNeigbourReadout=neighbourReadout;}
void CbmTrdOccupancy::SetTriggerThreshold(Double_t triggerthreshold){fTriggerThreshold=triggerthreshold;}
void CbmTrdOccupancy::SaveHistos2File() 
{
  cout << "SaveHistos2File" << endl;
  TString title;
  /*
    TFile *outFile = new TFile("data/CbmTrdOccupancy.root","UPDATE","output of CbmTrdOccupancy");
    outFile->cd();
  */
  gDirectory->pwd();
  
  fDigiChargeSpectrum->Write("", TObject::kOverwrite);
  /*
  for (fLayerOccupancyMapIt = fLayerOccupancyMap.begin();
       fLayerOccupancyMapIt != fLayerOccupancyMap.end(); ++fLayerOccupancyMapIt) {
    fLayerOccupancyMapIt->second->Write("", TObject::kOverwrite);
  }
  */
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