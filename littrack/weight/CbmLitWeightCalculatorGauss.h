#ifndef CBMLITWEIGHTCALCULATORGAUSS_H_
#define CBMLITWEIGHTCALCULATORGAUSS_H_

#include "base/CbmLitTypes.h"
#include "interface/CbmLitWeightCalculator.h"

class CbmLitHit;
class CbmLitStripHit;
#include "data/CbmLitPixelHit.h"

class CbmLitWeightCalculatorGauss : public CbmLitWeightCalculator
{
public:
	CbmLitWeightCalculatorGauss();
	virtual ~CbmLitWeightCalculatorGauss();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual LitStatus DoCalculate(
			const CbmLitTrackParam* par,
			HitPtrIterator itBegin,
			HitPtrIterator itEnd,
			myf T);

	virtual LitStatus DoCalculate(
			const CbmLitTrackParam* par,
			HitPtrVector& hits,
			myf T);

private:
	LitStatus MultivariateGaussWeight(
			const CbmLitTrackParam* par,
			CbmLitHit* hit,
			myf T) const;
	LitStatus MultivariateGaussWeight(
			const CbmLitTrackParam* par,
			CbmLitStripHit* hit,
			myf T) const;
	LitStatus MultivariateGaussWeight(
			const CbmLitTrackParam* par,
			CbmLitPixelHit* hit,
			myf T) const;

	myf MultivariateGaussCut(
			const CbmLitHit* hit,
			myf T,
			myf cutValue) const;
	myf MultivariateGaussCut(
			const CbmLitStripHit* hit,
			myf T,
			myf cutValue) const;
	myf MultivariateGaussCut(
			const CbmLitPixelHit* hit,
			myf T,
			myf cutValue) const;

	LitStatus Normalize(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd) const;
};

#endif /* CBMLITWEIGHTCALCULATORGAUSS_H_ */
