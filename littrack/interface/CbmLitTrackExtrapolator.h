// -------------------------------------------------------------------------
// -----                  CbmLitTrackExtrapolator header file               -----
// -----                  Created 04/06/08  by A. Lebedev               -----
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

#include "CbmTool.h"

class CbmLitTrackParam;

class CbmLitTrackExtrapolator : public CbmTool {

public:

   //Constructors
   CbmLitTrackExtrapolator();
   CbmLitTrackExtrapolator(const std::string& name);   

   //Destructors
   virtual ~CbmLitTrackExtrapolator();

   // Pure virtual functions for track parameters extrapolation
   virtual LitStatus Extrapolate( 
		   const CbmLitTrackParam *pParamIn,
           CbmLitTrackParam *pParamOut,
           Double_t zOut) = 0;
                                 
   virtual LitStatus Extrapolate( 
		   CbmLitTrackParam *pParam, 
           Double_t zOut ) = 0;              

   ClassDef(CbmLitTrackExtrapolator,1)
};

#endif

