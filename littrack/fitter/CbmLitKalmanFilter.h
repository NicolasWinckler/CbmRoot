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

class CbmLitHit;
class CbmLitStripHit;
class CbmLitPixelHit;
class CbmLitTrackParam;

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
		   const CbmLitTrackParam *parIn,
           CbmLitTrackParam *parOut,
           const CbmLitHit *hit);
                              
   virtual LitStatus Update( 
		   CbmLitTrackParam *par, 
		   const CbmLitHit *hit);
   
private:
	LitStatus Update( 
		   CbmLitTrackParam *par, 
		   const CbmLitPixelHit *hit);
	
	LitStatus Update( 
		   CbmLitTrackParam *par, 
		   const CbmLitStripHit *hit);
   
	ClassDef(CbmLitKalmanFilter,1)
}; 

#endif //CbmLitKalmanFilter

