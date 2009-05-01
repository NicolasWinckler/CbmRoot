#ifndef CBMLITNEARESTHITTOTRACKMERGER_H_
#define CBMLITNEARESTHITTOTRACKMERGER_H_

#include "CbmLitHitToTrackMerger.h"
#include "CbmLitGating.h"
#include "CbmLitPtrTypes.h"
#include "CbmLitStation.h"

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
	void SetPDG(Int_t pdg) {fPDG = pdg;}

private:
	Bool_t AddNearestHit(
			CbmLitTrack* track,
			HitPtrIteratorPair bounds) const;

private:
	TrackPropagatorPtr fPropagator;
	TrackUpdatePtr fFilter;
	CbmLitStation fStation;
	Int_t fPDG;

	ClassDef(CbmLitNearestHitToTrackMerger, 1);
};

#endif /* CBMLITNEARESTHITTOTRACKMERGER_H_ */
