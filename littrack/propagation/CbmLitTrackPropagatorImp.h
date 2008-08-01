// 
// -------------------------------------------------------------------------
// -----                  CbmLitTrackPropagatorImp header file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmLitTrackPropagatorImp.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 **
 **/ 

#ifndef CBMLITTRACKPROPAGATORIMP_H_
#define CBMLITTRACKPROPAGATORIMP_H_
   
#include "CbmLitTrackPropagator.h"

#include <vector>

#include "TMatrixD.h"

class CbmLitTrackExtrapolator;
class CbmLitGeoNavigatorImp;
class CbmLitTrackParam;
class CbmLitMaterialEffectsImp;

class CbmLitTrackPropagatorImp: public CbmLitTrackPropagator
{
public:
   //CbmLitTrackPropagatorImp();
   CbmLitTrackPropagatorImp(CbmLitTrackExtrapolator *extrapolator);
   virtual ~CbmLitTrackPropagatorImp();
   
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
		   CbmLitTrackParam *par, 
           Double_t zOut,
           Int_t pdg);
   
   virtual void TransportMatrix(
		   std::vector<Double_t>& F);
   
   virtual void TransportMatrix(
		   TMatrixD& F);
   
   Bool_t IsCalcTransportMatrix() const {return fCalcTransportMatrix;}
   void IsCalcTransportMatrix(Bool_t calcTransportMatrix) {fCalcTransportMatrix = calcTransportMatrix;}
                           
protected:
	
   void UpdateF(
			TMatrixD& F,
			const TMatrixD& newF);
   
   Bool_t IsParCorrect(
			const CbmLitTrackParam* par);
   
private:
   
   CbmLitTrackExtrapolator *fExtrapolator;
   CbmLitGeoNavigatorImp *fNavigator;
   CbmLitMaterialEffectsImp *fMaterial;

   TMatrixD fFm;
   
   Bool_t fDownstream;
   Int_t fPDG;
   
   Bool_t fCalcTransportMatrix;
   
   ClassDef(CbmLitTrackPropagatorImp,1);
}; 

#endif //CBMLITTRACKPROPAGATORIMP_H_

