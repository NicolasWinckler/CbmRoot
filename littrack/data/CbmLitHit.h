/** CbmLitHit.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **
 ** Base data class for the hit.
 **/

#ifndef CBMLITHIT_H_
#define CBMLITHIT_H_

#include "base/CbmLitFloat.h"
#include "base/CbmLitEnums.h"

#include <string>

class CbmLitHit
{
public:
   /* Constructor */
   CbmLitHit():
      fPlaneId(0),
      fRefId(0),
      fW(1.0),
      fIsOutlier(false),
      fHitType(kLITPIXELHIT),
      fZ(0.),
      fDz(0.),
      fDetectorId(kLITTRD) {};

   /* Destructor */
   virtual ~CbmLitHit() {};

   /* @return Detector plane id identificator. */
   int GetPlaneId() const { return fPlaneId; }

   /* @return Reference to MC */
   int GetRefId() const { return fRefId; }

   /* @return Weight of the hit (used in the weighting fitting method)*/
   myf GetW() const { return fW; }

   /* @return If true than the hit is outlier (used in the weighting fitting method)*/
   bool IsOutlier() const { return fIsOutlier; }

   /* @return Type of the hit (strip, pixel, etc). Used to safely cast to the proper type.*/
   LitHitType GetType() const { return fHitType; }

   /* @return Z position of the hit [cm]*/
   myf GetZ() const { return fZ; }

   /* @return Z position error of the hit [cm]*/
   myf GetDz() const { return fDz; }

   /* @return Detector identificator (MUCH, TRD, TOF etc.)*/
   LitDetectorId GetDetectorId() const {return fDetectorId;}

   /* Sets detector plane id identificator.
    * @param planeId Plane identificator.
    */
   void SetPlaneId(int planeId) { fPlaneId = planeId; }

   /* Sets reference to MC
    * @param refId reference to MC
    */
   void SetRefId(int refId) { fRefId = refId; }

   /* Sets weight of the hit (used in the weighting fitting method
    * @param w Weight of the hit.
    */
   void SetW(myf w) { fW = w; }

   /* Sets If the hit is outlier (used in the weighting fitting method.
    * @param isOutlier If true than hit is outlier.
    */
   void IsOutlier(bool isOutlier) { fIsOutlier = isOutlier;}

   /* Sets type of the hit (strip, pixel, etc). Used to safely cast to the proper type.
    * @param hitType Type of the hit.
    */
   void SetHitType(LitHitType hitType) { fHitType = hitType; }

   /* Sets Z position of the hit [cm].
    * @param z Z position of the hit [cm].
    */
   void SetZ(myf z) { fZ = z; }

   /* Sets Z position error of the hit [cm]
    * @param dz Z position error of the hit [cm]
    */
   void SetDz(myf dz) { fDz = dz; }

   /* Sets detector identificator (MUCH, TRD, TOF etc.)
    * @param detectorId Detector identificator.
    */
   void SetDetectorId(LitDetectorId detectorId) {fDetectorId = detectorId;}

   /* Returns string reperesentation of the class. Should be implemented in derived classes.
    * @return String reperesentation of the class.
    */
   virtual std::string ToString() const {return "";}

private:
   int fPlaneId; // detector plane identificator (usually absolute substation number)
   int fRefId; // reference to MC
   myf fW; // weight of the hit (used in the weighting fitting method)
   bool fIsOutlier; // if true than the hit is outlier (used in the weighting fitting method)
   LitHitType fHitType; // type of the hit (strip, pixel, etc). Used to safely cast to the proper type.
   myf fZ; // Z position of the hit [cm]
   myf fDz; // Z position error of the hit [cm]
   LitDetectorId fDetectorId; // Detector identificator (MUCH, TRD, TOF etc.)
};

#endif /*CBMLITHIT_H_*/
