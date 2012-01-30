/** CbmLitTrackExtrapolator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2008
 *
 * Interface for track extrapolation algorithms.
 * Extrapolation algorithm do not have to take into account material effects.
 **/

#ifndef CBMLITTRACKEXTRAPOLATOR_H_
#define CBMLITTRACKEXTRAPOLATOR_H_

#include "base/CbmLitFloat.h"
#include "base/CbmLitEnums.h"

#include <vector>

class CbmLitTrackParam;

class CbmLitTrackExtrapolator
{
public:
   /* Constructor */
   CbmLitTrackExtrapolator() {}

   /* Destructor */
   virtual ~CbmLitTrackExtrapolator() {}

   /* Pure virtual function for track parameters extrapolation with calculation of transport matrix.
    * @param parIn Pointer to the initial track parameters.
    * @param parOut Pointer to the output track parameters.
    * @param zOut Z position to extrapolate to [cm].
    * @param F Output transport matrix. If F == NULL than transport matrix is not calculated.
    * @return Extrapolation status. */
   virtual LitStatus Extrapolate(
      const CbmLitTrackParam* parIn,
      CbmLitTrackParam* parOut,
      myf zOut,
      std::vector<myf>* F = NULL) = 0;

   /* Pure virtual function for track parameters extrapolation with calculation of transport matrix.
    * @param par Pointer to the initial and output track parameters.
    * @param zOut Z position to extrapolate to [cm].
    * @param F Output transport matrix. If F == NULL than transport matrix is not calculated.
    * @return Extrapolation status. */
   virtual LitStatus Extrapolate(
      CbmLitTrackParam* par,
      myf zOut,
      std::vector<myf>* F = NULL) = 0;
};

#endif
