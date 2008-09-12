#ifndef CBMLITTRACKFINDERBRANCH_H_
#define CBMLITTRACKFINDERBRANCH_H_

#include "CbmLitTrackFinderImp.h"

class CbmLitTrackFinderBranch : public CbmLitTrackFinderImp
{
public:
	CbmLitTrackFinderBranch();
	virtual ~CbmLitTrackFinderBranch();
	
	LitStatus DoFind(
			const HitVector& hits,
			const TrackVector& trackSeeds,
			TrackVector& tracks);
	
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();
	
protected:
	
	Int_t fMaxNofBranches;
    TrackVector fFoundTracks;
    TrackVector fTracksCopy;
    
	// Main track following procedure
    void TrackFollowing();
    
	// Follows track through the station
    void TrackFollowingStation(
    		const CbmLitTrack *track, 
    		Int_t station);
    
    // Process the layer
    Bool_t ProcessLayer(
    		Int_t layerInStation,
    		HitIterator &hitIt,
    		HitIteratorPair &bounds,
    		std::vector<Int_t>& nofMissingHits,  
            CbmLitTrackParam* par,     
            CbmLitTrackParam* uPar,
            HitVector& hits,
            std::vector<Double_t>& chi2);
    
    // Adds track candidate 
    void AddTrackCandidate(
    		const CbmLitTrack* track, 
    		const HitVector& hits, 
    		const std::vector<Double_t>& chi2,
    		const CbmLitTrackParam* par,
    		Int_t station);
    
    void RefitTracks(
    		TrackIterator itBegin,
    		TrackIterator itEnd);
    
    void CopyToOutputArray();
    
	ClassDef(CbmLitTrackFinderBranch, 1);
};

#endif /*CBMLITTRACKFINDERBRANCH_H_*/
