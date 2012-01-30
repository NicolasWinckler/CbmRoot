#ifndef CBMLITTRACKFINDERWEIGHT_H_
#define CBMLITTRACKFINDERWEIGHT_H_

#include "finder/CbmLitTrackFinderBase.h"
#include "data/CbmLitHitChiSq.h"

class CbmLitTrackFinderWeight : public CbmLitTrackFinderBase
{
public:
   CbmLitTrackFinderWeight();
   virtual ~CbmLitTrackFinderWeight();

   LitStatus DoFind(
      HitPtrVector& hits,
      TrackPtrVector& trackSeeds,
      TrackPtrVector& tracks);

   void SetSeedSelection(TrackSelectionPtr seedSelection) {fSeedSelection=seedSelection;}
   void SetPropagator(TrackPropagatorPtr propagator) {fPropagator = propagator;}
   void SetFinalSelection(TrackSelectionPtr finalSelection) {fFinalSelection = finalSelection;}
   void SetFitter(TrackFitterPtr fitter) { fFitter = fitter;}
   void SetFilter(TrackUpdatePtr filter) { fFilter = filter;}

protected:
   void SetIterationParameters(
      int iter);

   void InitTrackSeeds(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd);

   void FollowTracks(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd);

   void FollowTrack(
      CbmLitTrack* track);

   bool ProcessStationGroup(
      CbmLitTrack* track,
      int stationGroup);

   bool ProcessStation(
      CbmLitTrack* track,
      int stationGroup,
      int station);

   bool AddHits(
      CbmLitTrack* track,
      HitPtrIteratorPair bounds,
      std::vector<CbmLitHitChiSq>& hits);

   void FitTracks(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd);
private:
   TrackPtrVector fTracks;

   TrackFitterPtr fFitter;
   TrackUpdatePtr fFilter;
   TrackSelectionPtr fSeedSelection;
   TrackSelectionPtr fFinalSelection;
   TrackPropagatorPtr fPropagator;

   int fMaxNofHitsInValidationGate;
};

#endif /*CBMLITTRACKFINDERWEIGHT_H_*/
