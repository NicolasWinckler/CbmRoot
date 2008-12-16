#ifndef CBMLITEFFHITCALCULATOR_H_
#define CBMLITEFFHITCALCULATOR_H_

#include "CbmLitTool.h"
#include "CbmLitTypes.h"
#include "CbmLitPixelHit.h"

class CbmLitEffHitCalculator : public CbmLitTool
{
public:
	CbmLitEffHitCalculator();
	virtual ~CbmLitEffHitCalculator();
	
	virtual CbmLitPixelHit DoCalculate(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd) = 0;
	
	virtual CbmLitPixelHit DoCalculate(
			HitPtrVector& hits) = 0;
	
	ClassDef(CbmLitEffHitCalculator, 1);
};

#endif /*CBMLITEFFHITCALCULATOR_H_*/
