/******************************************************************************
*  $Id: CbmRichFindRings.h,v 1.2 2006/01/26 09:58:37 hoehne Exp $
*
*  Class  : CbmRichFindRings
*  Description: This is the header of the FindRings class. This
*               takes a particular Ring Finder to find the rings.
*             Input: TClonesArray of CbmRichHit
*             Output: TClonesArray of CbmRichRing
*
*  Author : Supriya Das
*  E-mail : S.Das@gsi.de
*
*******************************************************************************
*  $Log: CbmRichFindRings.h,v $
*  Revision 1.2  2006/01/26 09:58:37  hoehne
*  array of projected tracks added for track-based finders
*
*  Revision 1.1  2006/01/19 11:24:39  hoehne
*  initial revision
*
*
*******************************************************************************/

#ifndef CBM_RICH_FIND_RINGS
#define CBM_RICH_FIND_RINGS 1


#include "CbmTask.h"

class CbmRichRingFinder;
class TClonesArray;

class CbmRichFindRings : public CbmTask
{

 public:

  /** Default constructor **/
  CbmRichFindRings();


  /** Standard constructor
   *@param finder   Pointer to RICH ring finder concrete class
   *@param verbose  Verbosity level
   **/
  CbmRichFindRings(CbmRichRingFinder* finder, Int_t verbose = 1);


  /** Constructor with name and title
   *@param name     Name of class
   *@param title    Task title
   *@param finder   Pointer to RICH ring finder concrete class
   *@param verbose  Verbosity level
   **/
  CbmRichFindRings(const char* name, const char* title = "CbmTask",
		   CbmRichRingFinder* finder = NULL, Int_t verbose = 1);


  /** Destructor **/
  virtual ~CbmRichFindRings();


  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Finish at the end of each event **/
  virtual void Finish();


  /** Accessors **/
  CbmRichRingFinder* GetFinder() { return fFinder; };
  Int_t GetNofRings()           { return fNofRings; };


  /** Set concrete track finder **/
  void UseFinder(CbmRichRingFinder* finder) { fFinder = finder; };



 private:

  CbmRichRingFinder* fFinder;    //! Pointer to RingFinder concrete class
  TClonesArray* fRichHitArray;   //! Input array of RICH hits
  TClonesArray* fProjArray;     //! Input array of projected tracks
  TClonesArray* fRingArray;      //! Output array of CbmRichRings
  Int_t fNofRings;               //! Number of rings created
  Int_t fVerbose;                //! Verbosity level


  ClassDef(CbmRichFindRings,1);

};

#endif
