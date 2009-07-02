/** CbmLitTrackPropagator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2008
 **
 ** Base class for LIT all track propagation algorithms.
 ** Propagation algorithm has to take into account material and
 ** may use CbmExtrapolation to extrapolate tracks between the material layers.
 **/

#ifndef CBMLITTRACKPROPAGATOR_H_
#define CBMLITTRACKPROPAGATOR_H_

#include "CbmLitTool.h"
#include "CbmLitFloat.h"

#include <vector>

class CbmLitTrackParam;

class CbmLitTrackPropagator : public CbmLitTool {

public:
   CbmLitTrackPropagator();
   CbmLitTrackPropagator(const std::string& name);
   virtual ~CbmLitTrackPropagator();

   // Pure virtual functions for track parameters propagation
   virtual LitStatus Propagate(
		   const CbmLitTrackParam *parIn,
           CbmLitTrackParam *parOut,
           myf zOut,
           int pdg) = 0;

   virtual LitStatus Propagate(
		   CbmLitTrackParam *par,
           myf zOut,
           int pdg) = 0;

   virtual void TransportMatrix(
		   std::vector<myf>& F) = 0;
};

#endif //CbmLitTrackPropagator

