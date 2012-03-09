/** LitTrackFinderNNVecMuon.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Parallel SIMDized implementation of the MUCH tracking.
 ** Input: array with track seeds and array with hits.
 ** Output: reconstructed tracks.
 ** Algorithm is based on track following and Kalman Filter methods.
 ** The track is propagated from station to station and the track
 ** branch is created for each hit in the validation gate.
 **/

#ifndef LitTrackFinderNNVecMuon_H_
#define LitTrackFinderNNVecMuon_H_

#include "LitDetectorGeometryMuon.h"
#include "LitHitDataMuon.h"

#include <vector>
#include <map>

namespace lit {
namespace parallel {

class LitTrackFinderNNVecMuon
{
public:
   /* Constructor */
   LitTrackFinderNNVecMuon();

   /* Destructor */
   virtual ~LitTrackFinderNNVecMuon();

   /* Inherited from LitTrackFinder */
   virtual void DoFind(
      const PixelHitArray& hits,
      const TrackArray& trackSeeds,
      TrackArray& tracks);

   /* Sets detector layout for the tracking
    * @param layout Detector layout */
   void SetDetectorLayout(
         const LitDetectorLayoutMuon<fvec>& layout) {
      fLayout = layout;
      fHitData.SetDetectorLayout(layout);
   }

private:

   void ArrangeHits(
       const PixelHitArray& hits);

    /* Initializes the track seeds
     * @param itBegin Iterator to the first track seed.
     * @param itEnd iterator to the last track seed. */
    void InitTrackSeeds(
       const TrackArray& trackSeeds);

    /* Follows tracks through the detector
     * @param itBegin Iterator to the first track.
     * @param itEnd Iterator to the last track. */
    void FollowTracks();

    /* TODO: Add comments
     *
     */
    void PropagateThroughAbsorber(
          const std::vector<unsigned int>& tracksId1,
          const LitAbsorber<fvec>& absorber);

    /* TODO: Add comments
     *
     */
    inline void PropagateThroughAbsorber(
       const TrackArray& tracks,
       const LitAbsorber<fvec>& absorber);

    /*
     * TODO Add comments
     */
    void CollectHits(
       std::vector<LitTrackParamScal>& par,
       LitScalTrack* track,
       unsigned char stationGroup,
       unsigned char station,
       unsigned char nofSubstations);

    /*
     * TODO Add comments
     */
    void ProcessStation(
          const std::vector<unsigned int>& tracksId1,
          unsigned char stationGroup,
          unsigned char station);

    /* TODO: Add comment
     *
     */
    inline void ProcessStation(
       const TrackArray& tracks,
       unsigned char stationGroup,
       unsigned char station);

    /* Adds the nearest hit to the track.
     * @param track Pointer to the track.
     * @param hits Reference to hit vector from which the nearest hit will be attached.
     * @return True if the hit was attached to track.
     */
    bool AddNearestHit(
       LitScalTrack* track,
       const PixelHitArray& hits,
       const std::vector<LitTrackParamScal*>& pars,
       unsigned int nofHits);

    /*
     *
     */
    void MinMaxIndex(
          const LitTrackParamScal* par,
          const PixelHitArray& hits,
          fscal maxErr,
          PixelHitConstIterator& first,
          PixelHitConstIterator& last);

private:
   /* Local copy of tracks */
   TrackArray fTracks;
   /* Detector geometry */
   LitDetectorLayoutMuon<fvec> fLayout;
   /* Arranged hits */
   LitHitDataMuon<fvec> fHitData;
   /* Maximum number of missing hits */
   unsigned char fMaxNofMissingHits;
   /* Maximum number of branches created in the station
      for one input track */
   unsigned int fMaxNofBranchesStation;
   /* Maximum number of branches for one input track seed
      which can be created in a station group */
   unsigned int fMaxNofBranchesStationGroup;
   /* Maximum number of branches for one input track seed */
   unsigned int fMaxNofBranches;
   /* Counter for number of branches in the station group */
   unsigned int fNofBranchesStationGroup;
   /* Map for monitoring of number of branches for each input track seed
      map<previousTrackId, nofBranches> */
   std::map<int, int> fNofBranches;
   /* If true than a separate branch for a missing hit is always created
      if false than it is created only if there are no hits in the validation gate */
   bool fIsAlwaysCreateMissingHit;
   /* If true than hits from all substations are gathered together and
      hit selection is done over all this hits at a time */
   bool fIsProcessSubstationsTogether;
   /* Sigma coefficient for fast hit search */
   fscal fSigmaCoef;
   /* Maximum covariance value */
   fscal fMaxCovSq;
   /* Chi square cut for pixel hits */
   fvec fChiSqPixelHitCut;
};

} // namespace parallel
} // namespace lit
#endif /* LitTrackFinderNNVecMuon_H_ */
