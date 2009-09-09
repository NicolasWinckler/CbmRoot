#ifndef CBMLITTRACKFINDERNNPARALLEL_H_
#define CBMLITTRACKFINDERNNPARALLEL_H_

#include "CbmLitTrackFinderBase.h"
#include "CbmLitPtrTypes.h"

//#ifndef __CINT__
//#include <tbb/blocked_range.h>
//#endif

class CbmLitTrackFinderNNParallel : public CbmLitTrackFinderBase
{
public:
	CbmLitTrackFinderNNParallel();
	virtual ~CbmLitTrackFinderNNParallel();

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


//	#ifndef __CINT__
//		void operator() ( const tbb::blocked_range<unsigned int>& r ) const {
//			for (unsigned int i = r.begin(); i != r.end(); ++i) {
//				CbmLitTrack* track = fTracks[i];
//	//				FollowTrack(track);
//			}
//		}
//	#endif

public:
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

#endif /* CBMLITTRACKFINDERNNPARALLEL_H_ */
