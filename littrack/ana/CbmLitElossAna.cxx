#include "CbmLitElossAna.h"

#include "CbmLitTrackParam.h"
#include "CbmLitMaterialInfo.h"
#include "CbmLitMaterialEffectsImp.h"

#include "CbmRootManager.h"
#include "CbmMCTrack.h"
#include "CbmMCPoint.h"
#include "CbmMuchPoint.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGeoManager.h"
#include "TGeoPcon.h"

#include <iostream>
#include <vector>
#include <cmath>

CbmLitElossAna::CbmLitElossAna()
{
	fNEvents = 0;
}

CbmLitElossAna::~CbmLitElossAna()
{
	
}

InitStatus CbmLitElossAna::Init() 
{
	CbmRootManager* ioman = CbmRootManager::Instance();
    if (!ioman) Fatal("Init", "No CbmRootManager");
   
    fMCTrackArray  = (TClonesArray*) ioman->GetObject("MCTrack");
    if (!fMCTrackArray) Fatal("Init", "No MCTrack array!");

    fMCPointArray  = (TClonesArray*) ioman->GetObject("MuchPoint");
    if (!fMCPointArray) Fatal("Init", "No MuchPoint array!");
    
    Int_t nBins = 200;
    Int_t nBinsMom = 100;
    Double_t minEloss = 0.;
    Double_t maxEloss = 1.;
    Double_t minMom = 0.1;
    Double_t maxMom = 10.;
    Double_t minErr = -0.25;
    Double_t maxErr = 0.25;
    
    fh_eloss_mc = new TH1F("fh_eloss_mc", "mc_mom_in - mc_mom_out", nBins, minEloss, maxEloss);
    fh_eloss_bb = new TH1F("fh_eloss_bb", "eloss_bb", nBins, minEloss, maxEloss);
    fh_eloss_bb_simple = new TH1F("fh_eloss_bb_simple", "eloss_bb_simple", nBins, minEloss, maxEloss);
    fh_eloss_mpv = new TH1F("fh_eloss_mpv", "eloss_mpv", nBins, minEloss, maxEloss);
    fh_qp_err_mc = new TH1F("fh_qp_err_mc", "qp_out - mc_qp_out", nBins, minErr, maxErr);
    fh_qp_err_calc = new TH1F("fh_qp_err_calc", "qp_err_calc", nBins, minErr, maxErr);
    fh_pull_qp = new TH1F("fh_pull_qp", "pull_qp", nBins, -10, 10);
  
    fhm_eloss_mc = new TH2F("fhm_eloss_mc", "(mc_mom_in - mc_mom_out) vs. momentum",
            nBinsMom, minMom, maxMom, nBins, minEloss, maxEloss);
    fhm_eloss_bb = new TH2F("fhm_eloss_bb", "eloss_bb vs. momentum",
            nBinsMom, minMom, maxMom, nBins, minEloss, maxEloss);
    fhm_eloss_bb_simple = new TH2F("fhm_eloss_bb_simple", "eloss_bb_simple vs. momentum",
            nBinsMom, minMom, maxMom, nBins, minEloss, maxEloss);
    fhm_eloss_mpv = new TH2F("fhm_eloss_mpv", "fhm_eloss_mpv vs. momentum",
            nBinsMom, minMom, maxMom, nBins, minEloss, maxEloss);
    fhm_qp_err_mc = new TH2F("fhm_qp_err_mc", "(qp_out - mc_qp_out) vs. momentum",
            nBinsMom, minMom, maxMom, nBins, minErr, maxErr);
    fhm_qp_err_calc = new TH2F("fhm_qp_err_calc", "qp_err_calc vs. momentum",
            nBinsMom, minMom, maxMom, nBins, minErr, maxErr);
    fhm_pull_qp = new TH2F("fhm_pull_qp", "pull_qp vs. momentum",
            nBinsMom, minMom, maxMom, nBins, -10., 10.);       
    
    Double_t minTeta = 0.0;
    Double_t maxTeta = 0.1;
    fh_teta_mc = new TH1F("fh_teta_mc", "teta_mc", nBins, minTeta, maxTeta);
    fh_teta_calc = new TH1F("fh_teta_calc", "teta_calc", nBins, minTeta, maxTeta);
    fhm_teta_mc = new TH2F("fhm_teta_mc", "teta_mc vs. momentum",
            nBins, minTeta, maxTeta, nBinsMom, minMom, maxMom);
    fhm_teta_calc = new TH2F("fhm_teta_calc", "teta_calc vs. momentum",
            nBins, minTeta, maxTeta, nBinsMom, minMom, maxMom);
     
    
    fMatEff = new CbmLitMaterialEffectsImp();
    
    GetGeometry();
    fMaterial.Print();
    
    return kSUCCESS;
}

void CbmLitElossAna::SetParContainers()
{
}

void CbmLitElossAna::Exec(Option_t* opt) 
{
	Double_t mass = 0.105;
	Double_t Q;
	
	Int_t nofTracks = fMCTrackArray->GetEntriesFast();	
	for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
        CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTrackArray->At(iTrack);
        Int_t motherId = mcTrack->GetMotherId();
        if (motherId != -1) continue;
        if (mcTrack->GetPdgCode() == -13) Q = 1.;
        else if (mcTrack->GetPdgCode() == 13) Q = -1.;
        else continue;
       
        CbmMuchPoint* point = NULL;
        
    	Int_t nofPoints = fMCPointArray->GetEntriesFast();
        for (Int_t iPoint = 0; iPoint < nofPoints; iPoint++){
        	CbmMuchPoint* pnt = (CbmMuchPoint*) fMCPointArray->At(iPoint);
        	Int_t trackId = pnt->GetTrackID();
        	if (trackId != iTrack) continue;
        	point = pnt;
        	break;
        }
        if (point == NULL) continue;
        
        std::cout << "length=" << point->GetLength() << std::endl;
        
        TVector3 momIn, momOut;
        point->Momentum(momIn);
        point->MomentumOut(momOut);
        
        CbmLitTrackParam par;
        par.SetTx(point->GetPx()/point->GetPz());
        par.SetTy(point->GetPy()/point->GetPz());
        par.SetQp(Q/momIn.Mag());
        par.Print();
               
        Double_t dp = momIn.Mag() - momOut.Mag();
        fh_eloss_mc->Fill(dp);
        fhm_eloss_mc->Fill(momIn.Mag(), dp);
        
        Double_t bbLoss = CalcBBLoss(&par, &fMaterial);
        fh_eloss_bb->Fill(bbLoss);
        fhm_eloss_bb->Fill(momIn.Mag(), bbLoss);
        
        Double_t bbLossSimple = CalcBBLossSimple(&par, &fMaterial);
        fh_eloss_bb_simple->Fill(bbLossSimple);
        fhm_eloss_bb_simple->Fill(momIn.Mag(), bbLossSimple);
        
        Double_t mpvLoss = fMatEff->MPVEnergyLoss(&par, &fMaterial);
        fh_eloss_mpv->Fill(mpvLoss);
        fhm_eloss_mpv->Fill(momIn.Mag(), mpvLoss);
                
        Double_t qp_err_mc;
        if (momOut.Mag() != 0.) {
           	Double_t qp_out = fMatEff->CalcQpAfterEloss(par.GetQp(),bbLoss);        	   
        	Double_t qp_out_mc = Q/momOut.Mag();
        	qp_err_mc = std::abs(qp_out - qp_out_mc);
        	fh_qp_err_mc->Fill(qp_err_mc);
        	fhm_qp_err_mc->Fill(momIn.Mag(), qp_err_mc);
        }
        
        CbmLitTrackParam par1 = par;
        par1.SetQp(fMatEff->CalcQpAfterEloss(par.GetQp(),bbLoss));
        Double_t qpErr = CalcSigmaQp(&par1, &fMaterial);
        fh_qp_err_calc->Fill(qpErr);
        fhm_qp_err_calc->Fill(momIn.Mag(), qpErr);
    
        Double_t pull = qp_err_mc / qpErr;
        fh_pull_qp->Fill(pull);
    	fhm_pull_qp->Fill(momIn.Mag(), pull); 
    	
	} //loop over MC tracks
	
	std::cout << "Event no: " << fNEvents++ << std::endl;
}

void CbmLitElossAna::Finish()
{
	fh_eloss_mc->Write();
	fh_eloss_bb->Write();
	fh_eloss_bb_simple->Write();
	fh_eloss_mpv->Write();
	fh_qp_err_mc->Write();
	fh_qp_err_calc->Write();
	fh_pull_qp->Write();
		
	fhm_eloss_mc->Write();
	fhm_eloss_bb->Write();
	fhm_eloss_bb_simple->Write();
	fhm_eloss_mpv->Write();
	fhm_qp_err_mc->Write();
	fhm_qp_err_calc->Write();
	fhm_pull_qp->Write();
		
    fh_teta_mc->Write();
    fh_teta_calc->Write();
    fhm_teta_mc->Write();
    fhm_teta_calc->Write();	
}

void CbmLitElossAna::GetGeometry()
{
	TGeoNode* cave = gGeoManager->GetTopNode();
	TObjArray* nodes = cave->GetNodes();
	TGeoNode* node = (TGeoNode*) nodes->FindObject("muchstation01_0");
	TGeoPcon* shape = (TGeoPcon*) node->GetVolume()->GetShape();
	TGeoMaterial* mat = node->GetMedium()->GetMaterial();
	fMaterial.SetRL(mat->GetRadLen());
	fMaterial.SetRho(mat->GetDensity());
	fMaterial.SetZ(mat->GetZ());
	fMaterial.SetA(mat->GetA());
	fMaterial.SetLength(2 * shape->GetDZ());
}

Double_t CbmLitElossAna::CalcBBLoss( 
          const CbmLitTrackParam* par,
          const CbmLitMaterialInfo* mat) const 
{
	Double_t length = mat->GetRho() * mat->GetLength();
	Double_t bbLoss = fMatEff->BetheBloch(par, mat) * length;
	return bbLoss;
}

Double_t CbmLitElossAna::CalcBBLossSimple( 
          const CbmLitTrackParam* par,
          const CbmLitMaterialInfo* mat) const 
{
	Double_t length = mat->GetRho() * mat->GetLength();
	Double_t bbLoss = fMatEff->BetheBlochSimple(mat) * length;
	return bbLoss;
}

Double_t CbmLitElossAna::CalcSigmaQp( 
          const CbmLitTrackParam* par,
          const CbmLitMaterialInfo* mat) const
{
	return std::sqrt(fMatEff->CalcSigmaSqQp(par, mat));
}

ClassImp(CbmLitElossAna)
