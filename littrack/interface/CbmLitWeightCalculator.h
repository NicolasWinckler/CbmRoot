#ifndef CBMLITWEIGHTCALCULATOR_H_
#define CBMLITWEIGHTCALCULATOR_H_

#include "CbmLitTool.h"
#include "CbmLitTypes.h"

class CbmLitTrackParam;

class CbmLitWeightCalculator : public CbmLitTool
{
public:
	CbmLitWeightCalculator(){};
	virtual ~CbmLitWeightCalculator(){};

	virtual LitStatus DoCalculate(
			const CbmLitTrackParam* par,
			HitPtrIterator itBegin,
			HitPtrIterator itEnd,
			myf T) = 0;

	virtual LitStatus DoCalculate(
			const CbmLitTrackParam* par,
			HitPtrVector& hits,
			myf T) = 0;
};

#endif /*CBMLITWEIGHTCALCULATOR_H_*/
