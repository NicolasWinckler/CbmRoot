// -------------------------------------------------------------------------
// -----                  CbmLitRK4TrackExtrapolator header file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmLitRK4TrackExtrapolator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 ** 
 **/ 

#ifndef CBMLITRK4TRACKEXTRAPOLATOR_H_
#define CBMLITRK4TRACKEXTRAPOLATOR_H_

#include "CbmLitTrackExtrapolator.h"

#include "CbmLitTrackParam.h"
#include "FairField.h"

#include <vector>

class CbmLitRK4TrackExtrapolator: public CbmLitTrackExtrapolator {
public:
   CbmLitRK4TrackExtrapolator();
   virtual ~CbmLitRK4TrackExtrapolator();
   
   // derived from CbmTool
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
   
protected:
   
   void RK4Order(
		   const std::vector<Double_t>& xIn,
		   Double_t zIn,
		   std::vector<Double_t>& xOut,
           Double_t zOut,
           std::vector<Double_t>& derivs) const;
   
   Double_t CalcOut(
   		Double_t in,
   		const Double_t k[4]) const;
  
   void TransportC(
   		const std::vector<Double_t>& cIn,
   		const std::vector<Double_t>& F,
   		std::vector<Double_t>& cOut) const;

private:

	std::vector<Double_t> fF;
	FairField *fMagneticField;
   
   ClassDef(CbmLitRK4TrackExtrapolator,1)
}; 

#endif //CBMLITRK4TRACKEXTRAPOLATOR_H_

