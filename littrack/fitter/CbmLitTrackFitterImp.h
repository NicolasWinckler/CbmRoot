#ifndef CBMLITTRACKFITTERIMP_H_
#define CBMLITTRACKFITTERIMP_H_

#include "base/CbmLitPtrTypes.h"
#include "interface/CbmLitTrackFitter.h"

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
};

#endif /*CBMLITTRACKFITTERIMP_H_*/
