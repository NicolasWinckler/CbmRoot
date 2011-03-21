#ifndef CBMLITCOMPARATORS_H_
#define CBMLITCOMPARATORS_H_

#include "base/CbmLitDetectorLayout.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitStripHit.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitTrackParam.h"
#include "propagation/CbmLitMaterialInfo.h"

#include <functional>
#include <iostream>



class CompareMaterialInfoZLess:
   public std::binary_function<
   const CbmLitMaterialInfo&,
   const CbmLitMaterialInfo&,
   bool>
{
public:
   bool operator()(const CbmLitMaterialInfo& mat1, const CbmLitMaterialInfo& mat2) const {
      return mat1.GetZpos() < mat2.GetZpos();
   }
};



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



//class CompareHitPtrChiSqLess:
// public std::binary_function<
//    const std::pair<CbmLitHit*, myf>&,
//    const std::pair<CbmLitHit*, myf>&,
//    bool>
//{
//public:
// bool operator()(const std::pair<CbmLitHit*, myf>& hit1, const std::pair<CbmLitHit*, myf>& hit2) const {
//    return hit1.second < hit2.second;
// }
//};



class CompareHitPtrWLess:
   public std::binary_function<
   const CbmLitHit*,
   const CbmLitHit*,
   bool>
{
public:
   bool operator()(const CbmLitHit* hit1, const CbmLitHit* hit2) const {
      return hit1->GetW() < hit2->GetW();
   }
};



class CompareTrackParamZLess:
   public std::binary_function<
   const CbmLitTrackParam&,
   const CbmLitTrackParam&,
   bool>
{
public:
   bool operator()(const CbmLitTrackParam& par1, const CbmLitTrackParam& par2) const {
      return par1.GetZ() < par2.GetZ();
   }
};
//
//class CompareHitPtrPlaneIdLess:
// public std::binary_function<
//    const CbmLitPixelHit*,
//     const CbmLitPixelHit*,
//     bool>
//{
//public:
// bool operator()(const CbmLitPixelHit* hit1, const CbmLitPixelHit* hit2) const {
//    return hit1->GetPlaneId() < hit2->GetPlaneId();
// }
//};
//
//
//
//class CompareHitPtrPlaneIdMore:
// public std::binary_function<
//    const CbmLitPixelHit*,
//     const CbmLitPixelHit*,
//     bool>
//{
//public:
// bool operator()(const CbmLitPixelHit* hit1, const CbmLitPixelHit* hit2) const {
//    return hit1->GetPlaneId() > hit2->GetPlaneId();
// }
//};



class CompareHitPtrPlaneIdLess:
   public std::binary_function<
   const CbmLitHit*,
   const CbmLitHit*,
   bool>
{
public:
   bool operator()(const CbmLitHit* hit1, const CbmLitHit* hit2) const {
      return hit1->GetPlaneId() < hit2->GetPlaneId();
   }
};



class CompareHitPtrPlaneIdMore:
   public std::binary_function<
   const CbmLitHit*,
   const CbmLitHit*,
   bool>
{
public:
   bool operator()(const CbmLitHit* hit1, const CbmLitHit* hit2) const {
      return hit1->GetPlaneId() > hit2->GetPlaneId();
   }
};



class ComparePixelHitPtrYLess :
   public std::binary_function<
   const CbmLitPixelHit*,
   const CbmLitPixelHit*,
   bool>
{
public:
   bool operator()(const CbmLitPixelHit* hit1, const CbmLitPixelHit* hit2) const {
      return hit1->GetY() < hit2->GetY();
   }
};



class ComparePixelHitPtrXLess :
   public std::binary_function<
   const CbmLitPixelHit*,
   const CbmLitPixelHit*,
   bool>
{
public:
   bool operator()(const CbmLitPixelHit* hit1, const CbmLitPixelHit* hit2) const {
      return hit1->GetX() < hit2->GetX();
   }
};



class CompareHitPtrXULess :
   public std::binary_function<
   const CbmLitHit*,
   const CbmLitHit*,
   bool>
{
public:
   bool operator()(const CbmLitHit* hit1, const CbmLitHit* hit2) const {
      if (hit1->GetType() == kLITPIXELHIT) {
         const CbmLitPixelHit* phit1 = static_cast<const CbmLitPixelHit*>(hit1);
         const CbmLitPixelHit* phit2 = static_cast<const CbmLitPixelHit*>(hit2);
         return phit1->GetX() < phit2->GetX();
      } else if (hit1->GetType() == kLITSTRIPHIT) {
         const CbmLitStripHit* shit1 = static_cast<const CbmLitStripHit*>(hit1);
         const CbmLitStripHit* shit2 = static_cast<const CbmLitStripHit*>(hit2);
         return shit1->GetU() < shit2->GetU();
      } else {
         std::cout << "CompareHitPtrXULess: HIT TYPE NOT SUPPORTED" << std::endl;
         return false;
      }
   }
};



class CompareStripHitPtrULess :
   public std::binary_function<
   const CbmLitStripHit*,
   const CbmLitStripHit*,
   bool>
{
public:
   bool operator()(const CbmLitStripHit* hit1, const CbmLitStripHit* hit2) const {
      return hit1->GetU() < hit2->GetU();
   }
};



class CompareTrackPtrChi2OverNdfLess :
   public std::binary_function<
   const CbmLitTrack*,
   const CbmLitTrack*,
   bool>
{
public:
   bool operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
      return ( (track1->GetChi2() / track1->GetNDF()) < (track2->GetChi2() / track2->GetNDF()) );
   }
};



class CompareTrackPtrPrevTrackIdLess :
   public std::binary_function<
   const CbmLitTrack*,
   const CbmLitTrack*,
   bool>
{
public:
   bool operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
      return track1->GetPreviousTrackId() < track2->GetPreviousTrackId();
   }
};



class CompareTrackPtrNofHitsMore :
   public std::binary_function<
   const CbmLitTrack*,
   const CbmLitTrack*,
   bool>
{
public:
   bool operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
      return track1->GetNofHits() > track2->GetNofHits();
   }
};


class CompareTrackPtrNofHitsLess :
   public std::binary_function<
   const CbmLitTrack*,
   const CbmLitTrack*,
   bool>
{
public:
   bool operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
      return track1->GetNofHits() < track2->GetNofHits();
   }
};


class CompareTrackPtrLastPlaneIdMore :
   public std::binary_function<
   const CbmLitTrack*,
   const CbmLitTrack*,
   bool>
{
public:
   bool operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
      return track1->GetLastPlaneId() > track2->GetLastPlaneId();
   }
};


class CompareStationZLess :
   public std::binary_function<
   const CbmLitStation&,
   const CbmLitStation&,
   bool>
{
public:
   bool operator()(const CbmLitStation& st1, const CbmLitStation& st2) const {
      return st1.GetSubstation(0).GetZ() < st2.GetSubstation(0).GetZ();
   }
};

#endif /*CBMLITCOMPARATORS_H_*/
