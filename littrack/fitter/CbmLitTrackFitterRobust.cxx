#include "CbmLitTrackFitterRobust.h"

#include "CbmLitHit.h"
#include "CbmLitStripHit.h"
#include "CbmLitPixelHit.h"
#include "CbmLitTrack.h"
#include "CbmLitTrackParam.h"
#include "CbmLitWeightCalculator.h"
#include "CbmLitEffHitCalculator.h"
#include "CbmLitComparators.h"
#include "CbmLitWeightCalculatorSimple.h"
#include "CbmLitEffHitCalculatorImp.h"
//#include "CbmLitRobustAna.h"

#include <vector>
#include <cmath>
#include <iostream>

CbmLitTrackFitterRobust::CbmLitTrackFitterRobust(
		CbmLitTrackFitter* fitter,
		CbmLitTrackFitter* smoother)
{
	fFitter = fitter;
	fSmoother = smoother;
	//fWeightCalc = new CbmLitWeightCalculatorSimple;
	fWeightCalcSimple = new CbmLitWeightCalculatorSimple;
	fEffHitCalc = new CbmLitEffHitCalculatorImp;

	//track fit parameters
	fNofIterations = 2;
	// was 0, 81, 9, 4, 1, 1, 1
	fAnnealing.push_back(0.);
	fAnnealing.push_back(1.);
	fAnnealing.push_back(9.);
	fAnnealing.push_back(4.);
	fAnnealing.push_back(1.);
	fAnnealing.push_back(1.);
	fOutlierCut = 1e-20;
}

CbmLitTrackFitterRobust::~CbmLitTrackFitterRobust()
{
//	delete fWeightCalc;
	delete fWeightCalcSimple;
	delete fEffHitCalc;
}

LitStatus CbmLitTrackFitterRobust::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterRobust::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterRobust::Fit(
		CbmLitTrack *track,
		Bool_t downstream)
{
//	CbmLitRobustAna::Instance()->Fill(track);

	track->SortHits();
	std::vector<HitPtrIteratorPair> bounds;
	track->GetHitBounds(bounds);

	CbmLitTrack etrack;
	etrack.SetParamFirst(track->GetParamFirst());
	etrack.SetPDG(track->GetPDG());

	for(Int_t iter = 0; iter < fNofIterations; iter++){
		if (CreateEffTrack(bounds, iter, &etrack) == kLITERROR) {
			return kLITERROR;
		}

		if (CheckEffTrack(&etrack) == kLITERROR) {
			return kLITERROR;
		}
		if (fFitter->Fit(&etrack) == kLITERROR) {
//			CbmLitRobustAna::Instance()->FillErrFit(track, iter);
			return kLITERROR;
		}

		if (fSmoother->Fit(&etrack) == kLITERROR) {
			std::cout << "CbmLitTrackFitterRobust::Fit smoother error" << std::endl;
			return kLITERROR;
		}

//		CbmLitRobustAna::Instance()->Fill(&etrack, iter);
//		CbmLitRobustAna::Instance()->FillWeight(track, iter);
	}

	return kLITSUCCESS;
}

Bool_t CbmLitTrackFitterRobust::AreAllOutliers(
		HitPtrIterator itBegin,
		HitPtrIterator itEnd) const
{
	for(HitPtrIterator it = itBegin; it != itEnd; it++) {
		if (!(*it)->IsOutlier()) {
			return false;
		}
	}
	return true;
}

void CbmLitTrackFitterRobust::MarkOutliers(
		HitPtrIterator itBegin,
		HitPtrIterator itEnd) const
{
	for(HitPtrIterator it = itBegin; it != itEnd; it++) {
		if ((*it)->GetW() < fOutlierCut) (*it)->IsOutlier(true);
	}
}

LitStatus CbmLitTrackFitterRobust::MultivariateGaussWeight(
		const CbmLitTrackParam* par,
		CbmLitHit* hit,
		Double_t T) const
{
	if (hit->GetType() == kLITSTRIPHIT)
		MultivariateGaussWeight(par, static_cast<const CbmLitStripHit*>(hit), T);
	else if (hit->GetType() == kLITPIXELHIT)
		MultivariateGaussWeight(par, static_cast<const CbmLitPixelHit*>(hit), T);
}

LitStatus CbmLitTrackFitterRobust::MultivariateGaussWeight(
		const CbmLitTrackParam* par,
		CbmLitStripHit* hit,
		Double_t T) const
{
	std::cout << "MultivariateGaussWeight NOT IMPLEMENTED FOR STRIP HIT!!!!"<< std::endl;
}

LitStatus CbmLitTrackFitterRobust::MultivariateGaussWeight(
		const CbmLitTrackParam* par,
		CbmLitPixelHit* hit,
		Double_t T) const
{
	Double_t dim = 2.;
	const Double_t PI = 3.14159265;

	Double_t dx = hit->GetX() - par->GetX();
	Double_t dy = hit->GetY() - par->GetY();
	Double_t dxx = hit->GetDx() * hit->GetDx();
	Double_t dyy = hit->GetDy() * hit->GetDy();
	Double_t dxy = hit->GetDxy();
	Double_t det = dxx * dyy - dxy * dxy;
	Double_t s = (dx*dx*dyy - 2*dx*dy*dxy + dy*dy*dxx) / det;

	Double_t w = (1./(std::pow(2.*PI, dim/2.) * std::sqrt(T*det))) *
					std::exp(-s/(2.*T));
	hit->SetW(w);

	return kLITSUCCESS;
}

Double_t CbmLitTrackFitterRobust::MultivariateGaussCut(
		const CbmLitHit* hit,
		Double_t T,
		Double_t cutValue) const
{
	if (hit->GetType() == kLITSTRIPHIT)
		MultivariateGaussCut(static_cast<const CbmLitStripHit*>(hit), T, cutValue);
	else if (hit->GetType() == kLITPIXELHIT)
		MultivariateGaussCut(static_cast<const CbmLitPixelHit*>(hit), T, cutValue);
}

Double_t CbmLitTrackFitterRobust::MultivariateGaussCut(
		const CbmLitStripHit* hit,
		Double_t T,
		Double_t cutValue) const
{
	std::cout << "MultivariateGaussCut NOT IMPLEMENTED FOR STRIP HIT!!!!"<< std::endl;
}

Double_t CbmLitTrackFitterRobust::MultivariateGaussCut(
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

LitStatus CbmLitTrackFitterRobust::CalcWeights(
		const CbmLitTrackParam* par,
		HitPtrIterator itBegin,
		HitPtrIterator itEnd,
		Int_t iter) const
{
	if (iter == 0) {
		fWeightCalcSimple->DoCalculate(par, itBegin, itEnd);
	} else {
		Double_t T = fAnnealing[iter];
		for(HitPtrIterator it = itBegin; it != itEnd; it++) {
			CbmLitHit* hit = *it;
			if ((*it)->IsOutlier()) continue;
			MultivariateGaussWeight(par, *it, T);
		}
		Normalize(itBegin, itEnd);
	}
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterRobust::Normalize(
		HitPtrIterator itBegin,
		HitPtrIterator itEnd) const
{
	//Double_t cutValue = 3.;
	Double_t sumW = 0.;
	Double_t sumCut = 0.;
	for(HitPtrIterator it = itBegin; it != itEnd; it++) {
		if ((*it)->IsOutlier()) continue;
		sumW += (*it)->GetW();
		//sumCut += DAFCut(*it, T, cutValue);
	}

	if (sumW == 0.) {
		//return kLITERROR;
	} else {
		for(HitPtrIterator it = itBegin; it != itEnd; it++) {
			if ((*it)->IsOutlier()) continue;
			(*it)->SetW((*it)->GetW() / (sumW + sumCut));
		}
	}
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterRobust::CreateEffTrack(
		const std::vector<HitPtrIteratorPair>& bounds,
		Int_t iter,
		CbmLitTrack* etrack) const
{
	etrack->ClearHits();
	for (Int_t i = 0; i < bounds.size(); i++) {
		if (bounds[i].second - bounds[i].first > 1) { // more than 1 hit in station

			const CbmLitTrackParam* param =
				(iter > 0)? etrack->GetFitNode(i)->GetSmoothedParam() : NULL;
			if (CalcWeights(param, bounds[i].first, bounds[i].second, iter) == kLITERROR) {
				return kLITERROR;
			}

			//TODO check if some of the hit were marked as outliers,
			// is it neccessary to recalculate the weights
			MarkOutliers(bounds[i].first, bounds[i].second);

			if (!AreAllOutliers(bounds[i].first, bounds[i].second)) {
				// TODO DOESN'T WORK NOW TO BE IMPLEMENTED
				//CbmLitPixelHit ehit = fEffHitCalc->DoCalculate(bounds[i].first, bounds[i].second);
				//etrack->AddHit(&ehit);
//				std::cout << "eff hit:" << ehit.ToString();
			} else {
//				std::cout << "iter=" << iter << ", all hit outliers " << std::endl;
//				std::cout << etrack->ToString();
			}
		} else {
			etrack->AddHit(*bounds[i].first);
		}
	}
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterRobust::CheckEffTrack(
		const CbmLitTrack* track) const
{
	if (track->GetNofHits() < 5) return kLITERROR;
	return kLITSUCCESS;
}

//
//LitStatus CbmLitTrackFitterRobust::IsStopIterations(
//		const std::vector<HitIteratorPair>& bounds) const
//{
//	for (Int_t i = 0; i < bounds.size(); i++) {
//		for(HitIterator it = bounds[i].first; it != bounds[i].second; it++) {
//			//if ((*it)->IsOutlier()) continue;
//			//(*it)->SetW((*it)->GetW() / (sumW + sumCut));
//		}
//	}
//	return kLITSUCCESS;
//}

/*
LitStatus CbmLitTrackFitterRobust::KoshiWeight(
		const CbmLitTrackParam* par,
		HitIterator itBegin,
		HitIterator itEnd,
		Int_t iter)
{
	Double_t a = 5 - 0.5*iter;
	Double_t b = 0.16;
	//Double_t T = 10;
	//Double_t t = 9 * iter;

	Double_t tT = 7./Double_t(iter);

	Double_t w = b + (a - b) * std::exp(-tT);

	for(HitIterator it = itBegin; it != itEnd; it++) {
		Double_t dx = (*it)->GetX() - par->GetX();
		Double_t dy = (*it)->GetY() - par->GetY();
		Double_t ee = dx * dx + dy * dy;
		Double_t weight = (w * w) / (ee + w * w);
		(*it)->SetW(weight);
		//std::cout << ee << " ee ";
	}
	//std::cout << std::endl;
	Double_t sum = 0;
	for(HitIterator it = itBegin; it != itEnd; it++)
		sum += (*it)->GetW();

	for(HitIterator it = itBegin; it != itEnd; it++) {
		(*it)->SetW((*it)->GetW() / sum);
		if (itEnd - itBegin > 1)std::cout << (*it)->GetW() << " ";
	}
	if (itEnd - itBegin > 1) std::cout << std::endl;
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterRobust::OptimalWeight(
		const CbmLitTrackParam* par,
		HitIterator itBegin,
		HitIterator itEnd,
		Int_t iter)
{
	Double_t c = 0.8 - iter*0.1;

	Double_t sumWEE = 0;
	Double_t sumW = 0;
	for(HitIterator it = itBegin; it != itEnd; it++) {
		Double_t dx = (*it)->GetX() - par->GetX();
		Double_t dy = (*it)->GetY() - par->GetY();
		Double_t ee = dx * dx + dy * dy;
		sumWEE += (*it)->GetW() * ee;
		sumW += (*it)->GetW();
	}
	Double_t sigmaSq = sumWEE / sumW;

	sumW = 0;
	for(HitIterator it = itBegin; it != itEnd; it++) {
		Double_t dx = (*it)->GetX() - par->GetX();
		Double_t dy = (*it)->GetY() - par->GetY();
		Double_t ee = dx * dx + dy * dy;
		Double_t w = (1 + c) / (1 + c * std::exp(ee / (2 * sigmaSq)));
		(*it)->SetW(w);
		sumW+=w;
	}
	for(HitIterator it = itBegin; it != itEnd; it++) {
		(*it)->SetW((*it)->GetW() / sumW);
		if (itEnd - itBegin > 1)std::cout << (*it)->GetW() << " ";
	}
	if (itEnd - itBegin > 1) std::cout << std::endl;
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterRobust::TukeyWeight(
		const CbmLitTrackParam* par,
		HitIterator itBegin,
		HitIterator itEnd,
		Int_t iter)
{
	Double_t t = 4 - iter;
	Double_t ct = 2 + t;
	Double_t ctSq = ct * ct;

	Double_t sumWEE = 0;
	Double_t sumW = 0;
	for(HitIterator it = itBegin; it != itEnd; it++) {
		Double_t dx = (*it)->GetX() - par->GetX();
		Double_t dy = (*it)->GetY() - par->GetY();
		Double_t ee = dx * dx + dy * dy;
		sumWEE += (*it)->GetW() * ee;
		sumW += (*it)->GetW();
	}
	Double_t sigmaSq = sumWEE / sumW;

	sumW = 0.;
	for(HitIterator it = itBegin; it != itEnd; it++) {
		Double_t dx = (*it)->GetX() - par->GetX();
		Double_t dy = (*it)->GetY() - par->GetY();
		Double_t ee = dx * dx + dy * dy;
		Double_t w = 0;
		if (ctSq * sigmaSq > ee) {
			Double_t t = 1 - ee / (ctSq * sigmaSq);
			w = t * t;
		}

		(*it)->SetW(w);
		sumW+=w;
	}

	for(HitIterator it = itBegin; it != itEnd; it++) {
		if (sumW == 0.) (*it)->SetW(0.000001);
		else (*it)->SetW((*it)->GetW() / sumW);
		//std::cout << "tukey 4" << std::endl;
		//if (itEnd - itBegin > 1)std::cout << (*it)->GetW() << " ";
	}
	//if (itEnd - itBegin > 1) std::cout << std::endl;
	return kLITSUCCESS;
}
*/

ClassImp(CbmLitTrackFitterRobust)
