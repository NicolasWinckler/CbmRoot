#include "CbmTrdSPADIC.h"
#include "CbmTrdClusterFinderFast.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdDigi.h"

#include "CbmTrdModule.h"
#include "CbmTrdGeoHandler.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TArray.h"

#include <iostream>
#include <iomanip>
#include <cmath>
using std::cout;
using std::endl;

CbmTrdSPADIC::CbmTrdSPADIC()
  :FairTask("TrdSPADIC",1),
   fDigis(NULL),   
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fGeoHandler(NULL),
   fMinimumChargeTH(1.0e-06)
{
}
CbmTrdSPADIC::~CbmTrdSPADIC()
{
    fDigis->Delete();
    delete fDigis;
}
void CbmTrdSPADIC::SetParContainers()
{
  fDigiPar = (CbmTrdDigiPar*)(FairRunAna::Instance()->GetRuntimeDb()->getContainer("CbmTrdDigiPar"));
}

InitStatus CbmTrdSPADIC::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();

   fDigis = (TClonesArray*) ioman->GetObject("TrdDigi");
   if (fDigis == NULL) LOG(FATAL) << "CbmTrdSPADIC::Init No TrdDigi array" << FairLogger::endl;

   fGeoHandler = new CbmTrdGeoHandler();
   fGeoHandler->Init();

   return kSUCCESS;
} 
// --------------------------------------------------------------------
void CbmTrdSPADIC::SetTriggerThreshold(Double_t minCharge){
  fMinimumChargeTH = minCharge;//  To be used for test beam data processing
}

Bool_t CbmDigiSorter(std::pair< Int_t, Int_t/*CbmTrdDigi**/> a, std::pair< Int_t, Int_t/*CbmTrdDigi**/> b)
{return (a.first < b.first);}

// ---- Exec ----------------------------------------------------------
void CbmTrdSPADIC::Exec(Option_t *option)
{
  std::map< Int_t, std::map< Int_t, Int_t/*CbmTrdDigi**/> > moduleDigiNotTriggerMap; //<ModuleAddress, <combiId, digiId> >
  std::map< Int_t, std::list< std::pair< Int_t, Int_t/*CbmTrdDigi**/> > > moduleDigiTriggerMap; //<ModuleAddress, <combiId, digiId> >
  Int_t nDigis = fDigis->GetEntries();
  for (Int_t iDigi=0; iDigi < nDigis; iDigi++ ) {
    CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(iDigi);
    Int_t digiAddress  = digi->GetAddress();
    Int_t layerId    = CbmTrdAddress::GetLayerId(digiAddress);
    Int_t moduleId = CbmTrdAddress::GetModuleId(digiAddress);
    Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);
    fModuleInfo = fDigiPar->GetModule(CbmTrdAddress::GetModuleAddress(digiAddress));
    if (!fModuleInfo){
      printf("digi %3i digiAddress %i layer %i and modId %i  Sec%i Row:%i Col%i not found\n",
	     iDigi,digiAddress,layerId,moduleId,CbmTrdAddress::GetSectorId(digi->GetAddress()),
	     CbmTrdAddress::GetRowId(digi->GetAddress()),CbmTrdAddress::GetColumnId(digi->GetAddress())); 
      continue;
    }
    Int_t secRow = CbmTrdAddress::GetRowId(digi->GetAddress());
    Int_t iSector = CbmTrdAddress::GetSectorId(digi->GetAddress());
    Int_t globalRow = 0;
    globalRow = fModuleInfo->GetModuleRow(iSector,secRow);
    Int_t iCol = CbmTrdAddress::GetColumnId(digi->GetAddress());
  
    Int_t combiId = globalRow * (fModuleInfo->GetNofColumns() + 1) + iCol;
 
    if (digi->GetCharge() >= fMinimumChargeTH){
      digi->SetPrimeTriggerStatus(true);
      moduleDigiTriggerMap[moduleAddress].push_back(std::make_pair(combiId, iDigi));
    } else {    
      moduleDigiNotTriggerMap[moduleAddress][combiId] = iDigi;
    }

  }
  CbmTrdDigi *digi = NULL;
  for (std::map< Int_t, std::list< std::pair< Int_t, Int_t/*CbmTrdDigi**/> > >::iterator iModule = moduleDigiTriggerMap.begin(); iModule != moduleDigiTriggerMap.end(); ++iModule) {
    (*iModule).second.sort(CbmDigiSorter);
    for (std::list< std::pair< Int_t, Int_t/*CbmTrdDigi**/> >::iterator iDigi = (*iModule).second.begin(); iDigi != (*iModule).second.end(); iDigi++) {
      if (moduleDigiNotTriggerMap[(*iModule).first].find((*iDigi).first-1) != moduleDigiNotTriggerMap[(*iModule).first].end()){
	digi = (CbmTrdDigi*) fDigis->At(moduleDigiNotTriggerMap[(*iModule).first][(*iDigi).first-1]);
	digi->SetFNR_TriggerStatus(true);  
	digi->AddNeighbourTriggerId((*iDigi).second);
      }
      if (moduleDigiNotTriggerMap[(*iModule).first].find((*iDigi).first+1) != moduleDigiNotTriggerMap[(*iModule).first].end()){
	digi = (CbmTrdDigi*) fDigis->At(moduleDigiNotTriggerMap[(*iModule).first][(*iDigi).first+1]);
	digi->SetFNR_TriggerStatus(true); 
	digi->AddNeighbourTriggerId((*iDigi).second);	
      }
    }
  }
}

ClassImp(CbmTrdSPADIC)
