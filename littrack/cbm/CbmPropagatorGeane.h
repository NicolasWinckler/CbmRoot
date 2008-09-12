#ifndef CBMPROPAGATORGEANE_H
#define CBMPROPAGATORGEANE_H

#include "CbmPropagator.h"

#include "CbmField.h"
#include "CbmGeanePro.h"

#include "TMatrixD.h"

#include <vector>

class CbmPropagatorGeane: public CbmPropagator {
public:
   CbmPropagatorGeane();
   virtual ~CbmPropagatorGeane();

   virtual StatusCode Propagate( 
		   const CbmTrackParam *parIn,
           CbmTrackParam *parOut,
           Double_t zOut,
           Int_t pdg);
    
   virtual StatusCode Propagate( 
		   CbmTrackParam *par, 
           Double_t zOut,
           Int_t pdg);
   
   virtual void TransportMatrix(
		   std::vector<Double_t>& F);

   virtual void TransportMatrix(
		   TMatrixD& F);
   
private:
	void ToGeaneCovMatrix(
			const std::vector<Double_t>& cov,
			std::vector<Double_t>& gCov) const;

	void FromGeaneCovMatrix(
			const std::vector<Double_t>& gCov,
			std::vector<Double_t>& cov) const;
	
	Bool_t IsInParCorrect(
			const CbmTrackParam* par) const;
	
   CbmGeanePro *fPropagator;
   
   Double_t fMinSlope;
   Double_t fMaxSlope;
      
   ClassDef(CbmPropagatorGeane,1);
}; 

#endif //CbmPropagatorGeane
