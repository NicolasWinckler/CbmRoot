#include "CbmLitMaterialEffectsImp.h"

#include "CbmLitTrackParam.h"
#include "CbmLitMaterialInfo.h"
#include "CbmLitPDG.h"

#include <iostream>
#include <cmath>

CbmLitMaterialEffectsImp::CbmLitMaterialEffectsImp():
	fMass(0.105),
	fEnergyLoss(0.00354), //0.00354
	fDownstream(true),
	fIsElectron(false),
	fIsMuon(true)
{
}

CbmLitMaterialEffectsImp::~CbmLitMaterialEffectsImp()
{
}

LitStatus CbmLitMaterialEffectsImp::Initialize()
{
   return kLITSUCCESS;
}

LitStatus CbmLitMaterialEffectsImp::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitMaterialEffectsImp::Update(
		CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat,
        Int_t pdg,
        Bool_t downstream)
{
	if (mat->GetLength() * mat->GetRho() < 1e-10) return kLITSUCCESS;

	fDownstream = downstream;
	fMass = CbmLitPDG::GetMass(pdg);
	fIsElectron = CbmLitPDG::IsElectron(pdg);
	fIsMuon = CbmLitPDG::IsMuon(pdg);

	AddEnergyLoss(par, mat);

//	AddThinScatter(par, mat);
	AddThickScatter(par, mat);

	return kLITSUCCESS;
}

void CbmLitMaterialEffectsImp::AddEnergyLoss(
		CbmLitTrackParam* par,
		const CbmLitMaterialInfo* mat) const
{
   Double_t Eloss = EnergyLoss(par, mat);
   par->SetQp(CalcQpAfterEloss(par->GetQp(), Eloss));

   Double_t cov = par->GetCovariance(14);
   if (fIsElectron) cov += CalcSigmaSqQpElectron(par, mat);
   else cov += CalcSigmaSqQp(par, mat);
   par->SetCovariance(14, cov);
}

void CbmLitMaterialEffectsImp::AddThickScatter(
		CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
	if (mat->GetLength() < 1e-10) return;

   Double_t tx = par->GetTx();
   Double_t ty = par->GetTy();
   Double_t l = mat->GetLength(); //cm
   Double_t thetaSq = CalcThetaSq(par, mat);

   Double_t t = 1 + tx * tx + ty * ty;

   Double_t Q33 = (1 + tx * tx) * t * thetaSq;
   Double_t Q44 = (1 + ty * ty) * t * thetaSq;
   Double_t Q34 = tx * ty * t * thetaSq;

   Double_t T23 = (l * l) / 3.0;
   Double_t T2 = l / 2.0;

   Double_t D = (fDownstream) ? 1. : -1.;

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

void CbmLitMaterialEffectsImp::AddThinScatter(
		CbmLitTrackParam* par,
		const CbmLitMaterialInfo* mat) const
{
	 if (mat->GetLength() < 1e-10) return;
	 Double_t tx = par->GetTx();
	 Double_t ty = par->GetTy();
	 Double_t thetaSq = CalcThetaSq(par, mat);

	 Double_t t = 1 + tx * tx + ty * ty;

	 Double_t Q33 = (1 + tx * tx) * t * thetaSq;
	 Double_t Q44 = (1 + ty * ty) * t * thetaSq;
	 Double_t Q34 = tx * ty * t * thetaSq;

     std::vector<Double_t> C = par->GetCovMatrix();
     C[9] += Q33;
     C[12] += Q44;
     C[10] += Q34;
     par->SetCovMatrix(C);
}

Double_t CbmLitMaterialEffectsImp::CalcThetaSq(
		const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
   Double_t p = 1. / par->GetQp(); //GeV
   Double_t E = std::sqrt(fMass * fMass + p * p);
   Double_t beta = p / E;
   Double_t x = mat->GetLength(); //cm
   Double_t X0 = mat->GetRL(); //cm
   Double_t bcp = beta * p;
   Double_t z = 1.;

   Double_t theta = 0.0136 * (1./bcp) * z * std::sqrt(x/X0) *
   					(1. + 0.038 * std::log(x/X0));
   return theta * theta;
}

Double_t CbmLitMaterialEffectsImp::EnergyLoss(
		const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
	Double_t length = mat->GetRho() * mat->GetLength();
	if (!fIsElectron) return dEdx(par, mat) * length;
	else return BetheBlochSimple(mat) * length;
	//return MPVEnergyLoss(par, mat);
}

Double_t CbmLitMaterialEffectsImp::dEdx(
		const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
	Double_t dedx;
	if (fIsElectron) dedx = BetheBlochElectron(par, mat);
	else dedx = BetheBloch(par, mat);
	dedx += BetheHeitler(par, mat);
	if (fIsMuon) dedx += PairProduction(par, mat);
	return dedx;
}

Double_t CbmLitMaterialEffectsImp::BetheBloch(
		const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
	Double_t K = 0.000307075; // GeV * g^-1 * cm^2
	Double_t z = (par->GetQp() > 0.) ? 1 : -1.;
	Double_t Z = mat->GetZ();
	Double_t A = mat->GetA();

	Double_t M = fMass;
	Double_t p = std::abs(1. / par->GetQp()); //GeV
	Double_t E = std::sqrt(M * M + p * p);
	Double_t beta = p / E;
	Double_t betaSq = beta * beta;
	Double_t gamma = E / M;
	Double_t gammaSq = gamma * gamma;

	Double_t I = CalcI(Z) * 1e-9; // GeV

	Double_t me = 0.000511; // GeV
	Double_t ratio = me/M;
	Double_t Tmax = (2*me*betaSq*gammaSq) / (1+2*gamma*ratio+ratio*ratio);

	// density correction
	Double_t dc = 0.;
	if (p > 0.5) { // for particles above 1 Gev
		Double_t rho = mat->GetRho();
		Double_t hwp = 28.816 * std::sqrt(rho*Z/A) * 1e-9 ; // GeV
		dc = std::log(hwp/I) + std::log(beta*gamma) - 0.5;
	}

	return K*z*z*(Z/A)*(1./betaSq) * (0.5*std::log(2*me*betaSq*gammaSq*Tmax/(I*I))-betaSq - dc);
}

Double_t CbmLitMaterialEffectsImp::BetheBlochElectron(
		const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
	Double_t K = 0.000307075; // GeV * g^-1 * cm^2
	//Double_t z = (par->GetQp() > 0.) ? 1 : -1.;
	Double_t Z = mat->GetZ();
	Double_t A = mat->GetA();

	Double_t me = 0.000511; // GeV;
	Double_t p = std::abs(1. / par->GetQp()); //GeV
	Double_t E = std::sqrt(me * me + p * p);
	Double_t gamma = E / me;

	Double_t I = CalcI(Z) * 1e-9; // GeV

	if (par->GetQp() > 0) { // electrons
		return K*(Z/A) * (std::log(2*me/I)+1.5*std::log(gamma) - 0.975);
	} else { //positrons
		return K*(Z/A) * (std::log(2*me/I)+2.*std::log(gamma) - 1.);
	}
}

Double_t CbmLitMaterialEffectsImp::CalcQpAfterEloss(
		Double_t qp,
        Double_t eloss) const
{
	Double_t massSq = fMass*fMass;
	Double_t p = 1./qp;
	Double_t E = std::sqrt(p*p + massSq);
	Double_t q = (qp>0) ? 1.: -1.;
	if (!fDownstream) eloss *= -1.0; // TODO check this
	Double_t Enew = E-eloss;
	Double_t pnew = (Enew > fMass) ? std::sqrt(Enew * Enew - massSq) : 0.;
	if (pnew != 0) return q/pnew;
	else return 1e5;
	//if (!fDownstream) eloss *= -1.0;
	//if (p > 0.) p -= eloss;
	//else p += eloss;
	//return 1./p;
}

Double_t CbmLitMaterialEffectsImp::CalcSigmaSqQp(
          const CbmLitTrackParam* par,
          const CbmLitMaterialInfo* mat) const
{
	Double_t P = (1. / par->GetQp()); // GeV
	Double_t XMASS = fMass; // GeV
	Double_t E = std::sqrt(P * P + XMASS * XMASS);
	Double_t Z = mat->GetZ();
	Double_t A = mat->GetA();
	Double_t RHO = mat->GetRho();
	Double_t STEP = mat->GetLength();
	Double_t EMASS = 0.511 * 1e-3; // GeV

	Double_t BETA = P/E;
	Double_t GAMMA = E/XMASS;

	// Calculate xi factor (KeV).
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

	return std::abs(SDEDX);
}

Double_t CbmLitMaterialEffectsImp::CalcSigmaSqQpElectron(
          const CbmLitTrackParam* par,
          const CbmLitMaterialInfo* mat) const
{
	Double_t x = mat->GetLength(); //cm
	Double_t X0 = mat->GetRL(); //cm
	return par->GetQp() * par->GetQp() *
	           (std::exp(-x/X0 * std::log(3.0)/std::log(2.0)) -
	        	std::exp(-2.0 * x/X0));
}

Double_t CbmLitMaterialEffectsImp::CalcI(
		Double_t Z) const
{
	// mean excitation energy in eV
	if (Z > 16.) return 10 * Z;
	else return 16 * std::pow(Z, 0.9);
}

Double_t CbmLitMaterialEffectsImp::BetheHeitler(
        const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
   Double_t M = fMass; //GeV
   Double_t p = std::abs(1. / par->GetQp());  // GeV
   Double_t rho = mat->GetRho();
   Double_t X0 = mat->GetRL();
   Double_t me = 0.000511; // GeV
   Double_t E = std::sqrt(M * M + p * p);
   Double_t ratio = me/M;

   return (E*ratio*ratio)/(X0*rho);
}

Double_t CbmLitMaterialEffectsImp::PairProduction(
        const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
   Double_t p = std::abs(1. / par->GetQp());  // GeV
   Double_t M = fMass; //GeV
   Double_t rho = mat->GetRho();
   Double_t X0 = mat->GetRL();
   Double_t E = std::sqrt(M * M + p * p);

   return 7e-5*E/(X0*rho);
}

Double_t CbmLitMaterialEffectsImp::BetheBlochSimple(
        const CbmLitMaterialInfo* mat) const
{
	return fEnergyLoss * mat->GetZ() / mat->GetA();
}

Double_t CbmLitMaterialEffectsImp::MPVEnergyLoss(
        const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
   Double_t M = fMass * 1e3; //MeV
   Double_t p = std::abs(1. / par->GetQp()) * 1e3;  // MeV

//   Double_t rho = mat->GetRho();
   Double_t Z = mat->GetZ();
   Double_t A = mat->GetA();
   Double_t x = mat->GetRho() * mat->GetLength();

   Double_t I = CalcI(Z) * 1e-6; // MeV

   Double_t K = 0.307075; // MeV g^-1 cm^2
   Double_t j = 0.200;

   Double_t E = std::sqrt(M * M + p * p);
   Double_t beta = p / E;
   Double_t betaSq = beta * beta;
   Double_t gamma = E / M;
   Double_t gammaSq = gamma * gamma;

   Double_t ksi = (K/2.)*(Z/A)*(x/betaSq); // MeV

//   Double_t hwp = 28.816 * std::sqrt(rho*Z/A) * 1e-6 ; // MeV
//   Double_t dc = std::log(hwp/I) + std::log(beta*gamma) - 0.5;
//   dc *= 2;
   Double_t dc = 0.;

   Double_t eloss = ksi * (std::log(2*M*betaSq*gammaSq / I) + std::log(ksi/I) + j - betaSq - dc);

   return eloss * 1e-3; //GeV
}

ClassImp(CbmLitMaterialEffectsImp);
