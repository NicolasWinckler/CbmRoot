/** CbmLitTrackFinderBranch.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 **
 ** Branching tracking implementation. As input to tracking
 ** arrays with track seeds and hits from the detector are used.
 ** The track is prolongated from station to station and the new
 ** track branches are started from hits in the validation gate.
 **/

#ifndef CBMLITTRACKFINDERBASEBRANCH_H_
#define CBMLITTRACKFINDERBASEBRANCH_H_

#include "CbmLitTrackFinderBase.h"
#include "CbmLitHitChiSq.h"

class CbmLitTrackSelection;
class CbmLitTrackUpdate;
class CbmLitTrackFitter;

class CbmLitTrackFinderBranch : public CbmLitTrackFinderBase
{
public:
	/* Constructor*/
	CbmLitTrackFinderBranch();

	/* Destructor */
	virtual ~CbmLitTrackFinderBranch();

	/* Inherited from CbmLitTrackFinder */
	LitStatus DoFind(
			HitPtrVector& hits,
			TrackPtrVector& trackSeeds,
			TrackPtrVector& tracks);

	/* Inherited from CbmLitTool */
	virtual LitStatus Initialize();

	/* Inherited from CbmLitTool */
	virtual LitStatus Finalize();

	/* */
	void IsAlwaysCreateMissingHit(bool isAlwaysCreateMissingHit) {fIsAlwaysCreateMissingHit = isAlwaysCreateMissingHit;}

	/* Sets station group selection tool.
	 * @param stationGroupSelection Pointer to station group selection tool.
	 */
	void SetStationGroupSelection(TrackSelectionPtr stationGroupSelection) {fStationGroupSelection = stationGroupSelection;}

	/* Sets final selection tool.
	 * @param finalSelection Pointer to final selection tool.
	 */
	void SetFinalSelection(TrackSelectionPtr finalSelection) {fFinalSelection = finalSelection;}

	/* Sets seed selection tool.
	 * @param seedSelection Pointer to seed selection tool.
	 */
	void SetSeedSelection(TrackSelectionPtr seedSelection) {fSeedSelection=seedSelection;}

    /* Sets KF update tool.
     * @param filter Pointer to KF update tool.
     */
	void SetFilter(TrackUpdatePtr filter) {fFilter = filter;}

	/* Sets track propagator tool.
	 * @param propagator Pointer to track propagator tool.
	 */
	void SetPropagator(TrackPropagatorPtr propagator) {fPropagator = propagator;}

	/* Sets the flag which defines from which hits the branches will be created.
	 * If true than hits from all substations are gethered together and
	 * hit selection is done over all this hits at a time
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

    /* Follows tracks through the detector */
	void FollowTracks();

	void ProcessStationGroup(
			const CbmLitTrack* track,
			int stationGroup);

	bool ProcessStation(
			const CbmLitTrack* track,
			int stationGroup,
			int station,
			TrackPtrVector& tracksOut);

	bool ProcessStation1(
			const CbmLitTrack* track,
			int stationGroup,
			int station,
			TrackPtrVector& tracksOut);

	bool ProcessStation2(
				const CbmLitTrack *track,
				int stationGroup,
				int station,
				TrackPtrVector& tracksOut);

	void ProcessSubstation(
			const CbmLitTrackParam* par,
			HitPtrIteratorPair bounds,
			std::vector<CbmLitHitChiSq>& hits);

	bool AddTrackCandidate(
			TrackPtrVector& tracks,
			int stationGroup);

	void CopyToOutputArray();

private:
	TrackPtrVector fTracks;
	TrackPtrVector fTracksCopy;
	TrackPtrVector fFoundTracks;

	TrackSelectionPtr fSeedSelection;
	TrackSelectionPtr fFinalSelection;
	TrackSelectionPtr fStationGroupSelection;
	TrackPropagatorPtr fPropagator;
	TrackUpdatePtr fFilter;

	int fMaxNofHitsInValidationGate; // maximum number of hits in the validations
	                                 // gate for which a separate branch is created
	int fMaxNofBranches; // maximum number of branches for one input track seed
	                     // which can be created in a station group
	bool fIsAlwaysCreateMissingHit; // if true than branch for a missing hit is always created
	                                // if false than it is created only if there are no hits in the validation gate
	bool fIsProcessSubstationsTogether; // if true than hits from all substations are gethered together and
	                                    // hit selection is done over all this hits at a time
	int fNofBranches; // counter for number of branches
};

#endif /*CBMLITTRACKFINDERBASEBRANCH_H_*/
