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

class LitTrackFinderNNVecElectron : public LitTrackFinderNNBaseElectronVec,
   public LitTrackFinderNNBase,
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
      unsigned int& nofTracks);

   void SetDetectorLayout(LitDetectorLayoutElectron<fvec>& layout) {
      fLayout = layout;
      fHitData.SetDetectorLayout(layout);
   }

public:
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
      const std::pair<unsigned int, unsigned int>& hits,
      unsigned int nofHits,
      int stationGroup,
      int station);

//private:
// LitScalTrack* fTracks[MAX_NOF_TRACKS]; // local copy of tracks
// unsigned int fNofTracks;
//
// LitDetectorLayoutElectron<fvec> fLayout; // detector geometry
// LitHitDataElectron<fvec> fHitData; // arranged hits
//
// unsigned char fMaxNofMissingHits;
};

//#undef cnst
#endif /* LITTRACKFINDERNNVECELECTRON_H_ */
