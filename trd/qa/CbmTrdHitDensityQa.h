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
#include <fstream>

class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdGeoHandler;
class TH2I;
class TH1I;
class TH1D;
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
  void SetTriggerThreshold(Double_t triggerthreshold);
  void SetNeighbourTrigger(Bool_t trigger);

 private:

  std::ofstream myfile;

  Double_t fBitPerHit;
  TH1F* h1DataModule;
  TH1F* h1OptLinksModule;

  Bool_t fNeighbourTrigger;
  Bool_t fPlotResults;

  TClonesArray*     fDigis;       /** Input array of CbmTrdDigi **/
  TClonesArray*     fClusters; 

  CbmTrdDigiPar *fDigiPar;   //!
  CbmTrdModule  *fModuleInfo; //!  
  CbmTrdGeoHandler* fGeoHandler; //!

  Int_t fStation;
  Int_t fLayer;
  Int_t fModuleID;

  TH1I *fEventCounter;

  Double_t fTriggerThreshold;
  Double_t fEventRate;
  std::map<Int_t, Int_t> fUsedDigiMap;
  std::map<Int_t, TH2I*> fModuleHitMap;
  std::map<Int_t, TH2I*>::iterator fModuleHitMapIt;
  std::map<Int_t, TH1D*> fModuleHitASICMap;
  std::map<Int_t, TH1D*>::iterator fModuleHitASICMapIt;
  CbmTrdHitDensityQa(const CbmTrdHitDensityQa&);
  CbmTrdHitDensityQa& operator=(const CbmTrdHitDensityQa&);

  ClassDef(CbmTrdHitDensityQa,2);
};
#endif
