#ifndef CBMLITTRACKFINDERBASEBRANCH_H_
#define CBMLITTRACKFINDERBASEBRANCH_H_

#include "CbmLitTrackFinderBase.h"

class CbmLitTrackSelection;
class CbmLitTrackUpdate;
class CbmLitTrackFitter;

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

protected:
	TrackPtrVector fTracksCopy;
	TrackPtrVector fFoundTracks;

	CbmLitTrackSelection* fFinalPreSelection;
	CbmLitTrackSelection* fStationGroupSelection;
	CbmLitTrackUpdate* fFilter;
	CbmLitTrackFitter* fFitter;

	Int_t fMaxNofBranches;
	Int_t fNofBranches;
	Bool_t fIsAlwaysCreateMissingHit;

	void FollowTracks();

//	void ProcessStationGroup(
//			const CbmLitTrack *track,
//			Int_t stationGroup);
//
//	Bool_t ProcessStation(
//			Int_t station,
//			Int_t substation,
//			HitPtrIterator &hitIt,
//			HitPtrIteratorPair &bounds,
//	 		std::vector<Int_t>& nofMissingHits,
//			CbmLitTrackParam* par,
//			CbmLitTrackParam* uPar,
//			HitPtrVector& hits,
//			std::vector<Double_t>& chiSq);
//
//	void AddTrackCandidate(
//			const CbmLitTrack* track,
//			const HitPtrVector& hits,
//			const std::vector<Double_t>& chiSq,
//			const CbmLitTrackParam* par,
//			Int_t stationGroup);

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

	Bool_t ProcessSubstation(
			Int_t substation,
			HitPtrIterator hitIt,
			HitPtrIteratorPair bounds,
			const CbmLitTrackParam* par,
			CbmLitTrackParam* uPar,
			HitPtrVector& hits,
			std::vector<Double_t>& chiSq);

	Bool_t AddTrackBranch(
			const CbmLitTrack* track,
			const HitPtrVector& hits,
			const std::vector<Double_t>& chiSq,
			const CbmLitTrackParam* par,
			TrackPtrVector& tracksOut);

	void AddTrackCandidate(
			TrackPtrVector& tracks,
			Int_t stationGroup);

	void RefitTracks(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);

	void CopyToOutputArray();

	ClassDef(CbmLitTrackFinderBranch,1);
};

#endif /*CBMLITTRACKFINDERBASEBRANCH_H_*/
