#ifndef CBMLITWEIGHTEDHITCALCULATOR_H_
#define CBMLITWEIGHTEDHITCALCULATOR_H_

#include "base/CbmLitTool.h"
#include "base/CbmLitTypes.h"

class CbmLitHit;

class CbmLitWeightedHitCalculator : public CbmLitTool
{
public:
	CbmLitWeightedHitCalculator(){};
	virtual ~CbmLitWeightedHitCalculator(){};

	virtual void DoCalculate(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd,
			CbmLitHit* hit) = 0;

	virtual void DoCalculate(
			HitPtrVector& hits,
			CbmLitHit* hit) = 0;
};

#endif /*CBMLITWEIGHTEDHITCALCULATOR_H_*/
