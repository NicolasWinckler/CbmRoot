/** CbmLitStripHit.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **
 ** Base data class for strip hit.
 **/

#ifndef CBMLITSTRIPHIT_H_
#define CBMLITSTRIPHIT_H_

#include "data/CbmLitHit.h"

#include <sstream>

class CbmLitStripHit : public CbmLitHit
{
public:
   /* Constructor */
   CbmLitStripHit():
      fU(0.),
      fDu(0.),
      fPhi(0.),
      fCosPhi(0.),
      fSinPhi(0.),
      fSegment(0) {
      SetHitType(kLITSTRIPHIT);
   }

   /* Destructor */
   virtual ~CbmLitStripHit() {};

   /* Returns U measurement of hit */
   litfloat GetU() const { return fU; }

   /* Returns U measurement error */
   litfloat GetDu() const { return fDu; }

   /* Returns rotation angle of the strip */
   litfloat GetPhi() const { return fPhi; }

   /* Returns cosine of the strip rotation angle */
   litfloat GetCosPhi() const { return fCosPhi; }

   /* Returns sine of the strip rotation angle */
   litfloat GetSinPhi() const { return fSinPhi; }

   /* Returns straw tube segment */
   int GetSegment() const { return fSegment; }

   /* Sets U measurement of the hit */
   void SetU(litfloat u) { fU = u; }

   /* Sets U measurement error */
   void SetDu(litfloat du) { fDu = du; }

   /* Sets rotation angle of the strip */
   void SetPhi(litfloat phi) { fPhi = phi; }

   /* Sets cosine of strip rotation angle */
   void SetCosPhi(litfloat cosPhi) { fCosPhi = cosPhi; }

   /* Sets sine of strip rotation angle */
   void SetSinPhi(litfloat sinPhi) { fSinPhi = sinPhi; }

   /* Sets straw tube segment */
   void SetSegment(int segment) { fSegment = segment; }

   /* Returns string representation of the class */
   virtual std::string ToString() const {
      std::stringstream ss;
      ss << "StripHit: pos=(" << GetU() << "," << GetZ()
         << ") err=(" << GetDu() << "," << GetDz() << ") "
         << " phi=" << GetPhi()
         << " cosPhi=" << GetCosPhi() << " sinPhi=" << GetSinPhi()
         << " planeId=" << GetPlaneId()
         << " refId=" << GetRefId() << " w=" << GetW()
         << " isOutlier=" << IsOutlier()
         << " hitType=" << GetType() << std::endl;
      return ss.str();
   }

protected:
   /* U measurement of the hit in [cm] */
   litfloat fU;
   /* U measurement error in [cm] */
   litfloat fDu;
   /* Strip rotation angle in [rad] */
   litfloat fPhi;
   /* Cosine of the strip rotation angle */
   litfloat fCosPhi;
   /* Sine of the strip rotation angle */
   litfloat fSinPhi;
   /* Up or down segment of the straw tube */
   int fSegment;
};

#endif /*CBMLITSTRIPHIT_H_*/
