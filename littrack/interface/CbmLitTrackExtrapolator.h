// -------------------------------------------------------------------------
// -----                  CbmLitTrackExtrapolator header file          -----
// -----                  Created 04/06/08  by A. Lebedev              -----
// -------------------------------------------------------------------------

/** CbmLitTrackExtrapolator.h
 *@author A.Lebedev <alebedev@jinr.ru>
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
   CbmLitTrackExtrapolator(const std::string& name);   
   virtual ~CbmLitTrackExtrapolator();

   // Pure virtual functions for track parameters extrapolation
   virtual LitStatus Extrapolate( 
		   const CbmLitTrackParam *parIn,
           CbmLitTrackParam *parOut,
           Double_t zOut) = 0;
                                 
   virtual LitStatus Extrapolate( 
		   CbmLitTrackParam *par, 
           Double_t zOut ) = 0;              
   
   virtual void TransportMatrix(
		   std::vector<Double_t>& F) = 0;
   
   virtual void TransportMatrix(
		   TMatrixD& F) = 0;
   
   ClassDef(CbmLitTrackExtrapolator,1)
};

#endif
