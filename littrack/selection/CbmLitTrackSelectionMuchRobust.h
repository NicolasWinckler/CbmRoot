#ifndef CBMLITTRACKSELECTIONMUCHROBUST_H_
#define CBMLITTRACKSELECTIONMUCHROBUST_H_

#include "CbmLitTrackSelection.h"
#include "CbmLitPtrTypes.h"

class CbmLitTrackSelectionMuchRobust : public CbmLitTrackSelection
{
public:
	CbmLitTrackSelectionMuchRobust();
	virtual ~CbmLitTrackSelectionMuchRobust();

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
	TrackSelectionPtr fSelectionRobust;
	TrackSelectionPtr fSelectionA;
	TrackSelectionPtr fSelectionD;
	TrackSelectionPtr fSelectionC;
	TrackSelectionPtr fSelectionChiSq;

	Int_t fNofSharedHits;
	Int_t fMinNofHits;
	Int_t fMinLastPlaneId;
	Int_t fMaxChi2;
};

#endif /*CBMLITTRACKSELECTIONMUCHROBUST_H_*/
