#include "utils/CbmLitMath.h"

#include "data/CbmLitTrackParam.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitStripHit.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitTrack.h"

#include <iostream>
#include <cmath>

namespace lit {

myf ChiSq(
		const CbmLitTrackParam* par,
		const CbmLitHit* hit)
{
	myf chisq = 0.;
	if (hit->GetType() == kLITSTRIPHIT) {
		chisq = ChiSq(par, static_cast<const CbmLitStripHit*>(hit));
	} else if (hit->GetType() == kLITPIXELHIT) {
		chisq = ChiSq(par, static_cast<const CbmLitPixelHit*>(hit));
	}
	return chisq;
}

myf ChiSq(
		const CbmLitTrackParam* par,
		const CbmLitStripHit* hit)
{
	myf duu = hit->GetDu() * hit->GetDu();
	myf phiCos = hit->GetCosPhi();
	myf phiSin = hit->GetSinPhi();
	myf phiCosSq = phiCos * phiCos;
	myf phiSinSq = phiSin * phiSin;
	myf phi2SinCos = 2 * phiCos * phiSin;
	myf C0 = par->GetCovariance(0);
	myf C1 = par->GetCovariance(1);
	myf C5 = par->GetCovariance(5);

	myf ru = hit->GetU() - par->GetX() * phiCos - par->GetY() * phiSin;

	return (ru * ru) / (duu - phiCosSq*C0 - phi2SinCos*C1 - phiSinSq*C5);
}

myf ChiSq(
		const CbmLitTrackParam* par,
		const CbmLitPixelHit* hit)
{
	myf dxx = hit->GetDx() * hit->GetDx();
	myf dxy = hit->GetDxy();
	myf dyy = hit->GetDy() * hit->GetDy();
	myf xmx = hit->GetX() - par->GetX();
	myf ymy = hit->GetY() - par->GetY();
	myf C0 = par->GetCovariance(0);
	myf C1 = par->GetCovariance(1);
	myf C5 = par->GetCovariance(5);

	myf norm = dxx * dyy - dxx * C5 - dyy * C0 + C0 * C5
	         - dxy * dxy + 2 * dxy * C1 - C1 * C1;
	if (norm == 0.) norm = 1e-10;
	return ((xmx * (dyy - C5) - ymy * (dxy - C1)) * xmx
			+(-xmx * (dxy - C1) + ymy * (dxx - C0)) * ymy) / norm;
}

int NDF(
		const CbmLitTrack* track)
{
	int ndf = 0;
	for (int i = 0; i < track->GetNofHits(); i++){
		if (track->GetHit(i)->GetType() == kLITPIXELHIT) ndf += 2;
		else if (track->GetHit(i)->GetType() == kLITSTRIPHIT) ndf++;
	}
	ndf -= 5;
	if (ndf > 0) return ndf; else return 1;
}

}

