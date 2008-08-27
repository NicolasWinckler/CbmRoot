#ifndef CBMLITTRACKSELECTIONMUCHROBUST_H_
#define CBMLITTRACKSELECTIONMUCHROBUST_H_

#include "CbmLitTrackSelection.h"

class CbmLitTrackSelectionMuchRobust : public CbmLitTrackSelection
{
public:
	CbmLitTrackSelectionMuchRobust();
	virtual ~CbmLitTrackSelectionMuchRobust();
	
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();
	
	virtual LitStatus DoSelect(
			TrackIterator itBegin,
			TrackIterator itEnd);
	
	virtual LitStatus DoSelect(
			TrackVector& tracks);
	
	Int_t GetNofSharedHits() const {return fNofSharedHits;}
	Int_t GetMinLastPlaneId() const {return fMinLastPlaneId;}
	Int_t GetMinNofHits() const {return fMinNofHits;}

	void SetNofSharedHits(Int_t nofHits) {fNofSharedHits = nofHits;}
	void SetMinLastPlaneId(Int_t minLastPlaneId) {fMinLastPlaneId = minLastPlaneId;}
	void SetMinNofHits(Int_t minNofHits) {fMinNofHits = minNofHits;}
private:
	
	CbmLitTrackSelection* fSelectionRobust;
	CbmLitTrackSelection* fSelectionA;
	CbmLitTrackSelection* fSelectionD;
	CbmLitTrackSelection* fSelectionC;
	CbmLitTrackSelection* fSelectionChiSq;
	
	Int_t fNofSharedHits;
	Int_t fMinNofHits;
	Int_t fMinLastPlaneId;
	Int_t fMaxChi2;
	
	ClassDef(CbmLitTrackSelectionMuchRobust, 1);
};

#endif /*CBMLITTRACKSELECTIONMUCHROBUST_H_*/
