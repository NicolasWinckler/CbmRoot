#ifndef CBMLITMUCHTRACKFINDERROBUST_H_
#define CBMLITMUCHTRACKFINDERROBUST_H_

#include "CbmMuchTrackFinder.h"
#include "CbmLitTrackFinderRobust.h"
#include "CbmLitTypes.h"

#include <vector>

class CbmLitTrackPropagator;
class CbmLitTrackExtrapolator;
class CbmLitTrackUpdate;
class CbmLitTrackFitter;
class CbmLitTrackSelection;
class CbmLitTrack;
class CbmLitPixelHit;
class TClonesArray;

class CbmLitMuchTrackFinderRobust : public CbmMuchTrackFinder,
									private CbmLitTrackFinderRobust
{
public:
	CbmLitMuchTrackFinderRobust();
	virtual ~CbmLitMuchTrackFinderRobust();
	

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
	
	ClassDef(CbmLitMuchTrackFinderRobust, 1);	
};

#endif /*CBMLITMUCHTRACKFINDERROBUST_H_*/
