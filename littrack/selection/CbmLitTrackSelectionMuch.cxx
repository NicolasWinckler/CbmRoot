
#include "CbmLitTrackSelectionMuch.h"

#include "CbmLitTrack.h"
#include "CbmLitComparators.h"
#include "CbmLitTrackSelectionA.h"
#include "CbmLitTrackSelectionB.h"
#include "CbmLitTrackSelectionC.h"
#include "CbmLitTrackSelectionD.h"

#include <set>
#include <iostream>

CbmLitTrackSelectionMuch::CbmLitTrackSelectionMuch():
	fNofSharedHits(0),
	fMinNofHits(0),
	fMinLastPlaneId(0)
{
	fSelectionA = new CbmLitTrackSelectionA();
	fSelectionA->Initialize();
	fSelectionB = new CbmLitTrackSelectionB();
	fSelectionB->Initialize();
	fSelectionC = new CbmLitTrackSelectionC();
	fSelectionC->Initialize();
	fSelectionD = new CbmLitTrackSelectionD();
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
	
	((CbmLitTrackSelectionC*)fSelectionC)->SetNofSharedHits(fNofSharedHits);
	((CbmLitTrackSelectionD*)fSelectionD)->SetMinNofHits(fMinNofHits);
	((CbmLitTrackSelectionD*)fSelectionD)->SetMinLastPlaneId(fMinLastPlaneId);
		
	for (TrackPtrIterator iTrack = itBegin; iTrack != itEnd; iTrack++) 
		(*iTrack)->SetQuality(kLITGOOD);

	//fSelectionA->DoSelect(itBegin, itEnd);
	//fSelectionB->DoSelect(itBegin, itEnd);
	fSelectionC->DoSelect(itBegin, itEnd);
	fSelectionD->DoSelect(itBegin, itEnd);
	
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionMuch::DoSelect(
		TrackPtrVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}

ClassImp(CbmLitTrackSelectionMuch)

