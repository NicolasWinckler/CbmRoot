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
	
private:
	
	CbmLitTrackSelection* fSelectionRobust;
	CbmLitTrackSelection* fSelectionA;
	CbmLitTrackSelection* fSelectionD;
	CbmLitTrackSelection* fSelectionC;
	
	ClassDef(CbmLitTrackSelectionMuchRobust, 1);
};

#endif /*CBMLITTRACKSELECTIONMUCHROBUST_H_*/
