#include "CbmTrdClusterFinderDPB.h"
//#include "CbmTrdClusterFinderFast.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdDigi.h"
#include "CbmTrdCluster.h"
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

CbmTrdClusterFinderDPB::CbmTrdClusterFinderDPB()
  :FairTask("TrdClusterFinderDPB",1),
   fDigis(NULL),
   fClusters(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fGeoHandler(NULL)
{
}

CbmTrdClusterFinderDPB::~CbmTrdClusterFinderDPB()
{
    fClusters->Delete();
    delete fClusters;
}

void CbmTrdClusterFinderDPB::SetParContainers()
{
  fDigiPar = (CbmTrdDigiPar*)(FairRunAna::Instance()->GetRuntimeDb()->getContainer("CbmTrdDigiPar"));
}

InitStatus CbmTrdClusterFinderDPB::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();

   fDigis = (TClonesArray*) ioman->GetObject("TrdDigi");
   if (fDigis == NULL) LOG(FATAL) << "CbmTrdClusterFinderDPB::Init No TrdDigi array" << FairLogger::endl;

   fClusters = new TClonesArray("CbmTrdCluster", 100);
   ioman->Register("TrdCluster","Trd Cluster",fClusters,kTRUE);

   fGeoHandler = new CbmTrdGeoHandler();
   fGeoHandler->Init();

   return kSUCCESS;
} 

void CbmTrdClusterFinderDPB::Exec(Option_t *option)
{
  std::map<Int_t, DigiList*> ModuleDigiMap;
  Int_t nDigis = fDigis->GetEntries();
  for (Int_t iDigi=0; iDigi < nDigis; iDigi++ ) {
    CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(iDigi);  
    Int_t digiAddress  = digi->GetAddress();
    Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);
    if (ModuleDigiMap.find(moduleAddress)==ModuleDigiMap.end())
      ModuleDigiMap[moduleAddress] = new DigiList;
    ModuleDigiMap[moduleAddress]->push_back(std::make_pair(iDigi,digi));
  }
  for (std::map<Int_t, DigiList*>::iterator iMod = ModuleDigiMap.begin(); iMod != ModuleDigiMap.end(); iMod++){
    doClustering(iMod->first, iMod->second);   
  }
  for (std::map<Int_t, DigiList*>::iterator iMod = ModuleDigiMap.begin(); iMod != ModuleDigiMap.end(); iMod++){
    delete iMod->second;
  }
  ModuleDigiMap.clear();
}

void CbmTrdClusterFinderDPB::doClustering(Int_t ModuleAddress, DigiList* digiList)
{
  /*
    Any sorting of the digi stream is done here. 
    All infos about the module and the digi are provided by CbmTrdAddress and CbmTrdModule. 
    Criteria for cluster separation are not jet included. 
    One cluster per event per module is created.
  */
  Bool_t newCluster = false;
  DigiList *clusterList = new DigiList;
  for (DigiList::iterator iDigi = digiList->begin(); iDigi != digiList->end(); iDigi++) {   
    CbmTrdDigi *digi = (*iDigi).second; 

    Int_t digiAddress  = digi->GetAddress();
    Int_t layerId      = CbmTrdAddress::GetLayerId(digiAddress);
    Int_t moduleId     = CbmTrdAddress::GetModuleId(digiAddress);
    fModuleInfo        = fDigiPar->GetModule(CbmTrdAddress::GetModuleAddress(digiAddress));
    Int_t secRow       = CbmTrdAddress::GetRowId(digi->GetAddress());
    Int_t iSector      = CbmTrdAddress::GetSectorId(digi->GetAddress());
    Int_t globalRow    = 0;
    Double_t charge    = digi->GetCharge();
    Double_t time      = digi->GetTime();
    globalRow          = fModuleInfo->GetModuleRow(iSector,secRow);

    if (newCluster)                                                         //new cluster is started
      {     
	addCluster(clusterList);  
	clusterList->clear();
	newCluster = false;
      }
    clusterList->push_back(std::make_pair((*iDigi).first,digi));
  }
  addCluster(clusterList);                                                // process last cluster   
}

void CbmTrdClusterFinderDPB::addCluster(DigiList* clusterList)
{   
  Int_t idigi = 0;
  std::vector<Int_t> digiIndices( (*clusterList).size() );
  for (DigiList::iterator iDigi = clusterList->begin(); iDigi != clusterList->end(); iDigi++) {
    digiIndices[idigi] = (*iDigi).first;                                                             // store all digiIds (position in TClonesArray) to a vector
    idigi++;
  }
  Int_t size = fClusters->GetEntriesFast();                                                          // put new cluster to the end of the TClonesArray
  CbmTrdCluster* cluster = new ((*fClusters)[size]) CbmTrdCluster();                                 // Create final CbmTrdDigi
  cluster->SetAddress(CbmTrdAddress::GetModuleAddress((*clusterList->begin()).second->GetAddress()));//Add module address
  cluster->SetDigis(digiIndices);                                                                    // add all digis of this cluster
}
ClassImp(CbmTrdClusterFinderDPB)
