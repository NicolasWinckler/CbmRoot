#ifndef CBMLITTRACKFINDERIMP_H_
#define CBMLITTRACKFINDERIMP_H_

#include "CbmLitTrackFinder.h"
#include "CbmLitHit.h"
#include "CbmLitTrack.h"
#include "CbmLitTypes.h"
#include "CbmLitDetectorLayout.h"

#include <vector>
#include <set>

class CbmLitTrackExtrapolator;
class CbmLitTrackPropagator;
class CbmLitTrackUpdate;
class CbmLitTrackSelection;
class CbmLitTrackFitter;


class CbmLitTrackFinderImp : public CbmLitTrackFinder
{
public:
	
	CbmLitTrackFinderImp();
	virtual ~CbmLitTrackFinderImp();
	
	void SetDetectorLayout(const CbmLitDetectorLayout& layout) {
		fLayout = layout;
	}
	void SetPropagator(CbmLitTrackPropagator *propagator) {
	   	fPropagator = propagator;
	}
	void SetFilter(CbmLitTrackUpdate *filter) {
	   	fFilter = filter;
	}
	void SetFitter(CbmLitTrackFitter *fitter) {
	   	fFitter = fitter;
	}
	void SetTrackSeedSelection(CbmLitTrackSelection *selection) {
		fTrackSeedSelection = selection;
	}
	void SetTrackSelectionStation(CbmLitTrackSelection *selection) {
		fTrackSelectionStation = selection;
	}
	void SetTrackSelectionFinal(CbmLitTrackSelection *selection) {
		fTrackSelectionFinal = selection;
	}
	void SetVerbose(Int_t verbose) { fVerbose = verbose; }
    void SetNofIter(Int_t nofIter) {fNofIter = nofIter;}
	void SetBeginStation(Int_t beginStation) {fBeginStation = beginStation;}
	void SetEndStation(Int_t endStation) {fEndStation = endStation;}
	void SetMaxNofMissingHitsInStation(Int_t maxNofMissingHitsInStation) {
	   	fMaxNofMissingHitsInStation = maxNofMissingHitsInStation;
	}
	void SetMaxNofMissingHits(Int_t maxNofMissingHits) {
	   	fMaxNofMissingHits = maxNofMissingHits;
	}
	void SetSigmaCoef(Double_t sigmaCoef) { fSigmaCoef = sigmaCoef;}
	void SetPrecalcSearchRegions(Bool_t precalcSearchRegions) { 
	   	fPrecalcSearchRegions = precalcSearchRegions;
	}
	void SetApplyUpdateInLayer(Bool_t applyUpdateInLayer) {
	   	fApplyUpdateInLayer = applyUpdateInLayer;
	}
	void SetSigmaX(const std::vector<Double_t>& sigmaX){
	 	fSigmaX = sigmaX;
	}
	void SetSigmaY(const std::vector<Double_t>& sigmaY){
	  	fSigmaY = sigmaY;
	}
	void SetPDG(Int_t pdg) { fPDG = pdg;}
	
protected:

    CbmLitDetectorLayout fLayout;
       
    // Data
    HitVector fHitsArray;
    TrackVector fTrackSeeds;
    std::vector<HitVector> fHits;
    TrackVector fTracks;    

    // Propagation and track update tools
    CbmLitTrackPropagator *fPropagator;
    CbmLitTrackUpdate *fFilter;
    CbmLitTrackFitter *fFitter;
    
    // track selection tools
    CbmLitTrackSelection* fTrackSeedSelection;
    CbmLitTrackSelection* fTrackSelectionStation;
    CbmLitTrackSelection* fTrackSelectionFinal;
    
    // Verbosity level
    Int_t fVerbose;
    
    // Event number
    Int_t fEventNo;

    Int_t fNofIter;
    Int_t fBeginStation;
    Int_t fEndStation;
    Int_t fMaxNofMissingHitsInStation;
    Int_t fMaxNofMissingHits;
    Double_t fSigmaCoef;
    Bool_t fPrecalcSearchRegions;  
    Bool_t fApplyUpdateInLayer;
    Int_t fPDG;
        
    // used only for precalculated tables of deviations
    std::vector<Double_t> fSigmaX;
    std::vector<Double_t> fSigmaY;
    
    // Max measurement error for each layer
    std::vector<Double_t> fMaxErrX;
    std::vector<Double_t> fMaxErrY;
        
    // sets with hits and track seeds that have been used
    std::set<Int_t> fUsedHitsSet;
    std::set<Int_t> fSeedsIdSet;
        
	
	virtual void SetIterationParameters(Int_t iter) { };
	
    // Arrange hits by layers
    void ArrangeHits();    
    
    // Init track seeds
    void InitTrackSeeds();

    // Checks if the hit in the area near the track
    bool IsIn(
    		const CbmLitTrackParam* par, 
    		const CbmLitHit *hit);    
    
    // Defines min and max index of hits 
    // which can be used for futher following 
    HitIteratorPair MinMaxIndex(
    		const CbmLitTrackParam* par,
    		Int_t layer);
    
    //
    Double_t CalcDevX(
    		const CbmLitTrackParam* par,
    		Int_t layer);
    //
    Double_t CalcDevY(
    		const CbmLitTrackParam* par,
    		Int_t layer);

    // Removes hits which belong to found tracks
    void RemoveHits(
    		TrackIterator itBegin,
    		TrackIterator itEnd);
    
    //
    void CopyToOutput(
    		TrackIterator itBegin,
    		TrackIterator itEnd,
    		TrackVector& tracks);

    ClassDef(CbmLitTrackFinderImp, 1);
};

#endif /*CBMLITTRACKFINDERIMP_H_*/
