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
			HitPtrIterator itEnd);
	
	virtual LitStatus DoCalculate(
			const CbmLitTrackParam* par,
			HitPtrVector& hits);
	
	ClassDef(CbmLitWeightCalculatorSimple, 1);
};

#endif /*CBMLITWEIGHTCALCULATORSIMPLE_H_*/
