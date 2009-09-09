#include "CbmLitGating.h"
#include "CbmLitMath.h"
#include "CbmLitTrackParam.h"
#include "CbmLitHit.h"
#include "CbmLitStation.h"
#include "CbmLitPixelHit.h"
#include "CbmLitComparators.h"

#include <algorithm>
#include <cmath>

CbmLitGating::CbmLitGating():
	fSigmaCoef(3.5),
	fMaxCovSq(20. * 20.),
	fChiSqStripHitCut(10.),
	fChiSqPixelHitCut(25.),
	fUseFastSearch(true)
{
}

CbmLitGating::~CbmLitGating()
{
}

LitStatus CbmLitGating::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitGating::Finalize()
{
	return kLITSUCCESS;
}

bool CbmLitGating::IsHitInValidationGate(
		const CbmLitTrackParam* par,
		const CbmLitHit* hit) const
{
	myf chiSq = ChiSq(par, hit);
	if (hit->GetType() == kLITSTRIPHIT) return chiSq < fChiSqStripHitCut;
	if (hit->GetType() == kLITPIXELHIT) return chiSq < fChiSqPixelHitCut;
	return false;
}

HitPtrIteratorPair CbmLitGating::MinMaxIndex(
		const CbmLitTrackParam* par,
		HitPtrIteratorPair hits,
		const CbmLitStation& station,
		const std::pair<myf, char>& maxErr) const
{
	HitPtrIteratorPair bounds;
	if (station.GetType() == kLITSTRIPHIT || station.GetType() == kLITMIXHIT || !fUseFastSearch) {
		bounds = HitPtrIteratorPair(hits.first, hits.second);
	} else
	if (station.GetType() == kLITPIXELHIT) {
		CbmLitPixelHit hit;
		myf C0 = par->GetCovariance(0);
		if(C0 > fMaxCovSq || C0 < 0.) return bounds;
		myf devX = fSigmaCoef * (std::sqrt(C0) + maxErr.first);
	    hit.SetX(par->GetX() - devX);
	    bounds.first = std::lower_bound(hits.first, hits.second, &hit, CompareHitPtrXULess());
	    hit.SetX(par->GetX() + devX);
	    bounds.second =	std::lower_bound(hits.first, hits.second, &hit, CompareHitPtrXULess());
	}
    return bounds;
}
