#ifndef CBMLITTRACKFINDERNN_H_
#define CBMLITTRACKFINDERNN_H_

#include "CbmLitTrackFinderBase.h"
#include "CbmLitPtrTypes.h"

class CbmLitTrackFinderNN : public CbmLitTrackFinderBase
{
public:
	CbmLitTrackFinderNN();
	virtual ~CbmLitTrackFinderNN();

	virtual LitStatus DoFind(
			HitPtrVector& hits,
			TrackPtrVector& trackSeeds,
			TrackPtrVector& tracks);

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	void SetSeedSelection(TrackSelectionPtr seedSelection) {fSeedSelection=seedSelection;}
	void SetPropagator(TrackPropagatorPtr propagator) {fPropagator = propagator;}
	void SetFinalSelection(TrackSelectionPtr finalSelection) {fFinalSelection = finalSelection;}
	void SetFilter(TrackUpdatePtr filter) {fFilter = filter;}

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

	bool AddNearestHit(
			CbmLitTrack* track,
			HitPtrIteratorPair bounds);
private:
	TrackPtrVector fTracks;

	TrackUpdatePtr fFilter;
	TrackSelectionPtr fSeedSelection;
	TrackSelectionPtr fFinalSelection;
	TrackPropagatorPtr fPropagator;
};

#endif /* CBMLITTRACKFINDERNN_H_ */
