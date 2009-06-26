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

	Int_t GetNofSharedHits() const {return fNofSharedHits;}
	Int_t GetMinLastPlaneId() const {return fMinLastPlaneId;}
	Int_t GetMinNofHits() const {return fMinNofHits;}

	void SetNofSharedHits(Int_t nofHits) {fNofSharedHits = nofHits;}
	void SetMinLastPlaneId(Int_t minLastPlaneId) {fMinLastPlaneId = minLastPlaneId;}
	void SetMinNofHits(Int_t minNofHits) {fMinNofHits = minNofHits;}

private:
	TrackSelectionPtr fSelectionA;
	TrackSelectionPtr fSelectionB;
	TrackSelectionPtr fSelectionC;
	TrackSelectionPtr fSelectionD;

	Int_t fNofSharedHits;
	Int_t fMinNofHits;
	Int_t fMinLastPlaneId;
};

#endif /*CBMLITTRACKSELECTIONMUCH_H_*/
