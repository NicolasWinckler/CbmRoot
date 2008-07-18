#ifndef CBMLITROBUSTSELECTION_H_
#define CBMLITROBUSTSELECTION_H_

#include "CbmLitTrackSelection.h"

class CbmLitTrackFitter;

class CbmLitRobustSelection : public CbmLitTrackSelection
{
public:
	CbmLitRobustSelection(
			CbmLitTrackFitter* robustFitter,
			CbmLitTrackFitter* fitter);
	virtual ~CbmLitRobustSelection();
	
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();
	
	virtual LitStatus DoSelect(
			TrackIterator itBegin,
			TrackIterator itEnd);
	
	virtual LitStatus DoSelect(
			TrackVector& tracks);
	
private:
	
	CbmLitTrackFitter* fRobustFitter;
	CbmLitTrackFitter* fFitter;

	ClassDef(CbmLitRobustSelection, 1);
};

#endif /*CBMLITROBUSTSELECTION_H_*/
