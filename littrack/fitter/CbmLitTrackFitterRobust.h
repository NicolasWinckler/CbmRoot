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
			CbmLitTrackFitter* fitter,
			CbmLitTrackFitter* smoother);
	
	virtual ~CbmLitTrackFitterRobust();
	
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();
	
	virtual LitStatus Fit(
			CbmLitTrack *track,
			Bool_t downstream = true);
private:
	
	CbmLitTrackFitter* fFitter;
	CbmLitTrackFitter* fSmoother;
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
	
	LitStatus DAFWeight(
			const CbmLitTrackParam* par,
			CbmLitHit* hit,
			Double_t beta);

	Double_t DAFCut(
			const CbmLitHit* hit,
			Double_t beta,
			Double_t cutValue);

	LitStatus DAFWeight(
			const CbmLitTrackParam* par,
			HitIterator itBegin,
			HitIterator itEnd,
			Int_t iter);
	
	Bool_t AreAllOutliers(
			HitIterator itBegin,
			HitIterator itEnd);
	
	void MarkOutliers(
			HitIterator itBegin,
			HitIterator itEnd);
	
	ClassDef(CbmLitTrackFitterRobust, 1);	
};

#endif /*CBMLITTRACKFITTERROBUST_H_*/
