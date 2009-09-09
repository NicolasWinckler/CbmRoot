// --------------------------------------------------------------------------------------
// -----                 CbmRichRingFinderHoughImpl source file                         -----
// ----- Algorithm idea: G.A. Ososkov (ososkov@jinr.ru) and Semen Lebedev (s.lebedev@gsi.de)                            -----
// ----- Implementation: Semen Lebedev (s.lebedev@gsi.de)-----

#ifndef CBM_RICH_RING_FINDER_HOUGH_PARALLEL_IMPL_H
#define CBM_RICH_RING_FINDER_HOUGH_PARALLEL_IMPL_H

#include "CbmRichRingFinderHoughImpl.h"

class CbmRichRingFinderHoughParallelImpl : public CbmRichRingFinderHoughImpl {

protected:


public:
  	CbmRichRingFinderHoughParallelImpl ();

  	/** Standard constructor **/
  	CbmRichRingFinderHoughParallelImpl (TString geometry);

	virtual ~CbmRichRingFinderHoughParallelImpl(){;}

	virtual void HoughTransform(unsigned short int indmin, unsigned short int indmax);

	ClassDef(CbmRichRingFinderHoughParallelImpl,1)

};
#endif // CBM_RICH_RING_FINDER_HOUGH_PARALLEL_IMPL_H

