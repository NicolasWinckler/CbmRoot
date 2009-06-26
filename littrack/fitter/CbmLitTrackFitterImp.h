#ifndef CBMLITTRACKFITTERIMP_H_
#define CBMLITTRACKFITTERIMP_H_

#include "CbmLitTrackFitter.h"
#include "CbmLitPtrTypes.h"

class CbmLitTrack;

class CbmLitTrackFitterImp : public CbmLitTrackFitter
{
public:
	CbmLitTrackFitterImp(
			TrackPropagatorPtr propagator,
			TrackUpdatePtr update);

	virtual ~CbmLitTrackFitterImp();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual LitStatus Fit(
			CbmLitTrack *track,
			bool downstream = true);
private:
	TrackPropagatorPtr fPropagator;
	TrackUpdatePtr fUpdate;
	bool fDownstream;
};

#endif /*CBMLITTRACKFITTERIMP_H_*/
