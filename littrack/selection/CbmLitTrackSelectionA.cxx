#include "CbmLitTrackSelectionA.h"

#include "CbmLitComparators.h"

CbmLitTrackSelectionA::CbmLitTrackSelectionA()
{
	
}

CbmLitTrackSelectionA::~CbmLitTrackSelectionA()
{
	
}

LitStatus CbmLitTrackSelectionA::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionA::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionA::DoSelect(
		TrackIterator itBegin,
		TrackIterator itEnd)
{
	if (itBegin == itEnd) return kLITSUCCESS;
	
	std::sort(itBegin, itEnd, CompareTrackPtrNofHitsLess());
	
	for (TrackIterator iTrack0 = itBegin; iTrack0 != itEnd - 1; iTrack0++){
		Int_t nofHits0 = (*iTrack0)->GetNofHits();
		if ((*iTrack0)->GetQuality() == kLITBAD) continue;
		for (TrackIterator iTrack1 = iTrack0 + 1; iTrack1 != itEnd; iTrack1++){
				Int_t nofHits1 = (*iTrack1)->GetNofHits();
				if (nofHits0 == nofHits1) continue;
				if (IsHitSharing(*iTrack0, *iTrack1)) (*iTrack0)->SetQuality(kLITBAD);				
		}		
	}	
	
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionA::DoSelect(
		TrackVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}

Bool_t CbmLitTrackSelectionA::IsHitSharing(
		const CbmLitTrack* track0,
		const CbmLitTrack* track1)
{
	for (Int_t iHit = 0; iHit < track0->GetNofHits(); iHit++) {
		Int_t refId0 = track0->GetHit(iHit)->GetRefId();
		Int_t refId1 = track1->GetHit(iHit)->GetRefId();
		if (refId0 != refId1) return false;
	}
	return true;
}


ClassImp(CbmLitTrackSelectionA)
