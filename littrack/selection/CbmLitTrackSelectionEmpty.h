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
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);

	virtual LitStatus DoSelect(
			TrackPtrVector& tracks);
};

#endif /*CBMLITTRACKSELECTIONEMPTY_H_*/
