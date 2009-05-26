// --------------------------------------------------------------------------------------
// -----                 CbmRichRingFinderHough source file                         -----
// ----- Algorithm idea: G.A. Ososkov (ososkov@jinr.ru) and Semen Lebedev (salebedev@jinr.ru)                            -----
// ----- Implementation: Semen Lebedev (salebedev@jinr.ru)-----

#ifndef CBM_RICH_RING_FINDER_HOUGH_H
#define CBM_RICH_RING_FINDER_HOUGH_H

#include "CbmRichRingFinderHoughBase.h"


class CbmRichRingFinderHough : public CbmRichRingFinderHoughBase {

public:

  	/** Standard constructor **/
  	CbmRichRingFinderHough ( Int_t verbose, TString geometry);

	virtual ~CbmRichRingFinderHough();

	virtual void HoughTransformReconstruction();

	ClassDef(CbmRichRingFinderHough,1)

};
#endif // CBM_RICH_RING_FINDER_HOUGH_H
