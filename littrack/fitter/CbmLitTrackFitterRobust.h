#ifndef CBMLITTRACKFITTERROBUST_H_
#define CBMLITTRACKFITTERROBUST_H_

#include "CbmLitTrackFitter.h"
#include "CbmLitTypes.h"
#include "CbmLitPtrTypes.h"

class CbmLitTrackParam;
class CbmLitStripHit;
class CbmLitPixelHit;
class CbmLitHit;
class CbmLitTrack;

class CbmLitTrackFitterRobust : public CbmLitTrackFitter
{
public:
	CbmLitTrackFitterRobust(
			TrackFitterPtr fitter,
			TrackFitterPtr smoother);

	virtual ~CbmLitTrackFitterRobust();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual LitStatus Fit(
			CbmLitTrack *track,
			Bool_t downstream = true);

private:
	LitStatus CreateEffectiveTrack(
			CbmLitTrack* track,
			Int_t iter,
			CbmLitTrack* etrack) const;

	LitStatus CreateEffectiveHit(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd,
			const CbmLitTrackParam* par,
			Int_t iter,
			CbmLitHit* hit) const;

	LitStatus CalculateWeights(
			const CbmLitTrackParam* par,
			HitPtrIterator itBegin,
			HitPtrIterator itEnd,
			Int_t iter) const;

	Bool_t AreAllOutliers(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd) const;

	Bool_t MarkOutliers(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd) const;

	LitStatus CheckEffectiveTrack(
			const CbmLitTrack* track) const;

	LitStatus CreateOutputTrack(
			CbmLitTrack* track);

	TrackFitterPtr fFitter;
	TrackFitterPtr fSmoother;

	WeightedHitCalculatorPtr fWeightedHitCalculator;
	WeightCalculatorPtr fSimpleWeightCalculator;
	WeightCalculatorPtr fGaussWeightCalculator;

	Int_t fNofIterations;
	std::vector<Double_t> fAnnealing;
	Double_t fOutlierCut;

	ClassDef(CbmLitTrackFitterRobust, 1);
};

#endif /*CBMLITTRACKFITTERROBUST_H_*/
