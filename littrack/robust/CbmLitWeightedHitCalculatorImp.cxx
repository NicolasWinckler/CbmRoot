#include "CbmLitWeightedHitCalculatorImp.h"

#include "CbmLitHit.h"
#include "CbmLitStripHit.h"
#include "CbmLitPixelHit.h"

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
	if (hit->GetType() == kLITSTRIPHIT)
		CalculateWeightedHit(itBegin, itEnd, static_cast<const CbmLitStripHit*>(hit));
	else if (hit->GetType() == kLITPIXELHIT)
		CalculateWeightedHit(itBegin, itEnd, static_cast<const CbmLitPixelHit*>(hit));
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
	Double_t eG00 = 0., eG01 = 0., eG11 = 0.;
	for (HitPtrIterator it = itBegin; it != itEnd; it++) {
		CbmLitPixelHit* h = static_cast<const CbmLitPixelHit*>(*it);
		if (h->IsOutlier()) continue;
		Double_t dxx = h->GetDx() * h->GetDx();
		Double_t dyy = h->GetDy() * h->GetDy();
		Double_t dxy = h->GetDxy();

		Double_t G00, G01, G11;
		Inverse(dxx, dxy, dyy, G00, G01, G11);

		Double_t w = h->GetW();
		eG00 += w * G00;
		eG01 += w * G01;
		eG11 += w * G11;
	}

	//calculate effective cov matrix V
	Double_t eV00, eV01, eV11;
	Inverse(eG00, eG01, eG11, eV00, eV01, eV11);

	// calculate effective x and y
	Double_t m0 = 0., m1 = 0.;
	for (HitPtrIterator it = itBegin; it != itEnd; it++) {
		CbmLitPixelHit* h = static_cast<const CbmLitPixelHit*>(*it);
		if (h->IsOutlier()) continue;
		Double_t dxx = h->GetDx() * h->GetDx();
		Double_t dyy = h->GetDy() * h->GetDy();
		Double_t dxy = h->GetDxy();
		Double_t x = h->GetX();
		Double_t y = h->GetY();

		Double_t G00, G01, G11;
		Inverse(dxx, dxy, dyy, G00, G01, G11);

		Double_t w = h->GetW();

		m0 += w * (G00 * x + G01 * y);
		m1 += w * (G11 * y + G01 * x);
	}
	Double_t x = eV00 * m0 + eV01 * m1;
	Double_t y = eV11 * m1 + eV01 * m0;

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
	Double_t eG = 0.;
	for (HitPtrIterator it = itBegin; it != itEnd; it++) {
		CbmLitStripHit* h = static_cast<const CbmLitStripHit*>(*it);
		if (h->IsOutlier()) continue;
		Double_t duu = h->GetDu() * h->GetDu();
		Double_t w = h->GetW();
		Double_t G = 1./duu;
		eG += w * G;
	}
	Double_t eV = 1./eG;

	Double_t m0 = 0.;
	for (HitPtrIterator it = itBegin; it != itEnd; it++) {
		CbmLitStripHit* h = static_cast<const CbmLitStripHit*>(*it);
		if (h->IsOutlier()) continue;
		Double_t duu = h->GetDu() * h->GetDu();
		Double_t u = h->GetU();
		Double_t w = h->GetW();
		m0 += w * (1./duu) * u;
	}
	Double_t eU = eV * m0;

	hit->SetU(eU);
	hit->SetZ((*itBegin)->GetZ());
	hit->SetDu(std::sqrt(eV));
	hit->SetPlaneId((*itBegin)->GetPlaneId());
}

void CbmLitWeightedHitCalculatorImp::Inverse(
		Double_t v00, Double_t v01, Double_t v11,
		Double_t& u00, Double_t& u01, Double_t& u11)
{
	if (v01 != 0.) {
		Double_t norm = v00 * v11 - v01 * v01;
		u00 = v11 / norm;
		u01 = -v01 / norm;
		u11 = v00 / norm;
	} else {
		u00 = 1. / v00;
		u01 = 0.;
		u11 = 1. / v11;
	}
}

ClassImp(CbmLitWeightedHitCalculatorImp)
