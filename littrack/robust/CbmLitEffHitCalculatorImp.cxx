#include "CbmLitEffHitCalculatorImp.h"
#include "CbmLitHit.h"

#include <iostream>

CbmLitEffHitCalculatorImp::CbmLitEffHitCalculatorImp()
{
}

CbmLitEffHitCalculatorImp::~CbmLitEffHitCalculatorImp()
{
}

LitStatus CbmLitEffHitCalculatorImp::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitEffHitCalculatorImp::Finalize()
{
	return kLITSUCCESS;	
}

CbmLitHit CbmLitEffHitCalculatorImp::DoCalculate(
		HitIterator itBegin,
		HitIterator itEnd)
{
	CbmLitHit hit;

	//calculate effective weight matrix G
	Double_t eG00 = 0., eG01 = 0., eG11 = 0.;
	for (HitIterator it = itBegin; it != itEnd; it++) {
		if ((*it)->IsOutlier()) continue;
		Double_t dxx = (*it)->GetDx() * (*it)->GetDx();
		Double_t dyy = (*it)->GetDy() * (*it)->GetDy();
		Double_t dxy = (*it)->GetDxy();
		
		Double_t G00, G01, G11;
		Inverse(dxx, dxy, dyy, G00, G01, G11);
	
		Double_t w = (*it)->GetW();
		
		eG00 += w * G00;
		eG01 += w * G01;
		eG11 += w * G11;
	}
	
	//calculate effective cov matrix V
	Double_t eV00, eV01, eV11;
	Inverse(eG00, eG01, eG11, eV00, eV01, eV11);
	
	// calculate effective x znd y
	Double_t m0 = 0., m1 = 0.;
	for (HitIterator it = itBegin; it != itEnd; it++) {
		if ((*it)->IsOutlier()) continue;
		Double_t dxx = (*it)->GetDx() * (*it)->GetDx();
		Double_t dyy = (*it)->GetDy() * (*it)->GetDy();
		Double_t dxy = (*it)->GetDxy();
		Double_t x = (*it)->GetX();
		Double_t y = (*it)->GetY();
		
		Double_t G00, G01, G11;
		Inverse(dxx, dxy, dyy, G00, G01, G11);
		
		Double_t w = (*it)->GetW();
		
		m0 += w * (G00 * x + G01 * y);
		m1 += w * (G11 * y + G01 * x);
	}
	Double_t x = eV00 * m0 + eV01 * m1;
	Double_t y = eV11 * m1 + eV01 * m0;
	
	hit.SetX(x);
	hit.SetY(y);
	hit.SetZ((*itBegin)->GetZ());
	hit.SetDx(std::sqrt(eV00));
	hit.SetDxy(eV01);
	hit.SetDy(std::sqrt(eV11));
	hit.SetPlaneId((*itBegin)->GetPlaneId());
	
	return hit;
}

CbmLitHit CbmLitEffHitCalculatorImp::DoCalculate(
		HitVector& hits)
{
	return DoCalculate(hits.begin(), hits.end());
}

void CbmLitEffHitCalculatorImp::Inverse(
		Double_t v00, Double_t v01, Double_t v11,
		Double_t& u00, Double_t& u01, Double_t& u11)
{
	if (v01 != 0.) {
		Double_t norm = v00 * v11 - v01 * v01;
		//if (norm == 0.) return;
		u00 = v11 / norm;
		u01 = -v01 / norm;
		u11 = v00 / norm;
	} else {
		u00 = 1. / v00;
		u01 = 0.;
		u11 = 1. / v11;
	}
}

ClassImp(CbmLitEffHitCalculatorImp)
