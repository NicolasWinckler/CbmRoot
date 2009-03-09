/** CbmMuchFindHitsSimple.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 22.03.07
 *@version 1.0
 **
 ** CBM task class for finding hits in the MUCH by simple algorithm
 ** Task level RECO
 ** Produces objects of type CbmMuchHits out of CbmMuchDigi.
 **/


#ifndef CBMMUCHFINDHITSSIMPLE_H
#define CBMMUCHFINDHITSSIMPLE_H 1

#include "CbmMuchTask.h"
#include "CbmMuchSector.h"

#include "FairTask.h"

#include "TFile.h"

class CbmMuchFindHitsSimple : public CbmMuchTask
{
 public :

  /** Default constructor **/
  CbmMuchFindHitsSimple();

  /** Standard constructor **/
  CbmMuchFindHitsSimple(Int_t iVerbose);

  /** Constructor with task name **/
  CbmMuchFindHitsSimple(const char* name, TFile* digiFile, Int_t iVerbose);

  /** Destructor **/
  virtual ~CbmMuchFindHitsSimple();

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

  ClassDef(CbmMuchFindHitsSimple,1);
};

#endif
