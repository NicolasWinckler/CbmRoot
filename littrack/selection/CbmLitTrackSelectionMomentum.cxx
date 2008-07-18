#include "CbmLitTrackSelectionMomentum.h"

#include "CbmLitTrack.h"

#include <cmath>
#include <iostream>

CbmLitTrackSelectionMomentum::CbmLitTrackSelectionMomentum()
{
	Properties().AddProperty("fMinMomentum", 0.);
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
		TrackIterator itBegin,
		TrackIterator itEnd)
{
	if (itBegin == itEnd) return kLITSUCCESS;
	
	Properties().GetProperty("fMinMomentum", fMinMomentum);
	
	for (TrackIterator iTrack = itBegin; iTrack != itEnd; iTrack++) {
		if ((*iTrack)->GetFlag() == 1) continue;
		if ((*iTrack)->GetParamLast()->GetQp() == 0.) {
			(*iTrack)->SetFlag(1);
			continue;
		}
		Double_t mom = std::abs(1. / (*iTrack)->GetParamLast()->GetQp());
		if (mom < fMinMomentum) (*iTrack)->SetFlag(1);		
	}	
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionMomentum::DoSelect(
		TrackVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}

ClassImp(CbmLitTrackSelectionMomentum)
