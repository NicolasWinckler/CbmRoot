/** CbmLitTrackFinderNNScalarElectron.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Scalar version of the nearest neighbor tracking adopted for
 ** the 'electron' setup of the CBM experiment.
 ** As input to tracking arrays with track seeds
 ** and hits from the detector are used. The track is prolongated
 ** from station to station and the nearest hit is attached to track.
 ** Threading Building Blocks library is used for multithreading.
 **/

#ifndef LITTRACKFINDERNNSCALARELECTRON_H_
#define LITTRACKFINDERNNSCALARELECTRON_H_

//#include "LitHit.h"
//#include "LitTrack.h"
#include "LitDetectorGeometryElectron.h"
#include "LitHitDataElectron.h"

class CbmLitField;

//#define cnst static const fvec

const unsigned int MAX_NOF_TRACKS_ELECTRON = 1500;

class LitTrackFinderNNScalarElectron
{
public:
	/* Constructor */
	LitTrackFinderNNScalarElectron();

	/* Destructor */
	virtual ~LitTrackFinderNNScalarElectron();

	virtual void DoFind(
			LitScalPixelHit* hits[],
			unsigned int nofHits,
			LitScalTrack* trackSeeds[],
			unsigned int nofTrackSeeds,
			LitScalTrack* tracks[],
			unsigned int &nofTracks);

	void SetDetectorLayout(LitDetectorLayoutElectronScal& layout) {
		fLayout = layout;
		fHitData.SetDetectorLayout(layout);
	}

public:
	void ArrangeHits(
			LitScalPixelHit* hits[],
			unsigned int nofHits);

	void InitTrackSeeds(
			LitScalTrack* trackSeeds[],
			unsigned int nofTrackSeeds);

	inline void MinMaxIndex(
			const LitTrackParamScal* par,
			LitScalPixelHit** hits,
			unsigned int nofHits,
			fscal maxErr,
			unsigned int &first,
			unsigned int &last);

	void FollowTracks();

	void PropagateToFirstStation(
			LitScalTrack* track);

	void FollowTrack(
			LitScalTrack *track);

	bool ProcessStationGroup(
			LitScalTrack *track,
			int stationGroup);

	bool ProcessStation(
			LitScalTrack *track,
			int stationGroup,
			int station);

//	bool AddNearestHit(
//			LitTrackScal* track,
//			HitPtrIteratorPair hits[],
//			const CbmLitTrackParam par[],
//			int nofSubstations);

private:
	LitScalTrack* fTracks[MAX_NOF_TRACKS_ELECTRON]; // local copy of tracks
	unsigned int fNofTracks;

	LitDetectorLayoutElectronScal fLayout; // detector geometry
	LitHitDataElectronScal fHitData; // arranged hits

	unsigned char fMaxNofMissingHits;
	fscal fSigmaCoef;
	fscal fMaxCovSq;

	CbmLitField* fField;
};

//#undef cnst

#endif /* LITTRACKFINDERNNSCALARELECTRON_H_ */
