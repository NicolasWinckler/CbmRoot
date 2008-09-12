#ifndef CBMLITTRACKFITTERITER_H_
#define CBMLITTRACKFITTERITER_H_

#include "CbmLitTrackFitter.h"

class CbmLitTrack;

class CbmLitTrackFitterIter : public CbmLitTrackFitter
{
public:
	CbmLitTrackFitterIter(
			CbmLitTrackFitter* fitter,
			CbmLitTrackFitter* smoother);
	virtual ~CbmLitTrackFitterIter();
	
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();
	
	virtual LitStatus Fit(
			CbmLitTrack *track,
			Bool_t downstream = true);
	
private:
	CbmLitTrackFitter* fFitter;
	CbmLitTrackFitter* fSmoother;	
	
	Int_t fNofIterations;
	Double_t fChiSqCut;
	Int_t fMinNofHits;
	
	ClassDef(CbmLitTrackFitterIter, 1);
};

#endif /*CBMLITTRACKFITTERITER_H_*/
