/** CbmLitWeightCalculatorOptimal.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2009
 **
 **
 **/

#ifndef CBMLITWEIGHTCALCULATOROPTIMAL_H_
#define CBMLITWEIGHTCALCULATOROPTIMAL_H_

#include "interface/CbmLitWeightCalculator.h"


class CbmLitWeightCalculatorOptimal : public CbmLitWeightCalculator
{
public:
   CbmLitWeightCalculatorOptimal();
   virtual ~CbmLitWeightCalculatorOptimal();

   virtual LitStatus DoCalculate(
      const CbmLitTrackParam* par,
      HitPtrIterator itBegin,
      HitPtrIterator itEnd,
      litfloat T);

   virtual LitStatus DoCalculate(
      const CbmLitTrackParam* par,
      HitPtrVector& hits,
      litfloat T);

private:
   LitStatus OptimalWeight(
      const CbmLitTrackParam* par,
      CbmLitHit* hit,
      litfloat T) const;

   LitStatus Normalize(
      HitPtrIterator itBegin,
      HitPtrIterator itEnd) const;
};

#endif /* CBMLITWEIGHTCALCULATOROPTIMAL_H_ */
