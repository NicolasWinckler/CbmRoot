/** CbmLitPixelHit.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **
 ** Base data class for pixel hit.
 **/

#ifndef CBMLITPIXELHIT_H_
#define CBMLITPIXELHIT_H_

#include "data/CbmLitHit.h"

#include <string>
#include <sstream>

class CbmLitPixelHit : public CbmLitHit
{
public:
   /* Constructor */
   CbmLitPixelHit():
      fX(0.),
      fY(0.),
      fDx(0.),
      fDy(0.),
      fDxy(0.) {
      SetHitType(kLITPIXELHIT);
   }

   /* Destructor */
   virtual ~CbmLitPixelHit() {};

   /* Returns X hit position */
   litfloat GetX() const { return fX; }

   /* Returns Y hit position */
   litfloat GetY() const { return fY; }

   /* Returns X hit position error */
   litfloat GetDx() const { return fDx; }

   /* Returns Y hit position error */
   litfloat GetDy() const { return fDy; }

   /* Returns XY hit covariance */
   litfloat GetDxy() const { return fDxy; }

   /* Sets X hit position */
   void SetX(litfloat x) { fX = x; }

   /* Sets Y hit position */
   void SetY(litfloat y) { fY = y; }

   /* Sets X hit position error */
   void SetDx(litfloat dx) { fDx = dx; }

   /* Sets Y hit position error */
   void SetDy(litfloat dy) { fDy = dy; }

   /* Sets XY hit covariance */
   void SetDxy(litfloat dxy) { fDxy = dxy; }

   /* Returns std::string representation of the class */
   virtual std::string ToString() const {
      std::stringstream ss;
      ss << "PixelHit: pos=(" << GetX() << "," << GetY() << "," << GetZ()
         << ") err=(" << GetDx() << "," << GetDy() << "," << GetDz() << ") "
         << " dxy=" << GetDxy() << " planeId=" << GetPlaneId()
         << " refId=" << GetRefId() << " w=" << GetW()
         << " isOutlier=" << IsOutlier()
         << " hitType=" << GetType()
         << " detId=" << GetDetectorId() << std::endl;
      return ss.str();
   }

private:
   /* X and Y hit positions in [cm] */
   litfloat fX, fY;
   /* X and Y hit position errors in [cm] */
   litfloat fDx, fDy;
   /* XY covariance */
   litfloat fDxy;
};

#endif /*CBMLITPIXELHIT_H_*/
