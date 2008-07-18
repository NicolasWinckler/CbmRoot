/******************************************************************************
*  $Id: CbmRichRingFitterRobustCOP.cxx,v 1.4 2006/09/13 14:58:09 hoehne Exp $
*
*  Class  : CbmRichRingFitterRobustCOP
*  Description: This is the implementation of a particular fitting class.
*               Here the ring is fitted with the RobustCOP algorithm from A. Ayriyan/ G. Ososkov
*
*  Algorithm:                     Alexander Ayriyan 10.08.2005, Gennadi Ososkov
*  Adoption to new Fitter Class : Claudia Hoehne
*  E-mail : C.Hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingFitterRobustCOP.cxx,v $
*  Revision 1.4  2006/09/13 14:58:09  hoehne
*  setting of ReconstructedFlag removed
*
*  Revision 1.3  2006/08/03 13:23:58  hoehne
*  new ring radius correction parameters (for rich_standard.geo, JUN06)
*  (Petr Stolpovsky)
*
*  Revision 1.2  2006/07/17 14:06:25  hoehne
*  ring radius correction added, see P. Stolpovsky, CBM simulation meeting 14.7.2006
*
*  Revision 1.1  2006/01/25 13:32:33  hoehne
*  initial version: ring fitting routines, so far implemented in CbmRichLightSpot
*
*
*
*******************************************************************************/
#include "CbmRichRingFitterRobustCOP.h"

#include "CbmRichRing.h"
#include "CbmRichHit.h"

#include "CbmRootManager.h"

#include "TMath.h"

#include <iostream>
#include <cmath>

using std::cout;
using std::endl;
using std::sqrt;
using std::fabs;

ClassImp(CbmRichRingFitterRobustCOP)

// -----   Default constructor   -------------------------------------------
CbmRichRingFitterRobustCOP::CbmRichRingFitterRobustCOP()
{
fVerbose      = 1;
fCorrection   =1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   -------------------------------------------
CbmRichRingFitterRobustCOP::CbmRichRingFitterRobustCOP(Int_t verbose,Double_t correction)
{
  fVerbose = verbose;
  fCorrection   = correction;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmRichRingFitterRobustCOP::~CbmRichRingFitterRobustCOP()
{
}
// -------------------------------------------------------------------------


// -----   Public method DoFit   ------------------------------------------
void CbmRichRingFitterRobustCOP::DoFit(CbmRichRing *pRing)
{

    Int_t fNhits=pRing->GetNofHits();

    if (fVerbose > 1) {
         cout<<" RingFitRobustCOP: Number of Hits in this ring : "<<fNhits<<endl;
	 if (fCorrection == 0) cout<<" No correction of fitted radius done " <<endl;
         if (fCorrection == 1) cout<<" Correction of ring radius chosen: valid for rich_standard.geo, JUN06 " <<endl;
         if (fCorrection > 1)  cout<<" - ERROR - correction value not valid: has to be either 0 (no cor.) or 1 (cor.) "<<endl;
    }
    
    // parameters for ring correction
    // note: optimized for rich_standard.geo JUN06
    Double_t A=1.4119e-2;
    Double_t B=-6.9651e-5;
    Double_t C=8.0868e-8;
    Double_t y0=-166.53;
    Double_t R0=6.08821;

    if (fNhits < 3) {
      fRadius  = 0;
      fCentreX = 0;
      fCentreY = 0;
      return;
    }


	CbmRichHit *hit = NULL;

	Int_t i,iter,iterMax=20;
	Int_t i_iter, i_max_Robust = 4;
	const Int_t MinNuberOfHits = 3;
	const Int_t MaxNuberOfHits = 2000;
	Double_t Xi,Yi,Zi;
	Double_t M0,Mx,My,Mz,Mxy,Mxx,Myy,Mxz,Myz,Mzz,Mxz2,Myz2,Cov_xy;//,temp;
	Double_t A0,A1,A2,A22,epsilon=0.000000000001;
	Double_t Dy,xnew,xold,ynew,yold=100000000000.;
	Double_t GAM,DET;
	Double_t Xcenter,Ycenter,Radius;
	Double_t SumS1 = 0., SumS2 = 0.;
	Double_t sigma;
	Double_t ctsigma;
	Double_t sigma_min = 0.05;
	Double_t dx, dy;
	Double_t d[MaxNuberOfHits];
	Double_t w[MaxNuberOfHits];
	Double_t ct = 7.;

    for(i = 0; i < MaxNuberOfHits; i++) w[i] = 1.;

    Mx=My=0.;
    for(i = 0; i < fNhits; i++) {
        hit = (CbmRichHit*)fHitsArray->At(pRing->GetHit(i));
        Mx += hit->X();
        My += hit->Y();
    }

    M0 = fNhits;
    Mx /= M0;
    My /= M0;

	for(i_iter = 0; i_iter < i_max_Robust; i_iter++){
		sigma = sigma_min;
		if(i_iter != 0){
			for(i = 0; i < fNhits; i++){
                                hit = (CbmRichHit*)fHitsArray->At(pRing->GetHit(i));
				dx = Mx - hit->X();
				dy = My - hit->Y();
				d[i] = sqrt(dx*dx + dy*dy) - Radius;
				SumS1 += w[i]*d[i]*d[i];
				SumS2 += w[i];
			}
			if(SumS2 == 0.){ sigma = sigma_min; } else{ sigma = sqrt(SumS1/SumS2);}
			if(sigma < sigma_min) sigma = sigma_min;
			ctsigma = ct*sigma;
			SumS1 = 0.;
			SumS2 = 0.;
			for(i = 0; i < fNhits; i++){
				if(d[i] <= ctsigma){
					w[i] = (1 - (d[i]/(ctsigma))*(d[i]/(ctsigma)))*(1 - (d[i]/(ctsigma))*(d[i]/(ctsigma)));
				}else{w[i] = 0.;}
			}
		}
		//computing moments (note: all moments are normed, i.e. divided by N)
		M0 = 0;
		Mxx=Myy=Mxy=Mxz=Myz=Mzz=0.;
		for (i = 0; i < fNhits; i++) {
			hit = (CbmRichHit*)fHitsArray->At(pRing->GetHit(i));
			if(w[i] != 0.){
				Xi = hit->X() - Mx;
				Yi = hit->Y() - My;
				Zi = Xi*Xi + Yi*Yi;
				Mxy += Xi*Yi;
				Mxx += Xi*Xi;
				Myy += Yi*Yi;
				Mxz += Xi*Zi;
				Myz += Yi*Zi;
				Mzz += Zi*Zi;
				M0++;
			}
		}
		if(M0 < MinNuberOfHits){
			M0 = 0;
			Mxx=Myy=Mxy=Mxz=Myz=Mzz=0.;
			M0 = 0;
			for (i = 0; i < fNhits; i++) {
				hit = (CbmRichHit*)fHitsArray->At(pRing->GetHit(i));
				Xi = hit->X() - Mx;
				Yi = hit->Y() - My;
				Zi = Xi*Xi + Yi*Yi;
				Mxy += Xi*Yi;
				Mxx += Xi*Xi;
				Myy += Yi*Yi;
				Mxz += Xi*Zi;
				Myz += Yi*Zi;
				Mzz += Zi*Zi;
				M0++;
			}
		}
		Mxx /= M0;
		Myy /= M0;
		Mxy /= M0;
		Mxz /= M0;
		Myz /= M0;
		Mzz /= M0;

		//computing the coefficients of the characteristic polynomial
		Mz = Mxx + Myy;
		Cov_xy = Mxx*Myy - Mxy*Mxy;
		Mxz2 = Mxz*Mxz;
		Myz2 = Myz*Myz;

		A2 = 4.*Cov_xy - 3.*Mz*Mz - Mzz;
		A1 = Mzz*Mz + 4.*Cov_xy*Mz - Mxz2 - Myz2 - Mz*Mz*Mz;
		A0 = Mxz2*Myy + Myz2*Mxx - Mzz*Cov_xy - 2.*Mxz*Myz*Mxy + Mz*Mz*Cov_xy;

		A22 = A2 + A2;
		iter = 0;
		xnew = 0.;

		//Newton's method starting at x=0
		for(iter = 0; iter < iterMax; iter++) {
			ynew = A0 + xnew*(A1 + xnew*(A2 + 4.*xnew*xnew));

			if (fabs(ynew)>fabs(yold))
			{
			//  printf("Newton2 goes wrong direction: ynew=%f  yold=%f\n",ynew,yold);
				xnew = 0.;
				break;
			}

			Dy = A1 + xnew*(A22 + 16.*xnew*xnew);
			xold = xnew;
			xnew = xold - ynew/Dy;

			if (fabs((xnew-xold)/xnew) < epsilon) break;
		}

		if (iter == iterMax-1) {
		//  printf("Newton2 does not converge in %d iterations\n",iterMax);
			xnew = 0.;
		}
		if (xnew<0.)
		{
			iter=30;
	//      printf("Negative root:  x=%f\n",xnew);
		}

	//    computing the circle parameters
		GAM = - Mz - xnew - xnew;
		DET = xnew*xnew - xnew*Mz + Cov_xy;
		Xcenter = (Mxz*(Myy-xnew) - Myz*Mxy)/DET/2.;
		Ycenter = (Myz*(Mxx-xnew) - Mxz*Mxy)/DET/2.;
		//cout << " Radius = " << Radius << endl;
		Radius = sqrt(Xcenter*Xcenter+Ycenter*Ycenter-GAM);
		Xcenter = Xcenter + Mx;
		Ycenter = Ycenter + My;
		Mx = Xcenter;
		My = Ycenter;

		if (DET == 0.) {
			fRadius  = 0.;
			fCentreX = 0.;
			fCentreY = 0.;
			return;
		}

	}


    fCentreX = Xcenter;
    fCentreY = Ycenter;
    fRadius  = Radius;
    
    if (fVerbose > 1 && fCorrection ==1) 
    cout << " Radius before correction: " << fRadius << endl;
    
  // Radius correction (Petr Stolpovsky, for the method see CBM simulation meeting, 14.7.06)
    Double_t xx,yy;
    xx=fCentreX;
    yy=TMath::Abs(fCentreY);
    Double_t Rxy=TMath::Sqrt(xx*xx+(yy-y0)*(yy-y0));
    Double_t N=(fCorrection*(A*Rxy+B*Rxy*Rxy+C*Rxy*Rxy*Rxy)/R0)+1.;      // if fCorrection = 0: no correction done
    fRadius=fRadius/N;
  //end of correction
  
    if (fVerbose > 1)
         cout<<"fRadius , fCentreX, fCentreY :"<<fRadius<<" "<<fCentreX<<" "<<fCentreY<<endl;

    pRing->SetRadius(fRadius);
    pRing->SetCenterX(fCentreX);
    pRing->SetCenterY(fCentreY);


  if (TMath::IsNaN(fRadius) == 1) pRing->SetRadius(0.);
  if (TMath::IsNaN(fCentreX) == 1) pRing->SetCenterX(0.);
  if (TMath::IsNaN(fCentreY) == 1) pRing->SetCenterY(0.);
}
