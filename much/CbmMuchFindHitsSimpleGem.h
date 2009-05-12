/** CbmMuchFindHitsSimpleGem.h
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

#include "CbmMuchSector.h"
#include "CbmMuchGeoScheme.h"
#include "TStopwatch.h"

#include "FairTask.h"

#include "TFile.h"
#include "TClonesArray.h"

class CbmMuchFindHitsSimpleGem : public FairTask
{
 public :

  /** Default constructor **/
  CbmMuchFindHitsSimpleGem();

  /** Standard constructor **/
  CbmMuchFindHitsSimpleGem(Int_t iVerbose);

  /** Constructor with task name **/
  CbmMuchFindHitsSimpleGem(const char* name, const char* digiFileName, Int_t iVerbose);

  /** Destructor **/
  virtual ~CbmMuchFindHitsSimpleGem();

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


  ClassDef(CbmMuchFindHitsSimpleGem,1);
};

#endif
