#ifndef CBMLITTRACKSELECTIONC_H_
#define CBMLITTRACKSELECTIONC_H_

//removes clones sorting by quality and checking common hits

#include "CbmLitTrackSelection.h"
#include "CbmLitTypes.h"

class CbmLitTrackSelectionC : public CbmLitTrackSelection
{
public:

	CbmLitTrackSelectionC();
	virtual ~CbmLitTrackSelectionC();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual LitStatus DoSelect(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);

	virtual LitStatus DoSelect(
			TrackPtrVector& tracks);

	int GetNofSharedHits() const {return fNofSharedHits;}

	void SetNofSharedHits(int nofHits) {fNofSharedHits = nofHits;}

private:

	int fNofSharedHits;

	void CheckSharedHits(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);

	void SortLastPlaneId(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);

	void SortNofHits(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);
};

#endif /*CBMLITTRACKSELECTIONC_H_*/
