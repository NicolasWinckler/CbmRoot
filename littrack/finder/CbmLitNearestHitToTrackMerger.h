/** CbmLitNearestHitToTrackMerger.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 *
 * Class implements hit-to-track merging
 * using nearest hit approach.
 **/

#ifndef CBMLITNEARESTHITTOTRACKMERGER_H_
#define CBMLITNEARESTHITTOTRACKMERGER_H_

#include "interface/CbmLitHitToTrackMerger.h"
#include "finder/CbmLitGating.h"
#include "base/CbmLitPtrTypes.h"
#include "base/CbmLitStation.h"

class CbmLitTrackPropagator;
class CbmLitTrackUpdate;

class CbmLitNearestHitToTrackMerger : public CbmLitHitToTrackMerger,
   public CbmLitGating
{
public:
   /* Constructor */
   CbmLitNearestHitToTrackMerger();

   /* Destructor */
   virtual ~CbmLitNearestHitToTrackMerger();

   /* Inherited from CbmLitTool */
   virtual LitStatus Initialize();

   /* Inherited from CbmLitTool */
   virtual LitStatus Finalize();

   /* Inherited from CbmLitHitToTrackMerger */
   virtual LitStatus DoMerge(
      HitPtrVector& hits, //TODO: add const here
      TrackPtrVector& tracks);

   /* Sets station to merge at */
   void SetStation(const CbmLitStation& station) {fStation = station;}

   /* Sets track propagation tool */
   void SetPropagator(TrackPropagatorPtr propagator) {fPropagator = propagator;}

   /* Sets track update tool */
   void SetFilter(TrackUpdatePtr filter) {fFilter = filter;}

   /* Sets PDG hypothesis */
   void SetPDG(int pdg) {fPDG = pdg;}

private:
   /* Attaches nearest hit to track */
   bool AddNearestHit(
      CbmLitTrack* track,
      HitPtrIteratorPair bounds) const;

private:
   /* Track propagation tool */
   TrackPropagatorPtr fPropagator;
   /* Track update tool */
   TrackUpdatePtr fFilter;
   /* station to merge at */
   CbmLitStation fStation;
   /* PDG hypothesis */
   int fPDG;
};

#endif /* CBMLITNEARESTHITTOTRACKMERGER_H_ */
