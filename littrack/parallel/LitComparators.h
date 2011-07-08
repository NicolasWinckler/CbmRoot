/**
 * \file LitComparators.h
 *
 * \brief Comparators used in the STL algorithms.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 *
 */
#ifndef LITCOMPARATORS_H_
#define LITCOMPARATORS_H_

#include "LitHit.h"
#include "LitTrack.h"

namespace lit {
namespace parallel {

/**
 * \class ComparePixelHitXLess
 *
 * \brief Comparator class used in STL algorithms for \c LitScalPixelHit class.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 * This comparator class can be used in STL sort and find algorithms
 * to sort in \a ascending order \c LitScalPixelHit objects by its X coordinate.
 */
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



/**
* \class CompareTrackChi2OverNdfLess
*
* \brief Comparator class used in STL algorithms for \c LitScalTrack class.
*
* \author Andrey Lebedev <andrey.lebedev@gsi.de>
* \date 2009
*
* This comparator class can be used in STL sort and find algorithms
* to sort in \a ascending order \c LitScalTracks objects by its
* (chi-square / NDF) value.
*/
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



/**
* \class CompareTrackNofHitsMore
*
* \brief Comparator class used in STL algorithms for \c LitScalTrack class.
*
* \author Andrey Lebedev <andrey.lebedev@gsi.de>
* \date 2009
*
* This comparator class can be used in STL sort and find algorithms
* to sort in \a descending order \c LitScalTracks objects by a
* number of hits.
*/
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



/**
* \class CompareTrackPrevTrackIdLess
*
* \brief Comparator class used in STL algorithms for \c LitScalTrack class.
*
* \author Andrey Lebedev <andrey.lebedev@gsi.de>
* \date 2009
*
* This comparator class can be used in STL sort and find algorithms
* to sort in \a ascending order \c LitScalTracks objects by its
* previous track index value.
*/
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
