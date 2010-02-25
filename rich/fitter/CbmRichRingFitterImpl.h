/******************************************************************************
*  $Id: CbmRichRingFitterImpl.h,v 1.2 2006/09/13 14:57:07 hoehne Exp $
*
*  Class  : CbmRichRingFitterImpl
*  Description: Abstract base class for concrete Rich Ring fitting algorithms.
*               Each derived class must implement the method DoFit.
*
*  Author : Supriya Das
*  E-mail : S.Das@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingFitterImpl.h,v $
*  Revision 1.2  2006/09/13 14:57:07  hoehne
*  task for calculating Chi2 of ring fit added
*
*  Revision 1.1  2006/01/19 11:33:12  hoehne
*  initial version: base class for RingFitters
*
*
*******************************************************************************/
#ifndef CBM_RICH_RING_FITTER_IMPL
#define CBM_RICH_RING_FITTER_IMPL 1

#include "CbmRichRingFitter.h"
#include "TClonesArray.h"
#include <vector>

class CbmRichRing;


class CbmRichRingFitterImpl : public CbmRichRingFitter
{

 public:

	/** Default constructor **/
	CbmRichRingFitterImpl() {
	}

	/** Destructor **/
	virtual ~CbmRichRingFitterImpl() {
	}

	/** Virtual method Init. If needed, to be implemented in the
	 ** concrete class. Else no action.
	 **/
	virtual void Init();

	/** Abstract method DoFit. To be implemented in the concrete class.
	 ** Task: Make a fit to the hits attached to the track by the track
	 ** finder. Fill the track parameter member variables.
	 **
	 *@param pRing      Pointer to CbmRichRing
	 */
	virtual void DoFit(CbmRichRing* pRing) = 0;

protected:

	/** Method CalcChi2. Implemented in the base class for
	 Rich Ring fitting algorithms.
	 Task: Calculate chi2 for reconstructed ring.
	 **/
	virtual void CalcChi2(CbmRichRing* pRing);

	TClonesArray* fHitsArray;

	static const Int_t MAX_NOF_HITS_IN_RING = 200;
	std::vector<Double_t> fHitX;
	std::vector<Double_t> fHitY;

	ClassDef(CbmRichRingFitterImpl,1);

};

#endif
