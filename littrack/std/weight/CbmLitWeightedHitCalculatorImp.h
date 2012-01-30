#ifndef CBMLITWEIGHTEDHITCALCULATORIMP_H_
#define CBMLITWEIGHTEDHITCALCULATORIMP_H_

#include "base/CbmLitTypes.h"
#include "interface/CbmLitWeightedHitCalculator.h"

class CbmLitHit;
class CbmLitStripHit;
class CbmLitPixelHit;

class CbmLitWeightedHitCalculatorImp : public CbmLitWeightedHitCalculator
{
public:
   CbmLitWeightedHitCalculatorImp();
   virtual ~CbmLitWeightedHitCalculatorImp();

   virtual void DoCalculate(
      HitPtrIterator itBegin,
      HitPtrIterator itEnd,
      CbmLitHit* hit);

   virtual void DoCalculate(
      HitPtrVector& hits,
      CbmLitHit* hit);
private:
   void CalculateWeightedHit(
      HitPtrIterator itBegin,
      HitPtrIterator itEnd,
      CbmLitStripHit* hit);

   void CalculateWeightedHit(
      HitPtrIterator itBegin,
      HitPtrIterator itEnd,
      CbmLitPixelHit* hit);

   inline void Inverse(
      litfloat v00, litfloat v01, litfloat v11,
      litfloat& u00, litfloat& u01, litfloat& u11);
};

#endif /*CBMLITWEIGHTEDHITCALCULATORIMP_H_*/
