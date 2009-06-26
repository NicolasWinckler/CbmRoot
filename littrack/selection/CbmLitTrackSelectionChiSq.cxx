#include "CbmLitTrackSelectionChiSq.h"

#include "CbmLitTrack.h"

CbmLitTrackSelectionChiSq::CbmLitTrackSelectionChiSq():
	fMaxChiSq(1000.)
{
}

CbmLitTrackSelectionChiSq::~CbmLitTrackSelectionChiSq()
{
}

LitStatus CbmLitTrackSelectionChiSq::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionChiSq::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionChiSq::DoSelect(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	if (itBegin == itEnd) return kLITSUCCESS;

	for (TrackPtrIterator iTrack = itBegin; iTrack != itEnd; iTrack++) {
		if ((*iTrack)->GetQuality() == kLITBAD) continue;
		if ((*iTrack)->GetChi2() / (*iTrack)->GetNDF() > fMaxChiSq)
			(*iTrack)->SetQuality(kLITBAD);
	}

	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionChiSq::DoSelect(
		TrackPtrVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}
