
#ifndef CBMLITTRACKSELECTION_H_
#define CBMLITTRACKSELECTION_H_

#include "CbmTool.h"
#include "CbmLitTypes.h"

class CbmLitTrackSelection : public CbmTool
{

public:

	CbmLitTrackSelection();
	virtual ~CbmLitTrackSelection();
	
	virtual LitStatus DoSelect(
			TrackIterator itBegin,
			TrackIterator itEnd) = 0;
	
	virtual LitStatus DoSelect(
			TrackVector& tracks) = 0;
	
	
	ClassDef(CbmLitTrackSelection, 1);
};

#endif /*CBMLITTRACKSELECTION_H_*/
