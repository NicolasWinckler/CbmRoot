/** LitTrackFinderNNVecElectron.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 **
 ** Parallel (multithreaded and SIMDized) version of the nearest
 ** neighbor tracking. As input to tracking arrays with track seeds
 ** and hits from the detector are used. The track is prolongated
 ** from station to station and the nearest hit is attached to track.
 ** Threading Building Blocks library is used for multithreading.
 **/

#ifndef LITTRACKFINDERNNVECELECTRON_H_
#define LITTRACKFINDERNNVECELECTRON_H_

#include "LitDetectorGeometryElectron.h"
#include "LitHitDataElectron.h"
#include "../LitHit.h"
#include "../LitTrack.h"
#include "../LitTrackFinderNNBase.h"
#include "../LitTrackFinder.h"
#include "LitTrackFinderNNBaseElectron.h"

#define cnst static const fvec

const unsigned int MAX_NOF_TRACKS = 1500;

class LitTrackFinderNNVecElectron : public LitTrackFinderNNBase,
                                    public LitTrackFinder
{
public:
	/* Constructor */
	LitTrackFinderNNVecElectron();

	/* Destructor */
	virtual ~LitTrackFinderNNVecElectron();

	/* Inherited from LitTrackFinder */
	virtual void DoFind(
			LitScalPixelHit* hits[],
			unsigned int nofHits,
			LitScalTrack* trackSeeds[],
			unsigned int nofTrackSeeds,
			LitScalTrack* tracks[],
			unsigned int &nofTracks);

	void SetDetectorLayout(LitDetectorLayoutElectron<fvec>& layout) {
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
			LitScalTrack* trackSeeds[],
			unsigned int nofTrackSeeds);

	/* Follows tracks through the detector
	 * @param itBegin Iterator to the first track.
	 * @param itEnd Iterator to the last track.
	 */
	void FollowTracks();

	/* TODO: Add comments
	 *
	 */
	inline void PropagateThroughAbsorber(
			LitScalTrack* tracks[],
			LitAbsorber<fvec>& absorber);

	/*
	 * TODO Add comments
	 */
	void CollectHits(
			LitTrackParamScal* par,
			LitScalTrack* track,
			unsigned char stationGroup,
			unsigned char station,
			unsigned char nofSubstations);

	/* TODO: Add comment
	 *
	 */
	inline void ProcessStation(
			LitScalTrack* tracks[],
			unsigned char stationGroup,
			unsigned char station);

	/* Adds the nearest hit to the track.
	 * @param track Pointer to the track.
	 * @param hits Reference to hit vector from which the nearest hit will be attached.
	 * @return True if the hit was attached to track.
	 */
	bool AddNearestHit(
			LitScalTrack* track,
			LitScalPixelHit* hits[],
			LitTrackParamScal* pars[],
			unsigned int nofHits);

//private:
//	LitScalTrack* fTracks[MAX_NOF_TRACKS]; // local copy of tracks
//	unsigned int fNofTracks;
//
//	LitDetectorLayoutElectron<fvec> fLayout; // detector geometry
//	LitHitDataElectron<fvec> fHitData; // arranged hits
//
//	unsigned char fMaxNofMissingHits;
};

//#undef cnst
#endif /* LITTRACKFINDERNNVECELECTRON_H_ */
