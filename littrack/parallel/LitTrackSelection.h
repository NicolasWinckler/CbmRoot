/**
 * \file LitTrackSelection.h
 *
 * \brief Implementation of the track selection algorithms.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 *
 */
#ifndef LITTRACKSELECTION_H_
#define LITTRACKSELECTION_H_

#include "LitTrack.h"
#include "LitComparators.h"

#include <vector>
#include <set>
#include <utility>
#include <functional>

namespace lit {
namespace parallel {

/**
 * \fn void SortTracksByQuality(TrackIterator itBegin, TrackIterator itEnd)
 * \brief Sorts track array by quality.
 *
 * First tracks are sorted by the number of hits.
 * Than each subset of tracks with equal number
 * of hits is sorted by the chi square.
 *
 * \param[in] itBegin Iterator to the first track.
 * \param[in] itEnd Iterator to the last track.
 */
void SortTracksByQuality(
   TrackIterator itBegin,
   TrackIterator itEnd)
{
   std::sort(itBegin, itEnd, CompareTrackNofHitsMore());

   unsigned short maxNofHits = (*itBegin)->GetNofHits();
   unsigned short minNofHits = (*(itEnd-1))->GetNofHits();

   for (unsigned short iNofHits = minNofHits; iNofHits <= maxNofHits; iNofHits++) {
      LitScalTrack value;
      value.SetNofHits(iNofHits);

      std::pair<std::vector<LitScalTrack*>::iterator, std::vector<LitScalTrack*>::iterator> bounds;
      bounds = std::equal_range(itBegin, itEnd, &value, CompareTrackNofHitsMore());

      if(bounds.first == bounds.second) { continue; }
      std::sort(bounds.first, bounds.second, CompareTrackChi2OverNdfLess());
   }
}



/**
 * \fn void DoTrackSelectionSameSeed(TrackIterator itBegin, TrackIterator itEnd)
 * \brief Selects the best track for each subset of tracks with the same previous track index.
 *
 * \param[in] itBegin Iterator to the first track.
 * \param[in] itEnd Iterator to the last track.
 */
void DoTrackSelectionSameSeed(
      TrackIterator itBegin,
      TrackIterator itEnd)
{
   std::sort(itBegin, itEnd, CompareTrackPrevTrackIdLess());

   unsigned short minId = (*itBegin)->GetPreviousTrackId();
   unsigned short maxId = (*(itEnd-1))->GetPreviousTrackId();

   for (unsigned short iId = minId; iId <= maxId; iId++) {
      LitScalTrack value;
      value.SetPreviousTrackId(iId);
      std::pair<std::vector<LitScalTrack*>::iterator, std::vector<LitScalTrack*>::iterator> bounds;
      bounds = std::equal_range(itBegin, itEnd, &value, CompareTrackPrevTrackIdLess());
      if(bounds.first == bounds.second) { continue; }

      SortTracksByQuality(bounds.first, bounds.second);

      for (std::vector<LitScalTrack*>::iterator i = bounds.first + 1; i != bounds.second; i++) {
         (*i)->IsGood(false);
      }
   }
}



/**
 * \fn void DoTrackSelectionSharedHits(TrackIterator itBegin, TrackIterator itEnd)
 * \brief Removes clone and ghost tracks sorting by quality and checking of shared hits.
 *
 * This selection algorithm works in two steps. First, tracks are sorted by
 * their quality which is defined by the track length and chi-square.
 * Then, starting from the highest quality tracks all hits belonging to
 * a track are checked. In particular, the number of hits shared with other
 * tracks is calculated and the track is rejected if more than
 * MAX_NOF_SHARED_HITS of the hits are shared.
 *
 * \param[in] itBegin Iterator to the first track.
 * \param[in] itEnd Iterator to the last track.
 */
void DoTrackSelectionSharedHits(
   TrackIterator itBegin,
   TrackIterator itEnd)
{
   SortTracksByQuality(itBegin, itEnd);

   std::set<unsigned int> hitsId;
//   static const int STRIPSTART = 100000;
//   static const int TRDSTART = 1000000;
   static const unsigned short MAX_NOF_SHARED_HITS = 3;

   for (std::vector<LitScalTrack*>::iterator iTrack = itBegin; iTrack != itEnd; iTrack++) {
      LitScalTrack* track = *iTrack;

      if (!track->IsGood()) { continue; }

      unsigned short nofSharedHits = 0;
      unsigned short nofHits = track->GetNofHits();

      for(unsigned short iHit = 0; iHit < nofHits; iHit++) {
         int hitId = track->GetHit(iHit)->refId;
         //LitHitType type = track->GetHit(iHit)->GetType();
         //LitDetectorId detId = track->GetHit(iHit)->GetDetectorId();
         //if (type == kLITSTRIPHIT) { hitId += STRIPSTART; }
         //if (detId == kLITTRD) { hitId += TRDSTART; }
         if(hitsId.find(hitId) != hitsId.end()) {
            nofSharedHits++;
            if (nofSharedHits > MAX_NOF_SHARED_HITS) {
               track->IsGood(false);
               break;
            }
         }
      }

      if (!track->IsGood()) { continue; }

      for(int iHit = 0; iHit < nofHits; iHit++) {
         int hitId = track->GetHit(iHit)->refId;
         //LitHitType type = track->GetHit(iHit)->GetType();
         //LitDetectorId detId = track->GetHit(iHit)->GetDetectorId();
         //if (type == kLITSTRIPHIT) { hitId += STRIPSTART; }
         //if (detId == kLITTRD) { hitId += TRDSTART; }
         hitsId.insert(hitId);
      }
   }
   hitsId.clear();
}



/**
 * \fn void DoTrackSelectionMuon(TrackIterator itBegin, TrackIterator itEnd)
 * \brief Track selection for the MUCH tracking.
 *
 * \param[in] itBegin Iterator to the first track.
 * \param[in] itEnd Iterator to the last track.
 */
void DoTrackSelectionMuon(
   TrackIterator itBegin,
   TrackIterator itEnd)
{
   for (std::vector<LitScalTrack*>::iterator iTrack = itBegin; iTrack != itEnd; iTrack++) {
      LitScalTrack* track = *iTrack;
      if (track->GetNofHits() < 2) track->IsGood(false);
   }
   DoTrackSelectionSharedHits(itBegin, itEnd);
   DoTrackSelectionSameSeed(itBegin, itEnd);
}



/**
 * \fn void DoTrackSelectionElectron(TrackIterator itBegin, TrackIterator itEnd)
 * \brief Track selection for the TRD tracking.
 *
 * \param[in] itBegin Iterator to the first track.
 * \param[in] itEnd Iterator to the last track.
 */
void DoTrackSelectionElectron(
   TrackIterator itBegin,
   TrackIterator itEnd)
{
   for (std::vector<LitScalTrack*>::iterator iTrack = itBegin; iTrack != itEnd; iTrack++) {
      LitScalTrack* track = *iTrack;
      if (track->GetNofHits() < 2) track->IsGood(false);
   }
}

} // namespace parallel
} // namespace lit
#endif /* LITTRACKSELECTION_H_ */
