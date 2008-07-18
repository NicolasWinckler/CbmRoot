/** CbmMuchFindHits.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 22.03.07
 *@version 1.0
 **
 ** CBM task class for finding hits in the MUCH
 ** Task level RECO
 ** Produces objects of type CbmMuchHits out of CbmMuchDigi.
 **/


#ifndef CBMMUCHFINDHITS_H
#define CBMMUCHFINDHITS_H 1

#include "CbmMuchGeoScheme.h"
#include "CbmMuchTask.h"
#include "CbmTask.h"

#include "TStopwatch.h"
#include "TClonesArray.h"
#include "TFile.h"

class CbmMuchFindHits : public CbmTask
{
 public :

  /** Default constructor **/
  CbmMuchFindHits();

  /** Standard constructor **/
  CbmMuchFindHits(Int_t iVerbose);

  /** Constructor with task name **/
  CbmMuchFindHits(const char* name, const char* digiFile, Int_t iVerbose);

  /** Destructor **/
  virtual ~CbmMuchFindHits();

  /** Execution **/
  virtual void Exec(Option_t* opt);

  void SetUseClustering(Int_t useClustering) {fUseClustering = useClustering;}


 private:
  CbmMuchTask*                     fSubTask;           // Subtask to exec
  Int_t                            fUseClustering;     // Switch showing whether to use clustering algorithm
  TFile*                           fDigiFile;          // Digitization file

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Intialisation **/
  virtual InitStatus Init();

  /** Reinitialisation **/
  virtual InitStatus ReInit();

  ClassDef(CbmMuchFindHits,1);
};

#endif
