#include "CbmLitMath.h"

#include "CbmLitTrackParam.h"
#include "CbmLitHit.h"
#include "CbmLitStripHit.h"
#include "CbmLitPixelHit.h"
#include "CbmLitTrack.h"

#include <iostream>

myf ChiSq(
		const CbmLitTrackParam* par,
		const CbmLitHit* hit)
{
	if (hit->GetType() == kLITSTRIPHIT) {
		return ChiSq(par, static_cast<const CbmLitStripHit*>(hit));
	} else if (hit->GetType() == kLITPIXELHIT) {
		return ChiSq(par, static_cast<const CbmLitPixelHit*>(hit));
	}
}

myf ChiSq(
		const CbmLitTrackParam* par,
		const CbmLitStripHit* hit)
{
	myf C0 = par->GetCovariance(0);
	myf C5 = par->GetCovariance(5);
	myf C1 = par->GetCovariance(1);
	myf u = hit->GetU();
	myf duu = hit->GetDu() * hit->GetDu();
	myf phiCos = hit->GetCosPhi();
	myf phiSin = hit->GetSinPhi();
	myf phiCosSq = phiCos * phiCos;
	myf phiSinSq = phiSin * phiSin;
	myf phi2SinCos = 2 * phiCos * phiSin;

	myf r = u - par->GetX() * phiCos - par->GetY() * phiSin;
	myf rr = r * r;
	myf norm = duu + C0 * phiCosSq + phi2SinCos * C1 + C5 * phiSinSq;

	return rr / norm;
}

myf ChiSq(
		const CbmLitTrackParam* par,
		const CbmLitPixelHit* hit)
{
	myf dxx = hit->GetDx() * hit->GetDx();
	myf dxy = hit->GetDxy();
	myf dyy = hit->GetDy() * hit->GetDy();
	myf C0 = par->GetCovariance(0);
	myf C5 = par->GetCovariance(5);
	myf C1 = par->GetCovariance(1);
	myf dx = hit->GetX() - par->GetX();
	myf dy = hit->GetY() - par->GetY();

	myf norm = -dxx * dyy + dxx * C5 + dyy * C0 - C0 * C5 + dxy * dxy - 2 * dxy * C1 + C1 * C1;
	if (norm == 0.) norm = 1e-10;

	myf chi2 = (-dx * dx * (dyy - C5) - dy * dy * (dxx - C0) + 2 * dx * dy * (dxy - C1)) / norm;

	return chi2;
}

int NDF(
		const CbmLitTrack* track)
{
   // TODO check NDF
	int ndf = 0;
	for (int i = 0; i < track->GetNofHits(); i++){
		if (track->GetHit(i)->GetType() == kLITPIXELHIT) ndf += 2;
		else if (track->GetHit(i)->GetType() == kLITSTRIPHIT) ndf++;
	}
	ndf -= 5;
	if (ndf > 0) return ndf; else return 1;
}
