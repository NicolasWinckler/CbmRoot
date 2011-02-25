/* CbmLitQualitySort.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 */

#include "selection/CbmLitQualitySort.h"

#include "utils/CbmLitComparators.h"

#include <algorithm>

CbmLitQualitySort::CbmLitQualitySort()
{

}

CbmLitQualitySort::~CbmLitQualitySort()
{

}

LitStatus CbmLitQualitySort::DoSort(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	if (itBegin == itEnd) return kLITSUCCESS;

	SortNofHits(itBegin, itEnd);
	//SortLastPlaneId(itBegin, itEnd);

	return kLITSUCCESS;
}

LitStatus CbmLitQualitySort::DoSort(
		TrackPtrVector& tracks)
{
	return DoSort(tracks.begin(), tracks.end());
}

void CbmLitQualitySort::SortNofHits(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	std::sort(itBegin, itEnd, CompareTrackPtrNofHitsMore());

	int maxNofHits = (*itBegin)->GetNofHits();
	int minNofHits = (*(itEnd-1))->GetNofHits();

	for (int iNofHits = minNofHits; iNofHits <= maxNofHits; iNofHits++) {
		CbmLitTrack value;
		value.SetNofHits(iNofHits);

		std::pair<TrackPtrIterator, TrackPtrIterator> bounds;
		bounds = std::equal_range(itBegin, itEnd, &value, CompareTrackPtrNofHitsMore());

		if(bounds.first == bounds.second) continue;

		std::sort(bounds.first, bounds.second, CompareTrackPtrChi2OverNdfLess());
	}
}

void CbmLitQualitySort::SortLastPlaneId(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	std::sort(itBegin, itEnd, CompareTrackPtrLastPlaneIdMore());

	int maxPlaneId = (*itBegin)->GetLastPlaneId();
	int minPlaneId = (*(itEnd-1))->GetLastPlaneId();

	for (int iPlaneId = minPlaneId; iPlaneId <= maxPlaneId; iPlaneId++) {
		CbmLitTrack value;
		value.SetLastPlaneId(iPlaneId);

		std::pair<TrackPtrIterator, TrackPtrIterator> bounds;
		bounds = std::equal_range(itBegin, itEnd, &value, CompareTrackPtrLastPlaneIdMore());

		if(bounds.first == bounds.second) continue;

		std::sort(bounds.first, bounds.second, CompareTrackPtrChi2OverNdfLess());
	}
}
