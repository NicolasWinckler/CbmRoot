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
			bool downstream = true);

private:
	LitStatus CreateEffectiveTrack(
			CbmLitTrack* track,
			int iter,
			CbmLitTrack* etrack) const;

	LitStatus CreateEffectiveHit(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd,
			const CbmLitTrackParam* par,
			int iter,
			CbmLitHit* hit) const;

	LitStatus CalculateWeights(
			const CbmLitTrackParam* par,
			HitPtrIterator itBegin,
			HitPtrIterator itEnd,
			int iter) const;

	bool AreAllOutliers(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd) const;

	bool MarkOutliers(
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

	int fNofIterations;
	std::vector<double> fAnnealing;
	double fOutlierCut;
};

#endif /*CBMLITTRACKFITTERROBUST_H_*/
