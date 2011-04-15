/* CbmLitTrackSelectionSameSeed.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 2.0
 *
 * Track selection that selects the best track
 * for each subset of tracks with the same previous track indices.
 */

#ifndef CBMLITTRACKSELECTIONSAMESEED_H_
#define CBMLITTRACKSELECTIONSAMESEED_H_

#include "interface/CbmLitTrackSelection.h"
#include "selection/CbmLitQualitySort.h"

class CbmLitTrackSelectionSameSeed : public CbmLitTrackSelection,
   public CbmLitQualitySort
{
public:
   /* Constructor */
   CbmLitTrackSelectionSameSeed();

   /* Destructor */
   virtual ~CbmLitTrackSelectionSameSeed();

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
};

#endif /*CBMLITTRACKSELECTIONSAMESEED_H_*/
