#ifndef CBMTRDHITDENSITYQA_H_
#define CBMTRDHITDENSITYQA_H_

#include "FairTask.h"

#include "CbmTrdDigitizerMathieson.h"
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
class CbmTrdGeoHandler;

class TClonesArray;

typedef std::list<MyPoint*> MyPointList;
typedef std::map<Int_t, MyPointList*> MyPointListMap;

class CbmTrdHitDensityQa : public FairTask
{
 public:
  // ---- Default constructor -------------------------------------------
  CbmTrdHitDensityQa();
  // ---- Destructor ----------------------------------------------------
  virtual ~CbmTrdHitDensityQa();
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
  CbmTrdGeoHandler* fGeoHandler; //!

  Int_t fStation;
  Int_t fLayer;
  Int_t fModuleID;

  Int_t fMCindex;

  std::map<Int_t, MyPointList*> ModulePointMap;

  CbmTrdHitDensityQa(const CbmTrdHitDensityQa&);
  CbmTrdHitDensityQa& operator=(const CbmTrdHitDensityQa&);

  ClassDef(CbmTrdHitDensityQa,2);
};
#endif
