#include "CbmLitTrackSelectionB.h"

#include "CbmLitTrack.h"
#include "CbmLitComparators.h"
//#include "CbmLitTrackSelectionC.h"

#include <algorithm>

CbmLitTrackSelectionB::CbmLitTrackSelectionB()
{
//	Properties().AddProperty("fNofSharedHits", 0);
//	fSelection = new CbmLitTrackSelectionC();
//	fSelection->Initialize();
}

CbmLitTrackSelectionB::~CbmLitTrackSelectionB()
{

}
LitStatus CbmLitTrackSelectionB::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionB::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionB::DoSelect(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	if (itBegin == itEnd) return kLITSUCCESS;

//	Properties().GetProperty("fNofSharedHits", fNofSharedHits);
//	fSelection->Properties().SetProperty("fNofSharedHits", fNofSharedHits);

	std::sort(itBegin, itEnd, CompareTrackPtrPrevTrackIdLess());

	Int_t minId = (*itBegin)->GetPreviousTrackId();
	Int_t maxId = (*(itEnd-1))->GetPreviousTrackId();

	//std::cout << "minId=" << minId << ", maxId=" << maxId << std::endl;

	for (Int_t iId = minId; iId <= maxId; iId++) {
		CbmLitTrack value;
		value.SetPreviousTrackId(iId);
		//if (!std::binary_search(tracks.begin(), tracks.end(), &value, CompareTrackPtrNofHitsMore())) continue;
		//std::cout << iId << " ";
		std::pair<TrackPtrIterator, TrackPtrIterator> bounds;
		bounds = std::equal_range(itBegin, itEnd, &value, CompareTrackPtrPrevTrackIdLess());
		if(bounds.first == bounds.second) continue;

//		std::cout << "stsTrackId = " << iId << std::endl;
//		std::cout << "bounds at positions " << int(bounds.first - itBegin)
//			<< " and " << int(bounds.second - itBegin) << std::endl;
		SortLastPlaneId(bounds.first, bounds.second);
		//if ((*bounds.first)->GetFlag() == 0) (*bounds.first)->SetFlag(0);
		for (TrackPtrIterator i = bounds.first + 1; i != bounds.second; i++)
			(*i)->SetQuality(kLITBAD);

		//fSelection->DoSelect(bounds.first, bounds.second);
	}

	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionB::DoSelect(
		TrackPtrVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}

void CbmLitTrackSelectionB::SortLastPlaneId(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
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

		std::pair<TrackPtrIterator, TrackPtrIterator> bounds;
		bounds = std::equal_range(itBegin, itEnd, &value, CompareTrackPtrLastPlaneIdMore());

		if(bounds.first == bounds.second) continue;

		//std::cout << "bounds at positions " << int(bounds.first - tracks.begin())
		//	<< " and " << int(bounds.second - tracks.begin()) << std::endl;

		std::sort(bounds.first, bounds.second, CompareTrackPtrChi2OverNdfLess());
	}
}
