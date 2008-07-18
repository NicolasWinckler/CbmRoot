#include "CbmLitElossAna.h"

#include "CbmLitMaterial.h"
#include "CbmLitTrackParam.h"
#include "CbmLitEnvironment.h"

#include "CbmRootManager.h"
#include "CbmMCTrack.h"
#include "CbmMCPoint.h"
#include "CbmMuchPoint.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"

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
    
    CbmLitEnvironment* env = CbmLitEnvironment::Instance();
    fvMaterials = env->GetMaterialsSimple();
    
    Int_t nBins = 200;
    Int_t nBinsMom = 100;
    Double_t minEloss = 0;
    Double_t maxEloss = 1.;
    Double_t minMom = 1.;
    Double_t maxMom = 10.;
    Double_t minErr = -0.25;
    Double_t maxErr = 0.25;
    
    fh_dp_mc = new TH1F("fh_dp_mc", "mc_mom_in - mc_mom_out", nBins, minEloss, maxEloss);
    fh_eloss_bb = new TH1F("fh_eloss_bb", "eloss_bb", nBins, minEloss, maxEloss);
    fh_eloss_vl = new TH1F("fh_eloss_vl", "eloss_vl", nBins, minEloss, maxEloss);
    fh_qp_err = new TH1F("fh_qp_err", "qp_out - mc_qp_out", nBins, minErr, maxErr);
    fh_qp_err_calc = new TH1F("fh_qp_err_calc", "qp_err_calc", nBins, minErr, maxErr);
    fh_eloss_err = new TH1F("fh_eloss_err", "bb_eloss - mc_dp", nBins, minErr, maxErr);
    fh_eloss_err_calc = new TH1F("fh_eloss_err_calc", "eloss_err_calc", nBins, minErr, maxErr);
    fh_pull_qp = new TH1F("fh_pull_qp", "pull_qp", nBins, -10, 10);
    
    fhm_dp_mc = new TH2F("fhm_dp_mc", "(mc_mom_in - mc_mom_out) vs. momentum",
            nBins, minEloss, maxEloss, nBinsMom, minMom, maxMom);
    fhm_eloss_bb = new TH2F("fhm_eloss_bb", "eloss_bb vs. momentum",
            nBins, minEloss, maxEloss, nBinsMom, minMom, maxMom);
    fhm_eloss_vl = new TH2F("fhm_eloss_vl", "eloss_vl vs. momentum",
            nBins, minEloss, maxEloss, nBinsMom, minMom, maxMom);   
    fhm_qp_err = new TH2F("fhm_qp_err", "(qp_out - mc_qp_out) vs. momentum",
            nBins, minErr, maxErr, nBinsMom, minMom, maxMom);
    fhm_qp_err_calc = new TH2F("fhm_qp_err_calc", "qp_err_calc vs. momentum",
            nBins, minErr, maxErr, nBinsMom, minMom, maxMom);
    fhm_eloss_err = new TH2F("fhm_eloss_err", "(bb_eloss - mc_dp) vs. momentum",
            nBins, minErr, maxErr, nBinsMom, minMom, maxMom);
    fhm_eloss_err_calc = new TH2F("fhm_eloss_err_calc", "eloss_err_calc vs. momentum",
            nBins, minErr, maxErr, nBinsMom, minMom, maxMom);
    fhm_pull_qp = new TH2F("fhm_pull_qp", "pull_qp vs. momentum",
            nBins, -10., 10., nBinsMom, minMom, maxMom);   
    
    Double_t minTeta = 0.0;
    Double_t maxTeta = 0.1;
    fh_teta_mc = new TH1F("fh_teta_mc", "teta_mc", nBins, minTeta, maxTeta);
    fh_teta_calc = new TH1F("fh_teta_calc", "teta_calc", nBins, minTeta, maxTeta);
    fhm_teta_mc = new TH2F("fhm_teta_mc", "teta_mc vs. momentum",
            nBins, minTeta, maxTeta, nBinsMom, minMom, maxMom);
    fhm_teta_calc = new TH2F("fhm_teta_calc", "teta_calc vs. momentum",
            nBins, minTeta, maxTeta, nBinsMom, minMom, maxMom);
    
    return kSUCCESS;
}

void CbmLitElossAna::SetParContainers()
{

}

void CbmLitElossAna::Exec(Option_t* opt) 
{
	Double_t mass = 0.105;
	
	Int_t nofTracks = fMCTrackArray->GetEntriesFast();
	
	for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
        CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTrackArray->At(iTrack);
        Int_t motherId = mcTrack->GetMotherId();
        if (motherId != -1) continue;
       
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
        
        TVector3 momIn, momOut;
        point->Momentum(momIn);
        point->MomentumOut(momOut);
        
        CbmLitTrackParam par;
        par.SetTx(point->GetPx()/point->GetPz());
        par.SetTy(point->GetPy()/point->GetPz());
        par.SetQp(1./momIn.Mag());
        
        Double_t teta = CalcTeta(&par, fvMaterials[0]);
        fh_teta_mc->Fill(momOut.Theta());
        fh_teta_calc->Fill(teta);
        fhm_teta_mc->Fill(momOut.Theta(), momIn.Mag());;
        fhm_teta_calc->Fill(teta, momIn.Mag());

        
        
        Double_t dp = momIn.Mag() - momOut.Mag();
        fh_dp_mc->Fill(dp);
        fhm_dp_mc->Fill(dp, momIn.Mag());

        

        CbmLitTrackParam parOut = par;
        Double_t bbLoss = CalcBBLoss(&parOut, fvMaterials[0]);
        fh_eloss_bb->Fill(bbLoss);
        fhm_eloss_bb->Fill(bbLoss, momIn.Mag());
        
        if (momOut.Mag() != 0.) {
        	fh_qp_err->Fill(fabs(1./momOut.Mag() - parOut.GetQp()));
        	fhm_qp_err->Fill(fabs(1./momOut.Mag() - parOut.GetQp()), momIn.Mag());
        }
        
        Double_t vlLoss = CalcVLLoss(&par, fvMaterials[0]);
        fh_eloss_vl->Fill(vlLoss);  
        fhm_eloss_vl->Fill(vlLoss, momIn.Mag());  
        
        Double_t qpErr, eErr;
        CalcErr(&par, fvMaterials[0], eErr, qpErr);
        fh_qp_err_calc->Fill(qpErr);
        fhm_qp_err_calc->Fill(qpErr, momIn.Mag());
        fh_eloss_err_calc->Fill(eErr);
        fhm_eloss_err_calc->Fill(eErr, momIn.Mag());
                
    	fh_eloss_err->Fill(fabs(bbLoss - dp));
    	fhm_eloss_err->Fill(fabs(bbLoss - dp), momIn.Mag());
    	
    	fh_pull_qp->Fill((1./momOut.Mag() - parOut.GetQp()) / qpErr);
    	fhm_pull_qp->Fill((1./momOut.Mag() - parOut.GetQp()) / qpErr, momIn.Mag());
       
	}
	
	std::cout << "Event no: " << fNEvents++ << std::endl;
}

void CbmLitElossAna::Finish()
{
	fh_dp_mc->Write();
	fh_eloss_bb->Write();
	fh_eloss_vl->Write();
	fh_qp_err->Write();
	fh_qp_err_calc->Write();
	fh_eloss_err->Write();
	fh_eloss_err_calc->Write();
	fh_pull_qp->Write();
	
	fhm_dp_mc->Write();
	fhm_eloss_bb->Write();
	fhm_eloss_vl->Write();
	fhm_qp_err->Write();
	fhm_qp_err_calc->Write();
	fhm_eloss_err->Write();
	fhm_eloss_err_calc->Write();
	fhm_pull_qp->Write();
	
    fh_teta_mc->Write();
    fh_teta_calc->Write();
    fhm_teta_mc->Write();
    fhm_teta_calc->Write();
	
}

Double_t CbmLitElossAna::CalcBBLoss( 
          CbmLitTrackParam* par,
          CbmLitMaterial* mat)
{
	Double_t fEnergyLoss = 0.00354;//0.00330;
	Bool_t fDownstream = true;

   // dE/dx energy loss (Bethe-Block)
   Double_t tx = par->GetTx();
   Double_t ty = par->GetTy();
   Double_t p = 1. / par->GetQp();  
   
   Double_t norm = TMath::Sqrt(1 + tx * tx + ty * ty);
   
   Double_t bbLoss = mat->GetThickness() * norm * 
                     mat->GetDensity() * fEnergyLoss * 
                     mat->GetZeff() / mat->GetAeff();

   //TODO bbEloss too big
   
   if (fDownstream) bbLoss *= -1.0;
   
   if (p > 0.) p += bbLoss;
   else p -= bbLoss;
   
   if (p != 0) par->SetQp(1./p);
   else par->SetQp(1./0.000001);
	
   return fabs(bbLoss);
}

Double_t CbmLitElossAna::CalcVLLoss( 
          CbmLitTrackParam* par,
          CbmLitMaterial* mat)
{
   Double_t mass = 105.; //MeV
   Double_t tx = par->GetTx();
   Double_t ty = par->GetTy();
   Double_t p = (1. / par->GetQp()) * 1e3;  // MeV
	   
   Double_t norm = std::sqrt(1 + tx * tx + ty * ty);
   Double_t thick = mat->GetThickness() * norm;
   
   Double_t rl = mat->GetRadLength();
   Double_t rho = mat->GetDensity();
   Double_t Z = mat->GetZeff();
   Double_t A = mat->GetAeff();
   
   Double_t I = 10 * Z * 10e-6; // MeV
   
   Double_t K = 0.307075; // MeV g^-1 cm^2
   Double_t j = 0.200;
   Double_t mcc = mass;
   
  // Double_t hwp = 28.816 * std::sqrt(rho*(Z/A)) * 1e-6; //MeV
   
   Double_t x = rho * thick;//rl;
   
   Double_t E = std::sqrt(mass * mass + p * p);
   Double_t beta = p / E; 
   Double_t betaSq = beta * beta;
   Double_t gamma = E / mass;
   Double_t gammaSq = gamma * gamma;
   
   Double_t ksi = (K/2.)*(Z/A)*(x/betaSq);
   
   Double_t eloss = ksi * (std::log(2*mcc*betaSq*gammaSq / I) + std::log(ksi/I) + j - betaSq);
   
   return eloss * 1e-3; //GeV
   
}

void CbmLitElossAna::CalcErr( 
          CbmLitTrackParam* par,
          CbmLitMaterial* mat,
          Double_t& e_err,
          Double_t& qp_err)
{
		
	    Double_t tx = par->GetTx();
	    Double_t ty = par->GetTy();
	    Double_t norm = std::sqrt(1 + tx * tx + ty * ty);
		
		Double_t P = (1. / par->GetQp()); // GeV    
		Double_t XMASS = 0.105; // GeV          
		Double_t E = std::sqrt(P * P + XMASS * XMASS);
		Double_t Z = mat->GetZeff();
		Double_t A = mat->GetAeff();
		Double_t RHO = mat->GetDensity();
		Double_t STEP = mat->GetThickness() * norm;
		Double_t EMASS = 0.511 * 1e-3; // GeV
		
		// Calculate xi factor (KeV).
		Double_t BETA = P/E;
		Double_t GAMMA = E/XMASS;
		Double_t XI = (153.5*Z*STEP*RHO)/(A*BETA*BETA);
		
		// Maximum energy transfer to atomic electron (KeV).
		Double_t ETA = BETA*GAMMA;
		Double_t ETASQ = ETA*ETA;
		Double_t RATIO = EMASS/XMASS;
		Double_t F1 = 2.*EMASS*ETASQ;
		Double_t F2 = 1.+2.*RATIO*GAMMA+RATIO*RATIO;
		Double_t EMAX = 1e6 * F1/F2;
		       
		Double_t DEDX2 = XI*EMAX*(1.-(BETA*BETA/2.))*1e-12;

		Double_t SDEDX = (E*E*DEDX2) / std::pow(P, 6); 
		
		e_err = std::sqrt(fabs(DEDX2));
		qp_err = std::sqrt(fabs(SDEDX));
}

Double_t CbmLitElossAna::CalcTeta(
		CbmLitTrackParam* par,
        CbmLitMaterial* mat)
{
   Double_t tx = par->GetTx();
   Double_t ty = par->GetTy();
   Double_t p = 1. / par->GetQp(); //GeV
   Double_t norm = 1 + tx * tx + ty * ty;
   
   Double_t fMass = 0.105;
   
   Double_t E = std::sqrt(fMass * fMass + p * p);
   Double_t beta = p / E;
   
   Double_t x = mat->GetThickness() * norm; //cm
   Double_t X0 = mat->GetRadLength(); //cm
   Double_t bcp = beta * p;
   Double_t z = 1;
   
   Double_t fFms = 1.05;
   
   Double_t teta = fFms * 0.0136 * (1./bcp) * z * std::sqrt(x/X0) *
   					(1. + 0.038 * std::log(x/X0));
   
   return teta;
}

ClassImp(CbmLitElossAna)
