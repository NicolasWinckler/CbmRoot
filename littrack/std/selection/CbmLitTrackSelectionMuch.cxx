/* CbmLitTrackSelectionMuch.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 */

#include "selection/CbmLitTrackSelectionMuch.h"

#include "data/CbmLitTrack.h"
#include "selection/CbmLitTrackSelectionSharedHits.h"
#include "selection/CbmLitTrackSelectionCuts.h"
#include "selection/CbmLitTrackSelectionSameSeed.h"
#include "utils/CbmLitComparators.h"

CbmLitTrackSelectionMuch::CbmLitTrackSelectionMuch():
   fNofSharedHits(0),
   fMinNofHits(0),
   fMinLastPlaneId(0)
{
   fSharedHitsSelection = TrackSelectionPtr(new CbmLitTrackSelectionSharedHits());
   fCutsSelection = TrackSelectionPtr(new CbmLitTrackSelectionCuts());
   fSameSeedSelection = TrackSelectionPtr(new CbmLitTrackSelectionSameSeed());
}

CbmLitTrackSelectionMuch::~CbmLitTrackSelectionMuch()
{
}

LitStatus CbmLitTrackSelectionMuch::DoSelect(
   TrackPtrIterator itBegin,
   TrackPtrIterator itEnd)
{
   if (itBegin == itEnd) { return kLITSUCCESS; }

   ((CbmLitTrackSelectionSharedHits*)fSharedHitsSelection.get())->SetNofSharedHits(fNofSharedHits);
   ((CbmLitTrackSelectionCuts*)fCutsSelection.get())->SetMinNofHits(fMinNofHits);
   ((CbmLitTrackSelectionCuts*)fCutsSelection.get())->SetMinLastPlaneId(fMinLastPlaneId);

   for (TrackPtrIterator iTrack = itBegin; iTrack != itEnd; iTrack++) {
      (*iTrack)->SetQuality(kLITGOOD);
   }

   fSharedHitsSelection->DoSelect(itBegin, itEnd);
   fSameSeedSelection->DoSelect(itBegin, itEnd);
   fCutsSelection->DoSelect(itBegin, itEnd);

   return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionMuch::DoSelect(
   TrackPtrVector& tracks)
{
   return DoSelect(tracks.begin(), tracks.end());
}
