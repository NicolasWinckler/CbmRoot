// -------------------------------------------------------------------------
// -----                      CbmTrdDigiDraw header file                  -----
// -----          Created 10/12/07  by M. Al-Turany                    -----
// -------------------------------------------------------------------------


/** CbmTrdDigiDraw         
 * @author M. Al-Turany
 * @since 03.01.08
 *   Task to display Hits 
 **
 **/

#ifndef CBMTRDDIGIDRAW_H
#define CBMTRDDIGIDRAW_H


#include "FairTask.h"

//#include "CbmTrdDigiPar.h"
//#include "CbmTrdModule.h"
#include "CbmTrdDetectorId.h"

class CbmTrdDigiPar;
class CbmTrdModule;
class FairEventManager;

class TClonesArray;
class TEveBoxSet;

class CbmTrdDigiDraw : public FairTask {

 public:

  /** Default constructor **/
  CbmTrdDigiDraw();


  /** Standard constructor 
  *@param name        Name of task
  *@param iVerbose    Verbosity level
  **/
  CbmTrdDigiDraw(const char* name, Color_t color ,Style_t mstyle, Int_t iVerbose = 1);

  /** Destructor **/
  virtual ~CbmTrdDigiDraw();

  /** Set verbosity level. For this task and all of the subtasks. **/
  void SetVerbose(Int_t iVerbose);
   /** Executed task **/ 
  virtual void Exec(Option_t* option);
  void Reset();

  void SetLayerStation1(Bool_t Layer1, Bool_t Layer2,
                        Bool_t Layer3, Bool_t Layer4);
  void SetLayerStation2(Bool_t Layer1, Bool_t Layer2,
                        Bool_t Layer3, Bool_t Layer4);
  void SetLayerStation3(Bool_t Layer1, Bool_t Layer2,
                        Bool_t Layer3, Bool_t Layer4);

protected:

  Int_t   fVerbose;       //  Verbosity level
  virtual void SetParContainers() ;
  virtual InitStatus Init();
  /** Action after each event**/
  virtual void Finish() ;
  TClonesArray *fPointList; //!
  CbmTrdDigiPar *fDigiPar; //!
  CbmTrdModule *fModuleInfo; //!
  FairEventManager *fEventManager;   //!
  TEveBoxSet* fq;    //!
  CbmTrdDetectorId fTrdId; //!
  Color_t fColor; //!
  Style_t fStyle; //!

  Bool_t fActiveLayers[3][4];

  ClassDef(CbmTrdDigiDraw,1);
    
};

                            
#endif
