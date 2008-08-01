#ifndef CBMLITWEIGHTCALCULATOR_H_
#define CBMLITWEIGHTCALCULATOR_H_

#include "CbmLitTool.h"
#include "CbmLitTypes.h"

class CbmLitTrackParam;

class CbmLitWeightCalculator : public CbmLitTool
{
public:
	CbmLitWeightCalculator();
	virtual ~CbmLitWeightCalculator();
	
	virtual LitStatus DoCalculate(
			const CbmLitTrackParam* par,
			HitIterator itBegin,
			HitIterator itEnd) = 0;
	
	virtual LitStatus DoCalculate(
			const CbmLitTrackParam* par,
			HitVector& hits) = 0;
	
	
	ClassDef(CbmLitWeightCalculator, 1);
};

#endif /*CBMLITWEIGHTCALCULATOR_H_*/
