#ifndef CBMLITTRACKFINDERBASE_H_
#define CBMLITTRACKFINDERBASE_H_

#include "interface/CbmLitTrackFinder.h"
#include "finder/CbmLitGating.h"
#include "base/CbmLitFloat.h"
#include "base/CbmLitTypes.h"
#include "base/CbmLitDetectorLayout.h"
#include "base/CbmLitHitData.h"
#include "base/CbmLitPtrTypes.h"

#include <set>

class CbmLitTrackFinderBase : public CbmLitTrackFinder,
                               public CbmLitGating
{
public:
	CbmLitTrackFinderBase();
	virtual ~CbmLitTrackFinderBase();

	// Setters
//    void SetSeedSelection(TrackSelectionPtr seedSelection) {fSeedSelection=seedSelection;}
//    void SetPropagator(TrackPropagatorPtr propagator) {fPropagator = propagator;}
//	void SetFinalSelection(TrackSelectionPtr finalSelection) {fFinalSelection = finalSelection;}
	void SetLayout(CbmLitDetectorLayout layout) {fLayout = layout;}
	void SetNofIter(int nofIter) { fNofIter = nofIter; }
	void SetMaxNofMissingHits(int maxNofMissingHits) { fMaxNofMissingHits = maxNofMissingHits;}
	void SetPDG(int pdg) { fPDG = pdg; }
    void SetVerbose(int verbose) {fVerbose = verbose;}

protected:
    virtual void SetIterationParameters(
    		int iter){;}

    // Arranges hits by stations
    void ArrangeHits(
    		HitPtrIterator itBegin,
    		HitPtrIterator itEnd);

    void RemoveHits(
    		TrackPtrIterator itBegin,
    		TrackPtrIterator itEnd);

    void CopyToOutput(
    		TrackPtrIterator itBegin,
    		TrackPtrIterator itEnd,
    		TrackPtrVector& tracks);

protected:
    CbmLitDetectorLayout fLayout;

    CbmLitHitData fHitData;

//    TrackPtrVector fTracks;

    // Tools
//    TrackSelectionPtr fSeedSelection;
//    TrackSelectionPtr fFinalSelection;
//    TrackPropagatorPtr fPropagator;


    // sets with hits and track seeds that have been used
    std::set<int> fUsedHitsSet;
    std::set<int> fUsedSeedsSet;

    // Tracking parameters
	int fNofIter;
	int fMaxNofMissingHits;
	int fPDG;

    int fVerbose;
    int fEventNo;
};

#endif /*CBMLITTRACKFINDERBASE_H_*/
