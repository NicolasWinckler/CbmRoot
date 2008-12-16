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
	
	Double_t GetMinMomentum() const {return fMinMomentum;}
	
	void SetMinMomentum(Double_t minMomentum) {fMinMomentum = minMomentum;}
	
private:
	
	Double_t fMinMomentum; // in GeV

	ClassDef(CbmLitTrackSelectionMomentum, 1);	
};

#endif /*CBMLITTRACKSELECTIONMOMENTUM_H_*/
