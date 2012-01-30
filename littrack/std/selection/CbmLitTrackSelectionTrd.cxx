/* CbmLitTrackSelectionTrd.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 */

#include "selection/CbmLitTrackSelectionTrd.h"

#include "data/CbmLitTrack.h"
#include "selection/CbmLitTrackSelectionSharedHits.h"
#include "selection/CbmLitTrackSelectionSameSeed.h"
#include "selection/CbmLitTrackSelectionCuts.h"

#include <set>
#include <functional>

CbmLitTrackSelectionTrd::CbmLitTrackSelectionTrd():
   fNofSharedHits(0),
   fMinNofHits(0)
{
   fSharedHitsSelection = TrackSelectionPtr(new CbmLitTrackSelectionSharedHits());
   fSameSeedSelection = TrackSelectionPtr(new CbmLitTrackSelectionSameSeed());
   fCutsSelection = TrackSelectionPtr(new CbmLitTrackSelectionCuts());
}

CbmLitTrackSelectionTrd::~CbmLitTrackSelectionTrd()
{
}

LitStatus CbmLitTrackSelectionTrd::DoSelect(
   TrackPtrIterator itBegin,
   TrackPtrIterator itEnd)
{
   if (itBegin == itEnd) { return kLITSUCCESS; }

   ((CbmLitTrackSelectionSharedHits*)fSharedHitsSelection.get())->SetNofSharedHits(fNofSharedHits);
   ((CbmLitTrackSelectionCuts*)fCutsSelection.get())->SetMinNofHits(fMinNofHits);

   for (TrackPtrIterator iTrack = itBegin; iTrack != itEnd; iTrack++) {
      (*iTrack)->SetQuality(kLITGOOD);
   }

   fSharedHitsSelection->DoSelect(itBegin, itEnd);
   fSameSeedSelection->DoSelect(itBegin, itEnd);
   fCutsSelection->DoSelect(itBegin, itEnd);

   return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionTrd::DoSelect(
   TrackPtrVector& tracks)
{
   return DoSelect(tracks.begin(), tracks.end());
}
