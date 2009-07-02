#ifndef CBMLITTRACKSELECTIONMUCH_H_
#define CBMLITTRACKSELECTIONMUCH_H_

#include "CbmLitTrackSelection.h"
#include "CbmLitTypes.h"
#include "CbmLitPtrTypes.h"

class CbmLitTrackSelectionMuch : public CbmLitTrackSelection
{
public:
	CbmLitTrackSelectionMuch();
	virtual ~CbmLitTrackSelectionMuch();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual LitStatus DoSelect(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);

	virtual LitStatus DoSelect(
			TrackPtrVector& tracks);

	int GetNofSharedHits() const {return fNofSharedHits;}
	int GetMinLastPlaneId() const {return fMinLastPlaneId;}
	int GetMinNofHits() const {return fMinNofHits;}

	void SetNofSharedHits(int nofHits) {fNofSharedHits = nofHits;}
	void SetMinLastPlaneId(int minLastPlaneId) {fMinLastPlaneId = minLastPlaneId;}
	void SetMinNofHits(int minNofHits) {fMinNofHits = minNofHits;}

private:
	TrackSelectionPtr fSelectionA;
	TrackSelectionPtr fSelectionB;
	TrackSelectionPtr fSelectionC;
	TrackSelectionPtr fSelectionD;

	int fNofSharedHits;
	int fMinNofHits;
	int fMinLastPlaneId;
};

#endif /*CBMLITTRACKSELECTIONMUCH_H_*/
