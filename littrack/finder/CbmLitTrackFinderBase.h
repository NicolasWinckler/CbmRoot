#ifndef CBMLITTRACKFINDERBASE_H_
#define CBMLITTRACKFINDERBASE_H_

#include "CbmLitTrackFinder.h"
#include "CbmLitTypes.h"
#include "CbmLitDetectorLayout.h"
#include "CbmLitHitData.h"
#include "CbmLitPtrTypes.h"

#include <set>

class CbmLitTrackFinderBase : public CbmLitTrackFinder
{
public:
	CbmLitTrackFinderBase();
	virtual ~CbmLitTrackFinderBase();

	// Setters
    void SetSeedSelection(TrackSelectionPtr seedSelection) {fSeedSelection=seedSelection;}
    void SetPropagator(TrackPropagatorPtr propagator) {fPropagator = propagator;}
	void SetFinalSelection(TrackSelectionPtr finalSelection) {fFinalSelection = finalSelection;}
	void SetLayout(CbmLitDetectorLayout layout) {fLayout = layout;}
    void SetDetectorLayout(const CbmLitDetectorLayout& layout) { fLayout = layout; }
	void SetNofIter(Int_t nofIter) { fNofIter = nofIter; }
	void SetMaxNofMissingHits(Int_t maxNofMissingHits) { fMaxNofMissingHits = maxNofMissingHits;}
    void SetBeginStationGroup(Int_t beginStationGroup) { fBeginStationGroup = beginStationGroup;}
    void SetEndStationGroup(Int_t endStationGroup) { fEndStationGroup = endStationGroup;}
    void SetPDG(Int_t pdg) { fPDG = pdg; }
    void SetSigmaCoef(Double_t sigmaCoef) { fSigmaCoef = sigmaCoef;}
    void SetChiSqStripHitCut(Double_t chiSqStripHitCut) { fChiSqStripHitCut = chiSqStripHitCut;}
    void SetChiSqPixelHitCut(Double_t chiSqPixelHitCut) { fChiSqPixelHitCut = chiSqPixelHitCut;}
    void IsUseFastSearch(Bool_t useFastSearch) { fUseFastSearch = useFastSearch;}

    void SetVerbose(Int_t verbose) {fVerbose = verbose;}

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


protected:
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
