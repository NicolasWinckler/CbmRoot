#ifndef CBMLITEFFHITCALCULATOR_H_
#define CBMLITEFFHITCALCULATOR_H_

#include "CbmTool.h"
#include "CbmLitTypes.h"
#include "CbmLitHit.h"

class CbmLitEffHitCalculator
{
public:
	CbmLitEffHitCalculator();
	virtual ~CbmLitEffHitCalculator();
	
	virtual CbmLitHit DoCalculate(
			HitIterator itBegin,
			HitIterator itEnd) = 0;
	
	virtual CbmLitHit DoCalculate(
			HitVector& hits) = 0;
	
	ClassDef(CbmLitEffHitCalculator, 1);
};

#endif /*CBMLITEFFHITCALCULATOR_H_*/
