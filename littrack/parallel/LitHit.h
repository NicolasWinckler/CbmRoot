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
#include "LitUtils.h"
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

   /* Returns std::string representation of the class */
   std::string ToString() const {
      return "LitStripHit: phiCos=" + lit::parallel::ToString<T>(phiCos)
            + " phiSin=" + lit::parallel::ToString<T>(phiSin)
            + " U=" + lit::parallel::ToString<T>(U)
            + " Du=" + lit::parallel::ToString<T>(Du) + "\n";
   }

   /* Operator << for convenient output to std::ostream */
   friend std::ostream& operator<<(std::ostream& strm, const LitStripHit& hit) {
      strm << hit.ToString();
      return strm;
   }

public:
   T phiCos; // Cosine of strip rotation angle
   T phiSin; // Sine of strip rotation angle
   T U; // U measurement [cm]
   T Du; // U measurement error [cm]
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

   /* Returns std::string representation of the class */
   std::string ToString() const {
      return "LitPixelHit: X=" + lit::parallel::ToString<T>(X)
         + " Y=" + lit::parallel::ToString<T>(Y)
         + " Dx=" + lit::parallel::ToString<T>(Dx)
         + " Dy=" + lit::parallel::ToString<T>(Dy)
         + " Dxy=" + lit::parallel::ToString<T>(Dxy) + "\n";
   }

   /* Operator << for convenient output to std::ostream */
   friend std::ostream& operator<<(std::ostream& strm, const LitPixelHit& hit) {
      strm << hit.ToString();
      return strm;
   }

public:
   T X, Y; // X and Y position measurements [cm]
   T Dx, Dy; // X and Y position error [cm]
   T Dxy; // Covariance between X and Y [cm]
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

   /* Returns std::string representation of the class */
   std::string ToString() const {
      return "LitScalStripHit: phiCos=" + lit::parallel::ToString<fscal>(phiCos)
            + " phiSin=" + lit::parallel::ToString<fscal>(phiSin)
            + " U=" + lit::parallel::ToString<fscal>(U)
            + " Du=" + lit::parallel::ToString<fscal>(Du)
            + " planeId=" + lit::parallel::ToString<int>(((int)planeId))
            + " refId=" + lit::parallel::ToString<fscal>(refId)
            + " Z=" + lit::parallel::ToString<fscal>(Z) + "\n";
   }

   /* Operator << for convenient output to std::ostream */
   friend std::ostream& operator<<(std::ostream& strm, const LitScalStripHit& hit) {
      strm << "LitScalStripHit: " << "phiCos=" << hit.phiCos << " phiSin=" << hit.phiSin
           << " U=" << hit.U << " Du=" << hit.Du << " planeId=" << (int)hit.planeId
           << " refId=" << hit.refId << " Z=" << hit.Z << std::endl;
      return strm;
   }

public:
   fscal phiCos; // Cosine of strip rotation angle
   fscal phiSin; // Sine of strip rotation angle
   fscal U; // U measurement [cm]
   fscal Du; // U measurement error [cm]
   unsigned char planeId; // Detector plane id
   unsigned short refId; // Reference id to smth
   fscal Z; // Z position [cm]
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

   /* Returns std::string representation of the class */
    std::string ToString() const {
       return "LitScalPixelHit: X=" + lit::parallel::ToString<fscal>(X)
          + " Y=" + lit::parallel::ToString<fscal>(Y)
          + " Dx=" + lit::parallel::ToString<fscal>(Dx)
          + " Dy=" + lit::parallel::ToString<fscal>(Dy)
          + " Dxy=" + lit::parallel::ToString<fscal>(Dxy)
          + " planeId=" + lit::parallel::ToString<int>((int)planeId)
          + " refId=" + lit::parallel::ToString<int>(refId)
          + " Z=" + lit::parallel::ToString<fscal>(Z) + "\n";
    }

   /* Operator << for convenient output to std::ostream */
   friend std::ostream& operator<<(std::ostream& strm, const LitScalPixelHit& hit) {
      strm << hit.ToString();
      return strm;
   }

public:
   fscal X, Y; // X and Y position measurements [cm]
   fscal Dx, Dy; // X and Y position error [cm]
   fscal Dxy; // Covariance between X and Y [cm]
   unsigned char planeId; // Detector plane id
   unsigned short refId; // Reference id to smth
   fscal Z; // Z position [cm]
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
