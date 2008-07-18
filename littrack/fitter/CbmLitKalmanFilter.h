// -------------------------------------------------------------------------
// -----                  CbmLitKalmanFilter header file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmLitKalmanFilter.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 ** 
 **/ 


#ifndef CBMLITKALMANFILTER_H
#define CBMLITKALMANFILTER_H

#include "CbmLitTrackUpdate.h"

#include "CbmLitHit.h"
#include "CbmLitTrackParam.h"


class CbmLitKalmanFilter: public CbmLitTrackUpdate 
{

public:

   CbmLitKalmanFilter();
   
   virtual ~CbmLitKalmanFilter();
   
   // derived from CbmTool
   virtual LitStatus Initialize();
   virtual LitStatus Finalize();
   
   // Filter
   virtual LitStatus Update(
		   const CbmLitTrackParam *pParamIn,
           CbmLitTrackParam *pParamOut,
           const CbmLitHit *pHit);
                              
   virtual LitStatus Update( 
		   CbmLitTrackParam *pParam, 
		   const CbmLitHit *pHit);
   
   ClassDef(CbmLitKalmanFilter,1)
}; 

#endif //CbmLitKalmanFilter

