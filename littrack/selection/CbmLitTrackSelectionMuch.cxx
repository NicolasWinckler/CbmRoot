/* CbmLitTrackSelectionMuch.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 */

#include "selection/CbmLitTrackSelectionMuch.h"

#include "data/CbmLitTrack.h"
#include "selection/CbmLitTrackSelectionSharedHits.h"
#include "selection/CbmLitTrackSelectionCuts.h"
#include "utils/CbmLitComparators.h"

CbmLitTrackSelectionMuch::CbmLitTrackSelectionMuch():
	fNofSharedHits(0),
	fMinNofHits(0),
	fMinLastPlaneId(0)
{
	fSelectionSharedHits = TrackSelectionPtr(new CbmLitTrackSelectionSharedHits());
	fSelectionSharedHits->Initialize();
	fSelectionCuts = TrackSelectionPtr(new CbmLitTrackSelectionCuts());
	fSelectionCuts->Initialize();
}

CbmLitTrackSelectionMuch::~CbmLitTrackSelectionMuch()
{
}

LitStatus CbmLitTrackSelectionMuch::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionMuch::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionMuch::DoSelect(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	if (itBegin == itEnd) return kLITSUCCESS;

	((CbmLitTrackSelectionSharedHits*)fSelectionSharedHits.get())->SetNofSharedHits(fNofSharedHits);
	((CbmLitTrackSelectionCuts*)fSelectionCuts.get())->SetMinNofHits(fMinNofHits);
	((CbmLitTrackSelectionCuts*)fSelectionCuts.get())->SetMinLastPlaneId(fMinLastPlaneId);

	for (TrackPtrIterator iTrack = itBegin; iTrack != itEnd; iTrack++)
		(*iTrack)->SetQuality(kLITGOOD);

	fSelectionSharedHits->DoSelect(itBegin, itEnd);
	fSelectionCuts->DoSelect(itBegin, itEnd);

	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionMuch::DoSelect(
		TrackPtrVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}
