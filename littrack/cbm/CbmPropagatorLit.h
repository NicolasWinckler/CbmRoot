#ifndef CBMPROPAGATORLIT_H_
#define CBMPROPAGATORLIT_H_

#include "CbmPropagator.h"

class FairTrackParam;
class CbmLitTrackPropagator;

class CbmPropagatorLit : public CbmPropagator
{
public:
	// type = "line" - uses line track extrapolation
	// type = "rk4" - uses Runge-Kutta 4th order track extrapolation
	CbmPropagatorLit(
			const std::string& type);
	virtual ~CbmPropagatorLit();
	
    virtual StatusCode Propagate( 
		   const FairTrackParam *parIn,
           FairTrackParam *parOut,
           Double_t zOut,
           Int_t pdg);
    
    virtual StatusCode Propagate( 
		   FairTrackParam *par, 
           Double_t zOut,
           Int_t pdg);
   
    virtual void TransportMatrix(
		   std::vector<Double_t>& F);

    virtual void TransportMatrix(
		   TMatrixD& F);
   
private:
    CbmLitTrackPropagator* fPropagator;
   
   ClassDef(CbmPropagatorLit, 1);
};

#endif /*CBMPROPAGATORLIT_H_*/
