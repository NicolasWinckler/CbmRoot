#include "LitTrackFinderNNVecElectron.h"

#include "LitDetectorGeometryElectron.h"
#include "LitHitDataElectron.h"
#include "../LitTypes.h"
#include "../LitHit.h"
#include "../LitAddMaterial.h"
#include "../LitExtrapolation.h"
#include "../LitFiltration.h"
#include "../LitField.h"
#include "../LitMath.h"
#include "../LitVecPack.h"
#include "../LitTrack.h"

//#include "CbmLitMemoryManagment.h"

#ifdef LIT_USE_TBB
#undef LIT_USE_TBB
#endif

#define LIT_USE_TBB // TBB will be used for multithreading

#ifdef LIT_USE_TBB
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_sort.h"
#endif // LIT_USE_TBB

#include <algorithm>
#include <iostream>
#include <limits>

#define cnst static const fvec

#ifdef LIT_USE_TBB
class PropagateToFirstStationClass
{
public:
   void operator() ( const tbb::blocked_range<unsigned int>& r ) const {
      for (unsigned int iTrack = r.begin(); iTrack != r.end(); ++iTrack) {
         unsigned int start = fvecLen * iTrack;
         // Collect track group
         lit::parallel::LitScalTrack* tracks[fvecLen];
         for(unsigned int i = 0; i < fvecLen; i++) { tracks[i] = fTracks[fTracksId[start + i]]; }
         fFinder->PropagateToFirstStation(tracks);
      }
   }

   PropagateToFirstStationClass(
      lit::parallel::LitTrackFinderNNVecElectron* finder,
      lit::parallel::LitScalTrack* tracks[],
      unsigned int* tracksId) :
      fFinder(finder),
      fTracks(tracks),
      fTracksId(tracksId) {}

private:
   lit::parallel::LitTrackFinderNNVecElectron* fFinder;
   lit::parallel::LitScalTrack** fTracks;
   unsigned int* fTracksId;
};



class ProcessStationClass
{
   lit::parallel::LitScalTrack** fTracks;
   lit::parallel::LitTrackFinderNNVecElectron* fFinder;
   unsigned int* fTracksId;
   unsigned char fStationGroup;
   unsigned char fStation;
public:
   void operator() ( const tbb::blocked_range<unsigned int>& r ) const {
      for (unsigned int iTrack = r.begin(); iTrack != r.end(); ++iTrack) {
         unsigned int start = fvecLen * iTrack;
         // Collect track group
         lit::parallel::LitScalTrack* tracks[fvecLen];
         for(unsigned int i = 0; i < fvecLen; i++) { tracks[i] = fTracks[fTracksId[start + i]]; }
         fFinder->ProcessStation(tracks, fStationGroup, fStation);
      }
   }
   ProcessStationClass(
      lit::parallel::LitTrackFinderNNVecElectron* finder,
      lit::parallel::LitScalTrack** tracks,
      unsigned int* tracksId,
      unsigned char stationGroup,
      unsigned char station) :
      fFinder(finder),
      fTracks(tracks),
      fTracksId(tracksId),
      fStationGroup(stationGroup),
      fStation(station) {}
};



class CollectHitsClass
{
   lit::parallel::LitTrackFinderNNVecElectron* fFinder;
   lit::parallel::LitTrackParam<fvec> flpar;
   lit::parallel::LitScalTrack** fTracks;
   unsigned char fStationGroup;
   unsigned char fStation;

public:
   void operator() ( const tbb::blocked_range<unsigned int>& r ) const {
      for (unsigned int i = r.begin(); i != r.end(); ++i) {
         lit::parallel::LitTrackParamScal spar;
         UnpackTrackParam(i, flpar, spar);

         fFinder->CollectHits(&spar, fTracks[i], fStationGroup, fStation);
      }
   }

   CollectHitsClass(
      lit::parallel::LitTrackFinderNNVecElectron* finder,
      lit::parallel::LitTrackParam<fvec> lpar,
      lit::parallel::LitScalTrack** tracks,
      unsigned char stationGroup,
      unsigned char station):
      fFinder(finder),
      flpar(lpar),
      fTracks(tracks),
      fStationGroup(stationGroup),
      fStation(station) {}
};

#endif //LIT_USE_TBB


lit::parallel::LitTrackFinderNNVecElectron::LitTrackFinderNNVecElectron()
{
   SetSigmaCoef(5.);
   SetMaxCovSq(20.*20.);
   SetMaxNofMissingHits(3);

#ifdef LIT_USE_TBB
   tbb::task_scheduler_init init;
#endif
}

lit::parallel::LitTrackFinderNNVecElectron::~LitTrackFinderNNVecElectron()
{
}

void lit::parallel::LitTrackFinderNNVecElectron::DoFind(
   LitScalPixelHit* hits[],
   unsigned int nofHits,
   LitScalTrack* trackSeeds[],
   unsigned int nofTrackSeeds,
   LitScalTrack* tracks[],
   unsigned int& nofTracks)
{
// std::cout << "Tracking started...nofHits="
//    << nofHits << " nofTrackSeeds=" << nofTrackSeeds <<  std::endl;

   ArrangeHits(hits, nofHits);
// std::cout << "Arrange hits finished..." << std::endl;
   InitTrackSeeds(trackSeeds, nofTrackSeeds);
// std::cout << "Init track seeds finished..." << std::endl;
   FollowTracks();
// std::cout << "Follow tracks finished..." << std::endl;

   //Copy tracks to output
   nofTracks = 0;
   for (unsigned int i = 0; i < fNofTracks; i++) {
      LitScalTrack* track = fTracks[i];
//    std::cout << *track;
//    if (track->nofHits < 11) continue;
//    std::cout << *track;
      tracks[nofTracks++] = new LitScalTrack(*track);
   }
// std::cout << "Number of found tracks: " << nofTracks << std::endl;

// for (unsigned int i = 0; i < nofTracks; i++)
//    std::cout << *tracks[i];

   for (unsigned int i = 0; i < fNofTracks; i++) {
      delete fTracks[i];
   }
   fNofTracks = 0;
// for_each(fTracks.begin(), fTracks.end(), DeleteObject());
// fTracks.clear();
   fHitData.Clear();
}

void lit::parallel::LitTrackFinderNNVecElectron::FollowTracks()
{
   // temporary arrays to store track indexes from the fTracks array
   unsigned int tracksId1[fNofTracks];
   unsigned int tracksId2[fNofTracks];
   unsigned int nofTracksId1 = fNofTracks;
   unsigned int nofTracksId2 = 0;

   //Initially use all tracks from fTracks array
   for (unsigned int i = 0; i < nofTracksId1; i++) { tracksId1[i] = i; }

   //First propagate all tracks to the first station
   unsigned int nofTracks = nofTracksId1; // number of tracks
   unsigned int nofTracksVec = nofTracks / fvecLen; // number of tracks grouped in vectors
   unsigned int dTracks = nofTracks - fvecLen * nofTracksVec; // number of tracks remained after grouping in vectors

   // loop over fTracks, pack data and propagate to the first station
#ifdef LIT_USE_TBB
   tbb::parallel_for(tbb::blocked_range<unsigned int>(0, nofTracksVec),
                     PropagateToFirstStationClass(this, fTracks, tracksId1), tbb::auto_partitioner());
#else
   for (unsigned int iTrack = 0; iTrack < nofTracksVec; iTrack++) {
      unsigned int start = fvecLen * iTrack;
      // Collect track group
      LitScalTrack* tracks[fvecLen];
      for(unsigned int i = 0; i < fvecLen; i++) { tracks[i] = fTracks[tracksId1[start + i]]; }
      PropagateToFirstStation(tracks);
   } // loop over tracks
#endif
   // Propagate remaining dTracks to the first station
   if (dTracks > 0) {
      LitScalTrack* tracks[fvecLen];
      LitScalTrack dummyTracks[fvecLen - dTracks];
      unsigned int start = fvecLen * nofTracksVec;
      for(unsigned int i = 0; i < dTracks; i++) { tracks[i] = fTracks[tracksId1[start + i]]; }
      // Check if the number of remaining tracks is less than fvecLen.
      if (dTracks < fvecLen) {
         for(unsigned int i = 0; i < fvecLen - dTracks; i++) { tracks[dTracks+i] = &dummyTracks[i]; }//tracks[dTracks-1];
      }
      PropagateToFirstStation(tracks);
   }
   // end propagation to the first station

// std::cout << "Propagation to the first station finished..." << std::endl;


   // Main loop over station groups for track following
   unsigned char nofStationGroups = fLayout.GetNofStationGroups();
   for(unsigned char iStationGroup = 0; iStationGroup < nofStationGroups; iStationGroup++) { // loop over station groups
//    std::cout << "Process station group " << (int)iStationGroup << "/" << (int)nofStationGroups << std::endl;

      LitStationGroupElectron<fvec>& stg = fLayout.stationGroups[iStationGroup];

      // Loop over stations, and propagate tracks from station to station
      unsigned char nofStations = stg.GetNofStations();
      for(unsigned char iStation = 0; iStation < nofStations; iStation++) { // loop over stations

//       std::cout << "tracksId1.size()=" << tracksId1.size() << std::endl;
         unsigned int nofTracks = nofTracksId1; // number of tracks
         unsigned int nofTracksVec = nofTracks / fvecLen; // number of tracks grouped in vectors
         unsigned int dTracks = nofTracks - fvecLen * nofTracksVec; // number of tracks remained after grouping in vectors
//       std::cout << "iStation --> nofTracks=" << nofTracks << " nofTracksVec=" << nofTracksVec
//          << " dTracks=" << dTracks << std::endl;
#ifdef LIT_USE_TBB
         tbb::parallel_for(tbb::blocked_range<unsigned int>(0, nofTracksVec),
                           ProcessStationClass(this, fTracks, tracksId1, iStationGroup, iStation), tbb::auto_partitioner());
#else
         for (unsigned int iTrack = 0; iTrack < nofTracksVec; iTrack++) { // loop over tracks
            unsigned int start = fvecLen * iTrack;
            // Collect track group
            LitScalTrack* tracks[fvecLen];
            for(unsigned int i = 0; i < fvecLen; i++) { tracks[i] = fTracks[tracksId1[start + i]]; }
            ProcessStation(tracks, iStationGroup, iStation);
         } // loop over tracks
#endif
         // Propagate remaining dTracks
         if (dTracks > 0) {
//          std::cout << "Process remaining dTracks=" << dTracks << " " << (int) iStation << ":" << (int) iStationGroup << std::endl;
            LitScalTrack* tracks[fvecLen];
            LitScalTrack dummyTracks[fvecLen - dTracks];
            unsigned int start = fvecLen * nofTracksVec;
            for(unsigned int i = 0; i < dTracks; i++) { tracks[i] = fTracks[tracksId1[start + i]]; }
            // Check if the number of remaining tracks is less than fvecLen.
            if (dTracks < fvecLen) {
               for(unsigned int i = 0; i < fvecLen - dTracks; i++) { tracks[dTracks+i] = &dummyTracks[i]; }//tracks[dTracks-1];
            }
            ProcessStation(tracks, iStationGroup, iStation);
         }

         for (unsigned int iTrack = 0; iTrack < nofTracks; iTrack++) {
            unsigned int id = tracksId1[iTrack];
            if (fTracks[id]->nofMissingHits <= fMaxNofMissingHits) {
               tracksId2[nofTracksId2++]= id;
            }
         }
         for (unsigned int i = 0; i < nofTracksId2; i++) { tracksId1[i] = tracksId2[i]; }
         nofTracksId1 = nofTracksId2;
         nofTracksId2 = 0;
      } // loop over stations

   } // loop over station groups
}

void lit::parallel::LitTrackFinderNNVecElectron::PropagateToFirstStation(
   LitScalTrack* tracks[])
{
   // Pack track parameters
   LitTrackParamScal par[fvecLen];
   for(unsigned int i = 0; i < fvecLen; i++) { par[i] = tracks[i]->paramLast; }
   LitTrackParamVec lpar;
   PackTrackParam(par, lpar);

   for (unsigned char ivp = 0; ivp < fLayout.GetNofVirtualPlanes()-1; ivp++) {
      LitVirtualPlaneElectronVec& vp1 = fLayout.virtualPlanes[ivp];
      LitVirtualPlaneElectronVec& vp2 = fLayout.virtualPlanes[ivp+1];

      LitRK4ExtrapolationElectron(lpar, vp2.Z, vp1.fieldSlice, vp1.fieldSliceMid, vp2.fieldSlice);
      LitAddMaterialElectron(lpar, vp2.material);
   }

   // Unpack track parameters
   UnpackTrackParam(lpar, par);
   for(unsigned int i = 0; i < fvecLen; i++) { tracks[i]->paramLast = par[i]; }
}

void lit::parallel::LitTrackFinderNNVecElectron::ProcessStation(
   LitScalTrack* tracks[],
   unsigned char stationGroup,
   unsigned char station)
{
// std::cout << "Processing station " << (int) station << ":" << (int)stationGroup << std::endl;
   LitStationGroupElectron<fvec>& stg = fLayout.stationGroups[stationGroup];
   LitStationElectron<fvec>& sta = stg.stations[station];

   // Pack track parameters
   LitTrackParamScal par[fvecLen];
   for(unsigned int i = 0; i < fvecLen; i++) { par[i] = tracks[i]->paramLast; }
   LitTrackParam<fvec> lpar;
   PackTrackParam(par, lpar);

   // Propagate to station
   LitLineExtrapolation(lpar, sta.Z);
   for (unsigned char im = 0; im < sta.GetNofMaterialsBefore(); im++) {
      LitAddMaterialElectron(lpar, sta.materialsBefore[im]);
   }

#ifdef LIT_USE_TBB
   tbb::parallel_for(tbb::blocked_range<unsigned int>(0, fvecLen, 1),
                     CollectHitsClass(this, lpar, tracks, stationGroup, station));
#else
   UnpackTrackParam(lpar, par);
   for(unsigned int i = 0; i < fvecLen; i++) {
      CollectHits(&par[i], tracks[i], stationGroup, station);
   }
#endif // LIT_USE_TBB

   for (unsigned char im = 0; im < sta.GetNofMaterialsAfter(); im++) {
      LitAddMaterialElectron(lpar, sta.materialsAfter[im]);
   }
}

void lit::parallel::LitTrackFinderNNVecElectron::CollectHits(
   LitTrackParamScal* par,
   LitScalTrack* track,
   unsigned char stationGroup,
   unsigned char station)
{
// std::cout << "Collecting hits " << (int) station << ":" << (int) stationGroup << std::endl;
   std::pair<unsigned int, unsigned int> hits;

   track->paramLast = *par;

   LitScalPixelHit** hitvec = fHitData.GetHits(stationGroup, station);
   unsigned int nh = fHitData.GetNofHits(stationGroup, station);
   fscal err = fHitData.GetMaxErr(stationGroup, station);

   MinMaxIndex(par, hitvec, nh, err, hits.first, hits.second);
   unsigned int nofHits = hits.second - hits.first;

   bool hitAdded = AddNearestHit(track, hits, nofHits, stationGroup, station);

   // Check if hit was added, if not than increase number of missing hits
   if (!hitAdded) { track->nofMissingHits++; }
}

bool lit::parallel::LitTrackFinderNNVecElectron::AddNearestHit(
   LitScalTrack* track,
   const std::pair<unsigned int, unsigned int>& hits,
   unsigned int nofHits,
   int stationGroup,
   int station)
{
   cnst CHISQCUT = 50.;

   bool hitAdded = false;
   LitScalPixelHit* hita = NULL;
   LitTrackParamScal param;
   fscal chiSq = std::numeric_limits<fscal>::max();

   // Pack track parameters
   LitTrackParamScal pars[fvecLen];
   for(unsigned int i = 0; i < fvecLen; i++) { pars[i] = track->paramLast; }
   LitTrackParam<fvec> lpar;
   PackTrackParam(pars, lpar);

   LitScalPixelHit** hitvec = fHitData.GetHits(stationGroup, station);
   unsigned int nofHitsVec = nofHits / fvecLen; // number of hits grouped in vectors
   unsigned int dHits = nofHits - fvecLen * nofHitsVec; // number of hits remained after grouping in vectors
   for (unsigned int iHit = 0; iHit < nofHitsVec; iHit++) {
      unsigned int start = hits.first + fvecLen * iHit;

      // Pack hit
      LitScalPixelHit hit[fvecLen];
      for(unsigned int i = 0; i < fvecLen; i++) { hit[i] = *hitvec[start + i]; }
      LitPixelHit<fvec> lhit;
      PackPixelHit(hit, lhit);

      LitTrackParam<fvec> ulpar = lpar;

      //First update track parameters with KF, than check whether the hit is in the validation gate.
      LitFiltration(ulpar, lhit);
      fvec chisq = ChiSq(ulpar, lhit);

      // Unpack track parameters
      UnpackTrackParam(ulpar, pars);
      for (unsigned int i = 0; i < fvecLen; i++) {
         if (chisq[i] < CHISQCUT[i] && chisq[i] < chiSq) {
            chiSq = chisq[i];
            hita = hitvec[start + i];//hit[start + i];
            param = pars[i];
         }
      }
   }
   if (dHits > 0) {
      unsigned int start = hits.first + fvecLen * nofHitsVec;
      LitScalPixelHit hit[fvecLen];
      LitPixelHit<fvec> lhit;
      LitTrackParamScal pars[fvecLen];
      LitTrackParam<fvec> lpar;
      for(unsigned int i = 0; i < dHits; i++) {
         hit[i] = *hitvec[start + i];
         pars[i] = track->paramLast;
      }
      // Check if the number of remaining tracks is less than fvecLen.
      if (dHits < fvecLen) {
         for(unsigned int i = 0; i < fvecLen - dHits; i++) {
            hit[dHits+i] = *hitvec[start + dHits - 1];
            pars[dHits+i] = track->paramLast;
         }
      }
      PackPixelHit(hit, lhit);
      PackTrackParam(pars, lpar);

      //First update track parameters with KF, than check whether the hit is in the validation gate.
      LitFiltration(lpar, lhit);
      fvec chisq = ChiSq(lpar, lhit);

      // Unpack track parameters
      UnpackTrackParam(lpar, pars);
      for (unsigned int i = 0; i < fvecLen; i++) {
         if (chisq[i] < CHISQCUT[i] && chisq[i] < chiSq) {
            chiSq = chisq[i];
            hita = hitvec[start + i];
            param = pars[i];
         }
      }
   }

   // if hit was attached than change track information
   if (hita != NULL) {
      track->AddHit(hita);
      track->paramLast = param;
      track->chiSq += chiSq;
      track->NDF = NDF(*track);
      hitAdded = true;
   }
   return hitAdded;
}

#undef cnst
