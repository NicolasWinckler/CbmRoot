/* CbmLitTrackSelectionSameSeed.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 2.0
 *
 * Track selection that selects the best track
 * for each subset of tracks with the same previous track indices.
 */

#ifndef CBMLITTRACKSELECTIONSAMESEED_H_
#define CBMLITTRACKSELECTIONSAMESEED_H_

#include "CbmLitTrackSelection.h"
#include "CbmLitQualitySort.h"

class CbmLitTrackSelectionSameSeed : public CbmLitTrackSelection,
                                     public CbmLitQualitySort
{
public:
	CbmLitTrackSelectionSameSeed();
	virtual ~CbmLitTrackSelectionSameSeed();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual LitStatus DoSelect(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);

	virtual LitStatus DoSelect(
			TrackPtrVector& tracks);
//
//private:
//	void SortQuality(
//			TrackPtrIterator itBegin,
//			TrackPtrIterator itEnd);
//
//	void SortLastPlaneId(
//			TrackPtrIterator itBegin,
//			TrackPtrIterator itEnd);
};

#endif /*CBMLITTRACKSELECTIONSAMESEED_H_*/
