/** CbmLitTrackFinderBase.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 *
 * Class implements basic functionality for track finders.
 **/

#ifndef CBMLITTRACKFINDERBASE_H_
#define CBMLITTRACKFINDERBASE_H_

#include "interface/CbmLitTrackFinder.h"
#include "finder/CbmLitGating.h"
#include "base/CbmLitFloat.h"
#include "base/CbmLitTypes.h"
#include "base/CbmLitDetectorLayout.h"
#include "base/CbmLitHitData.h"
#include "base/CbmLitTrackFinderSettings.h"

#include <set>

class CbmLitTrackFinderBase : public CbmLitTrackFinder, public CbmLitGating
{
public:
   /* Constructor */
   CbmLitTrackFinderBase();

   /* Destructor */
   virtual ~CbmLitTrackFinderBase();

   /* Sets detector layout */
   void SetLayout(CbmLitDetectorLayout layout) {fLayout = layout;}

   /* Sets settings for tracking */
   void SetSettings(const CbmLitTrackFinderSettings& settings) { fSettings = settings; }

   void SetZPropagationForTrackSeeds(Double_t zPropagationForTrackSeeds) { fZPropagationForTrackSeeds = zPropagationForTrackSeeds; }

   void SetUseTGeo(Bool_t useTGeo) { fUseTGeo = useTGeo; }

protected:
   /* Sets number of iterations for tracking */
   void SetNofIter(Int_t nofIter) { fNofIter = nofIter; }

   /* Sets maximum acceptable number of missing hits */
   void SetMaxNofMissingHits(Int_t maxNofMissingHits) { fMaxNofMissingHits = maxNofMissingHits;}

   /* Sets particle hypothesis for tracking */
   void SetPDG(Int_t pdg) { fPDG = pdg; }

protected:
   /* Parameters for each tracking iteration has to be set in this function
    * @param iter Iteration number */
   virtual void SetIterationParameters(
      Int_t iter) {}

   /* Arranges hits by stations
    * @param itBegin Iterator of the first hit
    * @param itEnd Iterator of the last hit */
   void ArrangeHits(
      HitPtrIterator itBegin,
      HitPtrIterator itEnd);

   /* Writes already used hits to a used hits set
    * @param Iterator to the first track
    * @param Iterator to the last track */
   void RemoveHits(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd);

   /* Copies tracks to output array
    * @param Iterator to the first track
    * @param Iterator to the last track
    * @param Output track array */
   void CopyToOutput(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd,
      TrackPtrVector& tracks);

protected:
   CbmLitDetectorLayout fLayout; // Detector layout.
   CbmLitHitData fHitData; // Hit storage.
   std::set<Int_t> fUsedHitsSet; // Sets with hits that have been used.
   std::set<Int_t> fUsedSeedsSet; // Set with track seeds that have been used.
   CbmLitTrackFinderSettings fSettings; // Settings for the track finder.
   Int_t fNofIter; // Number of iterations for tracking.
   Int_t fMaxNofMissingHits; // Maximum number of acceptable missing hits.
   Int_t fPDG; // Particle hypothesis for tracking.
   Double_t fZPropagationForTrackSeeds; // Z position for propagation of track seeds.
   Bool_t fUseTGeo; // If true than TGeoManager is used for navigation in tracking.
                    // Attention! This method works only in magnetic free regions.
                    // Track seed propagation also has to be done to magnetic
                    // field free region in order to make this method work correctly.
};

#endif /*CBMLITTRACKFINDERBASE_H_*/
