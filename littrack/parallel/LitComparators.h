/** LitComparators.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Comparators used in the STL algorithms.
 **/
#ifndef LITCOMPARATORS_H_
#define LITCOMPARATORS_H_

#include "LitHit.h"
#include "LitTrack.h"

namespace lit {
namespace parallel {

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



class CompareTrackChi2OverNdfLess :
   public std::binary_function<
   const LitScalTrack*,
   const LitScalTrack*,
   bool>
{
public:
   bool operator()(const LitScalTrack* track1, const LitScalTrack* track2) const {
      return ( (track1->GetChiSq() / track1->GetNDF()) < (track2->GetChiSq() / track2->GetNDF()) );
   }
};



class CompareTrackNofHitsMore :
   public std::binary_function<
   const LitScalTrack*,
   const LitScalTrack*,
   bool>
{
public:
   bool operator()(const LitScalTrack* track1, const LitScalTrack* track2) const {
      return track1->GetNofHits() > track2->GetNofHits();
   }
};



class CompareTrackPrevTrackIdLess :
   public std::binary_function<
   const LitScalTrack*,
   const LitScalTrack*,
   bool>
{
public:
   bool operator()(const LitScalTrack* track1, const LitScalTrack* track2) const {
      return track1->GetPreviousTrackId() < track2->GetPreviousTrackId();
   }
};

} // namespace parallel
} // namespace lit
#endif /* LITCOMPARATORS_H_ */
