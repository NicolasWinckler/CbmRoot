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
		TrackFitterPtr fitter,
		TrackFitterPtr smoother)
{
	fFitter = fitter;
	fSmoother = smoother;
	fWeightedHitCalculator = WeightedHitCalculatorPtr(new CbmLitWeightedHitCalculatorImp());
	fSimpleWeightCalculator = WeightCalculatorPtr(new CbmLitWeightCalculatorSimple());
	fGaussWeightCalculator = WeightCalculatorPtr(new CbmLitWeightCalculatorGauss());

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
//	if (track->GetLastPlaneId() < 5) return kLITERROR;
	if (track->GetNofHits() < 1) return kLITERROR;
	track->SortHits();

	CbmLitTrack etrack;
	etrack.SetPDG(track->GetPDG());
	//etrack.SetParamFirst(track->GetParamFirst());
	for(Int_t iter = 0; iter < fNofIterations; iter++){
		etrack.SetParamFirst(track->GetParamFirst());
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
//	std::cout << "-------iter " << iter << "--------" << std::endl;
//	std::cout << track->ToString();
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
			if (iter>0){
//			std::cout << "iter=" << iter << " " << par->ToString();
//			std::cout << "ehit " << result << " " << ehit->ToString();
			}
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
//	std::cout << "etrack " << etrack->ToString();
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

ClassImp(CbmLitTrackFitterRobust)
