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
	TrackUpdatePtr fFilter;
};

#endif /* CBMLITTRACKFINDERNN_H_ */
