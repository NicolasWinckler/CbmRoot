/**
* \file CbmRichRingFitterCircle.cxx
*
* \author Supriya Das
* \date 2006
**/

#include "CbmRichRingFitterCircle.h"

#include "CbmRichRing.h"
#include "CbmRichHit.h"
#include "FairRootManager.h"

#include "TMath.h"

#include <iostream>

using std::cout;
using std::endl;


ClassImp(CbmRichRingFitterCircle)

CbmRichRingFitterCircle::CbmRichRingFitterCircle()
{

}

CbmRichRingFitterCircle::~CbmRichRingFitterCircle()
{

}

void CbmRichRingFitterCircle::DoFit(
      CbmRichRing* ring)
{
   Int_t nofHits = ring->GetNofHits();
   if (nofHits < 3) return;

   Double_t radius = 0.;
   Double_t centerX = 0.;
   Double_t centerY = 0.;

   Float_t c[3], a[3][3];

   Float_t b1 = 0;
   Float_t b2 = 0;
   Float_t b3 = 0;

   Float_t b12 = 0;
   Float_t b22 = 0;
   Float_t b32 = nofHits;

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

   for (Int_t iHit = 0; iHit < nofHits; iHit++) {
      CbmRichHit* hit = (CbmRichHit*) fHitsArray->At(ring->GetHit(iHit));

      b1 += (hit->GetX()*hit->GetX() + hit->GetY()*hit->GetY()) * hit->GetX();
      b2 += (hit->GetX()*hit->GetX() + hit->GetY()*hit->GetY()) * hit->GetY();
      b3 += (hit->GetX()*hit->GetX() + hit->GetY()*hit->GetY());

      b12 += hit->GetX();
      b22 += hit->GetY();

      a11 += 2*hit->GetX()*hit->GetX();
      a12 += 2*hit->GetX()*hit->GetY();
      a22 += 2*hit->GetY()*hit->GetY();

      fMeanX = fMeanX + hit->GetX();
      fMeanY = fMeanY + hit->GetY();
   }

   if (nofHits !=0) {
      fMeanX = fMeanX/(Double_t)(nofHits);
      fMeanY = fMeanY/(Double_t)(nofHits);
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

   radius = TMath::Sqrt((b3 + b32*(x11*x11 + x21*x21) - a31*x11 - a32*x21)/a33);
   centerX = x11;
   centerY = x21;

   ring->SetRadius(radius);
   ring->SetCenterX(centerX);
   ring->SetCenterY(centerY);

   if (TMath::IsNaN(radius) == 1) ring->SetRadius(0.);
   if (TMath::IsNaN(centerX) == 1) ring->SetCenterX(0.);
   if (TMath::IsNaN(centerY) == 1) ring->SetCenterY(0.);
}
