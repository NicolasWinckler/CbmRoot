#ifndef CBMLITWEIGHTCALCULATORSIMPLE_H_
#define CBMLITWEIGHTCALCULATORSIMPLE_H_

#include "CbmLitWeightCalculator.h"

#include "CbmLitTypes.h"

class CbmLitWeightCalculatorSimple : public CbmLitWeightCalculator
{
public:
	CbmLitWeightCalculatorSimple();
	virtual ~CbmLitWeightCalculatorSimple();

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
};

#endif /*CBMLITWEIGHTCALCULATORSIMPLE_H_*/
