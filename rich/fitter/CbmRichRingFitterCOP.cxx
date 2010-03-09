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
	fNofHits = pRing->GetNofHits();
	if (fNofHits < 3) {
		pRing->SetRadius(0.);
		pRing->SetCenterX(0.);
		pRing->SetCenterY(0.);
		return;
	}

	if (fNofHits >= MAX_NOF_HITS_IN_RING) {
		cout << "-E- CbmRichRingFitterCOP::DoFit() - Too many hits in the ring:"
		<<fNofHits <<endl;
		pRing->SetRadius(0.);
		pRing->SetCenterX(0.);
		pRing->SetCenterY(0.);
		return;
	}

	for (Int_t i = 0; i < fNofHits; i++) {
		CbmRichHit* hit = (CbmRichHit*) fHitsArray->At(pRing->GetHit(i));
		fHitX[i] = hit->GetX();
		fHitY[i] = hit->GetY();
	}

	FitRing(pRing);
}

void CbmRichRingFitterCOP::FitRing(CbmRichRing* ring)
{
	Float_t radius = 0.;
	Float_t centerX = 0.;
	Float_t centerY = 0.;

	Int_t iterMax = 4;
	Float_t Xi, Yi, Zi;
	Float_t M0, Mx, My, Mz, Mxy, Mxx, Myy, Mxz, Myz, Mzz, Mxz2, Myz2, Cov_xy;
	Float_t A0, A1, A2, A22;
	Float_t epsilon = 0.00001;
	Float_t Dy, xnew, xold, ynew, yold = 10000000.;

	M0 = fNofHits;
	Mx = My = 0.;

	//calculate center of gravity
	for (Int_t i = 0; i < fNofHits; i++) {
		Mx += fHitX[i];
		My += fHitY[i];
	}
	Mx /= M0;
	My /= M0;

	//computing moments (note: all moments are normed, i.e. divided by N)
	Mxx = Myy = Mxy = Mxz = Myz = Mzz = 0.;

	for (Int_t i = 0; i < fNofHits; i++) {
		Xi = fHitX[i] - Mx; //transform to center of gravity coordinate system
		Yi = fHitY[i] - My;
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
	xnew = 0.;

	//Newton's method starting at x=0
	Int_t iter;
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
		if (xnew == 0 || fabs((xnew - xold) / xnew) < epsilon){
			//cout << "iter = " << iter << " N = " << fNhits << endl;
			break;
		}
	}

	//if (iter == iterMax - 1) {
		//  printf("Newton2 does not converge in %d iterations\n",iterMax);
	//	xnew = 0.;
	//}

	//computing the circle parameters
	Float_t GAM = -Mz - xnew - xnew;
	Float_t DET = xnew * xnew - xnew * Mz + Cov_xy;
	if (DET != 0.) {
		centerX = (Mxz * (Myy - xnew) - Myz * Mxy) / DET / 2.;
		centerY = (Myz * (Mxx - xnew) - Mxz * Mxy) / DET / 2.;
		radius = TMath::Sqrt(centerX * centerX + centerY * centerY - GAM);
		centerX += Mx;
		centerY += My;
	} else {
		centerX = 0.;
		centerY = 0.;
		radius = 0.;
	}

	ring->SetRadius(radius);
	ring->SetCenterX(centerX);
	ring->SetCenterY(centerY);

	CalcChi2(ring);

	if (TMath::IsNaN(radius)) ring->SetRadius(0.);
	if (TMath::IsNaN(centerX)) ring->SetCenterX(0.);
	if (TMath::IsNaN(centerY))	ring->SetCenterY(0.);

}
