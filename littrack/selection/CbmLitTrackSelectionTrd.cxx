#include "CbmLitTrackSelectionTrd.h"

#include "CbmLitTrack.h"
#include "CbmLitTrackSelectionC.h"
#include "CbmLitTrackSelectionD.h"

#include <set>
#include <iostream>

CbmLitTrackSelectionTrd::CbmLitTrackSelectionTrd()
{
	Properties().AddProperty("fNofSharedHits", 0);
	Properties().AddProperty("fMinNofHits", 0);
	Properties().AddProperty("fMinLastPlaneId", 0);
	
	fSelectionC = new CbmLitTrackSelectionC();
	fSelectionC->Initialize();
	fSelectionD = new CbmLitTrackSelectionD();
	fSelectionD->Initialize();
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
		TrackIterator itBegin,
		TrackIterator itEnd)
{
	
	if (itBegin == itEnd) return kLITSUCCESS;
	
	Properties().GetProperty("fNofSharedHits", fNofSharedHits);
	Properties().GetProperty("fMinNofHits", fMinNofHits);
	Properties().GetProperty("fMinLastPlaneId", fMinLastPlaneId);
	
	fSelectionC->Properties().SetProperty("fNofSharedHits", fNofSharedHits);
	fSelectionD->Properties().SetProperty("fMinNofHits", fMinNofHits);
	fSelectionD->Properties().SetProperty("fMinLastPlaneId", fMinLastPlaneId);
		
	for (TrackIterator iTrack = itBegin; iTrack != itEnd; iTrack++) 
		(*iTrack)->SetFlag(0);

	fSelectionC->DoSelect(itBegin, itEnd);
	fSelectionD->DoSelect(itBegin, itEnd);

	
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionTrd::DoSelect(
		TrackVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}

ClassImp(CbmLitTrackSelectionTrd)


