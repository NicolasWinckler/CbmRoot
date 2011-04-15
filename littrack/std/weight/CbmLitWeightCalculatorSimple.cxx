#include "weight/CbmLitWeightCalculatorSimple.h"

#include "data/CbmLitHit.h"

CbmLitWeightCalculatorSimple::CbmLitWeightCalculatorSimple()
{
}

CbmLitWeightCalculatorSimple::~CbmLitWeightCalculatorSimple()
{
}

LitStatus CbmLitWeightCalculatorSimple::Initialize()
{
   return kLITSUCCESS;
}

LitStatus CbmLitWeightCalculatorSimple::Finalize()
{
   return kLITSUCCESS;
}

LitStatus CbmLitWeightCalculatorSimple::DoCalculate(
   const CbmLitTrackParam* par,
   HitPtrIterator itBegin,
   HitPtrIterator itEnd,
   myf T)
{
   int nofHits = itEnd - itBegin;
   myf w = 1.0 / myf(nofHits);

   for(HitPtrIterator it = itBegin; it != itEnd; it++) {
      (*it)->SetW(w);
   }

   return kLITSUCCESS;
}

LitStatus CbmLitWeightCalculatorSimple::DoCalculate(
   const CbmLitTrackParam* par,
   HitPtrVector& hits,
   myf T)
{
   return DoCalculate(par, hits.begin(), hits.end(), T);
}
