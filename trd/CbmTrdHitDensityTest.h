#ifndef CBMTRDHITDENSITYTEST_H
#define CBMTRDHITDENSITYTEST_H 

#include "FairTask.h"
#include "CbmTrdDetectorId.h"

#include "CbmTrdClusterizer.h"
//#include "CbmTrdClusterFinderFast.h"
#include "CbmTrdHitProducerCluster.h"
#include "TLegend.h"
#include "TClonesArray.h"
#include "TSystem.h"
#include "TVector3.h"
#include <vector>
#include <list>
#include <map>

class CbmTrdDigiPar;
class CbmTrdModule;
class TClonesArray;

typedef std::list<MyPoint*> MyPointList;
typedef std::map<Int_t, MyPointList*> MyPointListMap;

class CbmTrdHitDensityTest : public FairTask
{
 public:
  // ---- Default constructor -------------------------------------------
  CbmTrdHitDensityTest();
  // ---- Destructor ----------------------------------------------------
  virtual ~CbmTrdHitDensityTest();
  /** Initialisation **/
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();
  
  /** Executed task **/
  virtual void Exec(Option_t * option);

  /** Virtual method EndOfEvent
   **
   ** Reset hit collection at the
   ** end of the event.
   **/
  virtual void FinishEvent();

  /** Finish task **/
  virtual void Finish();
  void Register();

 private:

  TClonesArray*     fTrdPoints;   /** Input array of Trd MC points **/
  TClonesArray*     fDigis;       /** Input array of CbmTrdDigi **/
  TClonesArray*     fClusters;    /** Input array of CbmTrdCluster **/
  TClonesArray*     fClusterHits; /** Output array of CbmTrdHit **/

  CbmTrdDigiPar *fDigiPar;   //!
  CbmTrdModule  *fModuleInfo; //!  
  CbmTrdDetectorId fTrdId; //!

  Int_t fStation;
  Int_t fLayer;
  Int_t fModuleID;

  Int_t fMCindex;

  std::vector<Int_t> fLayersBeforeStation; //! 

  std::map<Int_t, MyPointList*> ModulePointMap;

  CbmTrdHitDensityTest(const CbmTrdHitDensityTest&);
  CbmTrdHitDensityTest& operator=(const CbmTrdHitDensityTest&);

  ClassDef(CbmTrdHitDensityTest,1);
};
#endif
