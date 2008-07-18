
#ifndef CBMLITTRACKSELECTIONMUCH_H_
#define CBMLITTRACKSELECTIONMUCH_H_

#include "CbmLitTrackSelection.h"
#include "CbmLitTypes.h"


class CbmLitTrackSelectionMuch : public CbmLitTrackSelection
{

public:

	CbmLitTrackSelectionMuch();
	virtual ~CbmLitTrackSelectionMuch();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();
	
	virtual LitStatus DoSelect(
			TrackIterator itBegin,
			TrackIterator itEnd);
	
	virtual LitStatus DoSelect(
			TrackVector& tracks);

private:
	
	CbmLitTrackSelection* fSelectionA;
	CbmLitTrackSelection* fSelectionB;
	CbmLitTrackSelection* fSelectionC;
	CbmLitTrackSelection* fSelectionD;
	
	Int_t fNofSharedHits;
	Int_t fMinNofHits;
	Int_t fMinLastPlaneId;
	
	ClassDef(CbmLitTrackSelectionMuch, 1);
};

#endif /*CBMLITTRACKSELECTIONMUCH_H_*/
