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

	int GetNofSharedHits() const {return fNofSharedHits;}
	int GetMinLastPlaneId() const {return fMinLastPlaneId;}
	int GetMinNofHits() const {return fMinNofHits;}

	void SetNofSharedHits(int nofHits) {fNofSharedHits = nofHits;}
	void SetMinLastPlaneId(int minLastPlaneId) {fMinLastPlaneId = minLastPlaneId;}
	void SetMinNofHits(int minNofHits) {fMinNofHits = minNofHits;}

private:
	TrackSelectionPtr fSelectionRobust;
	TrackSelectionPtr fSelectionA;
	TrackSelectionPtr fSelectionD;
	TrackSelectionPtr fSelectionC;
	TrackSelectionPtr fSelectionChiSq;

	int fNofSharedHits;
	int fMinNofHits;
	int fMinLastPlaneId;
	int fMaxChi2;
};

#endif /*CBMLITTRACKSELECTIONMUCHROBUST_H_*/
