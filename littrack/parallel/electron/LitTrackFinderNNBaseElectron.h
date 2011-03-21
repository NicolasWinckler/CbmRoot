#ifndef LITTRACKFINDERNNBASEELECTRON_H_
#define LITTRACKFINDERNNBASEELECTRON_H_

#include "LitDetectorGeometryElectron.h"
#include "LitHitDataElectron.h"
//#include "../LitTrackFinderNNBase.h"
//#include "../LitTrackFinder.h"

//class LitScalPixelHit;
//class LitScalTrack;
#include "../LitHit.h"
#include "../LitTrack.h"

#include <algorithm>

template<class T>
class LitTrackFinderNNBaseElectron
{
public:
   LitTrackFinderNNBaseElectron();
   virtual ~LitTrackFinderNNBaseElectron();

   void SetMaxNofMissingHits(unsigned char maxNofMissingHits) {
      fMaxNofMissingHits = maxNofMissingHits;
   }

   static const unsigned int MAX_NOF_TRACKS = 1500;

protected:
   void ArrangeHits(
      LitScalPixelHit* hits[],
      unsigned int nofHits);

   void InitTrackSeeds(
      LitScalTrack* trackSeeds[],
      unsigned int nofTrackSeeds);


   LitScalTrack* fTracks[MAX_NOF_TRACKS]; // local copy of tracks
   unsigned int fNofTracks;

   LitDetectorLayoutElectron<T> fLayout; // detector geometry
   LitHitDataElectron<T> fHitData; // arranged hits

   unsigned char fMaxNofMissingHits;
};

typedef LitTrackFinderNNBaseElectron<fscal> LitTrackFinderNNBaseElectronScal;
typedef LitTrackFinderNNBaseElectron<fvec> LitTrackFinderNNBaseElectronVec;

template<class T>
LitTrackFinderNNBaseElectron<T>::LitTrackFinderNNBaseElectron():
   fMaxNofMissingHits(3)
{

}

template<class T>
LitTrackFinderNNBaseElectron<T>::~LitTrackFinderNNBaseElectron()
{

}

template<class T>
void LitTrackFinderNNBaseElectron<T>::ArrangeHits(
   LitScalPixelHit* hits[],
   unsigned int nofHits)
{
   // TODO : add threads here
   for(unsigned int i = 0; i < nofHits; i++) {
      LitScalPixelHit* hit = hits[i];
//       if (fUsedHitsSet.find(hit->GetRefId()) != fUsedHitsSet.end()) continue;
      fHitData.AddHit(hit->planeId, hit);
   }
//    std::cout << fHitData;

   // TODO : add threads here
   for (int i = 0; i < fLayout.GetNofStationGroups(); i++) {
      for (int j = 0; j < fLayout.GetNofStations(i); j++) {
         LitScalPixelHit** shits = fHitData.GetHits(i, j);
         LitScalPixelHit** begin = &shits[0];
         LitScalPixelHit** end = &shits[0] + fHitData.GetNofHits(i, j);

#ifdef LIT_USE_TBB
         tbb::parallel_sort(begin, end, ComparePixelHitXLess());
#else
         std::sort(begin, end, ComparePixelHitXLess());
#endif
      }
   }

//    std::cout << fHitData;
}

template<class T>
void LitTrackFinderNNBaseElectron<T>::InitTrackSeeds(
   LitScalTrack* trackSeeds[],
   unsigned int nofTrackSeeds)
{
   fNofTracks = 0;
   fscal QpCut = 1./0.1;
   //TODO : add threads here
   for (unsigned int i = 0; i < nofTrackSeeds; i++) {
      LitScalTrack* track = trackSeeds[i];
      if (fabs(track->paramLast.Qp) > QpCut) { continue; }
//    if (fUsedSeedsSet.find((*track)->GetPreviousTrackId()) != fUsedSeedsSet.end()) continue;
      track->previouseTrackId = i;

      LitScalTrack* newTrack = new LitScalTrack(*track);
//    newTrack->paramFirst = newTrack->paramLast;
      newTrack->paramLast = newTrack->paramFirst;
      fTracks[fNofTracks++] = newTrack;
//    fTracks.push_back(newTrack);
   }
}

#endif /* LITTRACKFINDERNNBASEELECTRON_H_ */
