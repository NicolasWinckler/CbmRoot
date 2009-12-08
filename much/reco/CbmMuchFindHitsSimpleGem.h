/** CbmMuchFindHitsSimpleGem.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 22.03.07
 *@version 1.0
 **
 ** CBM task class for finding hits in the MUCH by simple algorithm
 ** Task level RECO
 ** Produces objects of type CbmMuchHits out of CbmMuchDigi.
 **/


#ifndef CBMMUCHFINDHITSSIMPLEGEM_H
#define CBMMUCHFINDHITSSIMPLEGEM_H 1

#include "CbmMuchGeoScheme.h"

#include "FairTask.h"

#include "TStopwatch.h"
#include "TString.h"


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
  TString                          fDigiFile;          // Digitization file
  TClonesArray*                    fHits;              // Output array of CbmMuchHit
  TClonesArray*                    fClusters;          // Output array of CbmMuchCluster
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
