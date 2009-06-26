#ifndef CBMLITWEIGHTEDHITCALCULATOR_H_
#define CBMLITWEIGHTEDHITCALCULATOR_H_

#include "CbmLitTool.h"
#include "CbmLitTypes.h"

class CbmLitHit;

class CbmLitWeightedHitCalculator : public CbmLitTool
{
public:
	CbmLitWeightedHitCalculator();
	virtual ~CbmLitWeightedHitCalculator();

	virtual void DoCalculate(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd,
			CbmLitHit* hit) = 0;

	virtual void DoCalculate(
			HitPtrVector& hits,
			CbmLitHit* hit) = 0;
};

#endif /*CBMLITWEIGHTEDHITCALCULATOR_H_*/
