/******************************************************************************
*  $Id: CbmRichRingFitterCircle.cxx,v 1.6 2006/09/13 14:58:50 hoehne Exp $
*
*  Class  : CbmRichRingFitterCircle
*  Description: This is the implementation of a particular fitting class.
*               Here the ring is fitted with equation of a cicle.
*
*  Author : Supriya Das (Algorithm from F77 subroutine of S.Sadovsky)
*  E-mail : S.Das@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingFitterCircle.cxx,v $
*  Revision 1.6  2006/09/13 14:58:50  hoehne
*  setting of ReconstructedFlag removed
*
*  Revision 1.5  2006/08/03 13:23:58  hoehne
*  new ring radius correction parameters (for rich_standard.geo, JUN06)
*  (Petr Stolpovsky)
*
*  Revision 1.4  2006/07/17 14:06:25  hoehne
*  ring radius correction added, see P. Stolpovsky, CBM simulation meeting 14.7.2006
*
*  Revision 1.3  2006/04/11 11:16:21  hoehne
*  calculate mean values for comparison
*
*  Revision 1.2  2006/01/26 09:59:02  hoehne
*  CbmRichHit included
*
*  Revision 1.1  2006/01/19 11:49:34  hoehne
*  initial version: implementation of circle fit to ring
*
*
*******************************************************************************/
#include "CbmRichRingFitterCircle.h"

#include "CbmRichRing.h"
#include "CbmRichHit.h"

#include "CbmRootManager.h"

#include "TMath.h"

#include <iostream>

using std::cout;
using std::endl;


ClassImp(CbmRichRingFitterCircle)

// -----   Default constructor   -------------------------------------------
CbmRichRingFitterCircle::CbmRichRingFitterCircle()
{
  fVerbose      = 1;
  fCorrection   = 1;
}

// -------------------------------------------------------------------------

// -----   Standard constructor   -------------------------------------------
CbmRichRingFitterCircle::CbmRichRingFitterCircle(Int_t verbose,Double_t correction)
{
  fVerbose = verbose;
  fCorrection   = correction;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmRichRingFitterCircle::~CbmRichRingFitterCircle()
{
}
// -------------------------------------------------------------------------


// -----   Public method DoFit   ------------------------------------------
void CbmRichRingFitterCircle::DoFit(CbmRichRing* pRing)
{

    Int_t nHits=pRing->GetNofHits();
    if (fVerbose > 1) {
         cout<<" RingFitCircle: Number of Hits in this ring : "<<nHits<<endl;
	 if (fCorrection == 0) cout<<" No correction of fitted radius done " <<endl;
         if (fCorrection == 1) cout<<" Correction of ring radius chosen: valid for rich_standard.geo, JUN06 " <<endl;
         if (fCorrection > 1)  cout<<" - ERROR - correction value not valid: has to be either 0 (no cor.) or 1 (cor.) "<<endl;
    }
    
    // parameters for ring correction
    // note: optimized for rich_standard.geo JUN06
    Double_t A=1.068e-2;
    Double_t B=-6.8058e-5;
    Double_t C=9.4583e-8;
    Double_t y0=-108.962;
    Double_t R0=6.27781;
    
if (nHits < 3) {
    fRadius  = 0;
    fCentreX = 0;
    fCentreY = 0;
    return;
  }

  Float_t c[3], a[3][3];

  Float_t b1 = 0;
  Float_t b2 = 0;
  Float_t b3 = 0;

  Float_t b12 = 0;
  Float_t b22 = 0;
  Float_t b32 = nHits;

  Float_t a11 = 0;
  Float_t a12 = 0;
  Float_t a13;
  Float_t a21;
  Float_t a22 = 0;
  Float_t a23;
  Float_t a31;
  Float_t a32;
  Float_t a33;

  Double_t fMeanX = 0.;
  Double_t fMeanY = 0.;

  for (Int_t iHit=0; iHit<nHits; iHit++) {

    CbmRichHit* hit = (CbmRichHit*)fHitsArray->At(pRing->GetHit(iHit));

    b1 += (hit->X()*hit->X() + hit->Y()*hit->Y()) * hit->X();
    b2 += (hit->X()*hit->X() + hit->Y()*hit->Y()) * hit->Y();
    b3 += (hit->X()*hit->X() + hit->Y()*hit->Y());

    b12 += hit->X();
    b22 += hit->Y();

    a11 += 2*hit->X()*hit->X();
    a12 += 2*hit->X()*hit->Y();
    a22 += 2*hit->Y()*hit->Y();

    fMeanX = fMeanX + hit->X();
    fMeanY = fMeanY + hit->Y();

  }

  if (nHits !=0) {
   fMeanX = fMeanX/(Double_t)(nHits);
   fMeanY = fMeanY/(Double_t)(nHits);
   }

  a21 = a12;

  a13 = b12;
  a23 = b22;

  a31 = 2*b12;
  a32 = 2*b22;
  a33 = b32;

  c[0] = b1*b22 - b2*b12;
  c[1] = b1*b32 - b3*b12;
  c[2] = b2*b32 - b3*b22;

  a[0][0] = a11*b22 - a21*b12;
  a[1][0] = a12*b22 - a22*b12;
  a[2][0] = a13*b22 - a23*b12;

  a[0][1] = a11*b32 - a31*b12;
  a[1][1] = a12*b32 - a32*b12;
  a[2][1] = a13*b32 - a33*b12;

  a[0][2] = a21*b32-a31*b22;
  a[1][2] = a22*b32-a32*b22;
  a[2][2] = a23*b32-a33*b22;

  Float_t det1 = a[0][0]*a[1][1] - a[0][1]*a[1][0];

  Float_t x11 = (c[0]*a[1][1] - c[1]*a[1][0])/det1;
  Float_t x21 = (a[0][0]*c[1] - a[0][1]*c[0])/det1;

//   Float_t det2 = a[0][1]*a[1][2] - a[0][2]*a[1][1];
//   Float_t det3 = a[0][0]*a[1][2] - a[0][2]*a[1][0];
//   Float_t x12 = (c[1]*a[1][2] - c[2]*a[1][1])/det2;
//   Float_t x22 = (a[0][1]*c[2] - a[0][2]*c[1])/det2;

  fRadius  = TMath::Sqrt((b3 + b32*(x11*x11 + x21*x21) -
                          a31*x11 - a32*x21)/a33);
  fCentreX = x11;
  fCentreY = x21;
  
  if (fVerbose > 1 && fCorrection ==1) 
    cout << " Radius before correction: " << fRadius << endl;
  
  // Radius correction (Petr Stolpovsky, for the method see CBM simulation meeting, 14.7.06)
  x21=TMath::Abs(x21);
  Double_t Rxy=TMath::Sqrt(x11*x11+(x21-y0)*(x21-y0));
  Double_t N=(fCorrection*(A*Rxy+B*Rxy*Rxy+C*Rxy*Rxy*Rxy)/R0)+1.;           // if fCorrection = 0: no correction done
  fRadius=fRadius/N;
  //end of correction

  

  pRing->SetRadius(fRadius);
  pRing->SetCenterX(fCentreX);
  pRing->SetCenterY(fCentreY);
  
  if (TMath::IsNaN(fRadius) == 1) pRing->SetRadius(0.);
  if (TMath::IsNaN(fCentreX) == 1) pRing->SetCenterX(0.);
  if (TMath::IsNaN(fCentreY) == 1) pRing->SetCenterY(0.);
  
  if (fVerbose > 1)
         cout<<"fRadius , fCentreX, fCentreY , fMeanX, fMeanY:"<<fRadius<<" "<<fCentreX<<" "<<fCentreY<<
	 " " << fMeanX << " " << fMeanY << endl;

    
  //pRing->SetCenterX(fMeanX);
  //pRing->SetCenterY(fMeanY);


}
