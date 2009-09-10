/** CbmLitTrackFinderNN.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **
 ** Nearest neighbor tracking implementation. As input to tracking
 ** arrays with track seeds and hits from the detector are used.
 ** The track is prolongated from station to station and the
 ** nearest hit is attached to track. Flag fIsProcessSubstationsTogether
 ** defines which nearest hits should be attached to track.
 **/

#ifndef CBMLITTRACKFINDERNN_H_
#define CBMLITTRACKFINDERNN_H_

#include "CbmLitTrackFinderBase.h"
#include "CbmLitPtrTypes.h"

class CbmLitTrackFinderNN : public CbmLitTrackFinderBase
{
public:
	/* Constructor */
	CbmLitTrackFinderNN();

	/* Destructor */
	virtual ~CbmLitTrackFinderNN();

	/* Inherited from CbmLitTrackFinder */
	virtual LitStatus DoFind(
			HitPtrVector& hits,
			TrackPtrVector& trackSeeds,
			TrackPtrVector& tracks);

	/* Inherited from CbmLitTool */
	virtual LitStatus Initialize();

	/* Inherited from CbmLitTool */
	virtual LitStatus Finalize();

	/* Sets seed selection tool.
	 * @param seedSelection Pointer to seed selection tool.
	 */
	void SetSeedSelection(TrackSelectionPtr seedSelection) {
		fSeedSelection=seedSelection;
	}

	/* Sets track propagator tool.
	 * @param propagator Pointer to track propagator tool.
	 */
	void SetPropagator(TrackPropagatorPtr propagator) {
		fPropagator = propagator;
	}

	/* Sets final track selection tool.
	 * @param finalSelection Pointer to final track selection tool.
	 */
	void SetFinalSelection(TrackSelectionPtr finalSelection) {
		fFinalSelection = finalSelection;
	}

    /* Sets KF update tool.
     * @param filter Pointer to KF update tool.
     */
	void SetFilter(TrackUpdatePtr filter) {
		fFilter = filter;
	}

	/* Sets the flag which defines which nearest hits will be attached to track.
	 * If true than only one nearest hit from all substations in station will be attached to track.
	 * If false than the nearest hit from each substation in station will be attached to the track.
	 * @param isProcessSubstationsTogether Flag value.
	 */
	void IsProcessSubstationsTogether(bool isProcessSubstationsTogether) {
		fIsProcessSubstationsTogether = isProcessSubstationsTogether;
	}

protected:
	/* Initialize the track seeds, i.e. selects with the track selection tool
	 * proper ones and copies to local array.
	 * @param itBegin Iterator to the first track seed.
	 * @param itEnd iterator to the last track seed.
	 */
    void InitTrackSeeds(
    		TrackPtrIterator itBegin,
    		TrackPtrIterator itEnd);

    /* Follows tracks through the detector
     * @param itBegin Iterator to the first track.
     * @param itEnd Iterator to the last track.
     */
	void FollowTracks(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);

	/* Follow one track through the detector.
	 * @param track Pointer to the track.
	 */
	void FollowTrack(
			CbmLitTrack *track);

	/* Processes the station group in order to attach hit(s) to track.
	 * @param track Pointer to the track
	 * @param stationGroup Index of the station group.
	 * @return True if the track successfully passed through the station group.
	 */
	bool ProcessStationGroup(
			CbmLitTrack *track,
			int stationGroup);

	/* Processes the station in station group in order to attach hit(s) to track.
	 * @param track Pointer to the track.
	 * @param stationGroup Index of the station group.
	 * @param station Index of the station in the station group.
	 * @return True if the hit was attached to track.
	 */
	bool ProcessStation(
			CbmLitTrack *track,
			int stationGroup,
			int station);

	/* Adds the nearest hit to the track.
	 * @param track Pointer to the track.
	 * @param hits Reference to hit vector from which the nearest hit will be attached.
	 * @return True if the hit was attached to track.
	 */
	bool AddNearestHit(
			CbmLitTrack* track,
			HitPtrVector& hits);
private:
	TrackPtrVector fTracks; // local copy of tracks

	TrackUpdatePtr fFilter; // KF update tool
	TrackSelectionPtr fSeedSelection; // seed selection tool
	TrackSelectionPtr fFinalSelection; // final track selection tool
	TrackPropagatorPtr fPropagator; // track propagation tool

	// If true than only one nearest hit from all substations in station will be attached to track.
	// If false than the nearest hit from each substation in station will be attached to the track.
	bool fIsProcessSubstationsTogether;
};

#endif /* CBMLITTRACKFINDERNN_H_ */
