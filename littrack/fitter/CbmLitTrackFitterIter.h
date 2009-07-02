#ifndef CBMLITTRACKFITTERITER_H_
#define CBMLITTRACKFITTERITER_H_

#include "CbmLitTrackFitter.h"
#include "CbmLitPtrTypes.h"

class CbmLitTrack;

class CbmLitTrackFitterIter : public CbmLitTrackFitter
{
public:
	CbmLitTrackFitterIter(
			TrackFitterPtr fitter,
			TrackFitterPtr smoother);
	virtual ~CbmLitTrackFitterIter();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual LitStatus Fit(
			CbmLitTrack *track,
			bool downstream = true);

private:
	TrackFitterPtr fFitter;
	TrackFitterPtr fSmoother;

	int fNofIterations;
	myf fChiSqCut;
	int fMinNofHits;
};

#endif /*CBMLITTRACKFITTERITER_H_*/
