/* CbmLitTrackSelectionMomentum.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 */

#include "CbmLitTrackSelectionMomentum.h"
#include "CbmLitTrack.h"
#include <cmath>

CbmLitTrackSelectionMomentum::CbmLitTrackSelectionMomentum():
	fMinMomentum(0.)
{
}

CbmLitTrackSelectionMomentum::~CbmLitTrackSelectionMomentum()
{
}

LitStatus CbmLitTrackSelectionMomentum::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionMomentum::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionMomentum::DoSelect(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	if (itBegin == itEnd) return kLITSUCCESS;

	for (TrackPtrIterator iTrack = itBegin; iTrack != itEnd; iTrack++) {
		if ((*iTrack)->GetQuality() == kLITBAD) continue;
		if ((*iTrack)->GetParamLast()->GetQp() == 0.) {
			(*iTrack)->SetQuality(kLITBAD);
			continue;
		}
		myf mom = std::abs(1. / (*iTrack)->GetParamLast()->GetQp());
		if (mom < fMinMomentum) (*iTrack)->SetQuality(kLITBAD);
	}
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionMomentum::DoSelect(
		TrackPtrVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}
