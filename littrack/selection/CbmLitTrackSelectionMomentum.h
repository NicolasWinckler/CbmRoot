#ifndef CBMLITTRACKSELECTIONMOMENTUM_H_
#define CBMLITTRACKSELECTIONMOMENTUM_H_

// select the track cutting on the minimum momentum in GeV

#include "CbmLitTrackSelection.h"

class CbmLitTrackSelectionMomentum : public CbmLitTrackSelection
{
public:
	CbmLitTrackSelectionMomentum();
	virtual ~CbmLitTrackSelectionMomentum();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual LitStatus DoSelect(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);

	virtual LitStatus DoSelect(
			TrackPtrVector& tracks);

	myf GetMinMomentum() const {return fMinMomentum;}

	void SetMinMomentum(myf minMomentum) {fMinMomentum = minMomentum;}

private:
	myf fMinMomentum; // in GeV
};

#endif /*CBMLITTRACKSELECTIONMOMENTUM_H_*/
