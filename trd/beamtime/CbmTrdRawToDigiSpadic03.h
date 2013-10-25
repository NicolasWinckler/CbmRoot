
#ifndef CBMTRDRAWTODIGISPADIC03_H
#define CBMTRDRAWTODIGISPADIC03_H


#include "FairTask.h"
#include "CbmTrdDigi.h"

class TPrincipal;
class CbmTrdDigiPar;
class CbmTrdModule;
class TClonesArray;
class CbmTrdGeoHandler;

class CbmTrdRawToDigiSpadic03 : public FairTask {

 public:

  CbmTrdRawToDigiSpadic03();

  CbmTrdRawToDigiSpadic03(const char *name, const char *title="CBM Task");

  virtual ~CbmTrdRawToDigiSpadic03();

  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();

  /** Executed task **/
  virtual void Exec(Option_t * option);

  /** Finish (called after each event) **/
  virtual void FinishEvent();
 
  /** Finish task (called after all event) **/
  virtual void FinishTask(){;}

  void Register();

 private:

  TPrincipal* fPrincipal;
  CbmTrdDigiPar  *fDigiPar;    //!
  CbmTrdModule   *fModuleInfo; //!
  TClonesArray *fDigis; //! TRD digis
  CbmTrdGeoHandler* fGeoHandler; //!

  CbmTrdRawToDigiSpadic03& operator=(const CbmTrdRawToDigiSpadic03&);
  CbmTrdRawToDigiSpadic03(const CbmTrdRawToDigiSpadic03&);

  ClassDef(CbmTrdRawToDigiSpadic03,1)

    };

#endif//CBMTRDRAWTODIGISPADIC03_H
