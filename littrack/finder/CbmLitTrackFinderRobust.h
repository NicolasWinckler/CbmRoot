#ifndef CBMLITTRACKFINDERROBUST_H_
#define CBMLITTRACKFINDERROBUST_H_

#include "CbmLitTrackFinderBase.h"

class CbmLitTrackFitter;
class CbmLitTrackUpdate;

class CbmLitTrackFinderRobust : public CbmLitTrackFinderBase
{
public:
	CbmLitTrackFinderRobust();
	virtual ~CbmLitTrackFinderRobust();

	LitStatus DoFind(
			const HitPtrVector& hits,
			const TrackPtrVector& trackSeeds,
			TrackPtrVector& tracks);

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

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

	CbmLitTrackFitter* fFitter;
	CbmLitTrackUpdate* fFilter;

	ClassDef(CbmLitTrackFinderRobust, 1);
};

#endif /*CBMLITTRACKFINDERROBUST_H_*/
