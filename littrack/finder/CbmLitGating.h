#ifndef CBMLITGATING_H_
#define CBMLITGATING_H_

//#include "CbmLitTool.h"

#include "CbmLitTypes.h"
#include "CbmLitEnums.h"

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

	bool IsHitInValidationGate(
			const CbmLitTrackParam* par,
			const CbmLitHit* hit) const;
//
//	HitPtrIteratorPair MinMaxIndex(
//			const CbmLitTrackParam* par,
//			HitPtrVector& hits,
//			const CbmLitStation& station,
//			const std::pair<double, Char_t>& maxErr) const;

	HitPtrIteratorPair MinMaxIndex(
			const CbmLitTrackParam* par,
			CbmLitHitData& hitData,
			const CbmLitDetectorLayout& layout,
			int stationGroup,
			int station,
			int substation) const;

//	double CalcDevX(
//			const CbmLitTrackParam* par,
//			const CbmLitHitData& hitData,
//			int stationGroup,
//			int station,
//			int substation) const;

protected:
	double fChiSqStripHitCut;
	double fChiSqPixelHitCut;
	bool fUseFastSearch;
	double fMaxCovSq;
	double fSigmaCoef;
};

#endif /* CBMLITGATING_H_ */
