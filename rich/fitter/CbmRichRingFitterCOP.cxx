#include "CbmRichRingFitterCOP.h"
#include "FairRootManager.h"
#include "CbmRichRingLight.h"
#include "CbmRichRing.h"
#include "CbmRichHit.h"
#include "TClonesArray.h"
#include <iostream>
#include <cmath>

using namespace std;

// -----   Default constructor   -------------------------------------------
CbmRichRingFitterCOP::CbmRichRingFitterCOP()
{

}

CbmRichRingFitterCOP::~CbmRichRingFitterCOP()
{
	fHitX.clear();
	fHitY.clear();
}

void CbmRichRingFitterCOP::DoFit1(
      CbmRichRing *pRing,
      const vector<double>& hitX,
      const vector<double>& hitY)
{
   fHitX.clear();
   fHitY.clear();
   fHitX.assign(hitX.begin(), hitX.end());
   fHitY.assign(hitY.begin(), hitY.end());
   fNofHits = fHitX.size();
   FitRing(pRing);
}

void CbmRichRingFitterCOP::DoFit(
      CbmRichRing *pRing)
{
	fNofHits = pRing->GetNofHits();
	if (fNofHits < 3) {
		pRing->SetRadius(0.);
		pRing->SetCenterX(0.);
		pRing->SetCenterY(0.);
		return;
	}

	if (fNofHits >= MAX_NOF_HITS_IN_RING) {
		cout << "-E- CbmRichRingFitterCOP::DoFit() - Too many hits in the ring:"<<fNofHits <<endl;
		pRing->SetRadius(0.);
		pRing->SetCenterX(0.);
		pRing->SetCenterY(0.);
		return;
	}

	for (int i = 0; i < fNofHits; i++) {
		CbmRichHit* hit = (CbmRichHit*) fHitsArray->At(pRing->GetHit(i));
		fHitX[i] = hit->GetX();
		fHitY[i] = hit->GetY();
	}

	FitRing(pRing);
}

void CbmRichRingFitterCOP::FitRing(CbmRichRing* ring)
{
	float radius = 0.;
	float centerX = 0.;
	float centerY = 0.;

	int iterMax = 4;
	float Xi, Yi, Zi;
	float M0, Mx, My, Mz, Mxy, Mxx, Myy, Mxz, Myz, Mzz, Mxz2, Myz2, Cov_xy;
	float A0, A1, A2, A22;
	float epsilon = 0.00001;
	float Dy, xnew, xold, ynew, yold = 10000000.;

	M0 = fNofHits;
	Mx = My = 0.;

	//calculate center of gravity
	for (int i = 0; i < fNofHits; i++) {
		Mx += fHitX[i];
		My += fHitY[i];
	}
	Mx /= M0;
	My /= M0;

	//computing moments (note: all moments are normed, i.e. divided by N)
	Mxx = Myy = Mxy = Mxz = Myz = Mzz = 0.;

	for (int i = 0; i < fNofHits; i++) {
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
	int iter;
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
	float GAM = -Mz - xnew - xnew;
	float DET = xnew * xnew - xnew * Mz + Cov_xy;
	if (DET != 0.) {
		centerX = (Mxz * (Myy - xnew) - Myz * Mxy) / DET / 2.;
		centerY = (Myz * (Mxx - xnew) - Mxz * Mxy) / DET / 2.;
		radius = sqrt(centerX * centerX + centerY * centerY - GAM);
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
}

void CbmRichRingFitterCOP::Init()
{
	cout << "CbmRichRingFitterImpl::Init()"<<endl;
	// Get and check FairRootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if (! ioman) {
		cout << "-E- CbmRichRingFitterImpl::Init()"
		<< "RootManager not instantised!" << endl;
		return;
	}

	// Get hit Array
	fHitsArray = (TClonesArray*) ioman->GetObject("RichHit");
	if ( ! fHitsArray) {
		cout << "-W- CbmRichRingFitterImpl::Init(): No RichHit array!"<< endl;
	}
	fHitX.resize(MAX_NOF_HITS_IN_RING);
	fHitY.resize(MAX_NOF_HITS_IN_RING);
}

// -----   Protected method CalcChi2   ----------------------------------------
void CbmRichRingFitterCOP::CalcChi2(CbmRichRing* pRing)
{
    int fNhits=pRing->GetNofHits();

    if (fNhits < 4) {
      pRing->SetChi2(-1.);
      return;
    }

    float Xd2, Yd2;
    float chi2 = 0.;

    float Radius  = pRing->GetRadius();
    float Xcenter = pRing->GetCenterX();
    float Ycenter = pRing->GetCenterY();

    for (int i = 0; i < fNhits; i++) {
    	Xd2 = Xcenter - fHitX[i];
		Yd2 = Ycenter - fHitY[i];
		Xd2 *= Xd2;
		Yd2 *= Yd2;

		float d = sqrt( Xd2 + Yd2 ) - Radius;
        chi2 += d*d;
    }

    pRing->SetChi2(chi2);
}

