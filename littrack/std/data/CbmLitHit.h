/**
 * \file CbmLitHit.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Base data class for hits.
 **/

#ifndef CBMLITHIT_H_
#define CBMLITHIT_H_

#include "base/CbmLitEnums.h"
#include "base/CbmLitFloat.h"

#include <functional>
#include <string>
using std::string;

/**
 * \class CbmLitHit
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Base data class for hits.
 **/
class CbmLitHit
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitHit():
      fPlaneId(0),
      fRefId(0),
      fHitType(kLITPIXELHIT),
      fZ(0.),
      fDz(0.),
      fDetectorId(kLITTRD) {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitHit() {}

   /* Getters */
   int GetPlaneId() const { return fPlaneId; }
   int GetRefId() const { return fRefId; }
   LitHitType GetType() const { return fHitType; }
   litfloat GetZ() const { return fZ; }
   litfloat GetDz() const { return fDz; }
   LitDetectorId GetDetectorId() const {return fDetectorId;}

   /* Setters */
   void SetPlaneId(int planeId) { fPlaneId = planeId; }
   void SetRefId(int refId) { fRefId = refId; }
   void SetHitType(LitHitType hitType) { fHitType = hitType; }
   void SetZ(litfloat z) { fZ = z; }
   void SetDz(litfloat dz) { fDz = dz; }
   void SetDetectorId(LitDetectorId detectorId) {fDetectorId = detectorId;}

   /**
    * \brief Return string representation of class.
    * \return String representation of class.
    */
   virtual string ToString() const {return "";}

private:
   int fPlaneId; // detector plane identificator (usually absolute substation number)
   int fRefId; // reference to MC
   LitHitType fHitType; // type of the hit (strip, pixel, etc). Used to safely cast to the proper type.
   litfloat fZ; // Z position of the hit [cm]
   litfloat fDz; // Z position error of the hit [cm]
   LitDetectorId fDetectorId; // Detector identificator (MUCH, TRD, TOF etc.)
};

/**
 * \class Comparator for STL sorting algorithms.
 */
class CompareHitPtrZLess:
   public std::binary_function<
   const CbmLitHit*,
   const CbmLitHit*,
   bool>
{
public:
   bool operator()(const CbmLitHit* hit1, const CbmLitHit* hit2) const {
      return hit1->GetZ() < hit2->GetZ();
   }
};

/**
 * \class Comparator for STL sorting algorithms.
 */
class CompareHitPtrZMore:
   public std::binary_function<
   const CbmLitHit*,
   const CbmLitHit*,
   bool>
{
public:
   bool operator()(const CbmLitHit* hit1, const CbmLitHit* hit2) const {
      return hit1->GetZ() > hit2->GetZ();
   }
};

#endif /*CBMLITHIT_H_*/
