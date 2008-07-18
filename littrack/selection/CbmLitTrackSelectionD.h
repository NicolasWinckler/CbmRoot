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
			TrackIterator itBegin,
			TrackIterator itEnd);
	
	virtual LitStatus DoSelect(
			TrackVector& tracks);
private:
	
	Int_t fMinLastPlaneId;
	Int_t fMinNofHits;

	ClassDef(CbmLitTrackSelectionD, 1);	
};

#endif /*CBMLITTRACKSELECTIOND_H_*/
