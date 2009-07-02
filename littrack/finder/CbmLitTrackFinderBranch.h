#ifndef CBMLITTRACKFINDERBASEBRANCH_H_
#define CBMLITTRACKFINDERBASEBRANCH_H_

#include "CbmLitTrackFinderBase.h"

class CbmLitTrackSelection;
class CbmLitTrackUpdate;
class CbmLitTrackFitter;

class CbmLitHitChiSq
{
public:
	void SetHit(const CbmLitHit* hit) {fHit = hit;}
	void SetChiSq(myf chiSq) {fChiSq = chiSq;}
	void SetParam(const CbmLitTrackParam* param) {fParam = *param;}

	const CbmLitHit* GetHit() const {return fHit;}
	myf GetChiSq() const {return fChiSq;}
	const CbmLitTrackParam* GetParam() const {return &fParam;}
private:
	const CbmLitHit* fHit;
	myf fChiSq;
	CbmLitTrackParam fParam;
};

class CompareHitChiSqLess:
	public std::binary_function<
		const CbmLitHitChiSq&,
		const CbmLitHitChiSq&,
		bool>
{
public:
	bool operator()(const CbmLitHitChiSq& hit1, const CbmLitHitChiSq& hit2) const {
		return hit1.GetChiSq() < hit2.GetChiSq();
	}
};


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
