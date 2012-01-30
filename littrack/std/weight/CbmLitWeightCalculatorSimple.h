#ifndef CBMLITWEIGHTCALCULATORSIMPLE_H_
#define CBMLITWEIGHTCALCULATORSIMPLE_H_

#include "base/CbmLitTypes.h"
#include "interface/CbmLitWeightCalculator.h"

class CbmLitWeightCalculatorSimple : public CbmLitWeightCalculator
{
public:
   CbmLitWeightCalculatorSimple();
   virtual ~CbmLitWeightCalculatorSimple();

   virtual LitStatus DoCalculate(
      const CbmLitTrackParam* par,
      HitPtrIterator itBegin,
      HitPtrIterator itEnd,
      litfloat T);

   virtual LitStatus DoCalculate(
      const CbmLitTrackParam* par,
      HitPtrVector& hits,
      litfloat T);
};

#endif /*CBMLITWEIGHTCALCULATORSIMPLE_H_*/
