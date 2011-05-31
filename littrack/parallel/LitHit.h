/** LitHit.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Hit data classes.
 **/

#ifndef LITHIT_H_
#define LITHIT_H_

#include "LitTypes.h"
#include <vector>

namespace lit {
namespace parallel {

/* Base class for strip like hits.
 * Each hit contains U measurement and its error
 * and sine and cosine of strip rotation angle phi. */
template<class T>
class LitStripHit
{
public:
   /* Constructor */
   LitStripHit():
      phiCos(0.),
      phiSin(0.),
      U(0.),
      Du(0.) {}

   /* Destructor */
   virtual ~LitStripHit() {}

   T phiCos; // Cosine of strip rotation angle
   T phiSin; // Sine of strip rotation angle
   T U; // U measurement [cm]
   T Du; // U measurement error [cm]

   /* Operator << for convenient output to std::ostream */
   friend std::ostream& operator<<(std::ostream& strm, const LitStripHit& hit) {
      strm << "LitStripHit: " << "phiCos=" << hit.phiCos << " phiSin=" << hit.phiSin
           << " U=" << hit.U << " Du=" << hit.Du << std::endl;
      return strm;
   }
} _fvecalignment;

/* Some typedefs for convenience */
typedef LitStripHit<fscal> LitStripHitScal;
typedef LitStripHit<fvec> LitStripHitVec;



/* Base class for pixel hits.
 * Each hit contains X, Y position measurements and
 * corresponding errors and covariance between X and Y. */
template<class T>
class LitPixelHit
{
public:
   /* Constructor */
   LitPixelHit():
      X(0.),
      Y(0.),
      Dx(0.),
      Dy(0.),
      Dxy(0.) {}

   /* Destructor */
   virtual ~LitPixelHit() {}

   T X, Y; // X and Y position measurements [cm]
   T Dx, Dy; // X and Y position error [cm]
   T Dxy; // Covariance between X and Y [cm]

   /* Operator << for convenient output to std::ostream */
   friend std::ostream& operator<<(std::ostream& strm, const LitPixelHit& hit) {
      strm << "LitPixelHit: " << "X=" << hit.X << " Y=" << hit.Y
           << " Dx=" << hit.Dx << " Dy=" << hit.Dy << " Dxy=" << hit.Dxy << std::endl;
      return strm;
   }
} _fvecalignment;

/* Some typedefs for convenience */
typedef LitPixelHit<fscal> LitPixelHitScal;
typedef LitPixelHit<fvec> LitPixelHitVec;



/* Base class for scalar strip hits.
 * Contains more information in comparison with LitStripHit.
 * Used for input scalar strip hit data to tracking. */
class LitScalStripHit
{
public:
   /* Constructor */
   LitScalStripHit():
      phiCos(0.),
      phiSin(0.),
      U(0.),
      Du(0.),
      planeId(0),
      refId(0),
      Z(0.) {}

   /* Destructor */
   virtual ~LitScalStripHit() {}

   fscal phiCos; // Cosine of strip rotation angle
   fscal phiSin; // Sine of strip rotation angle
   fscal U; // U measurement [cm]
   fscal Du; // U measurement error [cm]
   unsigned char planeId; // Detector plane id
   unsigned short refId; // Reference id to smth
   fscal Z; // Z position [cm]

   /* Operator << for convenient output to std::ostream */
   friend std::ostream& operator<<(std::ostream& strm, const LitScalStripHit& hit) {
      strm << "LitScalStripHit: " << "phiCos=" << hit.phiCos << " phiSin=" << hit.phiSin
           << " U=" << hit.U << " Du=" << hit.Du << " planeId=" << (int)hit.planeId
           << " refId=" << hit.refId << " Z=" << hit.Z << std::endl;
      return strm;
   }
};



/* Base class for scalar pixel hits.
 * Contains more information in comparison with LitPixelHit.
 * Used for input scalar pixel hit data to tracking. */
class LitScalPixelHit
{
public:
   /* Constructor */
   LitScalPixelHit():
      X(0.),
      Y(0.),
      Dx(0.),
      Dy(0.),
      Dxy(0.),
      planeId(0),
      refId(0),
      Z(0.) {}

   /* Destructor */
   virtual ~LitScalPixelHit() {}

   fscal X, Y; // X and Y position measurements [cm]
   fscal Dx, Dy; // X and Y position error [cm]
   fscal Dxy; // Covariance between X and Y [cm]
   unsigned char planeId; // Detector plane id
   unsigned short refId; // Reference id to smth
   fscal Z; // Z position [cm]

   /* Operator << for convenient output to std::ostream */
   friend std::ostream& operator<<(std::ostream& strm, const LitScalPixelHit& hit) {
      strm << "LitScalPixelHit: " << "X=" << hit.X << " Y=" << hit.Y
           << " Dx=" << hit.Dx << " Dy=" << hit.Dy << " Dxy=" << hit.Dxy
           << " planeId=" << (int)hit.planeId << " refId=" << hit.refId << " Z=" << hit.Z << std::endl;
      return strm;
   }
};



/* Some typedefs for convenience */
typedef std::vector<LitScalPixelHit*> PixelHitArray;
typedef std::vector<LitScalPixelHit*>::iterator PixelHitIterator;
typedef std::vector<LitScalPixelHit*>::const_iterator PixelHitConstIterator;
typedef std::pair<PixelHitIterator, PixelHitIterator> PixelHitIteratorPair;
typedef std::pair<PixelHitConstIterator, PixelHitConstIterator> PixelHitConstIteratorPair;

} // namespace parallel
} // namespace lit
#endif /* LITHIT_H_ */
