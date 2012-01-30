/** CbmLitWeightCalculatorTukey.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2009
 **
 **
 **/

#ifndef CBMLITWEIGHTCALCULATORTUKEY_H_
#define CBMLITWEIGHTCALCULATORTUKEY_H_

#include "interface/CbmLitWeightCalculator.h"


class CbmLitWeightCalculatorTukey : public CbmLitWeightCalculator
{
public:
   CbmLitWeightCalculatorTukey();
   virtual ~CbmLitWeightCalculatorTukey();

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
   LitStatus TukeyWeight(
      const CbmLitTrackParam* par,
      CbmLitHit* hit,
      myf T) const;

   LitStatus Normalize(
      HitPtrIterator itBegin,
      HitPtrIterator itEnd) const;
};

#endif /* CBMLITWEIGHTCALCULATORTUKEY_H_ */
