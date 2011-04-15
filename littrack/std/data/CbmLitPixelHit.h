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
   myf GetX() const { return fX; }

   /* Returns Y hit position */
   myf GetY() const { return fY; }

   /* Returns X hit position error */
   myf GetDx() const { return fDx; }

   /* Returns Y hit position error */
   myf GetDy() const { return fDy; }

   /* Returns XY hit covariance */
   myf GetDxy() const { return fDxy; }

   /* Sets X hit position */
   void SetX(myf x) { fX = x; }

   /* Sets Y hit position */
   void SetY(myf y) { fY = y; }

   /* Sets X hit position error */
   void SetDx(myf dx) { fDx = dx; }

   /* Sets Y hit position error */
   void SetDy(myf dy) { fDy = dy; }

   /* Sets XY hit covariance */
   void SetDxy(myf dxy) { fDxy = dxy; }

   /* Returns std::string representation of the class */
   virtual std::string ToString() const {
      std::stringstream ss;
      ss << "PixelHit: pos=(" << GetX() << "," << GetY() << "," << GetZ()
         << ") err=(" << GetDx() << "," << GetDy() << "," << GetDz() << ") "
         << " dxy=" << GetDxy() << " planeId=" << GetPlaneId()
         << " refId=" << GetRefId() << " w=" << GetW()
         << " isOutlier=" << IsOutlier()
         << " hitType=" << GetType() << std::endl;
      return ss.str();
   }

private:
   /* X and Y hit positions in [cm] */
   myf fX, fY;
   /* X and Y hit position errors in [cm] */
   myf fDx, fDy;
   /* XY covariance */
   myf fDxy;
};

#endif /*CBMLITPIXELHIT_H_*/
