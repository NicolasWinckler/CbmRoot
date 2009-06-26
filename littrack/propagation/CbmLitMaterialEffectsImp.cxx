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
   double Eloss = EnergyLoss(par, mat);
   par->SetQp(CalcQpAfterEloss(par->GetQp(), Eloss));

   double cov = par->GetCovariance(14);
   if (fIsElectron) cov += CalcSigmaSqQpElectron(par, mat);
   else cov += CalcSigmaSqQp(par, mat);
   par->SetCovariance(14, cov);
}

void CbmLitMaterialEffectsImp::AddThickScatter(
		CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
	if (mat->GetLength() < 1e-10) return;

   double tx = par->GetTx();
   double ty = par->GetTy();
   double l = mat->GetLength(); //cm
   double thetaSq = CalcThetaSq(par, mat);

   double t = 1 + tx * tx + ty * ty;

   double Q33 = (1 + tx * tx) * t * thetaSq;
   double Q44 = (1 + ty * ty) * t * thetaSq;
   double Q34 = tx * ty * t * thetaSq;

   double T23 = (l * l) / 3.0;
   double T2 = l / 2.0;

   double D = (fDownstream) ? 1. : -1.;

   std::vector<double> C = par->GetCovMatrix();

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
	 double tx = par->GetTx();
	 double ty = par->GetTy();
	 double thetaSq = CalcThetaSq(par, mat);

	 double t = 1 + tx * tx + ty * ty;

	 double Q33 = (1 + tx * tx) * t * thetaSq;
	 double Q44 = (1 + ty * ty) * t * thetaSq;
	 double Q34 = tx * ty * t * thetaSq;

     std::vector<double> C = par->GetCovMatrix();
     C[9] += Q33;
     C[12] += Q44;
     C[10] += Q34;
     par->SetCovMatrix(C);
}

double CbmLitMaterialEffectsImp::CalcThetaSq(
		const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
   double p = 1. / par->GetQp(); //GeV
   double E = std::sqrt(fMass * fMass + p * p);
   double beta = p / E;
   double x = mat->GetLength(); //cm
   double X0 = mat->GetRL(); //cm
   double bcp = beta * p;
   double z = 1.;

   double theta = 0.0136 * (1./bcp) * z * std::sqrt(x/X0) *
   					(1. + 0.038 * std::log(x/X0));
   return theta * theta;
}

double CbmLitMaterialEffectsImp::EnergyLoss(
		const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
	double length = mat->GetRho() * mat->GetLength();
	if (!fIsElectron) return dEdx(par, mat) * length;
	else return BetheBlochSimple(mat) * length;
	//return MPVEnergyLoss(par, mat);
}

double CbmLitMaterialEffectsImp::dEdx(
		const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
	double dedx;
	if (fIsElectron) dedx = BetheBlochElectron(par, mat);
	else dedx = BetheBloch(par, mat);
	dedx += BetheHeitler(par, mat);
	if (fIsMuon) dedx += PairProduction(par, mat);
	return dedx;
}

double CbmLitMaterialEffectsImp::BetheBloch(
		const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
	double K = 0.000307075; // GeV * g^-1 * cm^2
	double z = (par->GetQp() > 0.) ? 1 : -1.;
	double Z = mat->GetZ();
	double A = mat->GetA();

	double M = fMass;
	double p = std::abs(1. / par->GetQp()); //GeV
	double E = std::sqrt(M * M + p * p);
	double beta = p / E;
	double betaSq = beta * beta;
	double gamma = E / M;
	double gammaSq = gamma * gamma;

	double I = CalcI(Z) * 1e-9; // GeV

	double me = 0.000511; // GeV
	double ratio = me/M;
	double Tmax = (2*me*betaSq*gammaSq) / (1+2*gamma*ratio+ratio*ratio);

	// density correction
	double dc = 0.;
	if (p > 0.5) { // for particles above 1 Gev
		double rho = mat->GetRho();
		double hwp = 28.816 * std::sqrt(rho*Z/A) * 1e-9 ; // GeV
		dc = std::log(hwp/I) + std::log(beta*gamma) - 0.5;
	}

	return K*z*z*(Z/A)*(1./betaSq) * (0.5*std::log(2*me*betaSq*gammaSq*Tmax/(I*I))-betaSq - dc);
}

double CbmLitMaterialEffectsImp::BetheBlochElectron(
		const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
	double K = 0.000307075; // GeV * g^-1 * cm^2
	//double z = (par->GetQp() > 0.) ? 1 : -1.;
	double Z = mat->GetZ();
	double A = mat->GetA();

	double me = 0.000511; // GeV;
	double p = std::abs(1. / par->GetQp()); //GeV
	double E = std::sqrt(me * me + p * p);
	double gamma = E / me;

	double I = CalcI(Z) * 1e-9; // GeV

	if (par->GetQp() > 0) { // electrons
		return K*(Z/A) * (std::log(2*me/I)+1.5*std::log(gamma) - 0.975);
	} else { //positrons
		return K*(Z/A) * (std::log(2*me/I)+2.*std::log(gamma) - 1.);
	}
}

double CbmLitMaterialEffectsImp::CalcQpAfterEloss(
		double qp,
        double eloss) const
{
	double massSq = fMass*fMass;
	double p = 1./qp;
	double E = std::sqrt(p*p + massSq);
	double q = (qp>0) ? 1.: -1.;
	if (!fDownstream) eloss *= -1.0; // TODO check this
	double Enew = E-eloss;
	double pnew = (Enew > fMass) ? std::sqrt(Enew * Enew - massSq) : 0.;
	if (pnew != 0) return q/pnew;
	else return 1e5;
	//if (!fDownstream) eloss *= -1.0;
	//if (p > 0.) p -= eloss;
	//else p += eloss;
	//return 1./p;
}

double CbmLitMaterialEffectsImp::CalcSigmaSqQp(
          const CbmLitTrackParam* par,
          const CbmLitMaterialInfo* mat) const
{
	double P = (1. / par->GetQp()); // GeV
	double XMASS = fMass; // GeV
	double E = std::sqrt(P * P + XMASS * XMASS);
	double Z = mat->GetZ();
	double A = mat->GetA();
	double RHO = mat->GetRho();
	double STEP = mat->GetLength();
	double EMASS = 0.511 * 1e-3; // GeV

	double BETA = P/E;
	double GAMMA = E/XMASS;

	// Calculate xi factor (KeV).
	double XI = (153.5*Z*STEP*RHO)/(A*BETA*BETA);

	// Maximum energy transfer to atomic electron (KeV).
	double ETA = BETA*GAMMA;
	double ETASQ = ETA*ETA;
	double RATIO = EMASS/XMASS;
	double F1 = 2.*EMASS*ETASQ;
	double F2 = 1.+2.*RATIO*GAMMA+RATIO*RATIO;
	double EMAX = 1e6 * F1/F2;

	double DEDX2 = XI*EMAX*(1.-(BETA*BETA/2.))*1e-12;

	double SDEDX = (E*E*DEDX2) / std::pow(P, 6);

	return std::abs(SDEDX);
}

double CbmLitMaterialEffectsImp::CalcSigmaSqQpElectron(
          const CbmLitTrackParam* par,
          const CbmLitMaterialInfo* mat) const
{
	double x = mat->GetLength(); //cm
	double X0 = mat->GetRL(); //cm
	return par->GetQp() * par->GetQp() *
	           (std::exp(-x/X0 * std::log(3.0)/std::log(2.0)) -
	        	std::exp(-2.0 * x/X0));
}

double CbmLitMaterialEffectsImp::CalcI(
		double Z) const
{
	// mean excitation energy in eV
	if (Z > 16.) return 10 * Z;
	else return 16 * std::pow(Z, 0.9);
}

double CbmLitMaterialEffectsImp::BetheHeitler(
        const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
   double M = fMass; //GeV
   double p = std::abs(1. / par->GetQp());  // GeV
   double rho = mat->GetRho();
   double X0 = mat->GetRL();
   double me = 0.000511; // GeV
   double E = std::sqrt(M * M + p * p);
   double ratio = me/M;

   return (E*ratio*ratio)/(X0*rho);
}

double CbmLitMaterialEffectsImp::PairProduction(
        const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
   double p = std::abs(1. / par->GetQp());  // GeV
   double M = fMass; //GeV
   double rho = mat->GetRho();
   double X0 = mat->GetRL();
   double E = std::sqrt(M * M + p * p);

   return 7e-5*E/(X0*rho);
}

double CbmLitMaterialEffectsImp::BetheBlochSimple(
        const CbmLitMaterialInfo* mat) const
{
	return fEnergyLoss * mat->GetZ() / mat->GetA();
}

double CbmLitMaterialEffectsImp::MPVEnergyLoss(
        const CbmLitTrackParam* par,
        const CbmLitMaterialInfo* mat) const
{
   double M = fMass * 1e3; //MeV
   double p = std::abs(1. / par->GetQp()) * 1e3;  // MeV

//   double rho = mat->GetRho();
   double Z = mat->GetZ();
   double A = mat->GetA();
   double x = mat->GetRho() * mat->GetLength();

   double I = CalcI(Z) * 1e-6; // MeV

   double K = 0.307075; // MeV g^-1 cm^2
   double j = 0.200;

   double E = std::sqrt(M * M + p * p);
   double beta = p / E;
   double betaSq = beta * beta;
   double gamma = E / M;
   double gammaSq = gamma * gamma;

   double ksi = (K/2.)*(Z/A)*(x/betaSq); // MeV

//   double hwp = 28.816 * std::sqrt(rho*Z/A) * 1e-6 ; // MeV
//   double dc = std::log(hwp/I) + std::log(beta*gamma) - 0.5;
//   dc *= 2;
   double dc = 0.;

   double eloss = ksi * (std::log(2*M*betaSq*gammaSq / I) + std::log(ksi/I) + j - betaSq - dc);

   return eloss * 1e-3; //GeV
}
