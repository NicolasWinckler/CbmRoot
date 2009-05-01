#ifndef CBMLITGATING_H_
#define CBMLITGATING_H_

//#include "CbmLitTool.h"

#include "CbmLitTypes.h"

#include <utility>

class CbmLitTrackParam;
class CbmLitHit;
class CbmLitHitData;
class CbmLitDetectorLayout;
class CbmLitStation;

class CbmLitGating //: public CbmLitTool
{
public:
	CbmLitGating();
	virtual ~CbmLitGating();

	// derived from CbmTool
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	Bool_t IsHitInValidationGate(
			const CbmLitTrackParam* par,
			const CbmLitHit* hit) const;
//
//	HitPtrIteratorPair MinMaxIndex(
//			const CbmLitTrackParam* par,
//			HitPtrVector& hits,
//			const CbmLitStation& station,
//			const std::pair<Double_t, Char_t>& maxErr) const;

	HitPtrIteratorPair MinMaxIndex(
			const CbmLitTrackParam* par,
			CbmLitHitData& hitData,
			const CbmLitDetectorLayout& layout,
			Int_t stationGroup,
			Int_t station,
			Int_t substation) const;

//	Double_t CalcDevX(
//			const CbmLitTrackParam* par,
//			const CbmLitHitData& hitData,
//			Int_t stationGroup,
//			Int_t station,
//			Int_t substation) const;

protected:

	Double_t fChiSqStripHitCut;
	Double_t fChiSqPixelHitCut;
	Bool_t fUseFastSearch;
	Double_t fMaxCovSq;
	Double_t fSigmaCoef;

	ClassDef(CbmLitGating, 1);
};

#endif /* CBMLITGATING_H_ */
