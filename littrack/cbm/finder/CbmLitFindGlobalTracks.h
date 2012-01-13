/** CbmLitFindGlobalTracks.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** CBM task class for global track reconstruction.
 ** Output is reconstructed global tracks CbmGlobalTrack and
 ** local track segments CbmMuchTrack, CbmTrdTrack.
 ** The task automatically determines the setup based on the
 ** geometry stored in the MC transport file.
 ** The tracking is performed in the MUCH and TRD detectors, than
 ** the hit-to-track merger attaches the TOF hit finally the track is refitted.
 ** If TRD detector presences in the muon detector setup,
 ** than the tracking is done in MUCH+TRD detectors in one goal.
 **/

#ifndef CBMLITFINDGLOBALTRACKS_H_
#define CBMLITFINDGLOBALTRACKS_H_

#include "FairTask.h"

#include "base/CbmLitTypes.h"
#include "base/CbmLitPtrTypes.h"
#include "base/CbmLitDetectorSetup.h"

#include "TStopwatch.h"

#include <string>

class TClonesArray;

class CbmLitFindGlobalTracks : public FairTask
{
public:
   /** Default constructor */
   CbmLitFindGlobalTracks();

   /** Destructor */
   virtual ~CbmLitFindGlobalTracks();

   /**
    * Derived from FairTask. Executed before starting event-by-event execution.
    */
   virtual InitStatus Init();

   /**
     * Derived from FairTask. Executed on each event.
     * @param opt Options
     */
   virtual void Exec(Option_t* opt);

   /**
     * Derived from FairTask. Set parameter containers.
     */
   virtual void SetParContainers();

   /**
     * Sets the tracking algorithm to be used.
     * @param trackingType Name of the tracking algorithm.
     * "branch" - branching tracking
     * "nn" - nearest neighbor tracking
     * "weight" - weighting tracking
     */
   void SetTrackingType(const std::string& trackingType) { fTrackingType = trackingType;}

   /**
     * Sets the hit-to-track merger algorithm to be used.
     * @param mergerType Name of the hit-to-track algorithm.
     * "nearest_hit" - attaches hit-to-track based on the closest statistical distance
     */
   void SetMergerType(const std::string& mergerType) { fMergerType = mergerType;}

   /**
     * Sets track fitter algorithm to be used for the final track fit.
     * @param fitterType Name fitter algorithm.
     * "lit_kalman" - forward Kalman filter track fit with TGeo geometry
     */
   void SetFitterType(const std::string& fitterType) { fFitterType = fitterType;}

private:
   /**
    * Derived from FairTask. Executed after all events are processed.
    */
   virtual void Finish();

   /**
    * Reads necessary data branches from the input data files and
    * creates branches for CbmGlobalTrack, CbmTrdTrack, CbmMuchTrack
    */
   void ReadAndCreateDataBranches();

   /**
    * Create and initializes track finder and track merger objects.
    */
   void InitTrackReconstruction();

   /**
    * Converts input data from CBMROOT data classes to LIT data classes.
    */
   void ConvertInputData();

   /**
    * Converts output data LIT data classes to CBMROOT data classes.
    */
   void ConvertOutputData();

   /*
    * Calculate length of the global track
    */
   void CalculateLength();

   /**
    * Clear arrays and frees the memory.
    */
   void ClearArrays();

   /**
    * Runs the track reconstruction
    */
   void RunTrackReconstruction();

   /**
    * Prints output stopwatch statistics for track-finder and hit-to-track merger.
    */
   void PrintStopwatchStatistics();

   /**
    * Selects tracks for further merging with TOF.
    * Track has to have at least one hit in the last station group.
    */
   void SelectTracksForTofMerging();

   CbmLitDetectorSetup fDet;

   // Pointers to data arrays
   TClonesArray* fStsTracks; // CbmStsTrack array
   TClonesArray* fStsHits; // CbmStsHit
   TClonesArray* fMuchPixelHits; // CbmMuchPixelHit array
   TClonesArray* fMuchStrawHits; // CbmMuchStrawHit array
   TClonesArray* fMuchTracks; // output CbmMuchTrack array
   TClonesArray* fTrdHits; // CbmTrdHit array
   TClonesArray* fTrdTracks; // output CbmTrdTrack array
   TClonesArray* fTofHits; // CbmTofHit array
   TClonesArray* fGlobalTracks; //output CbmGlobalTrack array

   // LIT data arrays
   TrackPtrVector fLitStsTracks; // STS tracks
   HitPtrVector fLitHits; // MUCH+TRD hits
   HitPtrVector fLitTofHits; // TOF hits
   TrackPtrVector fLitOutputTracks; // output Lit tracks

   // Tools
   TrackFinderPtr fFinder; // track finder
   HitToTrackMergerPtr fMerger; // hit-to-track merger
   TrackFitterPtr fFitter; // track fitter
   // track propagator
   // Used to propagate STS track to the last STS station!!!
   // Since this cannot be done in parallel mode!!!
   TrackPropagatorPtr fPropagator;

   // Settings
   // Tracking method to be used
   // "branch" - branching method
   // "nn" - nearest neighbor method
   // "weight" - weighting method
   std::string fTrackingType;

   // Merger method to be used
   // "nearest_hit" - assigns nearest hit to the track
   std::string fMergerType;

   // Track fitter to be used for the final track fit
   // "lit_kalman" - forward Kalman track fit with LIT propagation and TGeo navigation
   std::string fFitterType;

   // stopwatches
   TStopwatch fTrackingWatch; // stopwatch for tracking
   TStopwatch fMergerWatch; // stopwatch for merger

   Int_t fEventNo; // event counter

   ClassDef(CbmLitFindGlobalTracks, 1);
};

#endif /* CBMLITFINDGLOBALTRACKS_H_ */
