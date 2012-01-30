#include "weight/CbmLitWeightedHitCalculatorImp.h"

#include "data/CbmLitHit.h"
#include "data/CbmLitStripHit.h"
#include "data/CbmLitPixelHit.h"

#include <iostream>
#include <cmath>

CbmLitWeightedHitCalculatorImp::CbmLitWeightedHitCalculatorImp()
{
}

CbmLitWeightedHitCalculatorImp::~CbmLitWeightedHitCalculatorImp()
{
}

void CbmLitWeightedHitCalculatorImp::DoCalculate(
   HitPtrIterator itBegin,
   HitPtrIterator itEnd,
   CbmLitHit* hit)
{
   if (hit->GetType() == kLITSTRIPHIT) {
      CalculateWeightedHit(itBegin, itEnd, static_cast<CbmLitStripHit*>(hit));
   } else if (hit->GetType() == kLITPIXELHIT) {
      CalculateWeightedHit(itBegin, itEnd, static_cast<CbmLitPixelHit*>(hit));
   }
}

void CbmLitWeightedHitCalculatorImp::DoCalculate(
   HitPtrVector& hits,
   CbmLitHit* hit)
{
   return DoCalculate(hits.begin(), hits.end(), hit);
}

void CbmLitWeightedHitCalculatorImp::CalculateWeightedHit(
   HitPtrIterator itBegin,
   HitPtrIterator itEnd,
   CbmLitPixelHit* hit)
{
   //calculate effective weight matrix G
   litfloat eG00 = 0., eG01 = 0., eG11 = 0.;
   for (HitPtrIterator it = itBegin; it != itEnd; it++) {
      CbmLitPixelHit* h = static_cast<CbmLitPixelHit*>(*it);
      if (h->IsOutlier()) { continue; }
      litfloat dxx = h->GetDx() * h->GetDx();
      litfloat dyy = h->GetDy() * h->GetDy();
      litfloat dxy = h->GetDxy();

      litfloat G00, G01, G11;
      Inverse(dxx, dxy, dyy, G00, G01, G11);

      litfloat w = h->GetW();
      eG00 += w * G00;
      eG01 += w * G01;
      eG11 += w * G11;
   }

   //calculate effective cov matrix V
   litfloat eV00, eV01, eV11;
   Inverse(eG00, eG01, eG11, eV00, eV01, eV11);

   // calculate effective x and y
   litfloat m0 = 0., m1 = 0.;
   for (HitPtrIterator it = itBegin; it != itEnd; it++) {
      CbmLitPixelHit* h = static_cast<CbmLitPixelHit*>(*it);
      if (h->IsOutlier()) { continue; }
      litfloat dxx = h->GetDx() * h->GetDx();
      litfloat dyy = h->GetDy() * h->GetDy();
      litfloat dxy = h->GetDxy();
      litfloat x = h->GetX();
      litfloat y = h->GetY();

      litfloat G00, G01, G11;
      Inverse(dxx, dxy, dyy, G00, G01, G11);

      litfloat w = h->GetW();

      m0 += w * (G00 * x + G01 * y);
      m1 += w * (G11 * y + G01 * x);
   }
   litfloat x = eV00 * m0 + eV01 * m1;
   litfloat y = eV11 * m1 + eV01 * m0;

   hit->SetX(x);
   hit->SetY(y);
   hit->SetZ((*itBegin)->GetZ());
   hit->SetDx(std::sqrt(eV00));
   hit->SetDxy(eV01);
   hit->SetDy(std::sqrt(eV11));
   hit->SetPlaneId((*itBegin)->GetPlaneId());
}

void CbmLitWeightedHitCalculatorImp::CalculateWeightedHit(
   HitPtrIterator itBegin,
   HitPtrIterator itEnd,
   CbmLitStripHit* hit)
{
   //calculate effective weight matrix G
   litfloat eG = 0.;
   for (HitPtrIterator it = itBegin; it != itEnd; it++) {
      CbmLitStripHit* h = static_cast<CbmLitStripHit*>(*it);
      if (h->IsOutlier()) { continue; }
      litfloat duu = h->GetDu() * h->GetDu();
      litfloat w = h->GetW();
      litfloat G = 1./duu;
      eG += w * G;
   }
   litfloat eV = 1./eG;

   litfloat m0 = 0.;
   for (HitPtrIterator it = itBegin; it != itEnd; it++) {
      CbmLitStripHit* h = static_cast<CbmLitStripHit*>(*it);
      if (h->IsOutlier()) { continue; }
      litfloat duu = h->GetDu() * h->GetDu();
      litfloat u = h->GetU();
      litfloat w = h->GetW();
      m0 += w * (1./duu) * u;
   }
   litfloat eU = eV * m0;

   hit->SetU(eU);
   hit->SetZ((*itBegin)->GetZ());
   hit->SetDu(std::sqrt(eV));
   hit->SetPlaneId((*itBegin)->GetPlaneId());
}

void CbmLitWeightedHitCalculatorImp::Inverse(
   litfloat v00, litfloat v01, litfloat v11,
   litfloat& u00, litfloat& u01, litfloat& u11)
{
   if (v01 != 0.) {
      litfloat norm = v00 * v11 - v01 * v01;
      u00 = v11 / norm;
      u01 = -v01 / norm;
      u11 = v00 / norm;
   } else {
      u00 = 1. / v00;
      u01 = 0.;
      u11 = 1. / v11;
   }
}
