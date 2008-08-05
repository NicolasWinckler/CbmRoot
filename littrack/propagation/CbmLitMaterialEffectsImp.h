#ifndef CBMLITMATERIALEFFECTSIMP_H_
#define CBMLITMATERIALEFFECTSIMP_H_

#include "CbmLitTool.h"

class CbmLitTrackParam;
class CbmLitMaterialInfo;

class CbmLitMaterialEffectsImp : public CbmLitTool
{
public:
	CbmLitMaterialEffectsImp();
	virtual ~CbmLitMaterialEffectsImp();
	
	// derived from CbmTool
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();
	
	LitStatus Update(
			CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat,
	        Int_t pdg,
	        Bool_t downstream);
public:
	
	void AddThinScatter(
			CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;
	   
	void AddThickScatter(
			CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;
	   
	void AddEnergyLoss( 
			CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;
	                       
	void AddElectronEnergyLoss(
			CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;
	   
	Double_t CalcThetaSq(
			const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;
	
	Double_t BetheBlochSimple(
	        const CbmLitMaterialInfo* mat) const;
	
	Double_t BetheBloch(
			const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;
	
	Double_t ElectronEnergyLoss(
			const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;
	
	Double_t EnergyLoss(
			const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;
	
	Double_t CalcQpAfterEloss(
			Double_t qp,
	        Double_t eloss) const;
	
	Double_t CalcSigmaSqQp(
			   const CbmLitTrackParam* par,
	           const CbmLitMaterialInfo* mat) const;
	
	Double_t CalcSigmaSqQpElectron( 
	          const CbmLitTrackParam* par,
	          const CbmLitMaterialInfo* mat) const;
		 
	Double_t MPVEnergyLoss( 
	        const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;
	
	Double_t BetheHeitler( 
	        const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;
	
	Double_t PairProduction( 
	        const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;
	
	Double_t CalcI(
			Double_t Z) const;
private:
	Double_t fEnergyLoss;
	Bool_t fDownstream;
	Double_t fMass;
	Bool_t fIsElectron;
	Bool_t fIsMuon;
	
	ClassDef(CbmLitMaterialEffectsImp, 1);
};

#endif /*CBMLITMATERIALEFFECTSIMP_H_*/
