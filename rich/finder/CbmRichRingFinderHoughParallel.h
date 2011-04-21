// --------------------------------------------------------------------------------------
// -----                 CbmRichRingFinderHough source file                         -----
// ----- Algorithm idea: G.A. Ososkov (ososkov@jinr.ru) and Semen Lebedev (salebedev@jinr.ru)                            -----
// ----- Implementation: Semen Lebedev (salebedev@jinr.ru)-----

#ifndef CBM_RICH_RING_FINDER_HOUGH_PARALLEL_H
#define CBM_RICH_RING_FINDER_HOUGH_PARALLEL_H

#include "CbmRichRingFinderHoughParallelImpl.h"
#include "CbmRichRingFinderHoughImpl.h"
#include "CbmRichRingFinder.h"
#include "TClonesArray.h"
#include "CbmRichRingLight.h"
#include "CbmRichRing.h"
#include "CbmRichHit.h"

#include <vector>
#include <map>
#include <functional>

class CbmRichRingFinderHoughParallel: public CbmRichRingFinder{

protected:

	Int_t fRingCount;
	Int_t fNEvent; /// event number

	Double_t fExecTime;//evaluate execution time

#define HOUGH_IMPL_PARALLEL
//#define HOUGH_IMPL

#ifdef HOUGH_IMPL
	CbmRichRingFinderHoughImpl* fHTImpl1;
	CbmRichRingFinderHoughImpl* fHTImpl2;
#endif

#ifdef HOUGH_IMPL_PARALLEL
	CbmRichRingFinderHoughParallelImpl* fHTImpl1;
	CbmRichRingFinderHoughParallelImpl* fHTImpl2;
#endif

public:
  	CbmRichRingFinderHoughParallel ();

  	CbmRichRingFinderHoughParallel ( Int_t verbose, TString geometry);

	virtual ~CbmRichRingFinderHoughParallel();
	void SetParameters();
    void AddRingsToOutputArray(TClonesArray *rRingArray,
    		std::vector<CbmRichRingLight*>& rings);

	virtual void Init();

	virtual Int_t DoFind(TClonesArray* rHitArray,
	 		      		 TClonesArray* rProjArray,
		       	      	 TClonesArray* rRingArray);
	int DoFind(const std::vector<CbmRichHoughHit>& data);

ClassDef(CbmRichRingFinderHoughParallel,1);
};
#endif // CBM_RICH_RING_FINDER_HOUGH_PARALLEL_H
