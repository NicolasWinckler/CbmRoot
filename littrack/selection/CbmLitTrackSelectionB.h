#ifndef CBMLITTRACKSELECTIONB_H_
#define CBMLITTRACKSELECTIONB_H_

// select the best track for each subset of tracks with the same previouse track ids

#include "CbmLitTrackSelection.h"

class CbmLitTrackSelectionB : public CbmLitTrackSelection
{
public:
	CbmLitTrackSelectionB();
	virtual ~CbmLitTrackSelectionB();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual LitStatus DoSelect(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);

	virtual LitStatus DoSelect(
			TrackPtrVector& tracks);

private:
	//Int_t fNofSharedHits;

	CbmLitTrackSelection* fSelection;

	void SortLastPlaneId(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);
};

#endif /*CBMLITTRACKSELECTIONB_H_*/
