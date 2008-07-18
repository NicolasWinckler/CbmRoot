#ifndef CBMLITTRACKSELECTIONEMPTY_H_
#define CBMLITTRACKSELECTIONEMPTY_H_

#include "CbmLitTrackSelection.h"

class CbmLitTrackSelectionEmpty : public CbmLitTrackSelection
{

public:

	CbmLitTrackSelectionEmpty();
	virtual ~CbmLitTrackSelectionEmpty();
		
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();
	
	virtual LitStatus DoSelect(
			TrackIterator itBegin,
			TrackIterator itEnd);
	
	virtual LitStatus DoSelect(
			TrackVector& tracks);


	ClassDef(CbmLitTrackSelectionEmpty, 1);
};

#endif /*CBMLITTRACKSELECTIONEMPTY_H_*/
