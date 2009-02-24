#include "CbmLitTrackFitterRobust.h"

#include "CbmLitHit.h"
#include "CbmLitStripHit.h"
#include "CbmLitPixelHit.h"
#include "CbmLitTrack.h"
#include "CbmLitTrackParam.h"
#include "CbmLitComparators.h"
#include "CbmLitWeightCalculator.h"
#include "CbmLitWeightCalculatorSimple.h"
#include "CbmLitWeightCalculatorGauss.h"
#include "CbmLitWeightedHitCalculatorImp.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitEnums.h"

#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

CbmLitTrackFitterRobust::CbmLitTrackFitterRobust(
		CbmLitTrackFitter* fitter,
		CbmLitTrackFitter* smoother)
{
	fFitter = fitter;
	fSmoother = smoother;
	fWeightedHitCalculator = new CbmLitWeightedHitCalculatorImp;
	fSimpleWeightCalculator = new CbmLitWeightCalculatorSimple;
	fGaussWeightCalculator = new CbmLitWeightCalculatorGauss;

	//track fit parameters
	fNofIterations = 2;
	// was 0, 81, 9, 4, 1, 1, 1
	fAnnealing.push_back(0.);
	fAnnealing.push_back(1.);
	fAnnealing.push_back(1.);
	fAnnealing.push_back(4.);
	fAnnealing.push_back(1.);
	fAnnealing.push_back(1.);
	fOutlierCut = 1e-20;
}

CbmLitTrackFitterRobust::~CbmLitTrackFitterRobust()
{
	delete fWeightedHitCalculator;
	delete fSimpleWeightCalculator;
	delete fGaussWeightCalculator;
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
	if (track->GetLastPlaneId() < 5) return kLITERROR;

	track->SortHits();

	CbmLitTrack etrack;
	etrack.SetParamFirst(track->GetParamFirst());
	etrack.SetPDG(track->GetPDG());

	for(Int_t iter = 0; iter < fNofIterations; iter++){
		if (CreateEffectiveTrack(track, iter, &etrack) == kLITERROR) {
			return kLITERROR;
		}
		if (CheckEffectiveTrack(&etrack) == kLITERROR) {
			return kLITERROR;
		}
		if (fFitter->Fit(&etrack) == kLITERROR) {
			return kLITERROR;
		}
		if (fSmoother->Fit(&etrack) == kLITERROR) {
			return kLITERROR;
		}
	}
	CreateOutputTrack(track);

	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterRobust::CreateEffectiveTrack(
		CbmLitTrack* track,
		Int_t iter,
		CbmLitTrack* etrack) const
{
	std::vector<HitPtrIteratorPair> bounds;
	track->GetHitBounds(bounds);
	etrack->ClearHits();
	for (Int_t i = 0; i < bounds.size(); i++) {
		Int_t nofHits = bounds[i].second - bounds[i].first;
		if (nofHits > 1) {
			//Create effective hit, from set of hits
			//Add effective hit to track
			CbmLitHit* ehit;
			if ((*bounds[i].first)->GetType() == kLITPIXELHIT) ehit = new CbmLitPixelHit; else
			if ((*bounds[i].first)->GetType() == kLITSTRIPHIT) ehit = new CbmLitStripHit;
			const CbmLitTrackParam* par = (iter > 0)? etrack->GetFitNode(i)->GetSmoothedParam() : NULL;
			LitStatus result = CreateEffectiveHit(bounds[i].first, bounds[i].second, par, iter, ehit);
			if (result == kLITSUCCESS) {
				etrack->AddHit(ehit);
			} else {
				// hit not added
			}
			delete ehit;
		} else {
			//if only one hit, add it directly to the track
			etrack->AddHit(*bounds[i].first);
		}
	}
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterRobust::CreateEffectiveHit(
		HitPtrIterator itBegin,
		HitPtrIterator itEnd,
		const CbmLitTrackParam* par,
		Int_t iter,
		CbmLitHit* hit) const
{
	//Calculate hit weights
	//const CbmLitTrackParam* par;// = (iter > 0)? etrack->GetFitNode(i)->GetSmoothedParam() : NULL;
//	do {
		if (CalculateWeights(par, itBegin, itEnd, iter) == kLITERROR) {
			return kLITERROR;
		}
		MarkOutliers(itBegin, itEnd);
		if (AreAllOutliers(itBegin, itEnd)) return kLITERROR;
	// repeat weights recalculation until none of the weights will be marked
//	} while (MarkOutliers(itBegin, itEnd));

//	std::cout << "-iter-" << iter << "-----------------------------" << std::endl;
//	for (HitPtrIterator it = itBegin; it != itEnd; it++)
//		std::cout << (*it)->ToString();
	fWeightedHitCalculator->DoCalculate(itBegin, itEnd, hit);
//	std::cout << "weighted " << hit->ToString();

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

Bool_t CbmLitTrackFitterRobust::MarkOutliers(
		HitPtrIterator itBegin,
		HitPtrIterator itEnd) const
{
	Bool_t result = false;
	for(HitPtrIterator it = itBegin; it != itEnd; it++) {
		if ((*it)->GetW() < fOutlierCut) {
			(*it)->IsOutlier(true);
			result = true;
		}
	}
	return result;
}

LitStatus CbmLitTrackFitterRobust::CalculateWeights(
		const CbmLitTrackParam* par,
		HitPtrIterator itBegin,
		HitPtrIterator itEnd,
		Int_t iter) const
{
	if (iter == 0) {
		fSimpleWeightCalculator->DoCalculate(par, itBegin, itEnd, 0);
	} else {
		Double_t T = fAnnealing[iter];
		fGaussWeightCalculator->DoCalculate(par, itBegin, itEnd, T);
	}
	return kLITSUCCESS;
}


LitStatus CbmLitTrackFitterRobust::CheckEffectiveTrack(
		const CbmLitTrack* track) const
{
	if (track->GetNofHits() < 8) return kLITERROR;
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterRobust::CreateOutputTrack(
		CbmLitTrack* track)
{
	//find hit with the best weight for each station
	//other hits will be removed
	std::vector<HitPtrIteratorPair> bounds;
	track->GetHitBounds(bounds);
//	track->ClearHits();

	HitPtrVector newHits;
	for (Int_t i = 0; i < bounds.size(); i++) {
//		if (bounds[i].second - bounds[i].first < 2){
//			newHits.push_back(**bounds[i].first);
//			continue;
//		}
		std::sort(bounds[i].first, bounds[i].second, CompareHitPtrWLess());
		CbmLitHit* hit = (*(bounds[i].second-1));
		//std::cout << "best " << hit->ToString();
		if (!hit->IsOutlier()) {
			if (hit->GetType() == kLITSTRIPHIT){
				CbmLitStripHit* newHit = new CbmLitStripHit(*static_cast<const CbmLitStripHit*>(hit));
				newHits.push_back(newHit);
			}else if (hit->GetType() == kLITPIXELHIT){
				CbmLitPixelHit* newHit = new CbmLitPixelHit(*static_cast<const CbmLitPixelHit*>(hit));
				newHits.push_back(newHit);
			}
		}
	}
	track->ClearHits();
	for (Int_t i = 0; i < newHits.size(); i++)
		track->AddHit(newHits[i]);
	for_each(newHits.begin(), newHits.end(), DeleteObject());

//	std::cout << "created " << track->ToString();
//	for (Int_t i = 0; i < track->GetNofHits(); i++)
//		std::cout << track->GetHit(i)->ToString();

	if (track->GetNofHits() == 0) return kLITERROR;

	if (fFitter->Fit(track) == kLITERROR) return kLITERROR;
	if (fSmoother->Fit(track) == kLITERROR) return kLITERROR;

	track->SetLastPlaneId(track->GetHit(track->GetNofHits()-1)->GetPlaneId());

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
