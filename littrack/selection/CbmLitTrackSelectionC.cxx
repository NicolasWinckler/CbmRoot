#include "CbmLitTrackSelectionC.h"

#include "CbmLitTrack.h"
#include "CbmLitComparators.h"

#include <set>
#include <iostream>
#include <algorithm>

CbmLitTrackSelectionC::CbmLitTrackSelectionC():
	fNofSharedHits(0)
{
}

CbmLitTrackSelectionC::~CbmLitTrackSelectionC()
{
}

LitStatus CbmLitTrackSelectionC::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionC::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionC::DoSelect(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	if (itBegin == itEnd) return kLITSUCCESS;

	SortNofHits(itBegin, itEnd);
	//SortLastPlaneId(itBegin, itEnd);
	CheckSharedHits(itBegin, itEnd);

	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionC::DoSelect(
		TrackPtrVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}

void CbmLitTrackSelectionC::SortNofHits(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	std::sort(itBegin, itEnd, CompareTrackPtrNofHitsMore());

	int maxNofHits = (*itBegin)->GetNofHits();
	int minNofHits = (*(itEnd-1))->GetNofHits();

	for (int iNofHits = minNofHits; iNofHits <= maxNofHits; iNofHits++) {
		CbmLitTrack value;
		value.SetNofHits(iNofHits);
		//if (!std::binary_search(tracks.begin(), tracks.end(), &value, CompareTrackPtrNofHitsMore())) continue;

		std::pair<TrackPtrIterator, TrackPtrIterator> bounds;
		bounds = std::equal_range(itBegin, itEnd, &value, CompareTrackPtrNofHitsMore());

		if(bounds.first == bounds.second) continue;

		std::sort(bounds.first, bounds.second, CompareTrackPtrChi2OverNdfLess());
	}
}

void CbmLitTrackSelectionC::SortLastPlaneId(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	std::sort(itBegin, itEnd, CompareTrackPtrLastPlaneIdMore());

//	for (TrackIterator iTrack = tracks.begin(); iTrack != tracks.end(); iTrack++)
//			std::cout << (*iTrack)->GetLastPlaneId() << " ";
//	std::cout << std::endl << "NofTracks = " << tracks.size() << std::endl;

	int maxPlaneId = (*itBegin)->GetLastPlaneId();
	int minPlaneId = (*(itEnd-1))->GetLastPlaneId();

	for (int iPlaneId = minPlaneId; iPlaneId <= maxPlaneId; iPlaneId++) {
		CbmLitTrack value;
		value.SetLastPlaneId(iPlaneId);
		//if (!std::binary_search(tracks.begin(), tracks.end(), &value, CompareTrackPtrLastPlaneIdMore())) continue;

		std::pair<TrackPtrIterator, TrackPtrIterator> bounds;
		bounds = std::equal_range(itBegin, itEnd, &value, CompareTrackPtrLastPlaneIdMore());

		if(bounds.first == bounds.second) continue;

		//std::cout << "bounds at positions " << int(bounds.first - tracks.begin())
		//	<< " and " << int(bounds.second - tracks.begin()) << std::endl;

		std::sort(bounds.first, bounds.second, CompareTrackPtrChi2OverNdfLess());
	}
}


void CbmLitTrackSelectionC::CheckSharedHits(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	std::set<int> hitsId;
	const int STRIPSTART = 1e6;
	const int TRDSTART = 1e7;

	for (TrackPtrIterator iTrack = itBegin; iTrack != itEnd; iTrack++) {
		CbmLitTrack* track = *iTrack;

		if (track->GetQuality() == kLITBAD) continue;

	    int nofSharedHits = 0;
	    int nofHits = track->GetNofHits();

	    for(int iHit = 0; iHit < nofHits; iHit++) {
	    	int hitId = track->GetHit(iHit)->GetRefId();
	    	LitHitType type = track->GetHit(iHit)->GetType();
	    	LitDetectorId detId = track->GetHit(iHit)->GetDetectorId();
	    	if (type == kLITSTRIPHIT) hitId += STRIPSTART;
	    	if (detId == kLITTRD) hitId += TRDSTART;
	    	if(hitsId.find(hitId) != hitsId.end()) {
	            nofSharedHits++;
	            if (nofSharedHits > fNofSharedHits) {
	               track->SetQuality(kLITBAD);
	               break;
	            }
	        }
	    }

	    if (track->GetQuality() == kLITBAD) continue;

	    for(int iHit = 0; iHit < nofHits; iHit++) {
	    	int hitId = track->GetHit(iHit)->GetRefId();
	    	LitHitType type = track->GetHit(iHit)->GetType();
	    	if (type == kLITSTRIPHIT) hitId += STRIPSTART;
	        hitsId.insert(hitId);
	    }
	}
	hitsId.clear();
}
