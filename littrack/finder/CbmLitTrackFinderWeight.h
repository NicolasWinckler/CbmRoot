#ifndef CBMLITTRACKFINDERWEIGHT_H_
#define CBMLITTRACKFINDERWEIGHT_H_

#include "CbmLitTrackFinderBase.h"

class CbmLitTrackFinderWeight : public CbmLitTrackFinderBase
{
public:
	CbmLitTrackFinderWeight();
	virtual ~CbmLitTrackFinderWeight();

	LitStatus DoFind(
			HitPtrVector& hits,
			TrackPtrVector& trackSeeds,
			TrackPtrVector& tracks);

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	void SetSeedSelection(TrackSelectionPtr seedSelection) {fSeedSelection=seedSelection;}
	void SetPropagator(TrackPropagatorPtr propagator) {fPropagator = propagator;}
	void SetFinalSelection(TrackSelectionPtr finalSelection) {fFinalSelection = finalSelection;}
	void SetFitter(TrackFitterPtr fitter) { fFitter = fitter;}
	void SetFilter(TrackUpdatePtr filter) { fFilter = filter;}

protected:
    void InitTrackSeeds(
    		TrackPtrIterator itBegin,
    		TrackPtrIterator itEnd);

	void FollowTracks(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);

	void FollowTrack(
			CbmLitTrack *track);

	bool ProcessStationGroup(
			CbmLitTrack *track,
			int stationGroup);

	bool ProcessStation(
			CbmLitTrack *track,
			int stationGroup,
			int station);

	bool AddHits(
			CbmLitTrack* track,
			HitPtrIteratorPair bounds);

	void FitTracks(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);
private:
	TrackPtrVector fTracks;

	TrackFitterPtr fFitter;
	TrackUpdatePtr fFilter;
    TrackSelectionPtr fSeedSelection;
    TrackSelectionPtr fFinalSelection;
    TrackPropagatorPtr fPropagator;
};

#endif /*CBMLITTRACKFINDERWEIGHT_H_*/
