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
	void SetNofIter(int nofIter) { fNofIter = nofIter; }
	void SetMaxNofMissingHits(int maxNofMissingHits) { fMaxNofMissingHits = maxNofMissingHits;}
    void SetBeginStationGroup(int beginStationGroup) { fBeginStationGroup = beginStationGroup;}
    void SetEndStationGroup(int endStationGroup) { fEndStationGroup = endStationGroup;}
    void SetPDG(int pdg) { fPDG = pdg; }
    void SetSigmaCoef(double sigmaCoef) { fSigmaCoef = sigmaCoef;}
    void SetChiSqStripHitCut(double chiSqStripHitCut) { fChiSqStripHitCut = chiSqStripHitCut;}
    void SetChiSqPixelHitCut(double chiSqPixelHitCut) { fChiSqPixelHitCut = chiSqPixelHitCut;}
    void IsUseFastSearch(bool useFastSearch) { fUseFastSearch = useFastSearch;}

    void SetVerbose(int verbose) {fVerbose = verbose;}

protected:
    virtual void SetIterationParameters(
    		int iter){;}

    // Arranges hits by stations
    void ArrangeHits(
    		HitPtrIterator itBegin,
    		HitPtrIterator itEnd);

    // Initializes track seeds
    void InitTrackSeeds(
    		TrackPtrIterator itBegin,
    		TrackPtrIterator itEnd);

    bool IsHitInValidationWindow(
    		const CbmLitTrackParam* par,
    		const CbmLitHit* hit) const;

    HitPtrIteratorPair MinMaxIndex(
    		const CbmLitTrackParam* par,
    		int stationGroup,
    		int station,
    		int substation);

    double CalcDevX(
    		const CbmLitTrackParam* par,
    		int stationGroup,
    		int station,
    		int substation) const;

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
    std::set<int> fUsedHitsSet;
    std::set<int> fSeedsIdSet;

    // Tracking parameters
	int fNofIter;
	int fMaxNofMissingHits;
    int fBeginStationGroup;
    int fEndStationGroup;
    int fPDG;
    double fSigmaCoef;
    double fChiSqStripHitCut;
    double fChiSqPixelHitCut;
    double fMaxCovSq;
    bool fUseFastSearch;

    int fVerbose;
    int fEventNo;
};

#endif /*CBMLITTRACKFINDERBASE_H_*/
