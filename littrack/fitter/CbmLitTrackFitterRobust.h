/** CbmLitTrackFitterWeight.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2008
 **
 **
 **/
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

class CbmLitTrackFitterWeight : public CbmLitTrackFitter
{
public:
	CbmLitTrackFitterWeight(
			TrackFitterPtr fitter,
			TrackFitterPtr smoother);

	virtual ~CbmLitTrackFitterWeight();

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
//	WeightCalculatorPtr fGaussWeightCalculator;
	WeightCalculatorPtr fTukeyWeightCalculator;

	int fNofIterations;
	std::vector<myf> fAnnealing;
	myf fOutlierCut;
};

#endif /*CBMLITTRACKFITTERROBUST_H_*/
