/******************************************************************************
*  $Id: CbmRichRingFitterCOP.cxx,v 1.5 2006/09/13 14:59:21 hoehne Exp $
*
*  Class  : CbmRichRingFitterCOP
*  Description: This is the implementation of a particular fittng class.
*               Here the ring is fitted with theh COP algorithm from A. Ayriyan/ G. Ososkov
*
*  Algorithm:                     Alexander Ayriyan 10.08.2005, Gennadi Ososkov
*  Adoption to new Fitter Class : Claudia Hoehne
*  E-mail : C.Hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingFitterCOP.cxx,v $
*  Revision 1.5  2006/09/13 14:59:21  hoehne
*  setting of reconstructed flaf removed
*
*  Revision 1.4  2006/08/03 13:23:58  hoehne
*  new ring radius correction parameters (for rich_standard.geo, JUN06)
*  (Petr Stolpovsky)
*
*  Revision 1.3  2006/07/17 14:06:25  hoehne
*  ring radius correction added, see P. Stolpovsky, CBM simulation meeting 14.7.2006
*
*  Revision 1.2  2006/01/25 13:31:40  hoehne
*  change int -> Int_t and double -> Double_t
*
*  Revision 1.1  2006/01/19 11:50:31  hoehne
*  initial version: implementation of COP fit to ring
*
*
*******************************************************************************/
#include "CbmRichRingFitterCOP.h"

#include "CbmRichRing.h"
#include "CbmRichHit.h"

#include "FairRootManager.h"

#include "TMath.h"

#include <iostream>
#include <cmath>

using std::cout;
using std::endl;
using std::fabs;

ClassImp(CbmRichRingFitterCOP)

// -----   Default constructor   -------------------------------------------
CbmRichRingFitterCOP::CbmRichRingFitterCOP()
{

}
// -------------------------------------------------------------------------

// -----   Standard constructor   -------------------------------------------
CbmRichRingFitterCOP::CbmRichRingFitterCOP(Int_t verbose,Double_t correction)
{
//  fVerbose = verbose;
//  fCorrection   = correction;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmRichRingFitterCOP::~CbmRichRingFitterCOP()
{
}
// -------------------------------------------------------------------------

// -----   Public method DoFit   ------------------------------------------
void CbmRichRingFitterCOP::DoFit(CbmRichRing *pRing)
{
    Int_t fNhits = pRing->GetNofHits();
	Double_t fRadius = 0.;
	Double_t fCenterX = 0.;
	Double_t fCenterY = 0.;

	if (fNhits < 3) {
		return;
	}

	Int_t i, iter, iterMax = 20;

	Double_t Xi, Yi, Zi;
	Double_t M0, Mx, My, Mz, Mxy, Mxx, Myy, Mxz, Myz, Mzz, Mxz2, Myz2, Cov_xy;
	//    Double_t temp;
	Double_t A0, A1, A2, A22, epsilon = 0.000000000001;
	Double_t Dy, xnew, xold, ynew, yold = 100000000000.;
	Double_t GAM, DET;
	Double_t Xcenter = -1., Ycenter = -1., Radius = -1.;

	M0 = fNhits;
	Mx = My = 0.;

	for (i = 0; i < fNhits; i++) {
		CbmRichHit* hit = (CbmRichHit*) fHitsArray->At(pRing->GetHit(i));
		Mx += hit->GetX();
		My += hit->GetY();
	}

	Mx /= M0;
	My /= M0;

	//computing moments (note: all moments are normed, i.e. divided by N)
	Mxx = Myy = Mxy = Mxz = Myz = Mzz = 0.;

	for (i = 0; i < fNhits; i++) {
		CbmRichHit* hit = (CbmRichHit*) fHitsArray->At(pRing->GetHit(i));
		Xi = hit->GetX() - Mx;
		Yi = hit->GetY() - My;
		Zi = Xi * Xi + Yi * Yi;

		Mxy += Xi * Yi;
		Mxx += Xi * Xi;
		Myy += Yi * Yi;
		Mxz += Xi * Zi;
		Myz += Yi * Zi;
		Mzz += Zi * Zi;
	}
	Mxx /= M0;
	Myy /= M0;
	Mxy /= M0;
	Mxz /= M0;
	Myz /= M0;
	Mzz /= M0;

	//computing the coefficients of the characteristic polynomial
	Mz = Mxx + Myy;
	Cov_xy = Mxx * Myy - Mxy * Mxy;
	Mxz2 = Mxz * Mxz;
	Myz2 = Myz * Myz;

	A2 = 4. * Cov_xy - 3. * Mz * Mz - Mzz;
	A1 = Mzz * Mz + 4. * Cov_xy * Mz - Mxz2 - Myz2 - Mz * Mz * Mz;
	A0 = Mxz2 * Myy + Myz2 * Mxx - Mzz * Cov_xy - 2. * Mxz * Myz * Mxy + Mz
			* Mz * Cov_xy;

	A22 = A2 + A2;
	iter = 0;
	xnew = 0.;

	//Newton's method starting at x=0
	for (iter = 0; iter < iterMax; iter++) {
		ynew = A0 + xnew * (A1 + xnew * (A2 + 4. * xnew * xnew));

		if (fabs(ynew) > fabs(yold)) {
			//  printf("Newton2 goes wrong direction: ynew=%f  yold=%f\n",ynew,yold);
			xnew = 0.;
			break;
		}

		Dy = A1 + xnew * (A22 + 16. * xnew * xnew);
		xold = xnew;
		xnew = xold - ynew / Dy;
		//cout << " xnew = " << xnew ;
		if (xnew == 0 || fabs((xnew - xold) / xnew) < epsilon)
			break;
	}

	if (iter == iterMax - 1) {
		//  printf("Newton2 does not converge in %d iterations\n",iterMax);
		xnew = 0.;
	}
	if (xnew < 0.) {
		iter = 30;
		//      printf("Negative root:  x=%f\n",xnew);
	}

	//    computing the circle parameters
	GAM = -Mz - xnew - xnew;
	DET = xnew * xnew - xnew * Mz + Cov_xy;
	if (DET != 0) {
		Xcenter = (Mxz * (Myy - xnew) - Myz * Mxy) / DET / 2.;
		Ycenter = (Myz * (Mxx - xnew) - Mxz * Mxy) / DET / 2.;
		Radius = TMath::Sqrt(Xcenter * Xcenter + Ycenter * Ycenter - GAM);
		fCenterX = Xcenter + Mx;
		fCenterY = Ycenter + My;
		fRadius = Radius;
	} else {
		fCenterX = 0.;
		fCenterY = 0.;
		fRadius = 0.;
	}

	pRing->SetRadius(fRadius);
	pRing->SetCenterX(fCenterX);
	pRing->SetCenterY(fCenterY);

	CalcChi2(pRing);

	if (TMath::IsNaN(fRadius) == 1)
		pRing->SetRadius(0.);
	if (TMath::IsNaN(fCenterX) == 1)
		pRing->SetCenterX(0.);
	if (TMath::IsNaN(fCenterY) == 1)
		pRing->SetCenterY(0.);

}
