/** CbmLitMaterialEffectsImp.cxx
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2008
 **/
#include "CbmLitMaterialEffectsImp.h"

#include "CbmLitTrackParam.h"
#include "CbmLitMaterialInfo.h"
#include "CbmLitPDG.h"
#include "CbmLitDefaultSettings.h"

#include <iostream>
#include <cmath>

CbmLitMaterialEffectsImp::CbmLitMaterialEffectsImp():
	fMass(0.105),
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
        int pdg,
        bool downstream)
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
   myf Eloss = EnergyLoss(par, mat);
   par->SetQp(CalcQpAfterEloss(par->GetQp(), Eloss));

   myf cov = par->GetCovariance(14);
   if (fIsElectron) cov += CalcSigmaSqQpElectron(par, mat);
   else cov += CalcSigmaSqQp(par, mat);
   par->SetCovariance(14, cov);
}

void CbmLitMaterialEffectsImp::AddThickScatter(
		CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
	if (mat->GetLength() < 1e-10) return;

   myf tx = par->GetTx();
   myf ty = par->GetTy();
   myf l = mat->GetLength(); //cm
   myf thetaSq = CalcThetaSq(par, mat);

   myf t = 1 + tx * tx + ty * ty;

   myf Q33 = (1 + tx * tx) * t * thetaSq;
   myf Q44 = (1 + ty * ty) * t * thetaSq;
   myf Q34 = tx * ty * t * thetaSq;

   myf T23 = (l * l) / 3.0;
   myf T2 = l / 2.0;

   myf D = (fDownstream) ? 1. : -1.;

   std::vector<myf> C = par->GetCovMatrix();

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
	 myf tx = par->GetTx();
	 myf ty = par->GetTy();
	 myf thetaSq = CalcThetaSq(par, mat);

	 myf t = 1 + tx * tx + ty * ty;

	 myf Q33 = (1 + tx * tx) * t * thetaSq;
	 myf Q44 = (1 + ty * ty) * t * thetaSq;
	 myf Q34 = tx * ty * t * thetaSq;

     std::vector<myf> C = par->GetCovMatrix();
     C[9] += Q33;
     C[12] += Q44;
     C[10] += Q34;
     par->SetCovMatrix(C);
}

myf CbmLitMaterialEffectsImp::CalcThetaSq(
		const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
   myf p = 1. / par->GetQp(); //GeV
   myf E = std::sqrt(fMass * fMass + p * p);
   myf beta = p / E;
   myf x = mat->GetLength(); //cm
   myf X0 = mat->GetRL(); //cm
   myf bcp = beta * p;
   myf z = 1.;

   myf theta = 0.0136 * (1./bcp) * z * std::sqrt(x/X0) *
   					(1. + 0.038 * std::log(x/X0));
   return theta * theta;
}

myf CbmLitMaterialEffectsImp::EnergyLoss(
		const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
	myf length = mat->GetRho() * mat->GetLength();
//	return dEdx(par, mat) * length;
	if (!fIsElectron) return dEdx(par, mat) * length;
//	else {
//		myf M = fMass; //GeV
//		myf p = std::abs(1. / par->GetQp());  // GeV
//		myf rho = mat->GetRho();
//		myf X0 = mat->GetRL();
////		myf me = 0.000511; // GeV
//		myf E = std::sqrt(M * M + p * p);
////		myf ratio = me/M;
//
////		std::cout << "p=" << p << " E=" << E << " eloss=" << std::abs(E * (std::exp(-length/X0)))<< std::endl;
//
//		return std::abs(E - E * std::exp(-mat->GetLength()/X0));
//	}
//	else return BetheBlochSimple(mat) * length;
	else return BetheBlochElectron(par, mat) * length;
	//return MPVEnergyLoss(par, mat);
}

myf CbmLitMaterialEffectsImp::dEdx(
		const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
	myf dedx;
	if (fIsElectron) dedx = BetheBlochElectron(par, mat);
	else dedx = BetheBloch(par, mat);
	dedx += BetheHeitler(par, mat);
	if (fIsMuon) dedx += PairProduction(par, mat);
	return dedx;
}

myf CbmLitMaterialEffectsImp::BetheBloch(
		const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
	myf K = 0.000307075; // GeV * g^-1 * cm^2
	myf z = (par->GetQp() > 0.) ? 1 : -1.;
	myf Z = mat->GetZ();
	myf A = mat->GetA();

	myf M = fMass;
	myf p = std::abs(1. / par->GetQp()); //GeV
	myf E = std::sqrt(M * M + p * p);
	myf beta = p / E;
	myf betaSq = beta * beta;
	myf gamma = E / M;
	myf gammaSq = gamma * gamma;

	myf I = CalcI(Z) * 1e-9; // GeV

	myf me = 0.000511; // GeV
	myf ratio = me/M;
	myf Tmax = (2*me*betaSq*gammaSq) / (1+2*gamma*ratio+ratio*ratio);

	// density correction
	myf dc = 0.;
	if (p > 0.5) { // for particles above 1 Gev
		myf rho = mat->GetRho();
		myf hwp = 28.816 * std::sqrt(rho*Z/A) * 1e-9 ; // GeV
		dc = std::log(hwp/I) + std::log(beta*gamma) - 0.5;
	}

	return K*z*z*(Z/A)*(1./betaSq) * (0.5*std::log(2*me*betaSq*gammaSq*Tmax/(I*I))-betaSq - dc);
}

myf CbmLitMaterialEffectsImp::BetheBlochElectron(
		const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
	myf K = 0.000307075; // GeV * g^-1 * cm^2
	//myf z = (par->GetQp() > 0.) ? 1 : -1.;
	myf Z = mat->GetZ();
	myf A = mat->GetA();

	myf me = 0.000511; // GeV;
	myf p = std::abs(1. / par->GetQp()); //GeV
	myf E = std::sqrt(me * me + p * p);
	myf gamma = E / me;

	myf I = CalcI(Z) * 1e-9; // GeV

	if (par->GetQp() > 0) { // electrons
		return K*(Z/A) * (std::log(2*me/I)+1.5*std::log(gamma) - 0.975);
	} else { //positrons
		return K*(Z/A) * (std::log(2*me/I)+2.*std::log(gamma) - 1.);
	}
}

myf CbmLitMaterialEffectsImp::CalcQpAfterEloss(
		myf qp,
        myf eloss) const
{
	myf massSq = fMass*fMass;
	myf p = 1./qp;
	myf E = std::sqrt(p*p + massSq);
	myf q = (qp>0) ? 1.: -1.;
	if (!fDownstream) eloss *= -1.0; // TODO check this
	myf Enew = E-eloss;
	myf pnew = (Enew > fMass) ? std::sqrt(Enew * Enew - massSq) : 0.;
	if (pnew != 0) return q/pnew;
	else return 1e5;
	//if (!fDownstream) eloss *= -1.0;
	//if (p > 0.) p -= eloss;
	//else p += eloss;
	//return 1./p;
}

myf CbmLitMaterialEffectsImp::CalcSigmaSqQp(
          const CbmLitTrackParam* par,
          const CbmLitMaterialInfo* mat) const
{
	myf P = (1. / par->GetQp()); // GeV
	myf XMASS = fMass; // GeV
	myf E = std::sqrt(P * P + XMASS * XMASS);
	myf Z = mat->GetZ();
	myf A = mat->GetA();
	myf RHO = mat->GetRho();
	myf STEP = mat->GetLength();
	myf EMASS = 0.511 * 1e-3; // GeV

	myf BETA = P/E;
	myf GAMMA = E/XMASS;

	// Calculate xi factor (KeV).
	myf XI = (153.5*Z*STEP*RHO)/(A*BETA*BETA);

	// Maximum energy transfer to atomic electron (KeV).
	myf ETA = BETA*GAMMA;
	myf ETASQ = ETA*ETA;
	myf RATIO = EMASS/XMASS;
	myf F1 = 2.*EMASS*ETASQ;
	myf F2 = 1.+2.*RATIO*GAMMA+RATIO*RATIO;
	myf EMAX = 1e6 * F1/F2;

	myf DEDX2 = XI*EMAX*(1.-(BETA*BETA/2.))*1e-12;

	myf SDEDX = (E*E*DEDX2) / std::pow(P, 6);

	return std::abs(SDEDX);
}

myf CbmLitMaterialEffectsImp::CalcSigmaSqQpElectron(
          const CbmLitTrackParam* par,
          const CbmLitMaterialInfo* mat) const
{
	myf x = mat->GetLength(); //cm
	myf X0 = mat->GetRL(); //cm
	return par->GetQp() * par->GetQp() *
	           (std::exp(-x/X0 * std::log(3.0)/std::log(2.0)) -
	        	std::exp(-2.0 * x/X0));
}

myf CbmLitMaterialEffectsImp::CalcI(
		myf Z) const
{
	// mean excitation energy in eV
	if (Z > 16.) return 10 * Z;
	else return 16 * std::pow(Z, 0.9);
}

myf CbmLitMaterialEffectsImp::BetheHeitler(
        const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
   myf M = fMass; //GeV
   myf p = std::abs(1. / par->GetQp());  // GeV
   myf rho = mat->GetRho();
   myf X0 = mat->GetRL();
   myf me = 0.000511; // GeV
   myf E = std::sqrt(M * M + p * p);
   myf ratio = me/M;

   return (E*ratio*ratio)/(X0*rho);
}

myf CbmLitMaterialEffectsImp::PairProduction(
        const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
   myf p = std::abs(1. / par->GetQp());  // GeV
   myf M = fMass; //GeV
   myf rho = mat->GetRho();
   myf X0 = mat->GetRL();
   myf E = std::sqrt(M * M + p * p);

   return 7e-5*E/(X0*rho);
}

myf CbmLitMaterialEffectsImp::BetheBlochSimple(
        const CbmLitMaterialInfo* mat) const
{
	return lit::ENERGY_LOSS_CONST * mat->GetZ() / mat->GetA();
}

myf CbmLitMaterialEffectsImp::MPVEnergyLoss(
        const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
   myf M = fMass * 1e3; //MeV
   myf p = std::abs(1. / par->GetQp()) * 1e3;  // MeV

//   myf rho = mat->GetRho();
   myf Z = mat->GetZ();
   myf A = mat->GetA();
   myf x = mat->GetRho() * mat->GetLength();

   myf I = CalcI(Z) * 1e-6; // MeV

   myf K = 0.307075; // MeV g^-1 cm^2
   myf j = 0.200;

   myf E = std::sqrt(M * M + p * p);
   myf beta = p / E;
   myf betaSq = beta * beta;
   myf gamma = E / M;
   myf gammaSq = gamma * gamma;

   myf ksi = (K/2.)*(Z/A)*(x/betaSq); // MeV

//   myf hwp = 28.816 * std::sqrt(rho*Z/A) * 1e-6 ; // MeV
//   myf dc = std::log(hwp/I) + std::log(beta*gamma) - 0.5;
//   dc *= 2;
   myf dc = 0.;

   myf eloss = ksi * (std::log(2*M*betaSq*gammaSq / I) + std::log(ksi/I) + j - betaSq - dc);

   return eloss * 1e-3; //GeV
}
