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
	void SetChiSq(Double_t chiSq) {fChiSq = chiSq;}
	void SetParam(const CbmLitTrackParam* param) {fParam = *param;}

	const CbmLitHit* GetHit() const {return fHit;}
	Double_t GetChiSq() const {return fChiSq;}
	const CbmLitTrackParam* GetParam() const {return &fParam;}
private:
	const CbmLitHit* fHit;
	Double_t fChiSq;
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

	void IsAlwaysCreateMissingHit(Bool_t isAlwaysCreateMissingHit) {fIsAlwaysCreateMissingHit = isAlwaysCreateMissingHit;}
	void SetFinalPreSelection(TrackSelectionPtr finalPreSelection) {fFinalPreSelection = finalPreSelection;}
	void SetStationGroupSelection(TrackSelectionPtr stationGroupSelection) {fStationGroupSelection = stationGroupSelection;}
	void SetFilter(TrackUpdatePtr filter) {fFilter = filter;}
	void SetFitter(TrackFitterPtr fitter) {fFitter = fitter;}

protected:
	void FollowTracks();

	void ProcessStationGroup(
			const CbmLitTrack *track,
			Int_t stationGroup);

	Bool_t ProcessStation(
			const CbmLitTrack* track,
			Int_t stationGroup,
			Int_t station,
			TrackPtrVector& tracksOut);

	Bool_t ProcessStation1(
			const CbmLitTrack *track,
			Int_t stationGroup,
			Int_t station,
			TrackPtrVector& tracksOut);

//	Bool_t ProcessSubstation(
//			Int_t substation,
//			HitPtrIterator hitIt,
//			HitPtrIteratorPair bounds,
//			const CbmLitTrackParam* par,
//			CbmLitTrackParam* uPar,
//			HitPtrVector& hits,
//			std::vector<Double_t>& chiSq);

	void ProcessSubstation(
			const CbmLitTrackParam* par,
			HitPtrIteratorPair bounds,
			std::vector<CbmLitHitChiSq>& hits);

//	Bool_t AddTrackBranch(
//			const CbmLitTrack* track,
//			const HitPtrVector& hits,
//			const std::vector<Double_t>& chiSq,
//			const CbmLitTrackParam* par,
//			TrackPtrVector& tracksOut);

	Bool_t AddTrackCandidate(
			TrackPtrVector& tracks,
			Int_t stationGroup);

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

	Int_t fMaxNofHitsInValidationGate;
//	Int_t fMaxNofBranches;
//	Int_t fNofBranches;
	Bool_t fIsAlwaysCreateMissingHit;

	ClassDef(CbmLitTrackFinderBranch,1);
};

#endif /*CBMLITTRACKFINDERBASEBRANCH_H_*/
