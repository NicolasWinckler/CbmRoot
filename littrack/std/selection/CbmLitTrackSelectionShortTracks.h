/* CbmLitTrackSelectionShortTracks.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 *
 * This selection removes short tracks,
 * which have a long track with the same set of hits
 */

#ifndef CBMLITTRACKSELECTIONSHORTTRACKS_H_
#define CBMLITTRACKSELECTIONSHORTTRACKS_H_

#include "interface/CbmLitTrackSelection.h"

class CbmLitTrackSelectionShortTracks : public CbmLitTrackSelection
{
public:
   /* Constructor */
   CbmLitTrackSelectionShortTracks();

   /* Destructor */
   virtual ~CbmLitTrackSelectionShortTracks();

   /* Derived from CbmLitTool */
   virtual LitStatus Initialize();

   /* Derived from CbmLitTool */
   virtual LitStatus Finalize();

   /* Derived from CbmLitTrackSelection */
   virtual LitStatus DoSelect(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd);

   /* Derived from CbmLitTrackSelection */
   virtual LitStatus DoSelect(
      TrackPtrVector& tracks);

private:
   /* Returns true if two tracks share same set of hits */
   bool IsHitSharing(
      const CbmLitTrack* track0,
      const CbmLitTrack* track1);
};

#endif /*CBMLITTRACKSELECTIONSHORTTRACKS_H_*/
