#include "CbmLitRobustSelection.h"

#include "CbmLitTrackFitter.h"
#include "CbmLitTrack.h"
#include "CbmLitComparators.h"

CbmLitRobustSelection::CbmLitRobustSelection(
		CbmLitTrackFitter* robustFitter,
		CbmLitTrackFitter* fitter)
{
	fRobustFitter = robustFitter;
	fFitter = fitter;
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
		if ((*it)->GetFlag() == 1) continue;
		fRobustFitter->Fit(*it);
	
		// find the hit with maximum weight, other hits will be removed
		std::vector<HitIteratorPair> bounds = (*it)->GetHitBounds();
				
		std::vector<CbmLitHit> newHits;
		for (Int_t i = 0; i < bounds.size(); i++) {
			if (bounds[i].second - bounds[i].first < 2){
				newHits.push_back(**bounds[i].first);
				continue;
			}
			std::sort(bounds[i].first, bounds[i].second, CompareHitPtrWLess());
			std::cout << "max weight=" << (*(bounds[i].second-1))->GetW() << std::endl;
			newHits.push_back(**(bounds[i].second-1));
		}
		(*it)->ClearHits();
		for (Int_t i = 0; i < newHits.size(); i++) {
			(*it)->AddHit(&newHits[i]);
		}
		
		fFitter->Fit(*it);
	}
	
	return kLITSUCCESS;
}

LitStatus CbmLitRobustSelection::DoSelect(
		TrackVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}

ClassImp(CbmLitRobustSelection)
