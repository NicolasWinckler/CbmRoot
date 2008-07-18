 /******************************************************************************
*  $Id: CbmRichSelectRings.h,v 1.1 2006/09/13 14:53:31 hoehne Exp $
*
*  Class  : CbmRichSelectRings
*  Description: Header of the SelectRings class. 
*               This class takes a particular Ring Selection to 
*               distinguish good and fake rings
*
*  Author : Simeon Lebedev
*  E-mail : salebedev@jinr.ru
*
*******************************************************************************
*  $Log: CbmRichSelectRings.h,v $
*  Revision 1.1  2006/09/13 14:53:31  hoehne
*  initial version
*
*  
*
*******************************************************************************/


#ifndef CBM_RICH_SELECT_RINGS
#define CBM_RICH_SELECT_RINGS 1


#include "CbmTask.h"
#include "CbmRichRingSelect.h"


class CbmRichSelectRings : public CbmTask
{

 public:

  CbmRichSelectRings();

  CbmRichSelectRings(CbmRichRingSelect* select, Int_t verbose = 1);

  CbmRichSelectRings(const char* name, const char* title = "CbmTask",
		   CbmRichRingSelect* select = NULL, Int_t verbose = 1);


  /** Destructor **/
  virtual ~CbmRichSelectRings();


  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Finish at the end of each event **/
  virtual void Finish();


  /** Accessors **/
  CbmRichRingSelect* GetSelect() { return fSelect; };


  /** Set concrete track finder **/
  void UseSelect(CbmRichRingSelect* select) { fSelect = select; };



 private:

  CbmRichRingSelect* fSelect;    //! Pointer to RingFinder concrete class
  TClonesArray* fRichHitArray;   //! Input array of RICH hits
  TClonesArray* fProjArray;     //! Input array of projected tracks
  TClonesArray* fRingArray;      //! Input array of CbmRichRings
  Int_t fVerbose;                //! Verbosity level


  ClassDef(CbmRichSelectRings,1);

};

#endif

