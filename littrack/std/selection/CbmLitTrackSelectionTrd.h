/* CbmLitTrackSelectionTrd.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 *
 * Track selection for TRD.
 */

#ifndef CBMLITTRACKSELECTIONTRD_H_
#define CBMLITTRACKSELECTIONTRD_H_

#include "base/CbmLitPtrTypes.h"
#include "interface/CbmLitTrackSelection.h"

class CbmLitTrackSelectionTrd : public CbmLitTrackSelection
{
public:
   /* Constructor */
   CbmLitTrackSelectionTrd();

   /* Destructor */
   virtual ~CbmLitTrackSelectionTrd();

   /* Derived from CbmLitTrackSelection */
   virtual LitStatus DoSelect(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd);

   /* Derived from CbmLitTrackSelection */
   virtual LitStatus DoSelect(
      TrackPtrVector& tracks);

   /* Returns maximum number of shared hits */
   int GetNofSharedHits() const {return fNofSharedHits;}

   /* Sets maximum number of shared hits */
   void SetNofSharedHits(int nofHits) {fNofSharedHits = nofHits;}

   /* Returns minimum number of hits cut */
   int GetMinNofHits() const {return fMinNofHits;}

   /* Sets minimum number of hits cut */
   void SetMinNofHits(int nofHits) {fMinNofHits = nofHits;}

private:
   /* Shared hits track selection tool */
   TrackSelectionPtr fSharedHitsSelection;
   /* Same seed track selection tool */
   TrackSelectionPtr fSameSeedSelection;
   /* Track selection based on cuts */
   TrackSelectionPtr fCutsSelection;

   /* Maximum number of shared hits */
   int fNofSharedHits;
   /* Minimum number of hits cut */
   int fMinNofHits;
};

#endif /*CBMLITTRACKSELECTIONTRD_H_*/
