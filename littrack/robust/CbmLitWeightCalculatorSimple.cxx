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
		HitIterator itBegin,
		HitIterator itEnd)
{
	Int_t nofHits = itEnd - itBegin;
	Double_t w = 1.0 / Double_t(nofHits);
	
	for(HitIterator it = itBegin; it != itEnd; it++) {
		(*it)->SetW(w);
	}
	
	return kLITSUCCESS;
}

LitStatus CbmLitWeightCalculatorSimple::DoCalculate(
		const CbmLitTrackParam* par,
		HitVector& hits)
{
	return DoCalculate(par, hits.begin(), hits.end());
}

ClassImp(CbmLitWeightCalculatorSimple)
