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

	Int_t GetMinLastPlaneId() const {return fMinLastPlaneId;}
	Int_t GetMinNofHits() const {return fMinNofHits;}

	void SetMinLastPlaneId(Int_t minLastPlaneId) {fMinLastPlaneId = minLastPlaneId;}
	void SetMinNofHits(Int_t minNofHits) {fMinNofHits = minNofHits;}

private:
	Int_t fMinLastPlaneId;
	Int_t fMinNofHits;
};

#endif /*CBMLITTRACKSELECTIOND_H_*/
