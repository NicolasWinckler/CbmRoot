#include "CbmLitTrackSelectionC.h"

#include "CbmLitTrack.h"
#include "CbmLitComparators.h"

#include <set>
#include <iostream>

CbmLitTrackSelectionC::CbmLitTrackSelectionC()
{
	Properties().AddProperty("fNofSharedHits", 0);
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
		TrackIterator itBegin,
		TrackIterator itEnd)
{
	
	if (itBegin == itEnd) return kLITSUCCESS;
	
	Properties().GetProperty("fNofSharedHits", fNofSharedHits);
	
	//SortNofHits(tracks);
	SortLastPlaneId(itBegin, itEnd);
	CheckSharedHits(itBegin, itEnd);
	
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionC::DoSelect(
		TrackVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}

void CbmLitTrackSelectionC::SortNofHits(
		TrackIterator itBegin,
		TrackIterator itEnd)
{
	std::sort(itBegin, itEnd, CompareTrackPtrNofHitsMore());
	
	Int_t maxNofHits = (*itBegin)->GetNofHits();
	Int_t minNofHits = (*(itEnd-1))->GetNofHits();
	
	for (Int_t iNofHits = minNofHits; iNofHits <= maxNofHits; iNofHits++) {
		CbmLitTrack value;
		value.SetNofHits(iNofHits);
		//if (!std::binary_search(tracks.begin(), tracks.end(), &value, CompareTrackPtrNofHitsMore())) continue;
		
		std::pair<TrackIterator, TrackIterator> bounds;
		bounds = std::equal_range(itBegin, itEnd, &value, CompareTrackPtrNofHitsMore());
		
		if(bounds.first == bounds.second) continue;
		
		std::sort(bounds.first, bounds.second, CompareTrackPtrChi2OverNdfLess());
	}
}

void CbmLitTrackSelectionC::SortLastPlaneId(
		TrackIterator itBegin,
		TrackIterator itEnd)
{
	std::sort(itBegin, itEnd, CompareTrackPtrLastPlaneIdMore());
	
//	for (TrackIterator iTrack = tracks.begin(); iTrack != tracks.end(); iTrack++) 
//			std::cout << (*iTrack)->GetLastPlaneId() << " ";
//	std::cout << std::endl << "NofTracks = " << tracks.size() << std::endl;
		
	Int_t maxPlaneId = (*itBegin)->GetLastPlaneId();
	Int_t minPlaneId = (*(itEnd-1))->GetLastPlaneId();
		
	for (Int_t iPlaneId = minPlaneId; iPlaneId <= maxPlaneId; iPlaneId++) {
		CbmLitTrack value;
		value.SetLastPlaneId(iPlaneId);
		//if (!std::binary_search(tracks.begin(), tracks.end(), &value, CompareTrackPtrLastPlaneIdMore())) continue;
			
		std::pair<TrackIterator, TrackIterator> bounds;
		bounds = std::equal_range(itBegin, itEnd, &value, CompareTrackPtrLastPlaneIdMore());
		
		if(bounds.first == bounds.second) continue;
		
		//std::cout << "bounds at positions " << int(bounds.first - tracks.begin())
		//	<< " and " << int(bounds.second - tracks.begin()) << std::endl;
			
		std::sort(bounds.first, bounds.second, CompareTrackPtrChi2OverNdfLess());
	}
}


void CbmLitTrackSelectionC::CheckSharedHits(
		TrackIterator itBegin,
		TrackIterator itEnd)
{
	std::set<Int_t> hitsId;
	
	for (TrackIterator iTrack = itBegin; iTrack != itEnd; iTrack++) {

		if ((*iTrack)->GetFlag() == 1) continue;
	    
	    Int_t nofSharedHits = 0;
	    Int_t nofHits = (*iTrack)->GetNofHits();
   
	    for(Int_t iHit = 0; iHit < nofHits; iHit++) {
	    	Int_t hitId = (*iTrack)->GetHit(iHit)->GetRefId();
	    	if(hitsId.find(hitId) != hitsId.end()) {
	            nofSharedHits++;
	            if (nofSharedHits > fNofSharedHits) {
	               (*iTrack)->SetFlag(1);
	               break;
	            }
	        }
	    } 	    

	    if ( (*iTrack)->GetFlag() == 1 ) continue;
	      
	    for(Int_t iHit = 0; iHit < nofHits; iHit++) {
	    	Int_t hitId = (*iTrack)->GetHit(iHit)->GetRefId();
	        hitsId.insert(hitId);
	    }
	}	   
	hitsId.clear();
}


ClassImp(CbmLitTrackSelectionC)
