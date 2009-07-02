#ifndef CBMLITTRACKSELECTIONTRD_H_
#define CBMLITTRACKSELECTIONTRD_H_

#include "CbmLitTrackSelection.h"
#include "CbmLitTypes.h"
#include "CbmLitPtrTypes.h"

class CbmLitTrackSelectionTrd : public CbmLitTrackSelection
{
public:
	CbmLitTrackSelectionTrd();
	virtual ~CbmLitTrackSelectionTrd();

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
	TrackSelectionPtr fSelectionC;
	TrackSelectionPtr fSelectionD;

	int fNofSharedHits;
	int fMinNofHits;
	int fMinLastPlaneId;
};

#endif /*CBMLITTRACKSELECTIONTRD_H_*/
