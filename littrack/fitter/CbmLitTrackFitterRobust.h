#ifndef CBMLITTRACKFITTERROBUST_H_
#define CBMLITTRACKFITTERROBUST_H_

#include "CbmLitTrackFitter.h"
#include "CbmLitTypes.h"

class CbmLitWeightCalculator;
class CbmLitEffHitCalculator;

class CbmLitTrackParam;
class CbmLitStripHit;
class CbmLitPixelHit;
class CbmLitHit;

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

	LitStatus MultivariateGaussWeight(
			const CbmLitTrackParam* par,
			CbmLitHit* hit,
			Double_t T) const;
	LitStatus MultivariateGaussWeight(
			const CbmLitTrackParam* par,
			CbmLitStripHit* hit,
			Double_t T) const;
	LitStatus MultivariateGaussWeight(
			const CbmLitTrackParam* par,
			CbmLitPixelHit* hit,
			Double_t T) const;

	Double_t MultivariateGaussCut(
			const CbmLitHit* hit,
			Double_t T,
			Double_t cutValue) const;
	Double_t MultivariateGaussCut(
			const CbmLitStripHit* hit,
			Double_t T,
			Double_t cutValue) const;
	Double_t MultivariateGaussCut(
			const CbmLitPixelHit* hit,
			Double_t T,
			Double_t cutValue) const;

	LitStatus CalcWeights(
			const CbmLitTrackParam* par,
			HitPtrIterator itBegin,
			HitPtrIterator itEnd,
			Int_t iter) const;

	Bool_t AreAllOutliers(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd) const;

	void MarkOutliers(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd) const;

	LitStatus Normalize(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd) const;

	LitStatus CreateEffTrack(
			const std::vector<HitPtrIteratorPair>& bounds,
			Int_t iter,
			CbmLitTrack* etrack) const;

	LitStatus CheckEffTrack(
			const CbmLitTrack* track) const;

//	LitStatus KoshiWeight(
//			const CbmLitTrackParam* par,
//			HitIterator itBegin,
//			HitIterator itEnd,
//			Int_t iter);
//
//	LitStatus OptimalWeight(
//			const CbmLitTrackParam* par,
//			HitIterator itBegin,
//			HitIterator itEnd,
//			Int_t iter);
//
//	LitStatus TukeyWeight(
//			const CbmLitTrackParam* par,
//			HitIterator itBegin,
//			HitIterator itEnd,
//			Int_t iter);

	CbmLitTrackFitter* fFitter;
	CbmLitTrackFitter* fSmoother;
	CbmLitWeightCalculator* fWeightCalc;
	CbmLitEffHitCalculator* fEffHitCalc;
	CbmLitWeightCalculator* fWeightCalcSimple;

	Int_t fNofIterations;
	std::vector<Double_t> fAnnealing;
	Double_t fOutlierCut;

	ClassDef(CbmLitTrackFitterRobust, 1);
};

#endif /*CBMLITTRACKFITTERROBUST_H_*/
