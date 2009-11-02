/*
 * LitMath.h
 *
 *  Created on: Sep 25, 2009
 *      Author: andrey
 */

#ifndef LITMATH_H_
#define LITMATH_H_

#include "parallel/LitTypes.h"
#include "parallel/LitTrackParam.h"
#include "parallel/LitHit.h"
#include "parallel/LitTrack.h"

#define cnst static const fvec

inline fvec ChiSq(
		const LitTrackParam& par,
		const LitStripHit& hit)
{
	cnst TWO = 2.;

	fvec duu = hit.Du * hit.Du;
	fvec phiCosSq = hit.phiCos * hit.phiCos;
	fvec phiSinSq = hit.phiSin * hit.phiSin;
	fvec phi2SinCos = TWO * hit.phiCos * hit.phiSin;

	fvec r = hit.U - par.X * hit.phiCos - par.Y * hit.phiSin;
	fvec rr = r * r;
	fvec norm = duu + par.C0 * phiCosSq + phi2SinCos * par.C1 + par.C5 * phiSinSq;
//	fvec norm = duu + par.C0 * hit.phiCos + par.C5 * hitphiSin;

	return rr / norm;
}

inline fvec ChiSq(
		const LitTrackParam& par,
		const LitPixelHit& hit)
{
	cnst TWO = 2.;

	fvec dxx = hit.Dx * hit.Dx;
	fvec dyy = hit.Dy * hit.Dy;
	fvec dx = hit.X - par.X;
	fvec dy = hit.Y - par.Y;

	fvec norm = -dxx * dyy + dxx * par.C5 + dyy * par.C0 - par.C0 * par.C5 + hit.Dxy * hit.Dxy - TWO * hit.Dxy * par.C1 + par.C1 * par.C1;
//	if (norm == 0.) norm = 1e-10;

	return (-dx * dx * (dyy - par.C5) - dy * dy * (dxx - par.C0) + TWO * dx * dy * (hit.Dxy - par.C1)) / norm;
}

inline unsigned short NDF(const LitTrack& track)
{
   // TODO check NDF

	unsigned short ndf =  2 * track.nofHits - 5;
	if (ndf > 0) return ndf; else return 1;

//	int ndf = 0;
//	for (int i = 0; i < track->GetNofHits(); i++){
//		if (track->GetHit(i)->GetType() == kLITPIXELHIT) ndf += 2;
//		else if (track->GetHit(i)->GetType() == kLITSTRIPHIT) ndf++;
//	}
//	ndf -= 5;
//	if (ndf > 0) return ndf; else return 1;
}
#undef cnst
#endif /* LITMATH_H_ */
