#include "weight/CbmLitWeightCalculatorGauss.h"

#include "data/CbmLitHit.h"
#include "data/CbmLitStripHit.h"
#include "data/CbmLitPixelHit.h"

#include <iostream>
#include <cmath>

CbmLitWeightCalculatorGauss::CbmLitWeightCalculatorGauss()
{
}

CbmLitWeightCalculatorGauss::~CbmLitWeightCalculatorGauss()
{
}

LitStatus CbmLitWeightCalculatorGauss::DoCalculate(
   const CbmLitTrackParam* par,
   HitPtrIterator itBegin,
   HitPtrIterator itEnd,
   litfloat T)
{
   for(HitPtrIterator it = itBegin; it != itEnd; it++) {
      if ((*it)->IsOutlier()) { continue; }
      if (MultivariateGaussWeight(par, *it, T) == kLITERROR) { return kLITERROR; }
   }
   if (Normalize(itBegin, itEnd) == kLITERROR) { return kLITERROR; }

   return kLITSUCCESS;
}

LitStatus CbmLitWeightCalculatorGauss::DoCalculate(
   const CbmLitTrackParam* par,
   HitPtrVector& hits,
   litfloat T)
{
   return DoCalculate(par, hits.begin(), hits.end(), T);
}

LitStatus CbmLitWeightCalculatorGauss::MultivariateGaussWeight(
   const CbmLitTrackParam* par,
   CbmLitHit* hit,
   litfloat T) const
{
   LitStatus result = kLITSUCCESS;
   if (hit->GetType() == kLITSTRIPHIT) {
      result = MultivariateGaussWeight(par, static_cast<CbmLitStripHit*>(hit), T);
   } else if (hit->GetType() == kLITPIXELHIT) {
      result = MultivariateGaussWeight(par, static_cast<CbmLitPixelHit*>(hit), T);
   }
   return result;
}

LitStatus CbmLitWeightCalculatorGauss::MultivariateGaussWeight(
   const CbmLitTrackParam* par,
   CbmLitStripHit* hit,
   litfloat T) const
{
   std::cout << "MultivariateGaussWeight NOT IMPLEMENTED FOR STRIP HIT!!!!"<< std::endl;
   return kLITSUCCESS;
}

LitStatus CbmLitWeightCalculatorGauss::MultivariateGaussWeight(
   const CbmLitTrackParam* par,
   CbmLitPixelHit* hit,
   litfloat T) const
{
   const litfloat PI = 3.14159265;

   litfloat dx = hit->GetX() - par->GetX();
   litfloat dy = hit->GetY() - par->GetY();
   litfloat dxx = hit->GetDx() * hit->GetDx();
   litfloat dyy = hit->GetDy() * hit->GetDy();
   litfloat dxy = hit->GetDxy();
   litfloat det = dxx * dyy - dxy * dxy;
   litfloat s = (dx*dx*dyy - 2*dx*dy*dxy + dy*dy*dxx) / det;

   litfloat w = (1./((2.*PI) * std::sqrt(T*det))) * std::exp(-s/(2.*T));
   hit->SetW(w);

   return kLITSUCCESS;
}

litfloat CbmLitWeightCalculatorGauss::MultivariateGaussCut(
   const CbmLitHit* hit,
   litfloat T,
   litfloat cutValue) const
{
   litfloat cut = 0.;
   if (hit->GetType() == kLITSTRIPHIT) {
      cut = MultivariateGaussCut(static_cast<const CbmLitStripHit*>(hit), T, cutValue);
   } else if (hit->GetType() == kLITPIXELHIT) {
      cut = MultivariateGaussCut(static_cast<const CbmLitPixelHit*>(hit), T, cutValue);
   }
   return cut;
}

litfloat CbmLitWeightCalculatorGauss::MultivariateGaussCut(
   const CbmLitStripHit* hit,
   litfloat T,
   litfloat cutValue) const
{
   std::cout << "MultivariateGaussCut NOT IMPLEMENTED FOR STRIP HIT!!!!"<< std::endl;
   return 0;
}

litfloat CbmLitWeightCalculatorGauss::MultivariateGaussCut(
   const CbmLitPixelHit* hit,
   litfloat T,
   litfloat cutValue) const
{
   litfloat dim = 2.;
   const litfloat PI = 3.14159265;

   litfloat dxx = hit->GetDx() * hit->GetDx();
   litfloat dyy = hit->GetDy() * hit->GetDy();
   litfloat dxy = hit->GetDxy();
   litfloat det = dxx * dyy - dxy * dxy;

   litfloat cut = (1./(std::pow(2.*PI, dim/2.) * std::sqrt(T*det))) *
             std::exp(-cutValue/(2.*T));
   return cut;
}

LitStatus CbmLitWeightCalculatorGauss::Normalize(
   HitPtrIterator itBegin,
   HitPtrIterator itEnd) const
{
   //myf cutValue = 3.;
   litfloat sumW = 0.;
   litfloat sumCut = 0.;
   for(HitPtrIterator it = itBegin; it != itEnd; it++) {
      if ((*it)->IsOutlier()) { continue; }
      sumW += (*it)->GetW();
      //sumCut += MultivariateGaussCut(*it, T, cutValue);
   }

   if (sumW == 0.) {
      return kLITERROR;
   } else {
      for(HitPtrIterator it = itBegin; it != itEnd; it++) {
         if ((*it)->IsOutlier()) { continue; }
         (*it)->SetW((*it)->GetW() / (sumW + sumCut));
      }
   }
   return kLITSUCCESS;
}
