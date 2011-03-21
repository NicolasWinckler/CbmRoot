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

LitStatus CbmLitWeightedHitCalculatorImp::Initialize()
{
   return kLITSUCCESS;
}

LitStatus CbmLitWeightedHitCalculatorImp::Finalize()
{
   return kLITSUCCESS;
}

void CbmLitWeightedHitCalculatorImp::DoCalculate(
   HitPtrIterator itBegin,
   HitPtrIterator itEnd,
   CbmLitHit* hit)
{
   if (hit->GetType() == kLITSTRIPHIT) {
      CalculateWeightedHit(itBegin, itEnd, static_cast<const CbmLitStripHit*>(hit));
   } else if (hit->GetType() == kLITPIXELHIT) {
      CalculateWeightedHit(itBegin, itEnd, static_cast<const CbmLitPixelHit*>(hit));
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
   myf eG00 = 0., eG01 = 0., eG11 = 0.;
   for (HitPtrIterator it = itBegin; it != itEnd; it++) {
      CbmLitPixelHit* h = static_cast<const CbmLitPixelHit*>(*it);
      if (h->IsOutlier()) { continue; }
      myf dxx = h->GetDx() * h->GetDx();
      myf dyy = h->GetDy() * h->GetDy();
      myf dxy = h->GetDxy();

      myf G00, G01, G11;
      Inverse(dxx, dxy, dyy, G00, G01, G11);

      myf w = h->GetW();
      eG00 += w * G00;
      eG01 += w * G01;
      eG11 += w * G11;
   }

   //calculate effective cov matrix V
   myf eV00, eV01, eV11;
   Inverse(eG00, eG01, eG11, eV00, eV01, eV11);

   // calculate effective x and y
   myf m0 = 0., m1 = 0.;
   for (HitPtrIterator it = itBegin; it != itEnd; it++) {
      CbmLitPixelHit* h = static_cast<const CbmLitPixelHit*>(*it);
      if (h->IsOutlier()) { continue; }
      myf dxx = h->GetDx() * h->GetDx();
      myf dyy = h->GetDy() * h->GetDy();
      myf dxy = h->GetDxy();
      myf x = h->GetX();
      myf y = h->GetY();

      myf G00, G01, G11;
      Inverse(dxx, dxy, dyy, G00, G01, G11);

      myf w = h->GetW();

      m0 += w * (G00 * x + G01 * y);
      m1 += w * (G11 * y + G01 * x);
   }
   myf x = eV00 * m0 + eV01 * m1;
   myf y = eV11 * m1 + eV01 * m0;

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
   myf eG = 0.;
   for (HitPtrIterator it = itBegin; it != itEnd; it++) {
      CbmLitStripHit* h = static_cast<const CbmLitStripHit*>(*it);
      if (h->IsOutlier()) { continue; }
      myf duu = h->GetDu() * h->GetDu();
      myf w = h->GetW();
      myf G = 1./duu;
      eG += w * G;
   }
   myf eV = 1./eG;

   myf m0 = 0.;
   for (HitPtrIterator it = itBegin; it != itEnd; it++) {
      CbmLitStripHit* h = static_cast<const CbmLitStripHit*>(*it);
      if (h->IsOutlier()) { continue; }
      myf duu = h->GetDu() * h->GetDu();
      myf u = h->GetU();
      myf w = h->GetW();
      m0 += w * (1./duu) * u;
   }
   myf eU = eV * m0;

   hit->SetU(eU);
   hit->SetZ((*itBegin)->GetZ());
   hit->SetDu(std::sqrt(eV));
   hit->SetPlaneId((*itBegin)->GetPlaneId());
}

void CbmLitWeightedHitCalculatorImp::Inverse(
   myf v00, myf v01, myf v11,
   myf& u00, myf& u01, myf& u11)
{
   if (v01 != 0.) {
      myf norm = v00 * v11 - v01 * v01;
      u00 = v11 / norm;
      u01 = -v01 / norm;
      u11 = v00 / norm;
   } else {
      u00 = 1. / v00;
      u01 = 0.;
      u11 = 1. / v11;
   }
}
