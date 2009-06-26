/** CbmLitTrackExtrapolator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2008
 **
 **  Base class for LIT track extrapolation algorithms.
 **  Extrapolation algorithm do not have to take into account material.
 **
 **/

#ifndef CBMLITTRACKEXTRAPOLATOR_H_
#define CBMLITTRACKEXTRAPOLATOR_H_

#include "CbmLitTool.h"

#include "TMatrixD.h"

#include <vector>

class CbmLitTrackParam;

class CbmLitTrackExtrapolator : public CbmLitTool {
public:
   CbmLitTrackExtrapolator();
   CbmLitTrackExtrapolator(
		   const std::string& name);
   virtual ~CbmLitTrackExtrapolator();

   // Pure virtual functions for track parameters extrapolation
   virtual LitStatus Extrapolate(
		   const CbmLitTrackParam *parIn,
           CbmLitTrackParam *parOut,
           double zOut) = 0;

   virtual LitStatus Extrapolate(
		   CbmLitTrackParam *par,
           double zOut ) = 0;

   virtual void TransportMatrix(
		   std::vector<double>& F) = 0;

   virtual void TransportMatrix(
		   TMatrixD& F) = 0;
};

#endif
