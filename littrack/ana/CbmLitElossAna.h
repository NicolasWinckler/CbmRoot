#ifndef CBMLITELOSSANA_H_
#define CBMLITELOSSANA_H_

#include "CbmLitMaterialInfo.h"

#include "CbmTask.h"

class CbmLitMaterialEffectsImp;
class CbmLitTrackParam;
class CbmLitMaterialInfo;
class CbmMuchPoint;
class TH1F;
class TH2F;
class TClonesArray;

class CbmLitElossAna : public CbmTask
{
public:
	CbmLitElossAna();
	virtual ~CbmLitElossAna();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    virtual void SetParContainers();
  
private:
	
	void GetGeometry();
	
	Double_t CalcLitELoss( 
	          const CbmLitTrackParam* par,
	          const CbmLitMaterialInfo* mat) const;
	          
	Double_t CalcBBLoss( 
	          const CbmLitTrackParam* par,
	          const CbmLitMaterialInfo* mat) const;
	
	Double_t CalcBBLossSimple( 
	          const CbmLitTrackParam* par,
	          const CbmLitMaterialInfo* mat) const;
	
	Double_t CalcSigmaQp( 
	          const CbmLitTrackParam* par,
	          const CbmLitMaterialInfo* mat) const;
	
	
	Double_t dE(
			CbmMuchPoint* point);
	
	Double_t Length(
			CbmMuchPoint* point);
	
    TClonesArray* fMCTrackArray;
    TClonesArray* fMCPointArray;

    TH1F* fh_eloss_mc;
    TH1F* fh_eloss_lit;
    TH1F* fh_eloss_bb;
    TH1F* fh_eloss_bb_simple;
    TH1F* fh_eloss_mpv; 
    TH1F* fh_qp_err_mc;
    TH1F* fh_qp_err_calc;    
    TH1F* fh_pull_qp; 
    
    TH2F* fhm_eloss_mc;
    TH2F* fhm_eloss_lit;
    TH2F* fhm_eloss_bb;
    TH2F* fhm_eloss_bb_simple;
    TH2F* fhm_eloss_mpv;
    TH2F* fhm_qp_err_mc;
    TH2F* fhm_qp_err_calc;    
    TH2F* fhm_pull_qp; 
    
    TH1F* fh_teta_mc;
    TH1F* fh_teta_calc;
    TH2F* fhm_teta_mc;
    TH2F* fhm_teta_calc;

    CbmLitMaterialInfo fMaterial;
    CbmLitMaterialEffectsImp* fMatEff;
    
    Int_t fNEvents;

    ClassDef(CbmLitElossAna,1);
};

#endif /*CBMLITELOSSANA_H_*/
