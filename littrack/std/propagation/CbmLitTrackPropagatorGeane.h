/** CbmTrackPropagatorGeane.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 ** The class implements an interface between GEANE track propagation tool and
 ** Littrack package.
 **/

#ifndef CBMLITTRACKPROPAGATORGEANE_H
#define CBMLITTRACKPROPAGATORGEANE_H

#include "interface/CbmLitTrackPropagator.h"

#include <vector>

class CbmPropagator;

class CbmLitTrackPropagatorGeane: public CbmLitTrackPropagator
{
public:
   /* Constructor */
   CbmLitTrackPropagatorGeane();

   /* Destructor */
   virtual ~CbmLitTrackPropagatorGeane();

   /* Inherited from CbmLitTrackPropagator */
   virtual LitStatus Propagate(
      const CbmLitTrackParam* parIn,
      CbmLitTrackParam* parOut,
      litfloat zOut,
      int pdg,
      std::vector<litfloat>* F);

   /* Inherited from CbmLitTrackPropagator */
   virtual LitStatus Propagate(
      CbmLitTrackParam* pParam,
      litfloat zOut,
      int pdg,
      std::vector<litfloat>* F);

private:
   CbmPropagator* fPropagator; // FairRoot interface to GEANE
};

#endif //CbmLitTrackPropagatorGeane
