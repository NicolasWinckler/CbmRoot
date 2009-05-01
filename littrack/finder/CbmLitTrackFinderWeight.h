#ifndef CBMLITTRACKFINDERWEIGHT_H_
#define CBMLITTRACKFINDERWEIGHT_H_

#include "CbmLitTrackFinderBase.h"

class CbmLitTrackFinderWeight : public CbmLitTrackFinderBase
{
public:
	CbmLitTrackFinderWeight();
	virtual ~CbmLitTrackFinderWeight();

	LitStatus DoFind(
			const HitPtrVector& hits,
			const TrackPtrVector& trackSeeds,
			TrackPtrVector& tracks);

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	void SetFitter(TrackFitterPtr fitter) { fFitter = fitter;}
	void SetFilter(TrackUpdatePtr filter) { fFilter = filter;}

protected:
	void FollowTracks(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);

	void FollowTrack(
			CbmLitTrack *track);

	Bool_t ProcessStationGroup(
			CbmLitTrack *track,
			Int_t stationGroup);

	Bool_t ProcessStation(
			CbmLitTrack *track,
			Int_t stationGroup,
			Int_t station);

	Bool_t AddHits(
			CbmLitTrack* track,
			HitPtrIteratorPair bounds);

	void FitTracks(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);
private:
	TrackFitterPtr fFitter;
	TrackUpdatePtr fFilter;

	ClassDef(CbmLitTrackFinderWeight, 1);
};

#endif /*CBMLITTRACKFINDERWEIGHT_H_*/
