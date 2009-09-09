#ifndef CBMLITNEARESTHITTOTRACKMERGER_H_
#define CBMLITNEARESTHITTOTRACKMERGER_H_

#include "interface/CbmLitHitToTrackMerger.h"
#include "finder/CbmLitGating.h"
#include "base/CbmLitPtrTypes.h"
#include "base/CbmLitStation.h"

class CbmLitTrackPropagator;
class CbmLitTrackUpdate;

class CbmLitNearestHitToTrackMerger : public CbmLitHitToTrackMerger,
                                       public CbmLitGating
{
public:
	CbmLitNearestHitToTrackMerger();
	virtual ~CbmLitNearestHitToTrackMerger();

	// derived from CbmTool
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual LitStatus DoMerge(
			HitPtrVector& hits, //TODO: add const here
			TrackPtrVector& tracks);

	void SetStation(const CbmLitStation& station) {fStation = station;}
	void SetPropagator(TrackPropagatorPtr propagator) {fPropagator = propagator;}
	void SetFilter(TrackUpdatePtr filter) {fFilter = filter;}
	void SetPDG(int pdg) {fPDG = pdg;}

private:
	bool AddNearestHit(
			CbmLitTrack* track,
			HitPtrIteratorPair bounds) const;

private:
	TrackPropagatorPtr fPropagator;
	TrackUpdatePtr fFilter;
	CbmLitStation fStation;
	int fPDG;
};

#endif /* CBMLITNEARESTHITTOTRACKMERGER_H_ */
