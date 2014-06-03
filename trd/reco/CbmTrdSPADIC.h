// -------------------------------------------------------------------------
// -----          CbmTrdSPADIC header file                  -----
// -----   Created 03/06/14  by Cyrano Bergmann       -----
// -------------------------------------------------------------------------


#ifndef CBMTRDSPADIC_H
#define CBMTRDSPADIC_H 

#include "FairTask.h"

class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdGeoHandler;
class TClonesArray;

class CbmTrdSPADIC : public FairTask
{

 public:

  /**
   * Default constructor.
   */
  CbmTrdSPADIC();
  CbmTrdSPADIC(Bool_t MultiHit, Bool_t NeighbourReadout, Bool_t RowClusterMerger, Double_t MinimumChargeTH);

  /**
   * \brief Destructor.
   */
  virtual ~CbmTrdSPADIC();
 
  /**
   * \brief Inherited from FairTask.
   */
  virtual InitStatus Init();

  /**
   * \breif Inherited from FairTask.
   */
  virtual void SetParContainers();
  
  /**
   * \breif Inherited from FairTask.
   */
  virtual void Exec(Option_t * option);

  void SetTriggerThreshold(Double_t triggerthreshold);

  //Bool_t CbmDigiSorter(std::pair< Int_t, Int_t> a, std::pair< Int_t, Int_t> b);

  TClonesArray*     fDigis;       /** Input array of CbmTrdDigi **/

  CbmTrdDigiPar *fDigiPar;   //!
  CbmTrdModule  *fModuleInfo; //!
  
  CbmTrdGeoHandler* fGeoHandler; //!

  Double_t fMinimumChargeTH;

  CbmTrdSPADIC(const CbmTrdSPADIC&);
  CbmTrdSPADIC& operator=(const CbmTrdSPADIC&);

  ClassDef(CbmTrdSPADIC,2);
};
#endif
