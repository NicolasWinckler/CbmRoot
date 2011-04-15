/** CbmLitTrackFitter.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2008
 *
 * Interface for track fitter algorithms
 **/

#ifndef CBMLITTRACKFITTER_H_
#define CBMLITTRACKFITTER_H_

#include "base/CbmLitTool.h"
#include "base/CbmLitFloat.h"

class CbmLitTrack;

class CbmLitTrackFitter : public CbmLitTool
{
public:
   /* Constructor */
   CbmLitTrackFitter() {};

   /* Destructor */
   virtual ~CbmLitTrackFitter() {};

   /* Main function to be implemented for the concrete track fitter algorithm
    * @param track Pointer to the track to be fitted
    * @param downstream Track fit direction
    * @return Status code */
   virtual LitStatus Fit(
      CbmLitTrack* track,
      bool downstream = true) = 0;
};

#endif
