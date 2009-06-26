#include "CbmLitWeightCalculatorGauss.h"

#include "CbmLitHit.h"
#include "CbmLitStripHit.h"
#include "CbmLitPixelHit.h"

#include <iostream>
#include <cmath>

CbmLitWeightCalculatorGauss::CbmLitWeightCalculatorGauss()
{
}

CbmLitWeightCalculatorGauss::~CbmLitWeightCalculatorGauss()
{
}

LitStatus CbmLitWeightCalculatorGauss::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitWeightCalculatorGauss::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitWeightCalculatorGauss::DoCalculate(
		const CbmLitTrackParam* par,
		HitPtrIterator itBegin,
		HitPtrIterator itEnd,
		Double_t T)
{
	for(HitPtrIterator it = itBegin; it != itEnd; it++) {
		if ((*it)->IsOutlier()) continue;
		if (MultivariateGaussWeight(par, *it, T) == kLITERROR) return kLITERROR;
	}
	if (Normalize(itBegin, itEnd) == kLITERROR) return kLITERROR;

	return kLITSUCCESS;
}

LitStatus CbmLitWeightCalculatorGauss::DoCalculate(
		const CbmLitTrackParam* par,
		HitPtrVector& hits,
		Double_t T)
{
	return DoCalculate(par, hits.begin(), hits.end(), T);
}

LitStatus CbmLitWeightCalculatorGauss::MultivariateGaussWeight(
		const CbmLitTrackParam* par,
		CbmLitHit* hit,
		Double_t T) const
{
	if (hit->GetType() == kLITSTRIPHIT)
		return MultivariateGaussWeight(par, static_cast<const CbmLitStripHit*>(hit), T);
	else if (hit->GetType() == kLITPIXELHIT)
		return MultivariateGaussWeight(par, static_cast<const CbmLitPixelHit*>(hit), T);
}

LitStatus CbmLitWeightCalculatorGauss::MultivariateGaussWeight(
		const CbmLitTrackParam* par,
		CbmLitStripHit* hit,
		Double_t T) const
{
	std::cout << "MultivariateGaussWeight NOT IMPLEMENTED FOR STRIP HIT!!!!"<< std::endl;
}

LitStatus CbmLitWeightCalculatorGauss::MultivariateGaussWeight(
		const CbmLitTrackParam* par,
		CbmLitPixelHit* hit,
		Double_t T) const
{
	const Double_t PI = 3.14159265;

	Double_t dx = hit->GetX() - par->GetX();
	Double_t dy = hit->GetY() - par->GetY();
	Double_t dxx = hit->GetDx() * hit->GetDx();
	Double_t dyy = hit->GetDy() * hit->GetDy();
	Double_t dxy = hit->GetDxy();
	Double_t det = dxx * dyy - dxy * dxy;
	Double_t s = (dx*dx*dyy - 2*dx*dy*dxy + dy*dy*dxx) / det;

	Double_t w = (1./((2.*PI) * std::sqrt(T*det))) * std::exp(-s/(2.*T));
	hit->SetW(w);

	return kLITSUCCESS;
}

Double_t CbmLitWeightCalculatorGauss::MultivariateGaussCut(
		const CbmLitHit* hit,
		Double_t T,
		Double_t cutValue) const
{
	if (hit->GetType() == kLITSTRIPHIT)
		MultivariateGaussCut(static_cast<const CbmLitStripHit*>(hit), T, cutValue);
	else if (hit->GetType() == kLITPIXELHIT)
		MultivariateGaussCut(static_cast<const CbmLitPixelHit*>(hit), T, cutValue);
}

Double_t CbmLitWeightCalculatorGauss::MultivariateGaussCut(
		const CbmLitStripHit* hit,
		Double_t T,
		Double_t cutValue) const
{
	std::cout << "MultivariateGaussCut NOT IMPLEMENTED FOR STRIP HIT!!!!"<< std::endl;
}

Double_t CbmLitWeightCalculatorGauss::MultivariateGaussCut(
		const CbmLitPixelHit* hit,
		Double_t T,
		Double_t cutValue) const
{
	Double_t dim = 2.;
	const Double_t PI = 3.14159265;

	Double_t dxx = hit->GetDx() * hit->GetDx();
	Double_t dyy = hit->GetDy() * hit->GetDy();
	Double_t dxy = hit->GetDxy();
	Double_t det = dxx * dyy - dxy * dxy;

	Double_t cut = (1./(std::pow(2.*PI, dim/2.) * std::sqrt(T*det))) *
					std::exp(-cutValue/(2.*T));
	return cut;
}

LitStatus CbmLitWeightCalculatorGauss::Normalize(
		HitPtrIterator itBegin,
		HitPtrIterator itEnd) const
{
	//Double_t cutValue = 3.;
	Double_t sumW = 0.;
	Double_t sumCut = 0.;
	for(HitPtrIterator it = itBegin; it != itEnd; it++) {
		if ((*it)->IsOutlier()) continue;
		sumW += (*it)->GetW();
		//sumCut += MultivariateGaussCut(*it, T, cutValue);
	}

	if (sumW == 0.) {
		return kLITERROR;
	} else {
		for(HitPtrIterator it = itBegin; it != itEnd; it++) {
			if ((*it)->IsOutlier()) continue;
			(*it)->SetW((*it)->GetW() / (sumW + sumCut));
		}
	}
	return kLITSUCCESS;
}
