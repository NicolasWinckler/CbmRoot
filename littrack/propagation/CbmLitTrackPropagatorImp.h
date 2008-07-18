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



#ifndef CBMLITTRACKPROPAGATOR_H
#define CBMLITTRACKPROPAGATOR_H

#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackExtrapolator.h"
#include "CbmLitMaterial.h"

#include "CbmField.h"
#include "CbmLitTrackParam.h"

#include <vector>

class CbmLitTrackPropagatorImp: public CbmLitTrackPropagator {

public:
   
   // constructor
   //CbmLitTrackPropagatorImp();
   CbmLitTrackPropagatorImp(CbmLitTrackExtrapolator *extrapolator);
   
   // distructor
   virtual ~CbmLitTrackPropagatorImp();
   
   // derived from CbmTool
   virtual LitStatus Initialize();
   virtual LitStatus Finalize();
   
   // Propagator
   virtual LitStatus Propagate(
		   const CbmLitTrackParam *pParamIn,
           CbmLitTrackParam *pParamOut,
           Double_t zOut);
                                 
   virtual LitStatus Propagate( 
		   CbmLitTrackParam *pParam, 
           Double_t zOut );
                           
protected:
 
   void AddThinScatter(
			CbmLitTrackParam* par,
	        CbmLitMaterial* mat);
   
   void AddThickScatter(
			CbmLitTrackParam* par,
	        CbmLitMaterial* mat);
   
   void AddEnergyLoss( 
		   CbmLitTrackParam* pParam,
           CbmLitMaterial* Material);
                       
   void AddElectronEnergyLoss(
		   CbmLitTrackParam* pParam,
           CbmLitMaterial* Material);
   
   Double_t CalcQpErr( 
             const CbmLitTrackParam* par,
             const CbmLitMaterial* mat);
   
private:
   
   CbmLitTrackExtrapolator *fExtrapolator;
   
   Bool_t fDownstream;
   
   Double_t fFms;
   Double_t fMass;
   Double_t fEnergyLoss;
   Bool_t fApplyEnergyLoss; 
   Bool_t fApplyElectronEnergyLoss;
   
   std::vector<CbmLitMaterial*> fvAllMaterials;
   
   
   ClassDef(CbmLitTrackPropagatorImp,1)
}; 

#endif //CbmLitTrackPropagatorImp

