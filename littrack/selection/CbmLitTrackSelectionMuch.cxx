
#include "CbmLitTrackSelectionMuch.h"

#include "CbmLitTrack.h"
#include "CbmLitComparators.h"
#include "CbmLitTrackSelectionA.h"
#include "CbmLitTrackSelectionB.h"
#include "CbmLitTrackSelectionC.h"
#include "CbmLitTrackSelectionD.h"

#include <set>
#include <iostream>

CbmLitTrackSelectionMuch::CbmLitTrackSelectionMuch()
{
	Properties().AddProperty("fNofSharedHits", 0);
	Properties().AddProperty("fMinNofHits", 0);
	Properties().AddProperty("fMinLastPlaneId", 0);
	
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

	//fSelectionA->DoSelect(itBegin, itEnd);
	//fSelectionB->DoSelect(itBegin, itEnd);
	fSelectionC->DoSelect(itBegin, itEnd);
	fSelectionD->DoSelect(itBegin, itEnd);
	
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionMuch::DoSelect(
		TrackVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}

ClassImp(CbmLitTrackSelectionMuch)

