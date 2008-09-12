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

protected:

    CbmLitDetectorLayout fLayout;
       
    // Data
    HitVector fHitArray;
    TrackVector fTrackSeedArray;
    std::vector<HitVector> fHits;
    TrackVector fTracks;    

    // Propagation and track update tools
    CbmLitTrackPropagator *fPropagator;
    CbmLitTrackUpdate *fFilter;
    CbmLitTrackFitter *fFitter;
    
    // track selection tools
    CbmLitTrackSelection* fSeedSelection;
    CbmLitTrackSelection* fStationSelection;
    CbmLitTrackSelection* fFinalSelection;
    CbmLitTrackSelection* fFinalPreSelection;
        
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
    Int_t fPDG;
    Double_t fMaxErrSq;
    
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
    		const CbmLitHit *hit) const;    
    
    // Defines min and max index of hits 
    // which can be used for futher following 
    HitIteratorPair MinMaxIndex(
    		const CbmLitTrackParam* par,
    		Int_t layer);
    
    //
    Double_t CalcDevX(
    		const CbmLitTrackParam* par,
    		Int_t layer) const;
    //
    Double_t CalcDevY(
    		const CbmLitTrackParam* par,
    		Int_t layer) const;

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
