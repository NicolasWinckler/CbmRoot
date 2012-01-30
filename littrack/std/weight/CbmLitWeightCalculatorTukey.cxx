/** CbmLitWeightCalculatorTukey.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2009
 **
 **
 **/

#include "weight/CbmLitWeightCalculatorTukey.h"

#include "data/CbmLitHit.h"
//#include "CbmLitStripHit.h"
//#include "CbmLitPixelHit.h"
#include "utils/CbmLitMath.h"

#include <cmath>

CbmLitWeightCalculatorTukey::CbmLitWeightCalculatorTukey()
{
}

CbmLitWeightCalculatorTukey::~CbmLitWeightCalculatorTukey()
{
}

LitStatus CbmLitWeightCalculatorTukey::DoCalculate(
   const CbmLitTrackParam* par,
   HitPtrIterator itBegin,
   HitPtrIterator itEnd,
   myf T)
{
   for(HitPtrIterator it = itBegin; it != itEnd; it++) {
      if ((*it)->IsOutlier()) { continue; }
      if (TukeyWeight(par, *it, T) == kLITERROR) { return kLITERROR; }
   }
   if (Normalize(itBegin, itEnd) == kLITERROR) { return kLITERROR; }

   return kLITSUCCESS;
}

LitStatus CbmLitWeightCalculatorTukey::DoCalculate(
   const CbmLitTrackParam* par,
   HitPtrVector& hits,
   myf T)
{
   return DoCalculate(par, hits.begin(), hits.end(), T);
}

LitStatus CbmLitWeightCalculatorTukey::TukeyWeight(
   const CbmLitTrackParam* par,
   CbmLitHit* hit,
   myf T) const
{
   // FIXME:
   myf chiSq = 0.;//ChiSq(par, hit);
   if (chiSq < T) {
      myf t = 1 - (chiSq / T);
      hit->SetW(t * t);
   } else { hit->SetW(0.); }
   return kLITSUCCESS;
}

LitStatus CbmLitWeightCalculatorTukey::Normalize(
   HitPtrIterator itBegin,
   HitPtrIterator itEnd) const
{
   myf sumW = 0.;
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
