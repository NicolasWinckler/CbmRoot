/** CbmLitWeightCalculatorOptimal.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2009
 **
 **
 **/

#include "weight/CbmLitWeightCalculatorOptimal.h"

#include "data/CbmLitHit.h"
//#include "CbmLitStripHit.h"
//#include "CbmLitPixelHit.h"
#include "utils/CbmLitMath.h"

//#include <iostream>
#include <cmath>

CbmLitWeightCalculatorOptimal::CbmLitWeightCalculatorOptimal()
{
}

CbmLitWeightCalculatorOptimal::~CbmLitWeightCalculatorOptimal()
{
}

LitStatus CbmLitWeightCalculatorOptimal::DoCalculate(
   const CbmLitTrackParam* par,
   HitPtrIterator itBegin,
   HitPtrIterator itEnd,
   litfloat T)
{
   for(HitPtrIterator it = itBegin; it != itEnd; it++) {
      if ((*it)->IsOutlier()) { continue; }
      if (OptimalWeight(par, *it, T) == kLITERROR) { return kLITERROR; }
   }
   if (Normalize(itBegin, itEnd) == kLITERROR) { return kLITERROR; }

   return kLITSUCCESS;
}

LitStatus CbmLitWeightCalculatorOptimal::DoCalculate(
   const CbmLitTrackParam* par,
   HitPtrVector& hits,
   litfloat T)
{
   return DoCalculate(par, hits.begin(), hits.end(), T);
}

LitStatus CbmLitWeightCalculatorOptimal::OptimalWeight(
   const CbmLitTrackParam* par,
   CbmLitHit* hit,
   litfloat T) const
{
   litfloat chiSq = ChiSq(par, hit);
   hit->SetW((1 + T) / (1 + T * std::exp(chiSq)));
   return kLITSUCCESS;
}

LitStatus CbmLitWeightCalculatorOptimal::Normalize(
   HitPtrIterator itBegin,
   HitPtrIterator itEnd) const
{
   litfloat sumW = 0.;
   litfloat sumCut = 0.;
   for(HitPtrIterator it = itBegin; it != itEnd; it++) {
      if ((*it)->IsOutlier()) { continue; }
      sumW += (*it)->GetW();
   }

   if (sumW == 0.) {
      return kLITERROR;
   } else {
      for(HitPtrIterator it = itBegin; it != itEnd; it++) {
         if ((*it)->IsOutlier()) { continue; }
         (*it)->SetW((*it)->GetW() / sumW);
      }
   }
   return kLITSUCCESS;
}
