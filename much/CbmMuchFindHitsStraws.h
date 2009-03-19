/** CbmMuchFindHitsStraws.h
 **/


#ifndef CBMMUCHFINDHITSSTRAWS_H
#define CBMMUCHFINDHITSSTRAWS_H 1

#include "CbmMuchTask.h"
#include "CbmMuchSector.h"
#include "CbmMuchGeoScheme.h"

#include "FairTask.h"

#include "TFile.h"

class CbmMuchFindHitsStraws : public CbmMuchTask
{
 public :

  /** Default constructor **/
  CbmMuchFindHitsStraws();

  /** Standard constructor **/
  CbmMuchFindHitsStraws(Int_t iVerbose);

  /** Constructor with task name **/
  CbmMuchFindHitsStraws(const char* name, TFile* digiFile, Int_t iVerbose);

  /** Destructor **/
  virtual ~CbmMuchFindHitsStraws();

  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Initialization **/
  virtual InitStatus Init();

  /** Reinitialization **/
  virtual InitStatus ReInit();

 private:
  TFile*                           fDigiFile;          // Digitization file
  TClonesArray*                    fDigis;             // Input array of CbmMuchDigi

  ClassDef(CbmMuchFindHitsStraws,1);
};

#endif
