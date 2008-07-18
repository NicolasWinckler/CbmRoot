#ifndef CBMLITELOSSANA_H_
#define CBMLITELOSSANA_H_

#include "CbmTask.h"

#include <vector>

class CbmLitMaterial;
class CbmLitTrackParam;
class TH1F;
class TH2F;


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

    TClonesArray* fMCTrackArray;
    TClonesArray* fMCPointArray;

    TH1F* fh_dp_mc;
    TH1F* fh_eloss_bb;
    TH1F* fh_eloss_vl;   
    TH1F* fh_qp_err;
    TH1F* fh_qp_err_calc;
    TH1F* fh_eloss_err;
    TH1F* fh_eloss_err_calc;
    TH1F* fh_pull_qp; 
    
    TH2F* fhm_dp_mc;
    TH2F* fhm_eloss_bb;
    TH2F* fhm_eloss_vl;   
    TH2F* fhm_qp_err;
    TH2F* fhm_qp_err_calc;
    TH2F* fhm_eloss_err;
    TH2F* fhm_eloss_err_calc;
    TH2F* fhm_pull_qp; 
    
    TH1F* fh_teta_mc;
    TH1F* fh_teta_calc;
    TH2F* fhm_teta_mc;
    TH2F* fhm_teta_calc;
    
    
    std::vector<CbmLitMaterial*> fvMaterials;
    
    Int_t fNEvents;
    
    Double_t CalcBBLoss( 
              CbmLitTrackParam* par,
              CbmLitMaterial* mat);
    
    Double_t CalcVLLoss( 
              CbmLitTrackParam* par,
              CbmLitMaterial* mat);
    
    void CalcErr( 
              CbmLitTrackParam* par,
              CbmLitMaterial* mat,
              Double_t& e_err,
              Double_t& qp_err);
    
    Double_t CalcTeta(
    		CbmLitTrackParam* par,
            CbmLitMaterial* mat);
    

   ClassDef(CbmLitElossAna,1);
};

#endif /*CBMLITELOSSANA_H_*/
