//--------------------------------------------------------------------------------------
//CbmRichRingFinderHough source file
//Implementation: Semen Lebedev (s.lebedev@gsi.de)-----

#ifndef CBM_RICH_RING_FINDER_HOUGH_SIMD_H
#define CBM_RICH_RING_FINDER_HOUGH_SIMD_H

#include "CbmRichRingFinderHoughImpl.h"
//#include "../L1/L1Algo/L1Types.h"
#include "../L1/L1Algo/vectors/P4_F32vec4.h"

class CbmRichHoughHitVec {
public:
	fvec fX;
	fvec fY;
	fvec fX2plusY2;
	//unsigned short int fId;
	//Bool_t fIsUsed;
}  _fvecalignment;


class CbmRichRingFinderHoughSimd : public CbmRichRingFinderHoughImpl {

public:

  	/** Standard constructor **/
  	CbmRichRingFinderHoughSimd ( TString geometry);

	~CbmRichRingFinderHoughSimd(){;}

	virtual void HoughTransformReconstruction();
	virtual void HoughTransformGroup(unsigned short int indmin,
			unsigned short int indmax, Int_t iPart);

	std::vector<CbmRichHoughHitVec> fDataV;

	//ClassDef(CbmRichRingFinderHoughSimd,1)

};
#endif // CBM_RICH_RING_FINDER_HOUGH_SIMD_H
