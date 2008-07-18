#include "CbmLitTrackSelectionD.h"

#include "CbmLitTrack.h"

CbmLitTrackSelectionD::CbmLitTrackSelectionD()
{
	Properties().AddProperty("fMinNofHits", 0);
	Properties().AddProperty("fMinLastPlaneId", 0);
}

CbmLitTrackSelectionD::~CbmLitTrackSelectionD()
{
}

LitStatus CbmLitTrackSelectionD::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionD::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionD::DoSelect(
		TrackIterator itBegin,
		TrackIterator itEnd)
{
	if (itBegin == itEnd) return kLITSUCCESS;
	
	Properties().GetProperty("fMinNofHits", fMinNofHits);
	Properties().GetProperty("fMinLastPlaneId", fMinLastPlaneId);	
	
	for (TrackIterator iTrack = itBegin; iTrack != itEnd; iTrack++) {
		if ((*iTrack)->GetFlag() == 1) continue;
		if ((*iTrack)->GetNofHits() < fMinNofHits ||
			(*iTrack)->GetLastPlaneId() < fMinLastPlaneId) (*iTrack)->SetFlag(1);		
	}	
	
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionD::DoSelect(
		TrackVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}

ClassImp(CbmLitTrackSelectionD)
