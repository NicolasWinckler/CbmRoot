/** CbmLitTrackFinderBranch.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 *
 * Class implements track reconstruction using branching approach.
 * Tracks are propagated from station to station and new
 * track branches are started for each hit in the validation gate.
 **/

#ifndef CBMLITTRACKFINDERBASEBRANCH_H_
#define CBMLITTRACKFINDERBASEBRANCH_H_

#include "data/CbmLitHitChiSq.h"
#include "finder/CbmLitTrackFinderBase.h"

#include <map>

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

   /* Sets is always create missing hit */
   void IsAlwaysCreateMissingHit(bool isAlwaysCreateMissingHit) {fIsAlwaysCreateMissingHit = isAlwaysCreateMissingHit;}

   /* Sets station group track selection tool.
    * @param stationGroupSelection Track selection tool. */
   void SetStationGroupSelection(TrackSelectionPtr stationGroupSelection) {fStationGroupSelection = stationGroupSelection;}

   /* Sets final track selection tool.
    * @param finalSelection Final track selection tool. */
   void SetFinalSelection(TrackSelectionPtr finalSelection) {fFinalSelection = finalSelection;}

   /* Sets track seed selection tool.
    * @param seedSelection Track seed selection tool. */
   void SetSeedSelection(TrackSelectionPtr seedSelection) {fSeedSelection=seedSelection;}

   /* Sets Kalman Filter update tool.
    * @param filter Kalman Filter update tool. */
   void SetFilter(TrackUpdatePtr filter) {fFilter = filter;}

   /* Sets track propagation tool.
    * @param propagator Track propagation tool. */
   void SetPropagator(TrackPropagatorPtr propagator) {fPropagator = propagator;}

   /* Sets the flag which defines from which hits the branches will be created.
    * @param isProcessSubstationsTogether Flag value. */
   void IsProcessSubstationsTogether(bool isProcessSubstationsTogether) {
      fIsProcessSubstationsTogether = isProcessSubstationsTogether;
   }

protected:

   /* Inherited from CbmLitTrackFinderBase */
   virtual void SetIterationParameters(
      Int_t iter);

   /* Initializes track seeds
    * @param itBegin Iterator to the first track seed.
    * @param itEnd Iterator to the last track seed. */
   void InitTrackSeeds(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd);

   /* Follows tracks through the detector */
   void FollowTracks();

   /* Follows track through station group
    * @param track Track to be followed
    * @param stationGroup Station group index */
   void ProcessStationGroup(
      const CbmLitTrack* track,
      Int_t stationGroup);

   /* Follows track through station
    * @param track Track to be followed
    * @param stationGroup Station group index
    * @param station Station index
    * @param Output array with created track branches
    * @return True if branch was created */
   bool ProcessStation(
      const CbmLitTrack* track,
      Int_t stationGroup,
      Int_t station,
      TrackPtrVector& tracksOut);

   /* Processes station if fIsProcessSubstationsTogether == true
    * @param track Track to be followed
    * @param stationGroup Station group index
    * @param station Station index
    * @param Output array with created track branches
    * @return True if branch was created */
   bool ProcessStation1(
      const CbmLitTrack* track,
      Int_t stationGroup,
      Int_t station,
      TrackPtrVector& tracksOut);

   /* Processes station if fIsProcessSubstationsTogether == false
    * @param track Track to be followed
    * @param stationGroup Station group index
    * @param station Station index
    * @param Output array with created track branches
    * @return True if branch was created */
   bool ProcessStation2(
      const CbmLitTrack* track,
      Int_t stationGroup,
      Int_t station,
      TrackPtrVector& tracksOut);

   /* Processes substation
    * @param par Predicted track parameter
    * @param bounds Range of possible hit candidates to be attached to track
    * @param hits Output array of attached hits */
   void ProcessSubstation(
      const CbmLitTrackParam* par,
      HitPtrIteratorPair bounds,
      std::vector<CbmLitHitChiSq>& hits);

   /* Add track candidates to global track candidates array
    * @param tracks Track candidates to be added
    * @param stationGroup Station group index
    * @return True if OK */
   bool AddTrackCandidate(
      TrackPtrVector& tracks,
      Int_t stationGroup);

   /* Copies track candidates to local output array for final comparison */
   void CopyToOutputArray();

private:
   /* Local storage of tracks */
   TrackPtrVector fTracks;
   /* Copy of track for final track selection */
   TrackPtrVector fTracksCopy;
   /* Local storage of tracks */
   TrackPtrVector fFoundTracks;

   /* Track seed selection tool */
   TrackSelectionPtr fSeedSelection;
   /* Final track selection tool */
   TrackSelectionPtr fFinalSelection;
   /* Station group track selection tool */
   TrackSelectionPtr fStationGroupSelection;
   /* Track propagation tool */
   TrackPropagatorPtr fPropagator;
   /* Kalman Filter track update tool */
   TrackUpdatePtr fFilter;

   /* Maximum number of hits in the validation
      gate for which a separate branch is created */
   Int_t fMaxNofHitsInValidationGate;
   /* Maximum number of branches for one input track seed
      which can be created in a station group */
   Int_t fMaxNofBranchesStationGroup;
   /* If true than a separate branch for a missing hit is always created
      if false than it is created only if there are no hits in the validation gate */
   bool fIsAlwaysCreateMissingHit;
   /* If true than hits from all substations are gathered together and
      hit selection is done over all this hits at a time */
   bool fIsProcessSubstationsTogether;
   /* Counter for number of branches */
   Int_t fNofBranchesStationGroup;
   /* Maximum number of branches for one input track seed */
   Int_t fMaxNofBranches;
   /* Map for monitoring of number of branches for each input track seed
      map<previousTrackId, nofBranches> */
   std::map<Int_t, Int_t> fNofBranches;
};

#endif /*CBMLITTRACKFINDERBASEBRANCH_H_*/
