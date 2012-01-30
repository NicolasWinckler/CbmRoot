#ifndef CBMLITWEIGHTCALCULATORGAUSS_H_
#define CBMLITWEIGHTCALCULATORGAUSS_H_

#include "base/CbmLitTypes.h"
#include "interface/CbmLitWeightCalculator.h"

class CbmLitHit;
class CbmLitStripHit;
#include "data/CbmLitPixelHit.h"

class CbmLitWeightCalculatorGauss : public CbmLitWeightCalculator
{
public:
   CbmLitWeightCalculatorGauss();
   virtual ~CbmLitWeightCalculatorGauss();

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
   LitStatus MultivariateGaussWeight(
      const CbmLitTrackParam* par,
      CbmLitHit* hit,
      litfloat T) const;
   LitStatus MultivariateGaussWeight(
      const CbmLitTrackParam* par,
      CbmLitStripHit* hit,
      litfloat T) const;
   LitStatus MultivariateGaussWeight(
      const CbmLitTrackParam* par,
      CbmLitPixelHit* hit,
      litfloat T) const;

   litfloat MultivariateGaussCut(
      const CbmLitHit* hit,
      litfloat T,
      litfloat cutValue) const;
   litfloat MultivariateGaussCut(
      const CbmLitStripHit* hit,
      litfloat T,
      litfloat cutValue) const;
   litfloat MultivariateGaussCut(
      const CbmLitPixelHit* hit,
      litfloat T,
      litfloat cutValue) const;

   LitStatus Normalize(
      HitPtrIterator itBegin,
      HitPtrIterator itEnd) const;
};

#endif /* CBMLITWEIGHTCALCULATORGAUSS_H_ */
