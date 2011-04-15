/** CbmLitTrackFinder.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2008
 **
 ** Interface for track finding algorithms.
 **/

#ifndef CBMLITTRACKFINDER_H_
#define CBMLITTRACKFINDER_H_

#include "base/CbmLitTool.h"
#include "base/CbmLitTypes.h"

class CbmLitTrackFinder : public CbmLitTool
{
public:
   /* Constructor */
   CbmLitTrackFinder() {};

   /* Destructor */
   virtual ~CbmLitTrackFinder() {};

   /* Main function to be implemented for the concrete track finder algorithm
    * @param hits Input vector of hits
    * @param trackSeeds Input vector of track seeds
    * @param tracks Output vector of found tracks
    * @return Status code */
   virtual LitStatus DoFind(
      HitPtrVector& hits,
      TrackPtrVector& trackSeeds,
      TrackPtrVector& tracks) = 0;
};

#endif /*CBMLITTRACKFINDER_H_*/
