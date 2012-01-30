/** CbmLitTrackPropagator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2008
 *
 * Interface for track propagation algorithms.
 * Propagation algorithm has to take into account material and
 * can use CbmLitTrackExtrapolation to extrapolate tracks between material layers.
 **/

#ifndef CBMLITTRACKPROPAGATOR_H_
#define CBMLITTRACKPROPAGATOR_H_

#include "base/CbmLitFloat.h"
#include "base/CbmLitEnums.h"

#include <vector>

class CbmLitTrackParam;

class CbmLitTrackPropagator
{
public:
   /* Constructor */
   CbmLitTrackPropagator() {}

   /* Destructor */
   virtual ~CbmLitTrackPropagator() {}

   /* Pure virtual functions for track parameters propagation.
    * @param parIn Pointer to the initial track parameters.
    * @param parOut Pointer to the output track parameters.
    * @param zOut Z position to propagate to [cm].
    * @param pdg PDG code of the particle.
    * @param F Output transport matrix. If F == NULL than transport matrix is not calculated.
    * @return Propagation status. */
   virtual LitStatus Propagate(
      const CbmLitTrackParam* parIn,
      CbmLitTrackParam* parOut,
      myf zOut,
      int pdg,
      std::vector<myf>* F = NULL) = 0;

   /* Pure virtual functions for track parameters propagation.
    * @param par Pointer to the initial and output track parameters.
    * @param zOut Z position to propagate to [cm].
    * @param pdg PDG code of the particle.
    * @param F Output transport matrix. If F == NULL than transport matrix is not calculated.
    * @return Propagation status. */
   virtual LitStatus Propagate(
      CbmLitTrackParam* par,
      myf zOut,
      int pdg,
      std::vector<myf>* F = NULL) = 0;
};

#endif //CbmLitTrackPropagator
