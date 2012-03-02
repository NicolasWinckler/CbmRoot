/* CbmLitTrackSelectionShortTracks.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 */

#include "selection/CbmLitTrackSelectionShortTracks.h"
#include "data/CbmLitTrack.h"

#include <algorithm>

CbmLitTrackSelectionShortTracks::CbmLitTrackSelectionShortTracks()
{

}

CbmLitTrackSelectionShortTracks::~CbmLitTrackSelectionShortTracks()
{

}

LitStatus CbmLitTrackSelectionShortTracks::DoSelect(
   TrackPtrIterator itBegin,
   TrackPtrIterator itEnd)
{
   if (itBegin == itEnd) { return kLITSUCCESS; }

   std::sort(itBegin, itEnd, CompareTrackPtrNofHitsLess());

   for (TrackPtrIterator iTrack0 = itBegin; iTrack0 != itEnd - 1; iTrack0++) {
      int nofHits0 = (*iTrack0)->GetNofHits();
      if ((*iTrack0)->GetQuality() == kLITBAD) { continue; }
      for (TrackPtrIterator iTrack1 = iTrack0 + 1; iTrack1 != itEnd; iTrack1++) {
         int nofHits1 = (*iTrack1)->GetNofHits();
         if (nofHits0 == nofHits1) { continue; }
         if (IsHitSharing(*iTrack0, *iTrack1)) { (*iTrack0)->SetQuality(kLITBAD); }
      }
   }

   return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionShortTracks::DoSelect(
   TrackPtrVector& tracks)
{
   return DoSelect(tracks.begin(), tracks.end());
}

bool CbmLitTrackSelectionShortTracks::IsHitSharing(
   const CbmLitTrack* track0,
   const CbmLitTrack* track1)
{
   for (int iHit = 0; iHit < track0->GetNofHits(); iHit++) {
      int refId0 = track0->GetHit(iHit)->GetRefId();
      int refId1 = track1->GetHit(iHit)->GetRefId();
      if (refId0 != refId1) { return false; }
   }
   return true;
}
