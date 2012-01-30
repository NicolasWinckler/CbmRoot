#include "weight/CbmLitWeightCalculatorSimple.h"

#include "data/CbmLitHit.h"

CbmLitWeightCalculatorSimple::CbmLitWeightCalculatorSimple()
{
}

CbmLitWeightCalculatorSimple::~CbmLitWeightCalculatorSimple()
{
}

LitStatus CbmLitWeightCalculatorSimple::DoCalculate(
   const CbmLitTrackParam* par,
   HitPtrIterator itBegin,
   HitPtrIterator itEnd,
   litfloat T)
{
   int nofHits = itEnd - itBegin;
   litfloat w = 1.0 / litfloat(nofHits);

   for(HitPtrIterator it = itBegin; it != itEnd; it++) {
      (*it)->SetW(w);
   }

   return kLITSUCCESS;
}

LitStatus CbmLitWeightCalculatorSimple::DoCalculate(
   const CbmLitTrackParam* par,
   HitPtrVector& hits,
   litfloat T)
{
   return DoCalculate(par, hits.begin(), hits.end(), T);
}
