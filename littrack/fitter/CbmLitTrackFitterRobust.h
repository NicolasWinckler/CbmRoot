#ifndef CBMLITTRACKFITTERROBUST_H_
#define CBMLITTRACKFITTERROBUST_H_

#include "CbmLitTrackFitter.h"
#include "CbmLitTypes.h"

class CbmLitWeightCalculator;
class CbmLitEffHitCalculator;

class CbmLitTrackParam;

class CbmLitTrackFitterRobust : public CbmLitTrackFitter
{
public:
	CbmLitTrackFitterRobust(
			CbmLitTrackFitter* fitter);
	
	virtual ~CbmLitTrackFitterRobust();
	
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();
	
	virtual LitStatus Fit(
			CbmLitTrack *pTrack);
private:
	
	CbmLitTrackFitter* fFitter;
	CbmLitWeightCalculator* fWeightCalc;
	CbmLitEffHitCalculator* fEffHitCalc;
	CbmLitWeightCalculator* fWeightCalcSimple;
	
	LitStatus KoshiWeight(
			const CbmLitTrackParam* par,
			HitIterator itBegin,
			HitIterator itEnd,
			Int_t iter);
	
	LitStatus OptimalWeight(
			const CbmLitTrackParam* par,
			HitIterator itBegin,
			HitIterator itEnd,
			Int_t iter);
	
	LitStatus TukeyWeight(
			const CbmLitTrackParam* par,
			HitIterator itBegin,
			HitIterator itEnd,
			Int_t iter);
	
	ClassDef(CbmLitTrackFitterRobust, 1);	
};

#endif /*CBMLITTRACKFITTERROBUST_H_*/
