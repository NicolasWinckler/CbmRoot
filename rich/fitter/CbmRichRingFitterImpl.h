/**
* \file CbmRichRingFitterImpl.h
*
* \brief Abstract base class for concrete Rich Ring fitting algorithms.
*  Each derived class must implement the method DoFit.
*
* \author Supriya Das
* \date 2006
**/
#ifndef CBMRICHRINGFITTERIMPL
#define CBMRICHRINGFITTERIMPL

#include "CbmRichRingFitter.h"
#include "TClonesArray.h"
#include "CbmRichRingLight.h"
#include "CbmRichRing.h"
#include <vector>


/**
* \class CbmRichRingFitterImpl
*
* \brief Abstract base class for concrete Rich Ring fitting algorithms.
*  Each derived class must implement the method DoFit.
*
* \author Supriya Das
* \date 2006
**/
class CbmRichRingFitterImpl : public CbmRichRingFitter
{
public:

	/**
	 * \brief Default constructor.
	 */
	CbmRichRingFitterImpl(){ }

	/**
	 * \brief Destructor.
	 */
	virtual ~CbmRichRingFitterImpl() { }

	/**
	 * \brief Virtual method Init. If needed, to be implemented in the
	 * concrete class. Else no action.
	 */
	virtual void Init();

	/**
	 * \brief Abstract method DoFit. To be implemented in the concrete class.
	 * Perform a fit to the hits attached to the ring by a ring
	 * finder. Fill the track parameter member variables.
	 * \param[in,out] ring Pointer to CbmRichRing
	 */
	virtual void DoFit(
	      CbmRichRing* ring) = 0;

protected:

	/** Method CalcChi2. Implemented in the base class for
	 Rich Ring fitting algorithms.
	 Task: Calculate chi2 for reconstructed ring.
	 **/
	virtual void CalcChi2(CbmRichRing* pRing){;}
	virtual void CalcChi2(CbmRichRingLight* pRing);

	TClonesArray* fHitsArray;

	static const Int_t MAX_NOF_HITS_IN_RING = 200;
	std::vector<Float_t> fHitX;
	std::vector<Float_t> fHitY;
	Int_t fNofHits;

	ClassDef(CbmRichRingFitterImpl,1);

};

#endif
