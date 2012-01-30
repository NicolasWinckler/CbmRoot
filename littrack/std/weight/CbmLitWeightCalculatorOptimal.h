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
      myf T);

   virtual LitStatus DoCalculate(
      const CbmLitTrackParam* par,
      HitPtrVector& hits,
      myf T);

private:
   LitStatus OptimalWeight(
      const CbmLitTrackParam* par,
      CbmLitHit* hit,
      myf T) const;

   LitStatus Normalize(
      HitPtrIterator itBegin,
      HitPtrIterator itEnd) const;
};

#endif /* CBMLITWEIGHTCALCULATOROPTIMAL_H_ */
