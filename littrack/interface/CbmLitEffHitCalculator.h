#ifndef CBMLITEFFHITCALCULATOR_H_
#define CBMLITEFFHITCALCULATOR_H_

#include "CbmLitTool.h"
#include "CbmLitTypes.h"

class CbmLitEffHitCalculator : public CbmLitTool
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
