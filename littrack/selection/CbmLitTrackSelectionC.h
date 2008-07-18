#ifndef CBMLITTRACKSELECTIONC_H_
#define CBMLITTRACKSELECTIONC_H_

//removes clones sorting by quality and checking common hits

#include "CbmLitTrackSelection.h"
#include "CbmLitTypes.h"

class CbmLitTrackSelectionC : public CbmLitTrackSelection
{
public:
	
	CbmLitTrackSelectionC();
	virtual ~CbmLitTrackSelectionC();
	
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();
	
	virtual LitStatus DoSelect(
			TrackIterator itBegin,
			TrackIterator itEnd);
	
	virtual LitStatus DoSelect(
			TrackVector& tracks);
private:
	
	Int_t fNofSharedHits;
	
	void CheckSharedHits(
			TrackIterator itBegin,
			TrackIterator itEnd);
	
	void SortLastPlaneId(
			TrackIterator itBegin,
			TrackIterator itEnd);
	
	void SortNofHits(
			TrackIterator itBegin,
			TrackIterator itEnd);
	
	ClassDef(CbmLitTrackSelectionC, 1);
};

#endif /*CBMLITTRACKSELECTIONC_H_*/
