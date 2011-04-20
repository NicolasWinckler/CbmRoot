/** LitMath.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Mathematical functions for fast parallel version of the Littrack tracking.
 **/

#ifndef LITMATH_H_
#define LITMATH_H_

#include "LitTypes.h"
#include "LitTrackParam.h"
#include "LitHit.h"
#include "LitTrack.h"

namespace lit {
namespace parallel {

inline fscal rcp(const fscal& a) {return 1./a;}
inline fscal sgn(const fscal& a) {return a<0 ?-1 :(a>0 ?1 :0);}

template<class T>
inline T ChiSq(
   const LitTrackParam<T>& par,
   const LitStripHit<T>& hit)
{
   static const T TWO = 2.;

   T duu = hit.Du * hit.Du;
   T phiCosSq = hit.phiCos * hit.phiCos;
   T phiSinSq = hit.phiSin * hit.phiSin;
   T phi2SinCos = TWO * hit.phiCos * hit.phiSin;

   T r = hit.U - par.X * hit.phiCos - par.Y * hit.phiSin;
   T rr = r * r;
   T norm = duu + par.C0 * phiCosSq + phi2SinCos * par.C1 + par.C5 * phiSinSq;
// T norm = duu + par.C0 * hit.phiCos + par.C5 * hitphiSin;

   return rr / norm;
}

template<class T>
inline T ChiSq(
   const LitTrackParam<T>& par,
   const LitPixelHit<T>& hit)
{
   static const T TWO = 2.;

   T dxx = hit.Dx * hit.Dx;
   T dyy = hit.Dy * hit.Dy;
   T dx = hit.X - par.X;
   T dy = hit.Y - par.Y;

   T norm = -dxx * dyy + dxx * par.C5 + dyy * par.C0 - par.C0 * par.C5 + hit.Dxy * hit.Dxy - TWO * hit.Dxy * par.C1 + par.C1 * par.C1;
// if (norm == 0.) norm = 1e-10;

   return (-dx * dx * (dyy - par.C5) - dy * dy * (dxx - par.C0) + TWO * dx * dy * (hit.Dxy - par.C1)) / norm;
}

inline unsigned short NDF(
   const LitScalTrack& track)
{
   // TODO check NDF

   unsigned short ndf =  2 * track.nofHits - 5;
   if (ndf > 0) { return ndf; }
   else { return 1; }

// int ndf = 0;
// for (int i = 0; i < track->GetNofHits(); i++){
//    if (track->GetHit(i)->GetType() == kLITPIXELHIT) ndf += 2;
//    else if (track->GetHit(i)->GetType() == kLITSTRIPHIT) ndf++;
// }
// ndf -= 5;
// if (ndf > 0) return ndf; else return 1;
}

} // namespace parallel
} // namespace lit
#endif /* LITMATH_H_ */
