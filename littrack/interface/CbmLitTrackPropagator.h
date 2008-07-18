
// -------------------------------------------------------------------------
// -----                  CbmLitTrackPropagator header file               -----
// -----                  Created 04/06/08  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmLitTrackPropagator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 ** Base class for LIT all track propagation algorithms.
 ** Propagation algorithm has to take into account material and 
 ** may use CbmExtrapolation to extrapolate tracks between the material layers.
 **/ 

#ifndef CBMLITTRACKPROPAGATOR_H_
#define CBMLITTRACKPROPAGATOR_H_

#include "CbmTool.h"

class CbmLitTrackParam;

class CbmLitTrackPropagator : public CbmTool {

public:
   
   // constructor
   CbmLitTrackPropagator();
   CbmLitTrackPropagator(const std::string& name);
   
   // distructor
   virtual ~CbmLitTrackPropagator();
   
   // Pure virtual functions for track parameters propagation
   virtual LitStatus Propagate( 
		   const CbmLitTrackParam *pParamIn,
           CbmLitTrackParam *pParamOut,
           Double_t zOut) = 0;
                                 
   virtual LitStatus Propagate( 
		   CbmLitTrackParam *pParam, 
           Double_t zOut ) = 0;
      
   ClassDef(CbmLitTrackPropagator,1)
}; 

#endif //CbmLitTrackPropagator

