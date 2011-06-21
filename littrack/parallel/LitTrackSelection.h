/** LitTrackSelection.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 *
 * Implementation of the track selection algorithms.
 **/
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
