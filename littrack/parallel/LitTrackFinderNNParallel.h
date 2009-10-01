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

class LitTrackFinderNNParallel
{
public:
	/* Constructor */
	LitTrackFinderNNParallel();

	/* Destructor */
	virtual ~LitTrackFinderNNParallel();

	/* Inherited from CbmLitTrackFinder */
	virtual void DoFind(
			ScalPixelHitVector& hits,
			TrackVector& trackSeeds,
			TrackVector& tracks);

	void SetDetectorLayout(LitDetectorLayout& layout) {
		fLayout = layout;
		fHitData.SetDetectorLayout(layout);
	}

public:
	void ArrangeHits(
			ScalPixelHitVector& hits);

	/* Initialize the track seeds, i.e. selects with the track selection tool
	 * proper ones and copies to local array.
	 * @param itBegin Iterator to the first track seed.
	 * @param itEnd iterator to the last track seed.
	 */
	void InitTrackSeeds(TrackVector& seeds);

	inline void MinMaxIndex(
			const LitScalTrackParam* par,
			ScalPixelHitVector& hits,
			fscal maxErr,
			ScalPixelHitIterator& first,
			ScalPixelHitIterator& last) const;

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
			LitAbsorber& absorber);

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
			LitScalTrackParam* pars[],
			unsigned int nofHits);

private:
	TrackVector fTracks; // local copy of tracks

	LitDetectorLayout fLayout; // detector geometry
	LitHitData fHitData; // arranged hits

	unsigned char fMaxNofMissingHits;
	fscal fSigmaCoef;
	fscal fMaxCovSq;
};

//#undef cnst
#endif /* LITTRACKFINDERNNPARALLEL_H_ */
