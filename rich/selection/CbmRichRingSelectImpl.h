/******************************************************************************
*  $Id: CbmRichRingSelectImpl.h,v 1.1 2006/09/13 14:53:31 hoehne Exp $
*
*  Class  : CbmRichRingSelectImpl
*  Description : Abstract base class for concrete RICH ring selection algorithm:
*                to be run after ring-track assign for fake-ring rejection
*
*  Author : Simeon Lebedev
*  E-mail : salebedev@jinr.ru
*
*******************************************************************************
*  $Log: CbmRichRingSelectImpl.h,v $
*  Revision 1.1  2006/09/13 14:53:31  hoehne
*  initial version
*
*
*
*******************************************************************************/


#ifndef CBM_RICH_RING_SELECT_IMPL
#define CBM_RICH_RING_SELECT_IMPL 1

#include "CbmRichRingSelect.h"

#include "TObject.h"
#include "TClonesArray.h"

class CbmRichRing;

class CbmRichRingSelectImpl : public  CbmRichRingSelect
{

 public:

  /** Default constructor **/
  CbmRichRingSelectImpl() { };

  /** Destructor **/
  virtual ~CbmRichRingSelectImpl() { };

  virtual void Init();

  virtual void DoSelect(CbmRichRing* ring) = 0;

   void SetVerbose(Int_t verbose) { fVerbose = verbose; };

  Int_t GetNofHitsOnRing(CbmRichRing* ring);
  Int_t GetNofHitsOnRingCircle(CbmRichRing* ring);
  Double_t GetAngle(CbmRichRing* ring);
 protected:

  Int_t fVerbose;      // Verbosity level
	TClonesArray* fHitsArray;

  ClassDef(CbmRichRingSelectImpl,1);

};

#endif
