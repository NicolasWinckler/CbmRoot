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
			TrackIterator itBegin,
			TrackIterator itEnd);
	
	virtual LitStatus DoSelect(
			TrackVector& tracks);
	
	Double_t GetMaxChiSq() const {return fMaxChiSq;}
	
	void SetMaxChiSq(Double_t maxChiSq) {fMaxChiSq = maxChiSq;}
	
private:
	Double_t fMaxChiSq;

	ClassDef(CbmLitTrackSelectionChiSq, 1);	
};

#endif /*CBMLITTRACKSELECTIONCHISQ_H_*/
