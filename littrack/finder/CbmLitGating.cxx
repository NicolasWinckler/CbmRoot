/** CbmLitGating.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **/

#include "finder/CbmLitGating.h"

#include "base/CbmLitStation.h"
#include "data/CbmLitTrackParam.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitPixelHit.h"
#include "utils/CbmLitMath.h"
#include "utils/CbmLitComparators.h"

#include <algorithm>
#include <cmath>

CbmLitGating::CbmLitGating():
	fChiSqStripHitCut(4.),
	fChiSqPixelHitCut(15.),
	fUseFastSearch(true),
	fSigmaCoef(3.5),
	fMaxCovSq(20. * 20.)
{
}

CbmLitGating::~CbmLitGating()
{
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
