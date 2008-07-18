// -------------------------------------------------------------------------
// -----                  CbmLitTrackPropagatorImp source file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------
 


#include "CbmLitTrackPropagatorImp.h"
#include "CbmLitEnvironment.h"
#include "CbmLitComparators.h"

#include <vector>
#include <cmath>

//constructors
CbmLitTrackPropagatorImp::CbmLitTrackPropagatorImp(CbmLitTrackExtrapolator *extrapolator):
   CbmLitTrackPropagator("CbmLitTrackPropagatorImp"),
   fExtrapolator(extrapolator)
{
   CbmLitEnvironment* env = CbmLitEnvironment::Instance();
   fvAllMaterials = env->GetMaterials();
   
   // set default properties for the algorithm
   Properties().AddProperty("fFms", 1.05);
   Properties().AddProperty("fMass", 0.105);
   Properties().AddProperty("fEnergyLoss", 0.00354);
   Properties().AddProperty("fApplyEnergyLoss", true);
   Properties().AddProperty("fApplyElectronEnergyLoss", false);
}

//Destructor
CbmLitTrackPropagatorImp::~CbmLitTrackPropagatorImp() 
{
   //
}

// Initialization
LitStatus CbmLitTrackPropagatorImp::Initialize()
{
   //
   Properties().GetProperty("fFms", fFms);
   Properties().GetProperty("fMass", fMass);
   Properties().GetProperty("fEnergyLoss", fEnergyLoss);
   Properties().GetProperty("fApplyEnergyLoss", fApplyEnergyLoss);
   Properties().GetProperty("fApplyElectronEnergyLoss", fApplyElectronEnergyLoss);
   return kLITSUCCESS;
}

// Finalization
LitStatus CbmLitTrackPropagatorImp::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackPropagatorImp::Propagate(
		const CbmLitTrackParam *pParamIn,
        CbmLitTrackParam *pParamOut,
        Double_t zOut)
{
   *pParamOut = *pParamIn;
   return Propagate(pParamOut, zOut);
}

LitStatus CbmLitTrackPropagatorImp::Propagate(
		CbmLitTrackParam *pParam,
        Double_t zOut)
                                      
{
   Initialize();

   Double_t zIn = pParam->GetZ();
   
   if (zOut > zIn) fDownstream = kTRUE;
   else fDownstream = kFALSE;
   
   //Add noise from materials
   std::vector<CbmLitMaterial*>::iterator it_min, it_max;
   CbmLitMaterial* m = new CbmLitMaterial();
   
   m->SetZ(zIn - 1e-6);
   it_min = lower_bound(fvAllMaterials.begin(), fvAllMaterials.end(),
                              m, CompareMaterialPtrZLess());
   m->SetZ(zOut - 1e-6);
   it_max = lower_bound(fvAllMaterials.begin(), fvAllMaterials.end(), 
                              m, CompareMaterialPtrZLess());
   
   for(std::vector<CbmLitMaterial*>::iterator i = it_min; i != it_max; ) {

      Double_t Z = (*i)->GetZ();
      fExtrapolator->Extrapolate(pParam, Z);   

      if ((*i)->IsTraversed(pParam->GetX(), pParam->GetY())) {
        if ((*i)->GetThickness() < 5.) AddThinScatter(pParam, *i);
          else AddThickScatter(pParam, *i);
          
        if (fApplyEnergyLoss) {
        	if (!fApplyElectronEnergyLoss) AddEnergyLoss(pParam, *i);
        	else AddElectronEnergyLoss(pParam, *i);
        }
      }
      
      if (fDownstream) i++; else i--;
   }
   
   fExtrapolator->Extrapolate(pParam, zOut); 
   
   delete m;
   
   return kLITSUCCESS;
}


void CbmLitTrackPropagatorImp::AddThickScatter(
		CbmLitTrackParam* par,
        CbmLitMaterial* mat)
{
   Double_t tx = par->GetTx();
   Double_t ty = par->GetTy();
   Double_t p = 1. / par->GetQp(); //GeV
   Double_t norm = 1 + tx * tx + ty * ty;
      
   Double_t E = std::sqrt(fMass * fMass + p * p);
   Double_t beta = p / E;
   
   Double_t x = mat->GetThickness() * norm; //cm
   Double_t X0 = mat->GetRadLength(); //cm
   Double_t bcp = beta * p;
   Double_t z = 1;
   
   Double_t teta = fFms * 0.0136 * (1./bcp) * z * std::sqrt(x/X0) *
   					(1. + 0.038 * std::log(x/X0));
   Double_t tetaSq = teta * teta;
   
//   Double_t bcpSq = bcp * bcp;
//   Double_t tetaSq = 184.96 * 1e-6 * (1. / bcpSq) * (x/X0);
   
   Double_t t = 1 + tx * tx * ty * ty;
   
   Double_t Q33 = (1 + tx * tx) * t * tetaSq;
   Double_t Q44 = (1 + ty * ty) * t * tetaSq;
   Double_t Q34 = tx * ty * t * tetaSq;
   
   Double_t T23 = (x * x) / 3.0;
   Double_t T2 = x / 2.0;

   Double_t D = (fDownstream) ? 1. : -1. ;

   std::vector<Double_t> C = par->GetCovMatrix();
      
   C[0] += Q33 * T23; 
   C[1] += Q34 * T23; 
   C[2] += Q33 * D * T2; 
   C[3] += Q34 * D * T2;
                           
   C[5] += Q44 * T23;           
   C[6] += Q34 * D * T2; 
   C[7] += Q44 * D * T2;
                                                      
   C[9] += Q33;
   C[10] += Q34;
   
   C[12] += Q44;
      
   par->SetCovMatrix(C);
}


 void CbmLitTrackPropagatorImp::AddThinScatter(
			CbmLitTrackParam* par,
	        CbmLitMaterial* mat)
{ 
	 Double_t tx = par->GetTx();
	 Double_t ty = par->GetTy();
	 Double_t p = 1. / par->GetQp(); //GeV
	 Double_t norm = 1 + tx * tx + ty * ty;
	      
	 Double_t E = std::sqrt(fMass * fMass + p * p);
	 Double_t beta = p / E;
	  
	 Double_t x = mat->GetThickness() * norm; //cm
	 Double_t X0 = mat->GetRadLength(); //cm
	 Double_t bcp = beta * p;
	 Double_t z = 1;
	   
	 Double_t teta = fFms * 0.0136 * (1./bcp) * z * std::sqrt(x/X0) *
	 					(1. + 0.038 * std::log(x/X0));
	 Double_t tetaSq = teta * teta;
	 
//	 Double_t bcpSq = bcp * bcp;
//	 Double_t tetaSq = 184.96 * 1e-6 * (1. / bcpSq) * (x/X0);
	   
	 Double_t t = 1 + tx * tx * ty * ty;
	   
	 Double_t Q33 = (1 + tx * tx) * t * tetaSq;
	 Double_t Q44 = (1 + ty * ty) * t * tetaSq;
	 Double_t Q34 = tx * ty * t * tetaSq;
     
     std::vector<Double_t> C = par->GetCovMatrix();
     C[9] += Q33;
     C[12] += Q44;
     C[10] += Q34;
     par->SetCovMatrix(C);
}
 

void CbmLitTrackPropagatorImp::AddEnergyLoss( 
                             CbmLitTrackParam* par,
                             CbmLitMaterial* mat)
{
   // dE/dx energy loss (simplified Bethe-Block)
   Double_t tx = par->GetTx();
   Double_t ty = par->GetTy();
   Double_t p = 1. / par->GetQp();  
   
   Double_t norm = std::sqrt(1 + tx * tx + ty * ty);
   
   Double_t bbLoss = mat->GetThickness() * norm * 
                     mat->GetDensity() * fEnergyLoss * 
                     mat->GetZeff() / mat->GetAeff();
   
   if (fDownstream) bbLoss *= -1.0;
   
   if (p > 0.) p += bbLoss;
   else p -= bbLoss;
   
   Double_t qpErr = CalcQpErr(par, mat);
   
   // update parameters an cov matrix
   if (p != 0) par->SetQp(1./p);
   else par->SetQp(1./0.000001);
   Double_t cov = par->GetCovariance(14);
   cov += qpErr * qpErr;
   par->SetCovariance(14, cov);                 
}

 
void CbmLitTrackPropagatorImp::AddElectronEnergyLoss(
                             CbmLitTrackParam* pParam,
                             CbmLitMaterial* Material)
{
   //energy loss for electrons
 
   Double_t RadThick;
   
   Double_t tx = pParam->GetTx();
   Double_t ty = pParam->GetTy();
   Double_t qp = pParam->GetQp(); 
   
   Double_t norm = std::sqrt(1 + tx * tx + ty * ty);
   
   if (fDownstream){
      RadThick = - (Material->GetThickness() / Material->GetRadLength()) * norm;
   }
   else{
      RadThick = (Material->GetThickness() / Material->GetRadLength()) * norm;
   }
   

   Double_t cov = pParam->GetCovariance(14);
   cov += qp * qp * 
           (std::exp(-RadThick * std::log(3.0)/std::log(2.0)) - 
        	std::exp(-2.0 * RadThick));
   pParam->SetCovariance(14, cov);                 
   pParam->SetQp(pParam->GetQp() * std::exp(-RadThick));
}


Double_t CbmLitTrackPropagatorImp::CalcQpErr( 
          const CbmLitTrackParam* par,
          const CbmLitMaterial* mat)
{
    Double_t tx = par->GetTx();
    Double_t ty = par->GetTy();
    Double_t norm = std::sqrt(1 + tx * tx + ty * ty);
	
	Double_t P = (1. / par->GetQp()); // GeV    
	Double_t XMASS = fMass; // GeV          
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
	
	return std::sqrt(fabs(SDEDX));
}

ClassImp(CbmLitTrackPropagatorImp)
