#include "CbmLitTrackSelectionD.h"

#include "CbmLitTrack.h"

CbmLitTrackSelectionD::CbmLitTrackSelectionD():
	fMinNofHits(0),
	fMinLastPlaneId(0)
{
//	Properties().AddProperty("fMinNofHits", 0);
//	Properties().AddProperty("fMinLastPlaneId", 0);
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
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	if (itBegin == itEnd) return kLITSUCCESS;
	
//	Properties().GetProperty("fMinNofHits", fMinNofHits);
//	Properties().GetProperty("fMinLastPlaneId", fMinLastPlaneId);	
	
	for (TrackPtrIterator iTrack = itBegin; iTrack != itEnd; iTrack++) {
		if ((*iTrack)->GetQuality() == kLITBAD) continue;
		if ((*iTrack)->GetNofHits() < fMinNofHits ||
			(*iTrack)->GetLastPlaneId() < fMinLastPlaneId) (*iTrack)->SetQuality(kLITBAD);		
	}	
	
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionD::DoSelect(
		TrackPtrVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}

ClassImp(CbmLitTrackSelectionD)
