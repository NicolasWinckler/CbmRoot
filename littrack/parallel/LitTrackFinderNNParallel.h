/** CbmLitTrackFinderNNParallel.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Parallel (multithreaded and SIMDized) version of the nearest
 ** neighbor tracking. As input to tracking arrays with track seeds
 ** and hits from the detector are used. The track is prolongated
 ** from station to station and the nearest hit is attached to track.
 ** Threading Building Blocks library is used for multithreading.
 **/

#ifndef LITTRACKFINDERNNPARALLEL_H_
#define LITTRACKFINDERNNPARALLEL_H_

#include "LitHit.h"
#include "LitTrack.h"
#include "LitDetectorGeometry.h"
#include "LitHitData.h"

#define cnst static const fvec

const unsigned int MAX_NOF_TRACKS = 1500;

class LitTrackFinderNNParallel
{
public:
	/* Constructor */
	LitTrackFinderNNParallel();

	/* Destructor */
	virtual ~LitTrackFinderNNParallel();

	/* Inherited from CbmLitTrackFinder */
	virtual void DoFind(
			LitScalPixelHit* hits[],
			unsigned int nofHits,
			LitTrack* trackSeeds[],
			unsigned int nofTrackSeeds,
			LitTrack* tracks[],
			unsigned int &nofTracks);

	void SetDetectorLayout(LitDetectorLayout<fvec>& layout) {
		fLayout = layout;
		fHitData.SetDetectorLayout(layout);
	}

public:
	void ArrangeHits(
			LitScalPixelHit* hits[],
			unsigned int nofHits);

	/* Initialize the track seeds, i.e. selects with the track selection tool
	 * proper ones and copies to local array.
	 * @param itBegin Iterator to the first track seed.
	 * @param itEnd iterator to the last track seed.
	 */
	void InitTrackSeeds(
			LitTrack* trackSeeds[],
			unsigned int nofTrackSeeds);

	inline void MinMaxIndex(
			const LitTrackParam<fscal>* par,
			LitScalPixelHit** hits,
			unsigned int nofHits,
			fscal maxErr,
			unsigned int &first,
			unsigned int &last);

	/* Follows tracks through the detector
	 * @param itBegin Iterator to the first track.
	 * @param itEnd Iterator to the last track.
	 */
	void FollowTracks();

	/* TODO: Add comments
	 *
	 */
	inline void PropagateThroughAbsorber(
			LitTrack* tracks[],
			LitAbsorber<fvec>& absorber);

	/*
	 * TODO Add comments
	 */
	void CollectHits(
			LitTrackParam<fscal>* par,
			LitTrack* track,
			unsigned char stationGroup,
			unsigned char station,
			unsigned char nofSubstations);

	/* TODO: Add comment
	 *
	 */
	inline void ProcessStation(
			LitTrack* tracks[],
			unsigned char stationGroup,
			unsigned char station);

	/* Adds the nearest hit to the track.
	 * @param track Pointer to the track.
	 * @param hits Reference to hit vector from which the nearest hit will be attached.
	 * @return True if the hit was attached to track.
	 */
	bool AddNearestHit(
			LitTrack* track,
			LitScalPixelHit* hits[],
			LitTrackParam<fscal>* pars[],
			unsigned int nofHits);

private:
	LitTrack* fTracks[MAX_NOF_TRACKS]; // local copy of tracks
	unsigned int fNofTracks;

	LitDetectorLayout<fvec> fLayout; // detector geometry
	LitHitData<fvec> fHitData; // arranged hits

	unsigned char fMaxNofMissingHits;
	fscal fSigmaCoef;
	fscal fMaxCovSq;
};

//#undef cnst
#endif /* LITTRACKFINDERNNPARALLEL_H_ */
