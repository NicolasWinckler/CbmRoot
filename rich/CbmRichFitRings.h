/******************************************************************************
*  $Id: CbmRichFitRings.h,v 1.1 2006/01/19 12:19:27 hoehne Exp $
*
*  Class  : CbmRichRingFitter
*  Description: Header for the Task class for Ring Fitting in RICH.
*
*  Author : Supriya Das
*  E-mail : S.Das@gsi.de
*
*******************************************************************************
*  $Log: CbmRichFitRings.h,v $
*  Revision 1.1  2006/01/19 12:19:27  hoehne
*  initial version
*
*
*******************************************************************************/

#ifndef CBM_RICH_FIT_RINGS
#define CBM_RICH_FIT_RINGS 1


#include "CbmTask.h"

class CbmRichRingFitter;
class TClonesArray;

class CbmRichFitRings : public CbmTask
{

 public:

  /** Default constructor **/
  CbmRichFitRings();

  /** Standard constructor
   *@param fitter   Pointer to RICH ring fitter concrete class
   *@param verbose  Verbosity level
   **/
  CbmRichFitRings(CbmRichRingFitter* fitter, Int_t verbose = 1);


  /** Standard constructor with name and title
   **
   *@param name   Name of class
   *@param title  Task title
   *@param ftter  Pointer to RICH ring fitter concrete class
   *@param verbose  Verbosity level
   **/
  CbmRichFitRings(const char* name, const char* title = "CbmTask",
		  CbmRichRingFitter* fitter = NULL, Int_t verbose = 1);


  /** Destructor **/
  virtual ~CbmRichFitRings();


  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Finish at the end of each event **/
  virtual void Finish();


  /** Accessors **/
  CbmRichRingFitter* GetFitter() { return fFitter; };
  Int_t GetNofRings()            { return fNofRings; };


  /** Set concrete track finder **/
  void UseFitter(CbmRichRingFitter* fitter) { fFitter = fitter; };



 private:

  CbmRichRingFitter* fFitter;    /** Pointer to TrackFinder concrete class */
  TClonesArray* fRingArray;      /** Input array of RICH Rings */
  Int_t fNofRings;               /** Number of tracks successfully fitted */
  Int_t fVerbose;                //! Verbosity level

  ClassDef(CbmRichFitRings,1);

};

#endif
