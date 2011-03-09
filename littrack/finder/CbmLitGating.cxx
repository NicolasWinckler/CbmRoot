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
	fChiSqStripHitCut(9.),
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
		const CbmLitHit* hit,
		myf chiSq,
		const CbmLitTrackParam* par) const
{
	if (hit->GetType() == kLITSTRIPHIT) {
//		// Determine straw tube segment
//		const CbmLitStripHit* h = static_cast<const CbmLitStripHit*>(hit);
//		myf devX = fSigmaCoef * std::sqrt(par->GetCovariance(0));
//		myf devY = fSigmaCoef * std::sqrt(par->GetCovariance(5));
//		myf vmin = -(par->GetX() - devX) * h->GetSinPhi() + (par->GetY() - devY) * h->GetCosPhi();
//		myf vmax = -(par->GetX() + devX) * h->GetSinPhi() + (par->GetY() + devY) * h->GetCosPhi();
//
//		if (copysign(1.0, vmin) != copysign(1.0, vmax)) return chiSq < fChiSqStripHitCut;
//		else return (chiSq < fChiSqStripHitCut
//				&& copysign(1.0, vmin) == h->GetSegment());
		return chiSq < fChiSqStripHitCut;
	}
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
	if (station.GetType() == kLITSTRIPHIT) {
		bounds = HitPtrIteratorPair(hits.first, hits.second);

		// FIXME : check this for straw tubes

//		if (hits.first == hits.second) return bounds;
//
//		const CbmLitStripHit* h = static_cast<const CbmLitStripHit*>(*(hits.first));
//		CbmLitStripHit hit;
//		myf C0 = par->GetCovariance(0);
//		myf C5 = par->GetCovariance(5);
//		if(C0 > fMaxCovSq || C0 < 0. || C5 > fMaxCovSq || C5 < 0.) return bounds;
//		myf devX = fSigmaCoef * std::sqrt(C0);
//		myf devY = fSigmaCoef * std::sqrt(C5);
//		myf X = par->GetX();
//		myf Y = par->GetY();
//		myf cos = h->GetCosPhi();
//		myf sin = h->GetSinPhi();
//		myf umin = (X - devX) * cos + (Y - devY) * sin - h->GetDu();
//		myf umax = (X + devX) * cos + (Y + devY) * sin + h->GetDu();
//
//		if (umin > umax) {
//			return bounds;
//			std::cout << "-E- umin>umax:" << umin << ">" << umax << std::endl;
//		}
//
//		hit.SetU(umin);
//		bounds.first = std::lower_bound(hits.first, hits.second, &hit, CompareHitPtrXULess());
//		hit.SetU(umax);
//		bounds.second =	std::lower_bound(hits.first, hits.second, &hit, CompareHitPtrXULess());

	} else if (station.GetType() == kLITMIXHIT || !fUseFastSearch) {
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
