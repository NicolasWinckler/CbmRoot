#ifndef CBMLITWEIGHTEDHITCALCULATORIMP_H_
#define CBMLITWEIGHTEDHITCALCULATORIMP_H_

#include "CbmLitWeightedHitCalculator.h"
#include "CbmLitTypes.h"
class CbmLitHit;
class CbmLitStripHit;
class CbmLitPixelHit;

class CbmLitWeightedHitCalculatorImp : public CbmLitWeightedHitCalculator
{
public:
	CbmLitWeightedHitCalculatorImp();
	virtual ~CbmLitWeightedHitCalculatorImp();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual void DoCalculate(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd,
			CbmLitHit* hit);

	virtual void DoCalculate(
			HitPtrVector& hits,
			CbmLitHit* hit);
private:
	void CalculateWeightedHit(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd,
			CbmLitStripHit* hit);

	void CalculateWeightedHit(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd,
			CbmLitPixelHit* hit);

	inline void Inverse(
			myf v00, myf v01, myf v11,
			myf& u00, myf& u01, myf& u11);
};

#endif /*CBMLITWEIGHTEDHITCALCULATORIMP_H_*/
