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
#include "LitMath.h"

/* Function calculates material effects and adds them to the
 * track parameters and its covariance matrix.
 * @param par Reference to INPUT/OUTPUT track parameters.
 * @param mat Reference to material
 */
template<class T>
inline void LitAddMaterial(
		LitTrackParam<T>& par,
		const LitMaterialInfo<T>& mat) {
	static const T ZERO = 0.0, ONE = 1., TWO = 2., THREE = 3., INF = 1.e5;
	static const T mass = 0.105658369; // muon mass [GeV/c]
	static const T massSq = 0.105658369 * 0.105658369; // muon mass squared
	static const T C1_2 = 0.5, C1_3 = 1./3.;
	static const T me = 0.000511; // Electron mass [GeV/c]
	static const T ratio = me / mass;

	T p = ONE / par.Qp; // Momentum [GeV/c]
	T E = sqrt(massSq + p * p);
	T beta = p / E;
	T betaSq = beta * beta;
	T gamma = E / mass;
	T gammaSq = gamma * gamma;

	//scale material thickness
	T norm = sqrt(ONE + par.Tx * par.Tx + par.Ty * par.Ty);
	T thickness = norm * mat.Thickness;
	T radThick = thickness / mat.X0;
	T sqrtRadThick = sqrt(radThick);//mat.SqrtRadThick;//sqrt(radThick);
	T logRadThick = log(radThick);//mat.LogRadThick;//log(radThick);

	/*
	 * Energy loss corrections
	 */

	// Bethe-Block
	static const T K = 0.000307075; // GeV * g^-1 * cm^2
	T Tmax = (2 * me * betaSq * gammaSq) / (ONE + TWO * gamma * ratio + ratio * ratio);

	// density correction
	T dc = ZERO;
//	if (p > 0.5) { // for particles above 1 Gev
//		static const T c7 = 28.816;
//		static const T c8 = 1e-9;
//		T hwp = c7 * sqrt(mat.Rho * mat.Z / mat.A) * c8; // GeV
//		dc = log(hwp/mat.I) + log(beta*gamma) - C1_2;
//	}

	T bbLoss = K * (mat.Z / mat.A) * rcp(betaSq) *
			(C1_2 * log(TWO * me * betaSq * gammaSq * Tmax / (mat.I * mat.I)) - betaSq - dc);

	// Bethe-Heitler
	T bhLoss = (E * ratio * ratio)/(mat.X0 * mat.Rho);

	// Pair production approximation
	static const T c3 = 7e-5;
	T ppLoss = c3 * E / (mat.X0 * mat.Rho);

	// Integrated value of the energy loss
	T energyLoss = (bbLoss + bhLoss + ppLoss) * mat.Rho * thickness;

	// Correct Q/p value due to energy loss
//	T q = sgn(par.Qp);//(ZERO < par.Qp) ? ONE: -ONE;
	T Enew = E - energyLoss;
//	T pnew = (mass < Enew) ? sqrt(Enew * Enew - massSq) : ZERO;
//	par.Qp = (pnew != ZERO) ? q / pnew : INF;
	T pnew = sqrt(Enew * Enew - massSq);
	par.Qp = sgn(par.Qp) * rcp(pnew);

	// Calculate Q/p correction in the covariance matrix
	// Calculate xi factor (KeV).
	static const T c4 = 153.5;
	T XI = (c4 * mat.Z * thickness * mat.Rho)/(mat.A * betaSq);

	// Maximum energy transfer to atomic electron (KeV).
	T eta = beta * gamma;
	T etaSq = eta * eta;
	T F1 = TWO * me * etaSq;
	T F2 = ONE + TWO * ratio * gamma + ratio * ratio;
	static const T c5 = 1e6;
	T emax = c5 * F1/F2;

	static const T c6 = 1e-12;
	T dedxSq = XI * emax * (ONE - C1_2 * betaSq) * c6;

	T p2 = p * p;
	T p6 = p2 * p2 * p2;
	T qpCorr = (E * E * dedxSq) / p6;
	par.C14 += qpCorr; //abs(qpCorr);
	// end calculate Q/p correction in the covariance matrix

	/*
	 * End energy loss corrections
	 */

	/*
	 * Multiple scattering corrections
	 */
	T tx = par.Tx;
	T ty = par.Ty;
	T bcp = beta * p;

	static const T c1 = 0.0136, c2 = 0.038;
	T theta = c1 * rcp(bcp) * sqrtRadThick * (ONE + c2 * logRadThick);
	T thetaSq = theta * theta;

	T t = ONE + tx * tx + ty * ty;

	T Q33 = (1 + tx * tx) * t * thetaSq;
	T Q44 = (1 + ty * ty) * t * thetaSq;
	T Q34 = tx * ty * t * thetaSq;

	T T23 = thickness * thickness * C1_3;
	T T2 = thickness * C1_2;

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

#endif /* LITADDMATERIAL_H_ */
