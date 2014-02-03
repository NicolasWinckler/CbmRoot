#include "CbmFiberHodoClusterFinder.h"

#include "CbmFiberHodoDigi.h"
#include "CbmFiberHodoCluster.h"
#include "CbmFiberHodoAddress.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"

using std::map;
using std::set;

// ---- Default constructor -------------------------------------------
CbmFiberHodoClusterFinder::CbmFiberHodoClusterFinder()
  :FairTask("FiberHodoClusterFinder",1),
   fDigis(NULL),
   fClusters(NULL)
{
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmFiberHodoClusterFinder::~CbmFiberHodoClusterFinder()
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

}

// ----  Initialisation  ----------------------------------------------
void CbmFiberHodoClusterFinder::SetParContainers()
{

  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmFiberHodoClusterFinder::ReInit(){
  
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmFiberHodoClusterFinder::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();
  
  fDigis =(TClonesArray *)  ioman->GetObject("FiberHodoDigi");
  if ( ! fDigis ) {
    LOG(ERROR) << "No FiberHodoDigi array!" << FairLogger::endl;
    LOG(ERROR) << "Task will be inactive" << FairLogger::endl;
    return kERROR;
  }
  
  fClusters = new TClonesArray("CbmFiberHodoCluster", 100);
  ioman->Register("FiberHodoCluster","TRD",fClusters,kTRUE);

  return kSUCCESS;
  
} 
// --------------------------------------------------------------------

// ---- Exec ----------------------------------------------------------
void CbmFiberHodoClusterFinder::Exec(Option_t * option)
{
  
  // Clear all output structures/containers
  fClusters->Clear();
  map<Int_t, set<CbmFiberHodoDigi*, classcomp> >::iterator mapIt;
  for (mapIt=fDigiMap.begin(); mapIt!=fDigiMap.end(); mapIt++) {
    ((*mapIt).second).clear();
  }
  fDigiMap.clear();

  set<Int_t> layerSet;

  CbmFiberHodoDigi* digi = NULL;

  // sort all digis according the detector layer into different sets
  // defining a order functor for the set should result in a set which
  // is already ordered from small to big strip numbers 
  Int_t nofDigis = fDigis->GetEntries();
  for (Int_t iDigi=0; iDigi < nofDigis; iDigi++ ) {

    digi = (CbmFiberHodoDigi*) fDigis->At(iDigi);

    Int_t layer = CbmFiberHodoAddress::GetLayerAddress(digi->GetAddress());

    layerSet.insert(layer);
    fDigiMap[layer].insert(digi);

  }

  set <CbmFiberHodoDigi*, classcomp> digiSet;

  // loop over all detector layers and for each detector layer loop over the
  // already sorted strips to do the cluster finding. A cluster is defined by
  // neighboring strips which are above the threshold
  // TODO: Decide if the condition of the ADC threshold should be done here or
  //       in the unpacking
  //       Check if the set is realy ordered with increasing strip number
  for (set<Int_t>::iterator i = layerSet.begin(); i != layerSet.end(); i++) {
    digiSet = fDigiMap[*i];
    Bool_t newCluster = kTRUE;
    Int_t fiberNr = -1;
    Int_t fiberNrPrev = -1;
    for (set<CbmFiberHodoDigi*, classcomp>::iterator j = digiSet.begin(); j != digiSet.end(); j++) {

      if (newCluster) {
//      fClusters = new ((*fClusters)[fClusters->GetEntriesFast()]) 
//                  CbmFiberHodoCluster(*j);
        newCluster = kFALSE;
      } else {
        // check if the next fiber is a direct neighbor of the previos one
        fiberNr = CbmFiberHodoAddress::GetStripId((*j)->GetAddress());
        if (1 == fiberNr-fiberNrPrev) {
          fiberNrPrev = fiberNr;
          // Add digi to cluster
        } else {
          newCluster = kTRUE;
        } 
      }
    }
  }


}
// --------------------------------------------------------------------

  // ---- Finish --------------------------------------------------------
  void CbmFiberHodoClusterFinder::Finish()
  {
  }

  ClassImp(CbmFiberHodoClusterFinder)

