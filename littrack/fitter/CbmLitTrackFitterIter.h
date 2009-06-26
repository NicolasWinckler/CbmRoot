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
			bool downstream = true);

private:
	CbmLitTrackFitter* fFitter;
	CbmLitTrackFitter* fSmoother;

	int fNofIterations;
	double fChiSqCut;
	int fMinNofHits;
};

#endif /*CBMLITTRACKFITTERITER_H_*/
