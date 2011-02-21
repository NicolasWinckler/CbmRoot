/* CbmLitTrackSelectionSameSeed.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 2.0
 */

#include "CbmLitTrackSelectionSameSeed.h"

#include "CbmLitTrack.h"
#include "CbmLitComparators.h"

#include <algorithm>

CbmLitTrackSelectionSameSeed::CbmLitTrackSelectionSameSeed()
{

}

CbmLitTrackSelectionSameSeed::~CbmLitTrackSelectionSameSeed()
{

}

LitStatus CbmLitTrackSelectionSameSeed::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionSameSeed::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionSameSeed::DoSelect(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	if (itBegin == itEnd) return kLITSUCCESS;

	std::sort(itBegin, itEnd, CompareTrackPtrPrevTrackIdLess());

	int minId = (*itBegin)->GetPreviousTrackId();
	int maxId = (*(itEnd-1))->GetPreviousTrackId();

	for (int iId = minId; iId <= maxId; iId++) {
		CbmLitTrack value;
		value.SetPreviousTrackId(iId);
		std::pair<TrackPtrIterator, TrackPtrIterator> bounds;
		bounds = std::equal_range(itBegin, itEnd, &value, CompareTrackPtrPrevTrackIdLess());
		if(bounds.first == bounds.second) continue;

		CbmLitQualitySort::DoSort(bounds.first, bounds.second);

		for (TrackPtrIterator i = bounds.first + 1; i != bounds.second; i++)
			(*i)->SetQuality(kLITBAD);
	}

	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionSameSeed::DoSelect(
		TrackPtrVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}
