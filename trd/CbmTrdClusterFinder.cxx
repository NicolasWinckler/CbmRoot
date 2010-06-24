#include "CbmTrdClusterFinder.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdDigi.h"
#include "CbmTrdCluster.h"
#include "CbmTrdModule.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TArray.h"

#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using std::setprecision;

// ---- Default constructor -------------------------------------------
CbmTrdClusterFinder::CbmTrdClusterFinder()
  :FairTask("TrdClusterFinder",1),
   fUseSimpleClustering(kTRUE),
   fDigis(NULL),
   fClusters(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fTrdId(),
   fDigiMap()
{

}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdClusterFinder::~CbmTrdClusterFinder()
{

  if(fClusters){
    fClusters->Clear("C");
    fClusters->Delete();
    delete fClusters;
  }
  if(fDigis){
    fDigis->Delete();
    delete fDigis;
  }
  if(fDigiPar){
    delete fDigiPar;
  }
  if(fModuleInfo){
    delete fModuleInfo;
  }

}

// ----  Initialisation  ----------------------------------------------
void CbmTrdClusterFinder::SetParContainers()
{

  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
  fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));
  
}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdClusterFinder::ReInit(){
  
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
  fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdClusterFinder::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();
  
  fDigis =(TClonesArray *)  ioman->ActivateBranch("TrdDigi");
  if ( ! fDigis ) {
    cout << "-W CbmTrdHitProducerDigi::Init: No TrdDigi array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
  }
  
  fClusters = new TClonesArray("CbmTrdCluster", 100);
  ioman->Register("TrdCluster","TRD",fClusters,kTRUE);
  
  return kSUCCESS;
  
} 
// --------------------------------------------------------------------

// ---- Exec ----------------------------------------------------------
void CbmTrdClusterFinder::Exec(Option_t * option)
{

  fClusters->Clear();

  if(fUseSimpleClustering == kTRUE){
    SimpleClustering();
  } else {
    RealClustering();
  }

}
// --------------------------------------------------------------------
void CbmTrdClusterFinder::SimpleClustering()
{
  //Loop over all digis and create for each digi a cluster

  TArrayI digiIndex(1);
  CbmTrdDigi *digi=NULL;
  Int_t nentries = fDigis->GetEntries();

  for (int iDigi=0; iDigi < nentries; iDigi++ ) {

    digi =  (CbmTrdDigi*) fDigis->At(iDigi);

    digiIndex.SetAt(iDigi,0);


    TClonesArray& clref = *fClusters;
    Int_t size = clref.GetEntriesFast();

    
    new((*fClusters)[size]) CbmTrdCluster(digiIndex,digi->GetCharge(),
					  digi->GetCharge());
    
  }
}
// --------------------------------------------------------------------
void CbmTrdClusterFinder::RealClustering()
{
  SortDigis();

  //Loop over all modules with a fired pad in at least on of their
  //sectors. Loop for each of these modules over all sectors, check 
  //if they contain a fired pad and if any pad is fired loop over all 
  //fired pads of these sector and create clusters of neighbouring pads.
  //In the end check if the clusters end at a boundary between sectors. 
  //If the cluster ends at such a boundary and there is a corresponding
  //cluster in the neighbouring sector merge the two clusters.
}
// --------------------------------------------------------------------
void CbmTrdClusterFinder::SortDigis()
{
  //Put all digis which belong to one sector (unique sector 
  //in a unique module) into a set. Put all of theses sets
  //into a map.

  // Clear sector digi sets
  std::map<Int_t, std::set<Int_t> >::iterator mapIt;
  for (mapIt=fDigiMap.begin(); mapIt!=fDigiMap.end(); mapIt++)
    ((*mapIt).second).clear();

  for (mapIt=fModuleMap.begin(); mapIt!=fModuleMap.end(); mapIt++)
    ((*mapIt).second).clear();

  Int_t sectorId, moduleId;
  CbmTrdDigi *digi=NULL;
  Int_t nentries = fDigis->GetEntries();

  for (int iDigi=0; iDigi < nentries; iDigi++ ) {
    digi =  (CbmTrdDigi*) fDigis->At(iDigi);

    //Get the sectorId 
    sectorId = digi->GetDetId();
    moduleId= fTrdId.GetModuleId(sectorId);
 
    // TODO: Maybe do this in the initilization

    // Add all sectors of a module which have at least one
    // fired pad to the map of modules
    if ( fModuleMap.find(moduleId) == fModuleMap.end() ) {
      std::set<Int_t> a;
      a.insert(sectorId);
      fModuleMap[moduleId] = a;

    } else {
      fModuleMap[moduleId].insert(sectorId);
    }
  

    // if the sector is not in the map add it, otherwise
    // add the new digi to the map.
    if ( fDigiMap.find(sectorId) == fDigiMap.end() ) {
      std::set<Int_t> a;
      a.insert(iDigi);
      fDigiMap[sectorId] = a;

    } else {
      fDigiMap[sectorId].insert(iDigi);
    }
  }

}
// ---- Finish --------------------------------------------------------
void CbmTrdClusterFinder::Finish()
{
}

ClassImp(CbmTrdClusterFinder)

