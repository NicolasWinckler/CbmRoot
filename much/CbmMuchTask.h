/** CbmMuchTask.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 22.03.07
 *@version 1.0
 **
 ** Base CBM task class for finding hits in the MUCH
 ** Task level RECO
 ** Produces objects of type CbmMuchHits out of CbmMuchDigi.
 **/


#ifndef CBMMUCHTASK_H
#define CBMMUCHTASK_H 1

#include "CbmMuchGeoScheme.h"
#include "CbmTask.h"
#include "TStopwatch.h"
#include "TClonesArray.h"
#include "TString.h"


class CbmMuchTask //: public CbmTask
{
 public :

  /** Default constructor **/
  CbmMuchTask();

  /** Standard constructor **/
  CbmMuchTask(Int_t iVerbose);

  /** Constructor with task name **/
  CbmMuchTask(const char* name, Int_t iVerbose);

  /** Destructor **/
  virtual ~CbmMuchTask();

  /** Execution **/
  virtual void Exec(Option_t* opt){}

  /** Get parameter containers **/
  virtual void SetParContainers(){}

  /** Intialisation **/
  virtual InitStatus Init(){}

  /** Reinitialisation **/
  virtual InitStatus ReInit(){}


 protected:
  CbmMuchGeoScheme*                fGeoScheme;         // Geometry scheme
  TClonesArray*                    fHits;              // Output array of CbmMuchHit
  TStopwatch                       fTimer;             // Timer
  Int_t                            fVerbose;           // Verbosity level
  TString                          fName;              // Name
  
  ClassDef(CbmMuchTask,1);
};

#endif
