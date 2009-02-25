#ifndef CBMLITTRACKFINDERBASE_H_
#define CBMLITTRACKFINDERBASE_H_

#include "CbmLitTrackFinder.h"
#include "CbmLitTypes.h"
#include "CbmLitDetectorLayout.h"
#include "CbmLitHitData.h"
#include "CbmLitPtrTypes.h"

#include <set>

//class CbmLitTrackSelection;
//class CbmLitTrackPropagator;

class CbmLitTrackFinderBase : public CbmLitTrackFinder
{
public:
	CbmLitTrackFinderBase();
	virtual ~CbmLitTrackFinderBase();

protected:
    virtual void SetIterationParameters(
    		Int_t iter){}

    // Arranges hits by stations
    void ArrangeHits(
    		HitPtrIterator itBegin,
    		HitPtrIterator itEnd);

    // Initialises track seeds
    void InitTrackSeeds(
    		TrackPtrIterator itBegin,
    		TrackPtrIterator itEnd);

    Bool_t IsHitInValidationWindow(
    		const CbmLitTrackParam* par,
    		const CbmLitHit* hit) const;

    HitPtrIteratorPair MinMaxIndex(
    		const CbmLitTrackParam* par,
    		Int_t stationGroup,
    		Int_t station,
    		Int_t substation);

    Double_t CalcDevX(
    		const CbmLitTrackParam* par,
    		Int_t stationGroup,
    		Int_t station,
    		Int_t substation) const;

    void RemoveHits(
    		TrackPtrIterator itBegin,
    		TrackPtrIterator itEnd);

    void CopyToOutput(
    		TrackPtrIterator itBegin,
    		TrackPtrIterator itEnd,
    		TrackPtrVector& tracks);

    CbmLitDetectorLayout fLayout;

    CbmLitHitData fHitData;

	HitPtrVector fHitArray;
	TrackPtrVector fTrackSeedArray;
    TrackPtrVector fTracks;

    // Tools
    TrackSelectionPtr fSeedSelection;
    TrackPropagatorPtr fPropagator;
	TrackSelectionPtr fFinalSelection;

    // sets with hits and track seeds that have been used
    std::set<Int_t> fUsedHitsSet;
    std::set<Int_t> fSeedsIdSet;

    // Tracking parameters
	Int_t fNofIter;
	Int_t fMaxNofMissingHits;
    Int_t fBeginStationGroup;
    Int_t fEndStationGroup;
    Int_t fPDG;
    Double_t fSigmaCoef;
    Double_t fChiSqStripHitCut;
    Double_t fChiSqPixelHitCut;
    Double_t fMaxCovSq;
    Bool_t fUseFastSearch;
    Int_t fVerbose;
    Int_t fEventNo;

	ClassDef(CbmLitTrackFinderBase, 1);
};

#endif /*CBMLITTRACKFINDERBASE_H_*/
