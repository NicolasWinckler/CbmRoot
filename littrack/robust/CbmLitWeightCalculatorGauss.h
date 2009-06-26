#ifndef CBMLITWEIGHTCALCULATORGAUSS_H_
#define CBMLITWEIGHTCALCULATORGAUSS_H_

#include "CbmLitWeightCalculator.h"
#include "CbmLitTypes.h"

class CbmLitHit;
class CbmLitStripHit;
#include "CbmLitPixelHit.h"

class CbmLitWeightCalculatorGauss : public CbmLitWeightCalculator{
public:
	CbmLitWeightCalculatorGauss();
	virtual ~CbmLitWeightCalculatorGauss();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual LitStatus DoCalculate(
			const CbmLitTrackParam* par,
			HitPtrIterator itBegin,
			HitPtrIterator itEnd,
			Double_t T);

	virtual LitStatus DoCalculate(
			const CbmLitTrackParam* par,
			HitPtrVector& hits,
			Double_t T);

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

	LitStatus Normalize(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd) const;
};

#endif /* CBMLITWEIGHTCALCULATORGAUSS_H_ */
