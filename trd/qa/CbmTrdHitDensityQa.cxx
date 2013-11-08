#include "CbmTrdHitDensityQa.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdDigi.h"
#include "CbmTrdPoint.h"
#include "CbmTrdCluster.h"
#include "CbmTrdHit.h"
#include "CbmTrdModule.h"
#include "CbmTrdGeoHandler.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TGeoManager.h"
#include "TClonesArray.h"
#include "TArray.h"
#include "TF1.h"
#include "TH2F.h"
#include "TH2I.h"
#include "TH2D.h"
#include "TColor.h"
#include "TMath.h"

#include "TProfile.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TImage.h"
#include "TLegend.h"
#include "TStopwatch.h"

//#include <map>
#include <iostream>
#include <iomanip>
#include <cmath>
using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------
CbmTrdHitDensityQa::CbmTrdHitDensityQa()
  :FairTask("CbmTrdHitDensityQa",1),
   fDigis(NULL),
   fClusters(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fGeoHandler(new CbmTrdGeoHandler()),
   fTriggerThreshold(1e-6),
   fEventRate(1e7),
   fEventCounter(NULL),
   fStation(-1),
   fLayer(-1), 
   fModuleID(-1),
   fModuleHitMap(),
   fUsedDigiMap()
{
}

// ---- Destructor ----------------------------------------------------
CbmTrdHitDensityQa::~CbmTrdHitDensityQa()
{
  if(fDigis){
    fDigis->Delete();
    delete fDigis;
  }     
  if(fClusters){
    fClusters->Delete();
    delete fClusters;
  }
  if(fDigiPar){
    delete fDigiPar;
  }
  if(fModuleInfo){
    delete fModuleInfo;
  }
}

// ----  Initialisation  ----------------------------------------------
void CbmTrdHitDensityQa::SetParContainers()
{
  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  cout << " * CbmTrdHitDensityQa * :: SetParContainers()" << endl;
  fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));
}
// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdHitDensityQa::ReInit()
{  
  cout << " * CbmTrdHitDensityQa * :: ReInit()" << endl;
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
  //fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// ---- Init ----------------------------------------------------------
InitStatus CbmTrdHitDensityQa::Init()
{
  cout << " * CbmTrdHitDensityQa * :: Init()" << endl;
  FairRootManager *ioman = FairRootManager::Instance();
  fDigis =(TClonesArray *)  ioman->GetObject("TrdDigi");
  if ( ! fDigis ) {
    cout << "-W CbmTrdHitDensityQa::Init: No TrdDigi array!" << endl;
    cout << "                             Task will be inactive" << endl;
    return kERROR;
  }
  fClusters =(TClonesArray *)  ioman->GetObject("TrdCluster");
  if ( ! fClusters ) {
    cout << "-W CbmTrdHitDensityQa::Init: No TrdCluster array!" << endl;
    cout << "                             Task will be inactive" << endl;
    //return kERROR;
  }   
  // Extract information about the number of TRD stations and
  // the number of layers per TRD station from the geomanager.
  // Store the information about the number of layers at the entrance
  // of subsequent stations in a vector. 
  // This allows to calculate the layer number starting with 1 for the
  // first layer of the first station at a later stage by only adding 
  // the layer number in the station to the number of layers in 
  // previous stations 

  fGeoHandler->Init();

  fEventCounter = new TH1I("fEventCounter","fEventCounter",1,-0.5,0.5);
  return kSUCCESS;
  
}
void CbmTrdHitDensityQa::Exec(Option_t * option)
{
  printf("=================CbmTrdHitDensityQa====================\n");
  TString title;
  TStopwatch timer;
  timer.Start();
  fEventCounter->Fill(0);
  printf("\n  Event: %i\n\n",(Int_t)fEventCounter->GetEntries());
 
  CbmTrdDigi *digi = NULL;
  CbmTrdCluster *cluster = NULL;
  if (NULL != fClusters){
    Int_t nCluster = fClusters->GetEntries();
    for (Int_t iCluster = 0; iCluster < nCluster; iCluster++) {
      //cout << iCluster << endl;
      cluster = (CbmTrdCluster*) fClusters->At(iCluster);//pointer to the acvit cluster  
      Int_t nDigisInCluster = cluster->GetNofDigis();
      for (Int_t iDigi = 0; iDigi < nDigisInCluster; iDigi++){  
	digi = (CbmTrdDigi*)fDigis->At(cluster->GetDigi(iDigi));
	Int_t digiAddress = digi->GetAddress();
	Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);
	Int_t moduleId = CbmTrdAddress::GetModuleId(moduleAddress);// TODO
	fLayer    = CbmTrdAddress::GetLayerId(moduleAddress);
	fModuleInfo = fDigiPar->GetModule(moduleAddress);
	if (fModuleHitMap.find(moduleAddress) == fModuleHitMap.end()){
	  title.Form("hd_Module_%i",moduleAddress);
	  Int_t nRows = fModuleInfo->GetNofRows();
	  Int_t nCols = fModuleInfo->GetNofColumns();
	  fModuleHitMap[moduleAddress] = new TH2I(title,title,nCols,-0.5,nCols-0.5,nRows,-0.5,nRows-0.5);
	}
	Int_t iCol(CbmTrdAddress::GetColumnId(digiAddress)), local_Row(CbmTrdAddress::GetRowId(digiAddress)), iSec(CbmTrdAddress::GetSectorId(digiAddress));
	Int_t iRow = fModuleInfo->GetModuleRow(iSec, local_Row);
	if (fUsedDigiMap.find(digiAddress) == fUsedDigiMap.end()){ // Cluster include already neighbour trigger read-out. Two clusters can share associated neighbour digis, therefore test if digi is already used
	  fModuleHitMap[moduleAddress]->Fill(iCol, iRow);
	  fUsedDigiMap[digiAddress] = iDigi;
	} 
      }
    }  
  } else {
    Int_t nDigis = fDigis->GetEntries();
    for (Int_t iDigi=0; iDigi < nDigis; iDigi++ ) {
      digi = (CbmTrdDigi*) fDigis->At(iDigi);
      Int_t digiAddress = digi->GetAddress();
      Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);
      Int_t moduleId = CbmTrdAddress::GetModuleId(moduleAddress);// TODO
      fLayer    = CbmTrdAddress::GetLayerId(moduleAddress);
      fModuleInfo = fDigiPar->GetModule(moduleAddress);
      if (digi->GetCharge() > fTriggerThreshold) {
	if (fModuleHitMap.find(moduleAddress) == fModuleHitMap.end()){
	  title.Form("hd_Module_%i",moduleAddress);
	  Int_t nRows = fModuleInfo->GetNofRows();
	  Int_t nCols = fModuleInfo->GetNofColumns();
	  fModuleHitMap[moduleAddress] = new TH2I(title,title,nCols,-0.5,nCols-0.5,nRows,-0.5,nRows-0.5);
	}
	Int_t iCol(CbmTrdAddress::GetColumnId(digiAddress)), local_Row(CbmTrdAddress::GetRowId(digiAddress)), iSec(CbmTrdAddress::GetSectorId(digiAddress));
	Int_t iRow = fModuleInfo->GetModuleRow(iSec, local_Row);
	if (fUsedDigiMap.find(digiAddress) == fUsedDigiMap.end()){
	  fModuleHitMap[moduleAddress]->Fill(iCol, iRow);
	  fUsedDigiMap[digiAddress] = iDigi;
	} 
	Int_t neighbourAddress = 0;
	if (iRow > 0){
	  if (local_Row > 0)
	    neighbourAddress = CbmTrdAddress::GetAddress(fLayer, CbmTrdAddress::GetModuleId(moduleAddress), iSec, local_Row-1, iCol);
	  else if (iSec > 0)
	    neighbourAddress = CbmTrdAddress::GetAddress(fLayer, CbmTrdAddress::GetModuleId(moduleAddress), iSec-1, fModuleInfo->GetNofRowsInSector(iSec-1)-1, iCol);
	  if (fUsedDigiMap.find(neighbourAddress) == fUsedDigiMap.end()){
	    fModuleHitMap[moduleAddress]->Fill(iCol, iRow-1);
	    fUsedDigiMap[neighbourAddress] = iDigi;
	  }
	}
	if (iRow < fModuleInfo->GetNofRows()-1){ // Only cross like neighbour trigger 
	  if (local_Row+1 > fModuleInfo->GetNofRowsInSector(iSec)-1)
	    neighbourAddress = CbmTrdAddress::GetAddress(fLayer, CbmTrdAddress::GetModuleId(moduleAddress), iSec+1, 0, iCol);
	  else
	    neighbourAddress = CbmTrdAddress::GetAddress(fLayer, CbmTrdAddress::GetModuleId(moduleAddress), iSec, local_Row+1, iCol);
	  if (fUsedDigiMap.find(neighbourAddress) == fUsedDigiMap.end()){
	    fModuleHitMap[moduleAddress]->Fill(iCol, iRow+1);
	    fUsedDigiMap[neighbourAddress] = iDigi;
	  }
	}
	if (iCol > 0){
	  neighbourAddress = CbmTrdAddress::GetAddress(fLayer, CbmTrdAddress::GetModuleId(moduleAddress), iSec, local_Row, iCol-1);
	  if (fUsedDigiMap.find(neighbourAddress) == fUsedDigiMap.end()){
	    fModuleHitMap[moduleAddress]->Fill(iCol-1, iRow);
	    fUsedDigiMap[neighbourAddress] = iDigi;
	  }
	}
	if (iCol < fModuleInfo->GetNofColumns()-1){
	  neighbourAddress = CbmTrdAddress::GetAddress(fLayer, CbmTrdAddress::GetModuleId(moduleAddress), iSec, local_Row, iCol+1);
	  if (fUsedDigiMap.find(neighbourAddress) == fUsedDigiMap.end()){
	    fModuleHitMap[moduleAddress]->Fill(iCol+1, iRow);
	    fUsedDigiMap[neighbourAddress] = iDigi;
	  }
	}
      }
    }
  }
}
  // ---- Register ------------------------------------------------------
  void CbmTrdHitDensityQa::Register()
  {
    cout << " * CbmTrdHitDensityQa * :: Register()" << endl;

  }
  // --------------------------------------------------------------------


  // ---- Finish --------------------------------------------------------
void CbmTrdHitDensityQa::Finish()
{
  Double_t min(1), max(/*(Double_t)fEventCounter->GetEntries()/10.*/fEventRate);
  std::vector<Int_t> fColors;
  std::vector<Double_t> fZLevel;
  for (Int_t i = 0; i < TColor::GetNumberOfColors(); i++){
    fColors.push_back(TColor::GetColorPalette(i));
    fZLevel.push_back(min + TMath::Power(10, TMath::Log10(max) / TColor::GetNumberOfColors() * i));
  }
  TString title, name;
  std::map< Int_t, TCanvas*> LayerMap;
  std::map< Int_t, TCanvas*>::iterator LayerMapIt;
  gDirectory->pwd();
  fEventCounter->Write("", TObject::kOverwrite);
  if (!gDirectory->Cd("TrdHitDensityQa")) 
    gDirectory->mkdir("TrdHitDensityQa");
  gDirectory->Cd("TrdHitDensityQa");
  if (!gDirectory->Cd("Module")) 
    gDirectory->mkdir("Module");
  gDirectory->Cd("Module");
  for (fModuleHitMapIt = fModuleHitMap.begin();
       fModuleHitMapIt != fModuleHitMap.end(); ++fModuleHitMapIt) {
    fModuleHitMapIt->second->Write("", TObject::kOverwrite);
    Int_t LayerId = CbmTrdAddress::GetLayerId(fModuleHitMapIt->first);
    if (LayerMap.find(LayerId) == LayerMap.end()){
      fStation =  LayerId / 4 + 1;  // OK for SIS100 and SIS300
      fLayer   =  LayerId % 4 + 1;  // 
      name.Form("hd_S%d_L%d",fStation,fLayer);
      title.Form("hd Station %d, Layer %d",fStation,fLayer);
      LayerMap[LayerId] = new TCanvas(name,title,1000,900);
      name.Form("hdH_S%d_L%d",fStation,fLayer);
      title.Form("hdH Station %d, Layer %d",fStation,fLayer);
      TH2F *Layer = new TH2F(name,title,600,-6000,6000,600,-6000,6000);
      Layer->SetContour(99);
      Layer->SetXTitle("x-Coordinate [mm]");
      Layer->SetYTitle("y-Coordinate [mm]");
      Layer->SetZTitle("Trigger/Channel [Hz]");
      Layer->SetStats(kFALSE);
      Layer->GetXaxis()->SetLabelSize(0.02);
      Layer->GetYaxis()->SetLabelSize(0.02);
      Layer->GetZaxis()->SetLabelSize(0.02);
      Layer->GetXaxis()->SetTitleSize(0.02);
      Layer->GetXaxis()->SetTitleOffset(1.5);
      Layer->GetYaxis()->SetTitleSize(0.02);
      Layer->GetYaxis()->SetTitleOffset(2);
      Layer->GetZaxis()->SetTitleSize(0.02);
      Layer->GetZaxis()->SetTitleOffset(-2);
      Layer->GetZaxis()->SetRangeUser(min,max);
      LayerMap[LayerId]->cd()->SetLogz(1);
      Layer->Fill(0.,0.,0.);
      Layer->Draw("colz");
    }
 
    fModuleInfo = fDigiPar->GetModule(fModuleHitMapIt->first);
    const Int_t nSec = fModuleInfo->GetNofSectors();
    Int_t global_Row = 0;
    TVector3 padPos;
    TVector3 padSize;
    printf("Module: %6i   Maximum Trigger Rate: %EHz/Channel\n",fModuleHitMapIt->first,fModuleHitMapIt->second->GetBinContent(fModuleHitMapIt->second->GetMaximumBin()) / Double_t(fEventCounter->GetEntries()) * fEventRate);
    for (Int_t s = 0; s < nSec; s++){
      const Int_t nRow = fModuleInfo->GetNofRowsInSector(s);
      const Int_t nCol = fModuleInfo->GetNofColumnsInSector(s);
      for (Int_t r = 0; r < nRow; r++){
	for (Int_t c = 0; c < nCol; c++){
	  fModuleInfo->GetPosition(fModuleHitMapIt->first, s, c, r, padPos, padSize);// padPos local or global???
	  Double_t local_min[3] = {padPos[0]-0.5*padSize[0], padPos[1]-0.5*padSize[1], padPos[2]};
	  Double_t local_max[3] = {padPos[0]+0.5*padSize[0], padPos[1]+0.5*padSize[1], padPos[2]};
	  if (fModuleInfo->GetOrientation() == 1 || fModuleInfo->GetOrientation() == 3){ // Pad size is in local coordinate system where position is in global coordinate system
	    local_min[0] = padPos[0]-0.5*padSize[1];
	    local_min[1] = padPos[1]-0.5*padSize[0];
	    local_max[0] = padPos[0]+0.5*padSize[1];
	    local_max[1] = padPos[1]+0.5*padSize[0];
	  }
	  for (Int_t i = 0; i < 3; i++) {
	    //global_min[i] *= 10.;
	    //global_max[i] *= 10.;
	    local_min[i] *= 10.;
	    local_max[i] *= 10.;
	  }
	  Double_t rate = Double_t(fModuleHitMapIt->second->GetBinContent(c+1,global_Row+1)) / Double_t(fEventCounter->GetEntries()) * fEventRate;
	  TBox *pad = new TBox(local_min[0], local_min[1], local_max[0], local_max[1]);
	  //printf("    %i %i %i  (%f, %f)   (%f, %f)   %f\n",s,r,c,local_min[0],local_min[1],global_min[0],global_min[1],rate);
	  pad->SetLineColor(0);
	  pad->SetLineWidth(0);	
	  Int_t color(0), j(0);
	  while ((rate > fZLevel[j]) && (j < (Int_t)fZLevel.size())){
	    //printf ("              %i<%i %i    %E <= %E\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
	    j++;
	  }
	  //printf ("%i<%i %i    %E <= %E\n\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
	  pad->SetFillColor(fColors[j]);
	  if (j >= fZLevel.size() || rate > max)
	    pad->SetFillColor(12);
	  LayerMap[LayerId]->cd();
	  pad->Draw("same");
	}
	global_Row++;
      }
    }
    TBox *module = new TBox(fModuleInfo->GetX()*10-fModuleInfo->GetSizeX()*10,
			    fModuleInfo->GetY()*10-fModuleInfo->GetSizeY()*10,
			    fModuleInfo->GetX()*10+fModuleInfo->GetSizeX()*10,
			    fModuleInfo->GetY()*10+fModuleInfo->GetSizeY()*10);
    module->SetFillStyle(0);
    LayerMap[LayerId]->cd();
    module->Draw("same");
    delete fModuleHitMapIt->second;
  }
  fModuleHitMap.clear();
  gDirectory->Cd("..");
  for (LayerMapIt = LayerMap.begin();
       LayerMapIt != LayerMap.end(); ++LayerMapIt) {
    LayerMapIt->second->Write("", TObject::kOverwrite);
    fStation =  LayerMapIt->first / 4 + 1;  // OK for SIS100 and SIS300
    fLayer   =  LayerMapIt->first % 4 + 1;  // 
    name.Form("pics/CbmTrdHitDensityQa_S%i_L%i.png",fStation,fLayer);
    LayerMapIt->second->SaveAs(name);
    name.ReplaceAll("png","pdf");
    LayerMapIt->second->SaveAs(name);
  }
  gDirectory->Cd("..");
}

  // -----   Public method EndOfEvent   --------------------------------------
void CbmTrdHitDensityQa::FinishEvent() {
  if (fDigis){
    fDigis->Clear("C");
    fDigis->Delete();
  }
  if (fClusters){
    fClusters->Clear("C");
    fClusters->Delete();
  }
  fUsedDigiMap.clear();
}
  // -------------------------------------------------------------------------

  ClassImp(CbmTrdHitDensityQa)
