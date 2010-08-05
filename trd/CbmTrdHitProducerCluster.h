#ifndef CBMTRDHITPRODUCERCLUSTER_H
#define CBMTRDHITPRODUCERCLUSTER_H 

#include "FairTask.h"
#include "CbmTrdDetectorId.h"

#include <vector>
#include <list>
#include <map>

class CbmTrdDigiPar;
class CbmTrdModule;
class TClonesArray;


class CbmTrdHitProducerCluster : public FairTask
{
 public:

// ---- Default constructor -------------------------------------------
  CbmTrdHitProducerCluster();
// ---- Destructor ----------------------------------------------------
  virtual ~CbmTrdHitProducerCluster();
 /** Initialisation **/
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();
  
  /** Executed task **/
  virtual void Exec(Option_t * option);

  /** Finish task **/
  virtual void Finish();
  void Register();

 private:
  void GetModuleInfo(Int_t qMaxIndex);
  void SortClusterDigi();
  void PrfReco(Int_t qMaxIndex, Float_t qMax);
  void SimpleReco(Int_t qMaxIndex, Float_t qMax);
  void AddHit(Int_t iHit);


  TClonesArray*     fDigis;       /** Input array of CbmTrdDigi **/
  TClonesArray*     fClusters;    /** Input array of CbmTrdCluster **/
  TClonesArray*     fClusterHits; /** Output array of CbmTrdHit **/

  CbmTrdDigiPar *fDigiPar;   //!
  CbmTrdModule  *fModuleInfo; //!
  
  CbmTrdDetectorId fTrdId; //!



  ClassDef(CbmTrdHitProducerCluster,1);
};
#endif
