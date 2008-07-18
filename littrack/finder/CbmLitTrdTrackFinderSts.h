#ifndef CBMLITTRDTRACKFINDERSTS_H_
#define CBMLITTRDTRACKFINDERSTS_H_

#include "CbmTrdTrackFinder.h"
#include "CbmLitTrackFinderBranch.h"

#include <vector>

class CbmLitTrackPropagator;
class CbmLitTrackExtrapolator;
class CbmLitTrackUpdate;
class CbmLitTrackFitter;
class CbmLitTrackSelection;
class CbmLitTrack;
class CbmLitHit;
class TClonesArray;


class CbmLitTrdTrackFinderSts: public CbmTrdTrackFinder,
							   private CbmLitTrackFinderBranch
{

public:

	
	CbmLitTrdTrackFinderSts();
	virtual ~CbmLitTrdTrackFinderSts();
	
    virtual void Init();
    virtual Int_t DoFind(TClonesArray* hitArray,
                         TClonesArray* trackArray);

private:
	TClonesArray* fTrackSeedsArray;
	
	CbmLitTrackPropagator* fPropagatorToDet;
	CbmLitTrackExtrapolator* fExtrapolatorToDet;
	CbmLitTrackPropagator* fPropagator;
	CbmLitTrackExtrapolator* fExtrapolator;
	CbmLitTrackUpdate* fFilter;
	CbmLitTrackFitter* fFitter;
	CbmLitTrackSelection* fTrackSeedSelection;
	CbmLitTrackSelection* fTrackSelectionStation;
	CbmLitTrackSelection* fTrackSelectionFinal;
	
	CbmLitDetectorLayout fLayout;
    
    Int_t fVerbose;
    
    virtual void SetIterationParameters(Int_t iter);
    
	void CreateTrackSeeds(
    		TClonesArray* trackArray,
    		TrackVector& trackSeeds);

	ClassDef(CbmLitTrdTrackFinderSts, 1);
};

#endif /*CBMLITTRDTRACKFINDERSTS_H_*/
