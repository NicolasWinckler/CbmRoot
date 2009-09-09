#ifndef CBMLITTRACKFINDERBASEBRANCH_H_
#define CBMLITTRACKFINDERBASEBRANCH_H_

#include "finder/CbmLitTrackFinderBase.h"
#include "data/CbmLitHitChiSq.h"

class CbmLitTrackSelection;
class CbmLitTrackUpdate;
class CbmLitTrackFitter;

class CbmLitTrackFinderBranch : public CbmLitTrackFinderBase
{
public:
	CbmLitTrackFinderBranch();
	virtual ~CbmLitTrackFinderBranch();

	LitStatus DoFind(
			HitPtrVector& hits,
			TrackPtrVector& trackSeeds,
			TrackPtrVector& tracks);

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	void IsAlwaysCreateMissingHit(bool isAlwaysCreateMissingHit) {fIsAlwaysCreateMissingHit = isAlwaysCreateMissingHit;}
	void SetStationGroupSelection(TrackSelectionPtr stationGroupSelection) {fStationGroupSelection = stationGroupSelection;}
	void SetFinalSelection(TrackSelectionPtr finalSelection) {fFinalSelection = finalSelection;}
	void SetSeedSelection(TrackSelectionPtr seedSelection) {fSeedSelection=seedSelection;}
	void SetFilter(TrackUpdatePtr filter) {fFilter = filter;}
	void SetPropagator(TrackPropagatorPtr propagator) {fPropagator = propagator;}

protected:
    void InitTrackSeeds(
    		TrackPtrIterator itBegin,
    		TrackPtrIterator itEnd);

	void FollowTracks();

	void ProcessStationGroup(
			const CbmLitTrack* track,
			int stationGroup);

	bool ProcessStation(
			const CbmLitTrack* track,
			int stationGroup,
			int station,
			TrackPtrVector& tracksOut);

	bool ProcessStation1(
			const CbmLitTrack* track,
			int stationGroup,
			int station,
			TrackPtrVector& tracksOut);

	void ProcessSubstation(
			const CbmLitTrackParam* par,
			HitPtrIteratorPair bounds,
			std::vector<CbmLitHitChiSq>& hits);

	bool AddTrackCandidate(
			TrackPtrVector& tracks,
			int stationGroup);

	void CopyToOutputArray();

private:
	TrackPtrVector fTracks;
	TrackPtrVector fTracksCopy;
	TrackPtrVector fFoundTracks;

	TrackSelectionPtr fSeedSelection;
	TrackSelectionPtr fFinalSelection;
	TrackSelectionPtr fStationGroupSelection;
	TrackPropagatorPtr fPropagator;
	TrackUpdatePtr fFilter;

	int fMaxNofHitsInValidationGate;
	bool fIsAlwaysCreateMissingHit;
};

#endif /*CBMLITTRACKFINDERBASEBRANCH_H_*/
