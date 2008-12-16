#ifndef CBMLITTRDTRACKFINDERSTS_H_
#define CBMLITTRDTRACKFINDERSTS_H_

#include "CbmTrdTrackFinder.h"
#include "CbmLitTrackFinderBranch.h"

#include <vector>

class CbmLitTrackPropagator;
class CbmLitTrackExtrapolator;
class TClonesArray;

class CbmLitTrdTrackFinderSts: public CbmTrdTrackFinder,
							   private CbmLitTrackFinderBranch
{
public:
	CbmLitTrdTrackFinderSts();
	virtual ~CbmLitTrdTrackFinderSts();
	
    virtual void Init();
    virtual Int_t DoFind(
    		TClonesArray* hitArray,
            TClonesArray* trackArray);

private:
	TClonesArray* fTrackSeedsArray;
	
	CbmLitTrackPropagator* fPropagatorToDet;
	CbmLitTrackExtrapolator* fExtrapolatorToDet;
    
    virtual void SetIterationParameters(
    		Int_t iter);
    
	void CreateTrackSeeds(
    		TClonesArray* trackArray,
    		TrackPtrVector& trackSeeds);

	ClassDef(CbmLitTrdTrackFinderSts, 1);
};

#endif /*CBMLITTRDTRACKFINDERSTS_H_*/
