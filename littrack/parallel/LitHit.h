/** LitHit.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Hit data classes for fast parallel version of the Littrack tracking.
 **/

#ifndef LITHIT_H_
#define LITHIT_H_

#include "LitTypes.h"

namespace lit {
namespace parallel {

template<class T>
class LitStripHit
{
public:
   LitStripHit():
      phiCos(0.),
      phiSin(0.),
      U(0.),
      Du(0.) {}

   virtual ~LitStripHit() {}

   T phiCos;
   T phiSin;
   T U;
   T Du;

   friend std::ostream& operator<<(std::ostream& strm, const LitStripHit& hit) {
      strm << "LitStripHit: " << "phiCos=" << hit.phiCos << " phiSin=" << hit.phiSin
           << " U=" << hit.U << " Du=" << hit.Du
//       << " planeId=" << (int)hit.planeId
           << std::endl;
      return strm;
   }
} _fvecalignment;

typedef LitStripHit<fscal> LitStripHitScal;
typedef LitStripHit<fvec> LitStripHitVec;

template<class T>
class LitPixelHit
{
public:
   LitPixelHit():
      X(0.),
      Y(0.),
      Dx(0.),
      Dy(0.),
      Dxy(0.) {}

   virtual ~LitPixelHit() {}

   T X, Y;
   T Dx, Dy;
   T Dxy;

   friend std::ostream& operator<<(std::ostream& strm, const LitPixelHit& hit) {
      strm << "LitPixelHit: " << "X=" << hit.X << " Y=" << hit.Y
           << " Dx=" << hit.Dx << " Dy=" << hit.Dy << " Dxy=" << hit.Dxy
//       << " planeId=" << (int)hit.planeId
           << std::endl;
      return strm;
   }
} _fvecalignment;

typedef LitPixelHit<fscal> LitPixelHitScal;
typedef LitPixelHit<fvec> LitPixelHitVec;

class LitScalStripHit
{
public:
   LitScalStripHit():
      phiCos(0.),
      phiSin(0.),
      U(0.),
      Du(0.),
      planeId(0),
      refId(0),
      Z(0.) {}

   virtual ~LitScalStripHit() {}

   fscal phiCos;
   fscal phiSin;
   fscal U;
   fscal Du;
   unsigned char planeId;
   unsigned short refId;
   fscal Z;

   friend std::ostream& operator<<(std::ostream& strm, const LitScalStripHit& hit) {
      strm << "LitScalStripHit: " << "phiCos=" << hit.phiCos << " phiSin=" << hit.phiSin
           << " U=" << hit.U << " Du=" << hit.Du << " planeId=" << (int)hit.planeId
           << " refId=" << hit.refId << " Z=" << hit.Z << std::endl;
      return strm;
   }
};// _fvecalignment;



class LitScalPixelHit
{
public:
   LitScalPixelHit():
      X(0.),
      Y(0.),
      Dx(0.),
      Dy(0.),
      Dxy(0.),
      planeId(0),
      refId(0),
      Z(0.) {}

   virtual ~LitScalPixelHit() {}

   fscal X, Y;
   fscal Dx, Dy;
   fscal Dxy;
   unsigned char planeId;
   unsigned short refId;
   fscal Z;

   friend std::ostream& operator<<(std::ostream& strm, const LitScalPixelHit& hit) {
      strm << "LitScalPixelHit: " << "X=" << hit.X << " Y=" << hit.Y
           << " Dx=" << hit.Dx << " Dy=" << hit.Dy << " Dxy=" << hit.Dxy
           << " planeId=" << (int)hit.planeId << " refId=" << hit.refId << " Z=" << hit.Z << std::endl;
      return strm;
   }
};// _fvecalignment;



class ComparePixelHitXLess :
   public std::binary_function<
   const LitScalPixelHit*,
   const LitScalPixelHit*,
   bool>
{
public:
   bool operator()(const LitScalPixelHit* hit1, const LitScalPixelHit* hit2) const {
      return hit1->X < hit2->X;
   }
};

} // namespace parallel
} // namespace lit
#endif /* LITHIT_H_ */
