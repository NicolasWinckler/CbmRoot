#include "utils/CbmLitMath.h"

#include "data/CbmLitTrackParam.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitStripHit.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitTrack.h"

#include <iostream>
#include <cmath>

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

//myf ChiSq(
//		const CbmLitTrackParam* par,
//		const CbmLitStripHit* hit)
//{
//	myf C0 = par->GetCovariance(0);
//	myf C5 = par->GetCovariance(5);
//	myf C1 = par->GetCovariance(1);
//	myf u = hit->GetU();
//	myf duu = hit->GetDu() * hit->GetDu();
//	myf phiCos = hit->GetCosPhi();
//	myf phiSin = hit->GetSinPhi();
//	myf phiCosSq = phiCos * phiCos;
//	myf phiSinSq = phiSin * phiSin;
//	myf phi2SinCos = 2 * phiCos * phiSin;
//
//	myf r = u - par->GetX() * phiCos - par->GetY() * phiSin;
//	myf rr = r * r;
//	myf norm = duu + C0 * phiCosSq + phi2SinCos * C1 + C5 * phiSinSq;
////	myf norm = duu + C0 * phiCos + C5 * phiSin;
//
//	return rr / norm;
//}

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
//	myf phiCosSq = phiCos * phiCos;
//	myf phiSinSq = phiSin * phiSin;
//	myf phi2SinCos = 2 * phiCos * phiSin;

	  myf zeta = phiCos*par->GetX() + phiSin*par->GetY() - u;

	  // F = CH'
	  myf F0 = phiCos*C0 + phiSin*C1;
	  myf F1 = phiCos*C1 + phiSin*C5;

	  myf HCH = ( F0*phiCos + F1*phiSin );

	  myf wi = 1./(duu +HCH);
	  myf zetawi = zeta *wi;
	 return  zeta * zetawi;

//	myf r = u - par->GetX() * phiCos - par->GetY() * phiSin;
//	myf rr = r * r;
//	myf norm = duu + C0 * phiCosSq + phi2SinCos * C1 + C5 * phiSinSq;
////	myf norm = duu + C0 * phiCos + C5 * phiSin;
//
//	return rr / norm;
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

//    chi2 = dx * dx / (dxx - C0) + dy * dy / (dyy - C5);
//    std::cout << hit->GetPlaneId() << " chi2=" << chi2  << " first=" << dx * dx / (dxx - C0)<< " dx=" << dx << " dxx=" << dxx << " C0=" << C0
//	          << " second=" << dy * dy / (dyy - C5) << " dy=" << dy << " dyy=" << dyy << " C5=" << C5 << std::endl;
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
