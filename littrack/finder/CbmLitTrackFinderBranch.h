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
	void SetChiSq(double chiSq) {fChiSq = chiSq;}
	void SetParam(const CbmLitTrackParam* param) {fParam = *param;}

	const CbmLitHit* GetHit() const {return fHit;}
	double GetChiSq() const {return fChiSq;}
	const CbmLitTrackParam* GetParam() const {return &fParam;}
private:
	const CbmLitHit* fHit;
	double fChiSq;
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
			const HitPtrVector& hits,
			const TrackPtrVector& trackSeeds,
			TrackPtrVector& tracks);

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	void IsAlwaysCreateMissingHit(bool isAlwaysCreateMissingHit) {fIsAlwaysCreateMissingHit = isAlwaysCreateMissingHit;}
	void SetFinalPreSelection(TrackSelectionPtr finalPreSelection) {fFinalPreSelection = finalPreSelection;}
	void SetStationGroupSelection(TrackSelectionPtr stationGroupSelection) {fStationGroupSelection = stationGroupSelection;}
	void SetFilter(TrackUpdatePtr filter) {fFilter = filter;}
	void SetFitter(TrackFitterPtr fitter) {fFitter = fitter;}

protected:
	void FollowTracks();

	void ProcessStationGroup(
			const CbmLitTrack *track,
			int stationGroup);

	bool ProcessStation(
			const CbmLitTrack* track,
			int stationGroup,
			int station,
			TrackPtrVector& tracksOut);

	bool ProcessStation1(
			const CbmLitTrack *track,
			int stationGroup,
			int station,
			TrackPtrVector& tracksOut);

//	bool ProcessSubstation(
//			int substation,
//			HitPtrIterator hitIt,
//			HitPtrIteratorPair bounds,
//			const CbmLitTrackParam* par,
//			CbmLitTrackParam* uPar,
//			HitPtrVector& hits,
//			std::vector<double>& chiSq);

	void ProcessSubstation(
			const CbmLitTrackParam* par,
			HitPtrIteratorPair bounds,
			std::vector<CbmLitHitChiSq>& hits);

//	bool AddTrackBranch(
//			const CbmLitTrack* track,
//			const HitPtrVector& hits,
//			const std::vector<double>& chiSq,
//			const CbmLitTrackParam* par,
//			TrackPtrVector& tracksOut);

	bool AddTrackCandidate(
			TrackPtrVector& tracks,
			int stationGroup);

//	void RefitTracks(
//			TrackPtrIterator itBegin,
//			TrackPtrIterator itEnd);

	void CopyToOutputArray();

private:
	TrackPtrVector fTracksCopy;
	TrackPtrVector fFoundTracks;

	TrackSelectionPtr fFinalPreSelection;
	TrackSelectionPtr fStationGroupSelection;
	TrackUpdatePtr fFilter;
	TrackFitterPtr fFitter;

	int fMaxNofHitsInValidationGate;
//	int fMaxNofBranches;
//	int fNofBranches;
	bool fIsAlwaysCreateMissingHit;
};

#endif /*CBMLITTRACKFINDERBASEBRANCH_H_*/
