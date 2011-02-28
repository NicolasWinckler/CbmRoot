/* CbmLitTrackSelectionTrd.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 */

#include "selection/CbmLitTrackSelectionTrd.h"

#include "data/CbmLitTrack.h"
#include "selection/CbmLitTrackSelectionSharedHits.h"
#include "selection/CbmLitTrackSelectionSameSeed.h"

#include <set>
#include <functional>

CbmLitTrackSelectionTrd::CbmLitTrackSelectionTrd():
	fNofSharedHits(0)
{
	fSharedHitsSelection = TrackSelectionPtr(new CbmLitTrackSelectionSharedHits());
	fSharedHitsSelection->Initialize();
	fSameSeedSelection = TrackSelectionPtr(new CbmLitTrackSelectionSameSeed());
	fSameSeedSelection->Initialize();
}

CbmLitTrackSelectionTrd::~CbmLitTrackSelectionTrd()
{
}

LitStatus CbmLitTrackSelectionTrd::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionTrd::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionTrd::DoSelect(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	if (itBegin == itEnd) return kLITSUCCESS;

	((CbmLitTrackSelectionSharedHits*)fSharedHitsSelection.get())->SetNofSharedHits(fNofSharedHits);

	for (TrackPtrIterator iTrack = itBegin; iTrack != itEnd; iTrack++)
		(*iTrack)->SetQuality(kLITGOOD);

	fSharedHitsSelection->DoSelect(itBegin, itEnd);
	fSameSeedSelection->DoSelect(itBegin, itEnd);

	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionTrd::DoSelect(
		TrackPtrVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}
