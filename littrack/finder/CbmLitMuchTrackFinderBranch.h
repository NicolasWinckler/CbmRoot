#ifndef CBMLITMUCHTRACKFINDERBRANCH_H_
#define CBMLITMUCHTRACKFINDERBRANCH_H_

#include "CbmMuchTrackFinder.h"
#include "CbmLitTrackFinderBranch.h"
#include "CbmLitTypes.h"
#include "CbmLitDetectorLayout.h"

#include <vector>

class CbmLitTrackPropagator;
class CbmLitTrack;
class CbmLitPixelHit;
class TClonesArray;

class CbmLitMuchTrackFinderBranch : public CbmMuchTrackFinder,
									private CbmLitTrackFinderBranch
{
public:
	CbmLitMuchTrackFinderBranch();
	virtual ~CbmLitMuchTrackFinderBranch();
	

    virtual void Init();
    
    virtual Int_t DoFind(
    		TClonesArray* hitArray,
            TClonesArray* trackArray);
private:
	
	TClonesArray* fTrackSeedsArray;
	  
	CbmLitTrackPropagator* fPropagatorToDet;
    
	void CreateTrackSeeds(
    		TClonesArray* trackArray,
    		TrackPtrVector& trackSeeds);
	
	ClassDef(CbmLitMuchTrackFinderBranch, 1);	
};

#endif /*CBMLITMUCHTRACKFINDERBRANCH_H_*/
