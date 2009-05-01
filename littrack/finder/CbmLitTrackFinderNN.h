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
			const HitPtrVector& hits,
			const TrackPtrVector& trackSeeds,
			TrackPtrVector& tracks);

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	void SetFilter(TrackUpdatePtr filter) {fFilter = filter;}

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

	Bool_t AddNearestHit(
			CbmLitTrack* track,
			HitPtrIteratorPair bounds);
private:
	TrackUpdatePtr fFilter;

	ClassDef(CbmLitTrackFinderNN, 1);
};

#endif /* CBMLITTRACKFINDERNN_H_ */
