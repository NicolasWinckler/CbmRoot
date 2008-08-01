#ifndef CBMLITTRACKSELECTIONTRD_H_
#define CBMLITTRACKSELECTIONTRD_H_

#include "CbmLitTrackSelection.h"
#include "CbmLitTypes.h"

class CbmLitTrackSelectionTrd : public CbmLitTrackSelection
{
public:
	CbmLitTrackSelectionTrd();
	virtual ~CbmLitTrackSelectionTrd();
	
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
	CbmLitTrackSelection* fSelectionC;	
	CbmLitTrackSelection* fSelectionD;
	
	Int_t fNofSharedHits;
	Int_t fMinNofHits;
	Int_t fMinLastPlaneId;
	
	ClassDef(CbmLitTrackSelectionTrd, 1);
};

#endif /*CBMLITTRACKSELECTIONTRD_H_*/
