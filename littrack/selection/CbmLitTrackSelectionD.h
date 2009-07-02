#ifndef CBMLITTRACKSELECTIOND_H_
#define CBMLITTRACKSELECTIOND_H_

// cut tracks on number of hits and the last reached plane

#include "CbmLitTrackSelection.h"

class CbmLitTrackSelectionD : public CbmLitTrackSelection
{
public:
	CbmLitTrackSelectionD();
	virtual ~CbmLitTrackSelectionD();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual LitStatus DoSelect(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);

	virtual LitStatus DoSelect(
			TrackPtrVector& tracks);

	int GetMinLastPlaneId() const {return fMinLastPlaneId;}
	int GetMinNofHits() const {return fMinNofHits;}

	void SetMinLastPlaneId(int minLastPlaneId) {fMinLastPlaneId = minLastPlaneId;}
	void SetMinNofHits(int minNofHits) {fMinNofHits = minNofHits;}

private:
	int fMinLastPlaneId;
	int fMinNofHits;
};

#endif /*CBMLITTRACKSELECTIOND_H_*/
