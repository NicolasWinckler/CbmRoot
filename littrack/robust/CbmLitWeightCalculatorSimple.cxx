#include "CbmLitWeightCalculatorSimple.h"

#include "CbmLitPixelHit.h"

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
		HitPtrIterator itEnd)
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
		HitPtrVector& hits)
{
	return DoCalculate(par, hits.begin(), hits.end());
}

ClassImp(CbmLitWeightCalculatorSimple)
