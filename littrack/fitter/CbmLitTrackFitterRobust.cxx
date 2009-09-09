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
#include "CbmLitWeightCalculatorTukey.h"
#include "CbmLitWeightedHitCalculatorImp.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitEnums.h"

#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

CbmLitTrackFitterWeight::CbmLitTrackFitterWeight(
		TrackFitterPtr fitter,
		TrackFitterPtr smoother)
{
	fFitter = fitter;
	fSmoother = smoother;
	fWeightedHitCalculator = WeightedHitCalculatorPtr(new CbmLitWeightedHitCalculatorImp());
	fSimpleWeightCalculator = WeightCalculatorPtr(new CbmLitWeightCalculatorSimple());
//	fGaussWeightCalculator = WeightCalculatorPtr(new CbmLitWeightCalculatorGauss());
	fTukeyWeightCalculator = WeightCalculatorPtr(new CbmLitWeightCalculatorTukey());

	//track fit parameters
	fNofIterations = 3;
	fAnnealing.push_back(0.);
	fAnnealing.push_back(30.);
	fAnnealing.push_back(15.);
	fOutlierCut = 1e-20;
}

CbmLitTrackFitterWeight::~CbmLitTrackFitterWeight()
{
}

LitStatus CbmLitTrackFitterWeight::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterWeight::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterWeight::Fit(
		CbmLitTrack *track,
		bool downstream)
{
	if (track->GetNofHits() < 1) return kLITERROR;
	track->SortHits();

	CbmLitTrack etrack;
	etrack.SetPDG(track->GetPDG());
//	etrack.SetParamFirst(track->GetParamFirst());
	for(int iter = 0; iter < fNofIterations; iter++){
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

LitStatus CbmLitTrackFitterWeight::CreateEffectiveTrack(
		CbmLitTrack* track,
		int iter,
		CbmLitTrack* etrack) const
{
	std::vector<HitPtrIteratorPair> bounds;
	track->GetHitBounds(bounds);
	etrack->ClearHits();
	for (int i = 0; i < bounds.size(); i++) {
		int nofHits = bounds[i].second - bounds[i].first;
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

LitStatus CbmLitTrackFitterWeight::CreateEffectiveHit(
		HitPtrIterator itBegin,
		HitPtrIterator itEnd,
		const CbmLitTrackParam* par,
		int iter,
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

	fWeightedHitCalculator->DoCalculate(itBegin, itEnd, hit);

	return kLITSUCCESS;
}

bool CbmLitTrackFitterWeight::AreAllOutliers(
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

bool CbmLitTrackFitterWeight::MarkOutliers(
		HitPtrIterator itBegin,
		HitPtrIterator itEnd) const
{
	bool result = false;
	for(HitPtrIterator it = itBegin; it != itEnd; it++) {
		if ((*it)->GetW() < fOutlierCut) {
			(*it)->IsOutlier(true);
			result = true;
		}
	}
	return result;
}

LitStatus CbmLitTrackFitterWeight::CalculateWeights(
		const CbmLitTrackParam* par,
		HitPtrIterator itBegin,
		HitPtrIterator itEnd,
		int iter) const
{
	if (iter == 0) {
		fSimpleWeightCalculator->DoCalculate(par, itBegin, itEnd, 0);
	} else {
		myf T = fAnnealing[iter];
		fTukeyWeightCalculator->DoCalculate(par, itBegin, itEnd, T);
	}
	return kLITSUCCESS;
}


LitStatus CbmLitTrackFitterWeight::CheckEffectiveTrack(
		const CbmLitTrack* track) const
{
	if (track->GetNofHits() < 8) return kLITERROR;
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterWeight::CreateOutputTrack(
		CbmLitTrack* track)
{
	//find hit with the best weight for each station
	//other hits will be removed
	std::vector<HitPtrIteratorPair> bounds;
	track->GetHitBounds(bounds);
//	track->ClearHits();

	HitPtrVector newHits;
	for (int i = 0; i < bounds.size(); i++) {
		std::sort(bounds[i].first, bounds[i].second, CompareHitPtrWLess());
		CbmLitHit* hit = (*(bounds[i].second-1));
		if (!hit->IsOutlier()) {
			if (hit->GetType() == kLITSTRIPHIT) {
				CbmLitStripHit* newHit = new CbmLitStripHit(*static_cast<const CbmLitStripHit*>(hit));
				newHits.push_back(newHit);
			} else if (hit->GetType() == kLITPIXELHIT) {
				CbmLitPixelHit* newHit = new CbmLitPixelHit(*static_cast<const CbmLitPixelHit*>(hit));
				newHits.push_back(newHit);
			}
		}
	}
	track->ClearHits();
	for (int i = 0; i < newHits.size(); i++)
		track->AddHit(newHits[i]);
	for_each(newHits.begin(), newHits.end(), DeleteObject());

	if (track->GetNofHits() == 0) return kLITERROR;

	if (fFitter->Fit(track) == kLITERROR) return kLITERROR;
	if (fSmoother->Fit(track) == kLITERROR) return kLITERROR;

	track->SetLastPlaneId(track->GetHit(track->GetNofHits()-1)->GetPlaneId());

	return kLITSUCCESS;
}
