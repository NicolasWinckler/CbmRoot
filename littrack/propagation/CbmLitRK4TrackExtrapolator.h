 
// -------------------------------------------------------------------------
// -----                  CbmLitRK4TrackExtrapolator header file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmLitRK4TrackExtrapolator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 ** 
 **/ 



#ifndef CBMLITRK4TRACKEXTRAPOLATOR_H
#define CBMLITRK4TRACKEXTRAPOLATOR_H

#include "CbmLitTrackExtrapolator.h"

#include "CbmLitTrackParam.h"
#include "CbmField.h"

class CbmLitRK4TrackExtrapolator: public CbmLitTrackExtrapolator {

public:
   
   // constructor
   CbmLitRK4TrackExtrapolator();
   
   // distructor
   virtual ~CbmLitRK4TrackExtrapolator();
   
   // derived from CbmTool
   virtual LitStatus Initialize();
   virtual LitStatus Finalize();
   
   // Extrapolation 
   virtual LitStatus Extrapolate( 
		   const CbmLitTrackParam *pParamIn,
           CbmLitTrackParam *pParamOut,
           Double_t zOut);
                                 
   virtual LitStatus Extrapolate( 
		   CbmLitTrackParam *pParam, 
           Double_t zOut );
   
protected:
   
   void RK4Order( Double_t zIn ,
                  Double_t xIn[] ,
                  Double_t zOut,
                  Double_t xOut[],
                  Double_t derivs[] );
                  
   void RK4Step( Double_t zIn,
                 Double_t xIn[],
                 Double_t zOut,
                 Double_t cOut[],
                 Double_t derivs[] );
   
   void RK4Fast( Double_t zIn ,    
                 Double_t xIn[],   
                 Double_t zOut,    
                 Double_t xOut[] );
   
   Double_t fStepSizeMin;
   Double_t fAccError;
   CbmField *fMagneticField;
   
   ClassDef(CbmLitRK4TrackExtrapolator,1)
}; 

#endif //CbmLitRK4TrackExtrapolator

