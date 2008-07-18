 
 
// -------------------------------------------------------------------------
// -----                  CbmLitParabolicTrackExtrapolator header file               -----
// -----                  Created 30/10/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmLitParabolicTrackExtrapolator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 ** 
 **/ 



#ifndef CBMLITPARABOLICTRACKEXTRAPOLATOR_H
#define CBMLITPARABOLICTRACKEXTRAPOLATOR_H

#include "CbmLitTrackExtrapolator.h"

#include "CbmLitTrackParam.h"
#include "CbmField.h"

class CbmLitParabolicTrackExtrapolator: public CbmLitTrackExtrapolator {

public:
   
   // constructor
   CbmLitParabolicTrackExtrapolator();
   
   // distructor
   virtual ~CbmLitParabolicTrackExtrapolator();
   
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
   

   CbmField *fMagneticField;
   
   ClassDef(CbmLitParabolicTrackExtrapolator,1)
}; 

#endif //CbmLitParabolicTrackExtrapolator

