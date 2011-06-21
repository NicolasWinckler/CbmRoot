/** LitTrackFinderNNVecElectron.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 **
 ** Parallel SIMDized implementation of the TRD tracking.
 ** Input: array with track seeds and array with hits.
 ** Output: reconstructed tracks.
 ** Algorithm is based on track following and Kalman Filter methods.
 ** The track is propagated from station to station and the track
 ** branch is created for each hit in the validation gate.
 **/

#ifndef LITTRACKFINDERNNVECELECTRON_H_
#define LITTRACKFINDERNNVECELECTRON_H_

#include "LitDetectorGeometryElectron.h"
#include "LitHitDataElectron.h"
#include "../LitHit.h"
#include "../LitTrack.h"
//#include "../LitTrackFinderNNBase.h"
//#include "../LitTrackFinder.h"
//#include "LitTrackFinderNNBaseElectron.h"

#define cnst static const fvec

namespace lit {
namespace parallel {

const unsigned int MAX_NOF_TRACKS = 1500;

class LitTrackFinderNNVecElectron
{
public:
   /* Constructor */
   LitTrackFinderNNVecElectron();

   /* Destructor */
   virtual ~LitTrackFinderNNVecElectron();

   /* Inherited from LitTrackFinder */
   virtual void DoFind(
      const PixelHitArray& hits,
      const TrackArray& trackSeeds,
      TrackArray& tracks);

   void SetDetectorLayout(LitDetectorLayoutElectron<fvec>& layout) {
      fLayout = layout;
      fHitData.SetDetectorLayout(layout);
   }

public:

   void ArrangeHits(
       const PixelHitArray& hits);

   void InitTrackSeeds(
      const TrackArray& trackSeeds);

   /* Follows tracks through the detector
    * @param itBegin Iterator to the first track.
    * @param itEnd Iterator to the last track.
    */
   void FollowTracks();

   /* TODO: Add comments
    *
    */
   void PropagateToFirstStation(
      LitScalTrack* tracks[]);

   /*
    * TODO Add comments
    */
   void CollectHits(
      LitTrackParamScal* par,
      LitScalTrack* track,
      unsigned char stationGroup,
      unsigned char station);

   /* TODO: Add comment
    *
    */
   inline void ProcessStation(
      LitScalTrack* tracks[],
      unsigned char stationGroup,
      unsigned char station);

   /*
    *
    */
   bool AddNearestHit(
      LitScalTrack* track,
      const PixelHitConstIteratorPair& hits,
      unsigned int nofHits,
      int stationGroup,
      int station);

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
   LitDetectorLayoutElectron<fvec> fLayout;
   /* Arranged hits */
   LitHitDataElectron<fvec> fHitData;
   /* Maximum number of missing hits */
   unsigned char fMaxNofMissingHits;
   /* Sigma coefficient for fast hit search */
   fscal fSigmaCoef;
   /* Maximum covariance value */
   fscal fMaxCovSq;
   /* Chi square cut for pixel hits */
   fvec fChiSqPixelHitCut;
};

//#undef cnst

} // namespace parallel
} // namespace lit
#endif /* LITTRACKFINDERNNVECELECTRON_H_ */
