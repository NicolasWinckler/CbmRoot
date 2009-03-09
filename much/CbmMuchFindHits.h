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
#include "FairTask.h"

#include "TStopwatch.h"
#include "TClonesArray.h"
#include "TFile.h"

class CbmMuchFindHits : public FairTask
{
 public :

  /** Default constructor. **/
  CbmMuchFindHits();

  /**
   * Standard constructor.
   * @param iVerbose   Verbosity level.
   */
  CbmMuchFindHits(Int_t iVerbose);

  /**
   * Constructor with task name.
   * @param name      Task name.
   * @param digiFile  Input digitization scheme file.
   * @param iVerbose  Verbosity level.
   */
  CbmMuchFindHits(const char* name, const char* digiFile, Int_t iVerbose);

  /** Destructor. **/
  virtual ~CbmMuchFindHits();

  /** Execution. **/
  virtual void Exec(Option_t* opt);

  /**
   * Sets whether to use clustering or not.
   * @param useClustering  1 if clustering should be used, 0 otherwise (0 by default).
   */
  void SetUseClustering(Int_t useClustering) {fUseClustering = useClustering;}


 private:
  CbmMuchTask*                     fSubTask;           // Subtask to exec
  Int_t                            fUseClustering;     // Switch showing whether to use clustering algorithm
  TFile*                           fDigiFile;          // Digitization file

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Initialization **/
  virtual InitStatus Init();

  /** Reinitialization **/
  virtual InitStatus ReInit();

  ClassDef(CbmMuchFindHits,1);
};

#endif
