#ifndef CBMLITROBUSTSELECTION_H_
#define CBMLITROBUSTSELECTION_H_

#include "CbmLitTrackSelection.h"

class CbmLitTrackFitter;

class CbmLitRobustSelection : public CbmLitTrackSelection
{
public:
	CbmLitRobustSelection(
			CbmLitTrackFitter* robustFitter,
			CbmLitTrackFitter* fitter,
			CbmLitTrackFitter* smoother);
	virtual ~CbmLitRobustSelection();
	
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();
	
	virtual LitStatus DoSelect(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);
	
	virtual LitStatus DoSelect(
			TrackPtrVector& tracks);
	
private:
	
	CbmLitTrackFitter* fRobustFitter;
	CbmLitTrackFitter* fFitter;
	CbmLitTrackFitter* fSmoother;

	ClassDef(CbmLitRobustSelection, 1);
};

#endif /*CBMLITROBUSTSELECTION_H_*/
