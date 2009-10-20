/** LitAddMaterial.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Functions for calculation of the material effects used in the
 * parallel version of the tracking.
 **/

#ifndef LITADDMATERIAL_H_
#define LITADDMATERIAL_H_

#include "LitTrackParam.h"
#include "LitMaterialInfo.h"

#define cnst static const fvec

/* Function calculates effects from the material on the
 * track parameters and its covariance matrix.
 * @param par Reference to INPUT/OUTPUT vectorized track parameters.
 * @param mat Reference to material
 */
inline void LitAddMaterial(
		LitTrackParam& par,
		const LitMaterialInfo& mat) {
	cnst ZERO = 0.0, ONE = 1., TWO = 2., THREE = 3., INF = 1.e5;
	cnst mass = 0.105658369; // muon mass [GeV/c]
	cnst massSq = 0.105658369 * 0.105658369; // muon mass squared
	cnst C1_2 = 0.5, C1_3 = 1./3.;
	cnst me = 0.000511; // Electron mass [GeV/c]
	cnst ratio = me / mass;

	fvec p = ONE / par.Qp; // Momentum [GeV/c]
	fvec E = sqrt(massSq + p * p);
	fvec beta = p / E;
	fvec betaSq = beta * beta;
	fvec gamma = E / mass;
	fvec gammaSq = gamma * gamma;

	//scale material thickness
	fvec norm = sqrt(ONE + par.Tx * par.Tx + par.Ty * par.Ty);
	fvec thickness = norm * mat.Thickness;
	fvec radThick = thickness / mat.X0;
	fvec sqrtRadThick = sqrt(radThick);
	fvec logRadThick = log(radThick);

	/*
	 * Energy loss corrections
	 */

	// Bethe-Block
	cnst K = 0.000307075; // GeV * g^-1 * cm^2
	fvec Tmax = (2 * me * betaSq * gammaSq) / (ONE + TWO * gamma * ratio + ratio * ratio);

	// density correction
	fvec dc = ZERO;
//	if (p > 0.5) { // for particles above 1 Gev
//		cnst c7 = 28.816;
//		cnst c8 = 1e-9;
//		fvec hwp = c7 * sqrt(mat.Rho * mat.Z / mat.A) * c8; // GeV
//		dc = log(hwp/mat.I) + log(beta*gamma) - C1_2;
//	}

	fvec bbLoss = K * (mat.Z / mat.A) * rcp(betaSq) *
			(C1_2 * log(TWO * me * betaSq * gammaSq * Tmax / (mat.I * mat.I)) - betaSq - dc);

	// Bethe-Heitler
	fvec bhLoss = (E * ratio * ratio)/(mat.X0 * mat.Rho);

	// Pair production approximation
	cnst c3 = 7e-5;
	fvec ppLoss = c3 * E / (mat.X0 * mat.Rho);

	// Integrated value of the energy loss
	fvec energyLoss = (bbLoss + bhLoss + ppLoss) * mat.Rho * thickness;

	// Correct Q/p value due to energy loss
//	fvec q = sgn(par.Qp);//(ZERO < par.Qp) ? ONE: -ONE;
	fvec Enew = E - energyLoss;
//	fvec pnew = (mass < Enew) ? sqrt(Enew * Enew - massSq) : ZERO;
//	par.Qp = (pnew != ZERO) ? q / pnew : INF;
	fvec pnew = sqrt(Enew * Enew - massSq);
	par.Qp = sgn(par.Qp) * rcp(pnew);

	// Calculate Q/p correction in the covariance matrix
	// Calculate xi factor (KeV).
	cnst c4 = 153.5;
	fvec XI = (c4 * mat.Z * thickness * mat.Rho)/(mat.A * betaSq);

	// Maximum energy transfer to atomic electron (KeV).
	fvec eta = beta * gamma;
	fvec etaSq = eta * eta;
	fvec F1 = TWO * me * etaSq;
	fvec F2 = ONE + TWO * ratio * gamma + ratio * ratio;
	cnst c5 = 1e6;
	fvec emax = c5 * F1/F2;

	cnst c6 = 1e-12;
	fvec dedxSq = XI * emax * (ONE - C1_2 * betaSq) * c6;

	fvec p2 = p * p;
	fvec p6 = p2 * p2 * p2;
	fvec qpCorr = (E * E * dedxSq) / p6;
	par.C14 += qpCorr; //abs(qpCorr);
	// end calculate Q/p correction in the covariance matrix

	/*
	 * End energy loss corrections
	 */

	/*
	 * Multiple scattering corrections
	 */
	fvec tx = par.Tx;
	fvec ty = par.Ty;
	fvec bcp = beta * p;

	cnst c1 = 0.0136, c2 = 0.038;
	fvec theta = c1 * rcp(bcp) * sqrtRadThick * (ONE + c2 * logRadThick);
	fvec thetaSq = theta * theta;

	fvec t = ONE + tx * tx + ty * ty;

	fvec Q33 = (1 + tx * tx) * t * thetaSq;
	fvec Q44 = (1 + ty * ty) * t * thetaSq;
	fvec Q34 = tx * ty * t * thetaSq;

	fvec T23 = thickness * thickness * C1_3;
	fvec T2 = thickness * C1_2;

	par.C0 += Q33 * T23;
	par.C1 += Q34 * T23;
	par.C2 += Q33 * T2;
	par.C3 += Q34 * T2;

	par.C5 += Q44 * T23;
	par.C6 += Q34 * T2;
	par.C7 += Q44 * T2;

	par.C9 += Q33;
	par.C10 += Q34;

	par.C12 += Q44;

	/*
	 * End multiple scattering corrections
	 */
}

#undef cnst
#endif /* LITADDMATERIAL_H_ */
