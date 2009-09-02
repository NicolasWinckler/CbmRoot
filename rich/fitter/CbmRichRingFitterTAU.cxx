/******************************************************************************
*  $Id: CbmRichRingFitterTAU.cxx,v 1.4 2006/09/13 14:55:18 hoehne Exp $
*
*  Class  : CbmRichRingFitterTAU
*  Description: This is the implementation of a particular fittng class.
*               Here the ring is fitted with the TAU algorithm and different weight functions
*               from A. Ayriyan/ G. Ososkov
*
*  Algorithm:                     Alexander Ayriyan 10.08.2005, Gennadi Ososkov
*  Adoption to new Fitter Class : Claudia Hoehne
*  E-mail : C.Hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingFitterTAU.cxx,v $
*  Revision 1.4  2006/09/13 14:55:18  hoehne
*  new optimal weight function added for improved fitting
*
*  Revision 1.3  2006/08/03 13:23:58  hoehne
*  new ring radius correction parameters (for rich_standard.geo, JUN06)
*  (Petr Stolpovsky)
*
*  Revision 1.2  2006/07/17 14:06:25  hoehne
*  ring radius correction added, see P. Stolpovsky, CBM simulation meeting 14.7.2006
*
*  Revision 1.1  2006/01/25 13:33:19  hoehne
*  initial version: ring fitting routines so far implemented in CbmRichLightSpot
*
*
*
*******************************************************************************/
#include "CbmRichRingFitterTAU.h"

#include "CbmRichRing.h"
#include "CbmRichHit.h"

#include "FairRootManager.h"

#include "TMath.h"

#include <vector>
#include <iostream>
#include <cmath>

using std::cout;
using std::endl;
using std::vector;
using std::pow;
using std::fabs;
using std::sqrt;

ClassImp(CbmRichRingFitterTAU)

// -----   Default constructor   -------------------------------------------
CbmRichRingFitterTAU::CbmRichRingFitterTAU()
{
fRobust     = 0;
fVerbose      = 1;
fCorrection   = 1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   -------------------------------------------
CbmRichRingFitterTAU::CbmRichRingFitterTAU(Int_t verbose,Double_t correction,Int_t robust)
{
  fRobust     = robust;
  fVerbose = verbose;
  fCorrection   = correction;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmRichRingFitterTAU::~CbmRichRingFitterTAU()
{
}
// -------------------------------------------------------------------------


// -----   Public method DoFit   ------------------------------------------
void CbmRichRingFitterTAU::DoFit(CbmRichRing *pRing)
{

    Int_t fNhits=pRing->GetNofHits();

    if (fVerbose > 1) {
         cout<<" RingFitTAU: Number of Hits in this ring : "<<fNhits<<endl;
         if (fCorrection == 0) cout<<" No correction of fitted radius done " <<endl;
         if (fCorrection == 1) cout<<" Correction of ring radius chosen: valid for rich_standard.geo, JUN06 " <<endl;
         if (fCorrection > 1)  cout<<" - ERROR - correction value not valid: has to be either 0 (no cor.) or 1 (cor.) "<<endl;
    }

    // parameters for ring correction
    // note: optimized for rich_standard.geo JUN06
    Double_t A=1.0861e-2;
    Double_t B=-6.9254e-5;
    Double_t C=9.6712e-8;
    Double_t y0=-109.363;
    Double_t R0=6.27642;

    Double_t fRadius  = 0.;
    Double_t fCenterX = 0.;
    Double_t fCenterY = 0.;

    if (fNhits < 3) {
      pRing->SetRadius(fRadius);
      pRing->SetCenterX(fCenterX);
      pRing->SetCenterY(fCenterY);
      return;
    }

//    CbmRichHit *hit = NULL;

    Int_t i, iter, iterMax=20;

	Int_t riter, RiterMax = 4;

	Double_t Xi,Yi,Zi;
	Double_t M0,Mx,My,Mz,Mxy,Mxx,Myy,Mxz,Myz,Mzz,Mxz2,Myz2,Cov_xy;
	Double_t A0,A1,A2,A22,A3,A33,epsilon=0.000000000001;
	Double_t Dy,xnew,xold,ynew,yold=100000000000.;
	Double_t GAM,DET;
	Double_t Xcenter = -1.,Ycenter = -1.,Radius = -1.;

	Double_t sigma;
	Double_t ctsigma;
	Double_t dist,weight;
	const Double_t ct = 3.;
	const Double_t copt = 0.2;
	const Double_t zero = 0.0001;
	const Double_t SigmaMin = 0.18;
	Double_t amax = -100000;
	Double_t amin =  100000;
	Double_t sig1 = 0.;
	Double_t sig2 = 0.;

	vector<Double_t> x;		// x coordinats of hits;
	vector<Double_t> y;		// y coordinats of hits;
	vector<Double_t> a;		// amplitudes of hits;
	vector<Double_t> w;		// weights of points;
	vector<Double_t> d;		// distance between points and circle;

	if(fRobust < 1 || fRobust > 2){
	  if(fVerbose > 1) cout<<"TAU (Taubin) algorithm for fit points by circle"<<endl;
	  RiterMax = 1;
	}
	if(fRobust == 1){
	  if(fVerbose >1) cout <<"TAU (Taubin) algorithm for robust fit points by circle with Tukey's weight function"<<endl;
	  RiterMax = 4;
	}
	if(fRobust == 2){
	  if(fVerbose >1) cout <<"TAU (Taubin) algorithm for robust fit points by circle with optimal weight function"<<endl;
	  RiterMax = 4;
	}

	for(i = 0; i < fNhits; i++) {
		CbmRichHit* hit = (CbmRichHit*)fHitsArray->At(pRing->GetHit(i));
		x.push_back(hit->GetX());
		y.push_back(hit->GetY());
		a.push_back(hit->GetAmplitude());
//		cout<<"x =  "<<x[i]<<";   y="<<y[i]<<";   a="<<a[i]<<endl;
	}

	for(i = 0; i < fNhits; i++){
		if(a[i] > amax) amax = a[i];
		if(a[i] < amin) amin = a[i];
	}

	for(i = 0; i < fNhits; i++) w.push_back(1.);

	for(riter = 0; riter < RiterMax; riter++){

		M0 = 0;
		Mx=My=0.;

		for(i = 0; i < fNhits; i++) {
			Mx += x[i]*w[i];
			My += y[i]*w[i];
			M0 += w[i];
		}

		Mx /= M0;
		My /= M0;

	//computing moments (note: all moments are normed, i.e. divided by N)
		Mxx=Myy=Mxy=Mxz=Myz=Mzz=0.;

		for (i = 0; i < fNhits; i++) {
			Xi = x[i] - Mx;
			Yi = y[i] - My;
			Zi = Xi*Xi + Yi*Yi;

			Mxy += Xi*Yi*w[i];
			Mxx += Xi*Xi*w[i];
			Myy += Yi*Yi*w[i];
			Mxz += Xi*Zi*w[i];
			Myz += Yi*Zi*w[i];
			Mzz += Zi*Zi*w[i];
		}
		Mxx /= M0;
		Myy /= M0;
		Mxy /= M0;
		Mxz /= M0;
		Myz /= M0;
		Mzz /= M0;

	//    computing the coefficients of the characteristic polynomial
		Mz = Mxx + Myy;
		Cov_xy = Mxx*Myy - Mxy*Mxy;
		Mxz2 = Mxz*Mxz;
		Myz2 = Myz*Myz;

		A3 = 4.*Mz;
		A2 = - 3.*Mz*Mz - Mzz;
		A1 = Mzz*Mz + 4.*Cov_xy*Mz - Mxz2 - Myz2 - Mz*Mz*Mz;
		A0 = Mxz2*Myy + Myz2*Mxx - Mzz*Cov_xy - 2.*Mxz*Myz*Mxy+Mz*Mz*Cov_xy;

		A22 = A2 + A2;
		A33 = A3 + A3 + A3;
		iter = 0;
		xnew = 0.;

	//    Newton's method starting at x=0
		for (iter=0; iter<iterMax; iter++) {
			ynew = A0 + xnew*(A1 + xnew*(A2 + xnew*A3));

			if (fabs(ynew)>fabs(yold)) {
	//            printf("Newton3 goes wrong direction: ynew=%f  yold=%f\n",ynew,yold);
				xnew = 0.;
				break;
			}

			Dy = A1 + xnew*(A22 + xnew*A33);
			xold = xnew;
			xnew = xold - ynew/Dy;

			if (fabs((xnew-xold)/xnew) < epsilon) break;
		}

		if (iter == iterMax-1) {
	//        printf("Newton3 does not converge in %d iterations\n",iterMax);
			xnew = 0.;
		}
		if (xnew<0.) {
			iter=30;
	//        printf("Negative root:  x=%f\n",xnew);
		}

	//    computing the circle parameters
		GAM = - Mz;
		DET = xnew*xnew - xnew*Mz + Cov_xy;
		Xcenter = (Mxz*(Myy-xnew) - Myz*Mxy)/DET/2.;
		Ycenter = (Myz*(Mxx-xnew) - Mxz*Mxy)/DET/2.;
		Radius = TMath::Sqrt(Xcenter*Xcenter+Ycenter*Ycenter-GAM);
		Xcenter = (Mxz*(Myy - xnew) - Myz*Mxy)/DET/2. + Mx;
		Ycenter = (Myz*(Mxx - xnew) - Mxz*Mxy)/DET/2. + My;

		if(riter < RiterMax -1){

			for(i = 0; i< fNhits; i++){
				dist = sqrt(pow((Xcenter - x[i]), 2) + pow((Ycenter - y[i]), 2)) - Radius;
				dist = fabs(dist);
				d.push_back(dist);
			}

			for(i = 0; i < (Int_t)d.size(); i++){
				sig1 += w[i]*d[i]*d[i];
				sig2 += w[i];
			}
			sigma = sqrt(sig1/sig2);
			if(sigma < SigmaMin) sigma = SigmaMin;
			ctsigma = ct*sigma;
			sig1 = 0.;
			sig2 = 0.;

			w.clear();
			for(i = 0; i < (Int_t)d.size(); i++){
				if(fRobust == 1){	//! Tukey's weight function
					if(d[i] <= ctsigma){
						weight = pow((1 - pow((d[i]/ctsigma), 2)), 2);
						if(weight < zero) weight = zero;
					}else{
						weight = zero;
					}
					w.push_back(weight);
				}
				if(fRobust == 2 || fRobust == 3){	//! Optimal weight function
					sigma *= 2;
					weight = 1 + copt;
					weight /= 1 + copt*exp(pow((d[i]/sigma), 2));
					if(weight < zero) weight = zero;
					w.push_back(weight);
				}
			}
			d.clear();
		}

	}

	x.clear();
	y.clear();
	a.clear();
	w.clear();

    fCenterX = Xcenter;
    fCenterY = Ycenter;
    fRadius  = Radius;

    pRing->SetRadius(fRadius);
    pRing->SetCenterX(fCenterX);
    pRing->SetCenterY(fCenterY);

    CalcChi2(pRing);


    if (fVerbose > 1 && fCorrection ==1)
    cout << " Radius before correction: " << fRadius << endl;

  // Radius correction (Petr Stolpovsky, for the method see CBM simulation meeting, 14.7.06)
    Double_t xx,yy;
    xx=fCenterX;
    yy=TMath::Abs(fCenterY);
    Double_t Rxy=TMath::Sqrt(xx*xx+(yy-y0)*(yy-y0));
    Double_t N=(fCorrection*(A*Rxy+B*Rxy*Rxy+C*Rxy*Rxy*Rxy)/R0)+1.;      // if fCorrection = 0: no correction done
    fRadius=fRadius/N;
  //end of correction

    if (TMath::IsNaN(fRadius) == 1) fRadius = 0.;
    if (TMath::IsNaN(fCenterX) == 1) pRing->SetCenterX(0.);
    if (TMath::IsNaN(fCenterY) == 1) pRing->SetCenterY(0.);

    pRing->SetRadius(fRadius);


    if (fVerbose > 1)
         cout<<"fRadius , fCentreX, fCentreY :"<<fRadius<<" "<<fCenterX<<" "<<fCenterY<<endl;



}

ClassImp(CbmRichRingFitterTAU)
