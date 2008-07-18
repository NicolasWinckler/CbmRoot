// -------------------------------------------------------------------------
// -----                  CbmLitLineTrackExtrapolator header file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmLitLineTrackExtrapolator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 ** 
 **/ 


#ifndef CBMLITLINETRACKEXTRAPOLATOR_H
#define CBMLITLINETRACKEXTRAPOLATOR_H

#include "CbmLitTrackExtrapolator.h"

#include "CbmLitTrackParam.h"

class CbmLitLineTrackExtrapolator: public CbmLitTrackExtrapolator {

public:
   
   // constructor
   CbmLitLineTrackExtrapolator();
   
   // distructor
   virtual ~CbmLitLineTrackExtrapolator();
   
   // Derived from CbmTool
   virtual LitStatus Initialize();
   virtual LitStatus Finalize();
   
   // Propagator
   virtual LitStatus Extrapolate( 
		   const CbmLitTrackParam *pParamIn,
           CbmLitTrackParam *pParamOut,
           Double_t zOut);
                                 
   virtual LitStatus Extrapolate( 
		   CbmLitTrackParam *pParam, 
           Double_t zOut );
    

   ClassDef(CbmLitLineTrackExtrapolator,1)
}; 

#endif //CbmLitLineTrackExtrapolator

