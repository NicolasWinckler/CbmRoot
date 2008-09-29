#include "CbmLitRobustSelection.h"

#include "CbmLitTrackFitter.h"
#include "CbmLitTrack.h"
#include "CbmLitComparators.h"

#include <iostream>

CbmLitRobustSelection::CbmLitRobustSelection(
		CbmLitTrackFitter* robustFitter,
		CbmLitTrackFitter* fitter,
		CbmLitTrackFitter* smoother)
{
	fRobustFitter = robustFitter;
	fFitter = fitter;
	fSmoother = smoother;
}

CbmLitRobustSelection::~CbmLitRobustSelection()
{
}

LitStatus CbmLitRobustSelection::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitRobustSelection::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitRobustSelection::DoSelect(
		TrackIterator itBegin,
		TrackIterator itEnd)
{
	for(TrackIterator it = itBegin; it != itEnd; it++) {
		if ((*it)->GetQuality() == kLITBAD) continue;
		
		LitStatus code = fRobustFitter->Fit(*it);
		if (code == kLITERROR) {
			(*it)->SetQuality(kLITBAD);
			continue;
		}
	
		// find the hit with maximum weight, other hits will be removed
		std::vector<HitIteratorPair> bounds;
		(*it)->GetHitBounds(bounds);
				
		std::vector<CbmLitHit> newHits;
		for (Int_t i = 0; i < bounds.size(); i++) {
			if (bounds[i].second - bounds[i].first < 2){
				newHits.push_back(**bounds[i].first);
				continue;
			}
			std::sort(bounds[i].first, bounds[i].second, CompareHitPtrWLess());
			//std::cout << "max weight=" << (*(bounds[i].second-1))->GetW() << std::endl;
			CbmLitHit* hit = (*(bounds[i].second-1));
			if (!hit->IsOutlier()) newHits.push_back(*hit);
		}
		(*it)->ClearHits();
		for (Int_t i = 0; i < newHits.size(); i++) {
			(*it)->AddHit(&newHits[i]);
		}
		
		//std::cout << "nofHits=" << (*it)->GetNofHits() << std::endl;
		if ((*it)->GetNofHits() == 0) {
			(*it)->SetQuality(kLITBAD);
			continue;
		}
		
		code = fFitter->Fit(*it);
		if (code == kLITERROR) {
			(*it)->SetQuality(kLITBAD);
			continue;
		}
		code = fSmoother->Fit(*it);
		if (code == kLITERROR) {
		    (*it)->SetQuality(kLITBAD);
			continue;
		}
		(*it)->SetLastPlaneId((*it)->GetHit((*it)->GetNofHits()-1)->GetPlaneId());
	}
	
	return kLITSUCCESS;
}

LitStatus CbmLitRobustSelection::DoSelect(
		TrackVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}

ClassImp(CbmLitRobustSelection)
