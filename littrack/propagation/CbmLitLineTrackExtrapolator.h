// -------------------------------------------------------------------------
// -----                  CbmLitLineTrackExtrapolator header file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmLitLineTrackExtrapolator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 ** 
 **/ 

#ifndef CBMLITLINETRACKEXTRAPOLATOR_H_
#define CBMLITLINETRACKEXTRAPOLATOR_H_

#include "CbmLitTrackExtrapolator.h"

#include "TMatrixD.h"

class CbmLitTrackParam;

class CbmLitLineTrackExtrapolator: public CbmLitTrackExtrapolator {

public:
   CbmLitLineTrackExtrapolator();
   virtual ~CbmLitLineTrackExtrapolator();
   
   // Derived from CbmTool
   virtual LitStatus Initialize();
   virtual LitStatus Finalize();
   
   virtual LitStatus Extrapolate( 
		   const CbmLitTrackParam *parIn,
           CbmLitTrackParam *parOut,
           Double_t zOut);
                                 
   virtual LitStatus Extrapolate( 
		   CbmLitTrackParam *par, 
           Double_t zOut );
   
   virtual void TransportMatrix(
		   std::vector<Double_t>& F);
   virtual void TransportMatrix(
		   TMatrixD& F);
   
private:
	
	Double_t fDz;

    ClassDef(CbmLitLineTrackExtrapolator, 1);
}; 

#endif //CbmLitLineTrackExtrapolator

