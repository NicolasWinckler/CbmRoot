/******************************************************************************
*  $Id: CbmRichRingSelect.h,v 1.1 2006/09/13 14:53:31 hoehne Exp $
*
*  Class  : CbmRichRingSelect
*  Description : Abstract base class for concrete RICH ring selection algorithm:
*                to be run after ring-track assign for fake-ring rejection
*
*  Author : Simeon Lebedev
*  E-mail : salebedev@jinr.ru
*
*******************************************************************************
*  $Log: CbmRichRingSelect.h,v $
*  Revision 1.1  2006/09/13 14:53:31  hoehne
*  initial version
*
*  
*
*******************************************************************************/


#ifndef CBM_RICH_RING_SELECT
#define CBM_RICH_RING_SELECT 1

#include "TObject.h"
#include "CbmRichRing.h"
#include "TClonesArray.h"

class CbmRichRingSelect : public TObject
{

 public:

  /** Default constructor **/
  CbmRichRingSelect() { };
  
  /** Destructor **/
  virtual ~CbmRichRingSelect() { };

  virtual void Init();

  virtual void DoSelect(CbmRichRing* ring) = 0;

   void SetVerbose(Int_t verbose) { fVerbose = verbose; };
   
  Int_t GetNofHitsOnRing(CbmRichRing* ring);
  Double_t GetAngle(CbmRichRing* ring);   
 protected:

  Int_t fVerbose;      // Verbosity level
	TClonesArray* fHitsArray;

  ClassDef(CbmRichRingSelect,1);

};

#endif
