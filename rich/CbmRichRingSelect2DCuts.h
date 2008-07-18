/******************************************************************************
*  $Id: CbmRichRingSelect2DCuts.h,v 1.1 2006/09/13 14:53:31 hoehne Exp $
*
*  Class  : CbmRichRingSelect2DCuts
*  Description : Implementation for concrete RICH ring selection algorithm:
*                reject rings with a set of certain 2-dimensional cuts
*                store result in "Selection2D":
*                0 = good rings
*                1 = rings to be rejected
*
*  Author : Simeon Lebedev
*  E-mail : salebedev@jinr.ru
*
*******************************************************************************
*  $Log: CbmRichRingSelect2DCuts.h,v $
*  Revision 1.1  2006/09/13 14:53:31  hoehne
*  initial version
*
*  
*
*******************************************************************************/

#ifndef CBM_RICH_RING_SELECT_2DCUTS_H
#define CBM_RICH_RING_SELECT_2DCUTS_H

#include "TClonesArray.h"
#include "CbmRichRing.h"
/*#include "CbmTask.h"
#include "CbmRootManager.h"*/
#include "CbmRichRingSelect.h"


class CbmRichRingSelect2DCuts : public CbmRichRingSelect {

	  /** Verbosity level **/
  	Int_t fVerbose;

public:
  	CbmRichRingSelect2DCuts();// Default constructor
  	CbmRichRingSelect2DCuts ( Int_t verbose);// Standard constructor
	~CbmRichRingSelect2DCuts();
	
	//virtual void Init();
	void DoSelect(CbmRichRing* ring);

	ClassDef(CbmRichRingSelect2DCuts,1);
};
#endif // CBM_RICH_RING_SELECT_2DCUTS_H
