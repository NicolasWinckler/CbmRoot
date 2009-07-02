#ifndef CBMLITTRACKSELECTIONCHISQ_H_
#define CBMLITTRACKSELECTIONCHISQ_H_

#include "CbmLitTrackSelection.h"

class CbmLitTrackSelectionChiSq : public CbmLitTrackSelection
{
public:
	CbmLitTrackSelectionChiSq();
	virtual ~CbmLitTrackSelectionChiSq();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual LitStatus DoSelect(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);

	virtual LitStatus DoSelect(
			TrackPtrVector& tracks);

	myf GetMaxChiSq() const {return fMaxChiSq;}

	void SetMaxChiSq(myf maxChiSq) {fMaxChiSq = maxChiSq;}

private:
	myf fMaxChiSq;
};

#endif /*CBMLITTRACKSELECTIONCHISQ_H_*/
