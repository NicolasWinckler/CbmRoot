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
private:
	
	CbmLitTrackSelection* fSelectionC;	
	CbmLitTrackSelection* fSelectionD;
	
	Int_t fNofSharedHits;
	Int_t fMinNofHits;
	Int_t fMinLastPlaneId;
	
	ClassDef(CbmLitTrackSelectionTrd, 1);
};

#endif /*CBMLITTRACKSELECTIONTRD_H_*/
