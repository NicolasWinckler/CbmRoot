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
#include "CbmMuchGeoScheme.h"

#include "FairTask.h"

#include "TFile.h"

class CbmMuchFindHitsSimple : public FairTask
{
 public :

  /** Default constructor **/
  CbmMuchFindHitsSimple();

  /** Standard constructor **/
  CbmMuchFindHitsSimple(Int_t iVerbose);

  /** Constructor with task name **/
  CbmMuchFindHitsSimple(const char* name, const char* digiFileName, Int_t iVerbose);

  /** Destructor **/
  virtual ~CbmMuchFindHitsSimple();

  /** Execution **/
  virtual void Exec(Option_t* opt);

 private:
  CbmMuchGeoScheme*                fGeoScheme;         // Geometry scheme
  TFile*                           fDigiFile;          // Digitization file
  TClonesArray*                    fHits;              // Output array of CbmMuchHit
  TClonesArray*                    fDigis;             // Input array of CbmMuchDigi
  TStopwatch                       fTimer;             // Timer

  /** Get parameter containers. */
  virtual void SetParContainers();
  /** Initialization. */
  virtual InitStatus Init();
  /** Reinitialization. */
  virtual InitStatus ReInit();


  ClassDef(CbmMuchFindHitsSimple,1);
};

#endif
