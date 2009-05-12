/** CbmMuchFindHitsStraws.h
 **/


#ifndef CBMMUCHFINDHITSSTRAWS_H
#define CBMMUCHFINDHITSSTRAWS_H 1

#include "CbmMuchSector.h"
#include "CbmMuchGeoScheme.h"

#include "FairTask.h"

#include "TFile.h"
#include "TClonesArray.h"

class CbmMuchFindHitsStraws : public FairTask
{
 public :

  /** Default constructor **/
  CbmMuchFindHitsStraws();

  /** Standard constructor **/
  CbmMuchFindHitsStraws(Int_t iVerbose);

  /** Constructor with task name **/
  CbmMuchFindHitsStraws(const char* name, const char* digiFileName, Int_t iVerbose);

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
  CbmMuchGeoScheme*                fGeoScheme;         // Geometry scheme
  TFile*                           fDigiFile;          // Digitization file
  TClonesArray*                    fDigis;             // Input array of CbmMuchDigi
  TClonesArray*                    fHits;              // Output array of CbmMuchHit

  ClassDef(CbmMuchFindHitsStraws,1);
};

#endif
