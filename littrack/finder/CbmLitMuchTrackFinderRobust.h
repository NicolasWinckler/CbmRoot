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
class CbmLitHit;
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
	CbmLitTrackPropagator* fPropagator;
	CbmLitTrackUpdate* fFilter;
	CbmLitTrackFitter* fFitter;
	CbmLitTrackSelection* fTrackSeedSelection;
	CbmLitTrackSelection* fTrackSelectionStation;
	CbmLitTrackSelection* fTrackSelectionFinal;

	CbmLitDetectorLayout fLayout;
	
    Int_t fVerbose;
    
	void CreateTrackSeeds(
    		TClonesArray* trackArray,
    		TrackVector& trackSeeds);
	
	ClassDef(CbmLitMuchTrackFinderRobust, 1);	
};

#endif /*CBMLITMUCHTRACKFINDERROBUST_H_*/
