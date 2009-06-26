#include "CbmLitWeightCalculatorSimple.h"
#include "CbmLitHit.h"

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
		Double_t T)
{
	Int_t nofHits = itEnd - itBegin;
	Double_t w = 1.0 / Double_t(nofHits);

	for(HitPtrIterator it = itBegin; it != itEnd; it++) {
		(*it)->SetW(w);
	}

	return kLITSUCCESS;
}

LitStatus CbmLitWeightCalculatorSimple::DoCalculate(
		const CbmLitTrackParam* par,
		HitPtrVector& hits,
		Double_t T)
{
	return DoCalculate(par, hits.begin(), hits.end(), T);
}
