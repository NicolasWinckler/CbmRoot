#ifndef CBMLITTRACKSELECTIONA_H_
#define CBMLITTRACKSELECTIONA_H_

// This selection removes the short tracks, 
// which have a longer track with the same set of hits

#include "CbmLitTrackSelection.h"

class CbmLitTrackSelectionA : public CbmLitTrackSelection
{	
public:
	
	CbmLitTrackSelectionA();
	virtual ~CbmLitTrackSelectionA();
	
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();
	
	virtual LitStatus DoSelect(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);
	
	virtual LitStatus DoSelect(
			TrackPtrVector& tracks);

private:
	
	Bool_t IsHitSharing(
			const CbmLitTrack* track0,
			const CbmLitTrack* track1);

	ClassDef(CbmLitTrackSelectionA, 1);	
};

#endif /*CBMLITTRACKSELECTIONA_H_*/
