/* CbmLitTrackSelectionMuch.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 */

#include "CbmLitTrackSelectionMuch.h"
#include "CbmLitTrack.h"
#include "CbmLitComparators.h"
#include "CbmLitTrackSelectionSharedHits.h"
#include "CbmLitTrackSelectionD.h"

CbmLitTrackSelectionMuch::CbmLitTrackSelectionMuch():
	fNofSharedHits(0),
	fMinNofHits(0),
	fMinLastPlaneId(0)
{
	fSelectionSharedHits = TrackSelectionPtr(new CbmLitTrackSelectionSharedHits());
	fSelectionSharedHits->Initialize();
	fSelectionD = TrackSelectionPtr(new CbmLitTrackSelectionD());
	fSelectionD->Initialize();
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
	((CbmLitTrackSelectionD*)fSelectionD.get())->SetMinNofHits(fMinNofHits);
	((CbmLitTrackSelectionD*)fSelectionD.get())->SetMinLastPlaneId(fMinLastPlaneId);

	for (TrackPtrIterator iTrack = itBegin; iTrack != itEnd; iTrack++)
		(*iTrack)->SetQuality(kLITGOOD);

	fSelectionSharedHits->DoSelect(itBegin, itEnd);
	fSelectionD->DoSelect(itBegin, itEnd);

	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionMuch::DoSelect(
		TrackPtrVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}
