#include "CbmLitTrackSelectionTrd.h"

#include "CbmLitTrack.h"
#include "CbmLitTrackSelectionC.h"
#include "CbmLitTrackSelectionD.h"

#include <set>
#include <iostream>
#include <functional>

CbmLitTrackSelectionTrd::CbmLitTrackSelectionTrd():
	fNofSharedHits(0),
	fMinNofHits(0),
	fMinLastPlaneId(0)
{
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
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	if (itBegin == itEnd) return kLITSUCCESS;
	
	((CbmLitTrackSelectionC*)fSelectionC)->SetNofSharedHits(fNofSharedHits);
	((CbmLitTrackSelectionD*)fSelectionD)->SetMinNofHits(fMinNofHits);
	((CbmLitTrackSelectionD*)fSelectionD)->SetMinLastPlaneId(fMinLastPlaneId);
		
	for (TrackPtrIterator iTrack = itBegin; iTrack != itEnd; iTrack++) 
		(*iTrack)->SetQuality(kLITGOOD);

//	for_each(itBegin, itEnd, 
//			std::bind2nd(std::mem_fun(&CbmLitTrack::SetQuality),kLITGOOD));

	fSelectionC->DoSelect(itBegin, itEnd);
	fSelectionD->DoSelect(itBegin, itEnd);
	
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionTrd::DoSelect(
		TrackPtrVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}

ClassImp(CbmLitTrackSelectionTrd)
