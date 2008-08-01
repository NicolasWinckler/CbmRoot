// -------------------------------------------------------------------------
// -----                  CbmTrackPropagatorGeane header file               -----
// -----                  Created 22/11/07  by A. Lebedev               -----
// -------------------------------------------------------------------------

/** CbmTrackPropagatorGeane.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 **
 **/ 

#ifndef CBMTRACKPROPAGATORGEANE_H
#define CBMTRACKPROPAGATORGEANE_H

#include "CbmLitTrackPropagator.h"

#include "CbmField.h"
#include "CbmGeanePro.h"

#include "TMatrixD.h"

#include <vector>

class CbmTrackPropagatorGeane: public CbmLitTrackPropagator {

public:
   
   // constructor
   CbmTrackPropagatorGeane();
      
   // distructor
   virtual ~CbmTrackPropagatorGeane();
   
   // derived from CbmTool
   virtual LitStatus Initialize();
   virtual LitStatus Finalize();
   
   // Propagator
   virtual LitStatus Propagate( 
		   const CbmLitTrackParam *parIn,
           CbmLitTrackParam *parOut,
           Double_t zOut,
           Int_t pdg);
                                 
   virtual LitStatus Propagate( 
		   CbmLitTrackParam *pParam, 
           Double_t zOut,
           Int_t pdg);
   
   virtual void TransportMatrix(
		   std::vector<Double_t>& F){
	   //TODO: implement transport matrix for geane 
   }
   virtual void TransportMatrix(
		   TMatrixD& F){
	   //TODO: implement transport matrix for geane 
   }
   
private:
	void ToGeaneCovMatrix(
			const std::vector<Double_t>& cov,
			std::vector<Double_t>& gCov);

	void FromGeaneCovMatrix(
			const std::vector<Double_t>& gCov,
			std::vector<Double_t>& cov);
	
	Bool_t IsInParCorrect(
			const CbmLitTrackParam* par);
	
   CbmGeanePro *fPropagator;
      
   ClassDef(CbmTrackPropagatorGeane,1)
}; 

#endif //CbmTrackPropagatorGeane

