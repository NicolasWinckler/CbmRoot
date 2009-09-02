//--------------------------------------------------------------------------------------
//CbmRichRingFinderHough source file
//Implementation: Semen Lebedev (s.lebedev@gsi.de)-----

#ifndef CBM_RICH_RING_FINDER_HOUGH_SIMD_H
#define CBM_RICH_RING_FINDER_HOUGH_SIMD_H

#include "CbmRichRingFinderHoughBase.h"


class CbmRichRingFinderHoughSimd : public CbmRichRingFinderHoughBase {


public:

  	/** Standard constructor **/
  	CbmRichRingFinderHoughSimd ( Int_t verbose, TString geometry);

	~CbmRichRingFinderHoughSimd();

	virtual void HoughTransformReconstruction();
	virtual void HoughTransformSimd(unsigned short int indmin, unsigned short int indmax);
	virtual void HoughTransformSimd2(unsigned short int indmin, unsigned short int indmax);
	ClassDef(CbmRichRingFinderHoughSimd,1)

};
#endif // CBM_RICH_RING_FINDER_HOUGH_SIMD_H
