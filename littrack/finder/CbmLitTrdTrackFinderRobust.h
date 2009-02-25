#ifndef CBMLITTRDTRACKFINDERROBUST_H_
#define CBMLITTRDTRACKFINDERROBUST_H_

#include "CbmTrdTrackFinder.h"
#include "CbmLitTrackFinderRobust.h"
#include "CbmLitPtrTypes.h"

class CbmLitTrackPropagator;

class TClonesArray;

class CbmLitTrdTrackFinderRobust : public CbmTrdTrackFinder,
							       private CbmLitTrackFinderRobust
{
public:
	CbmLitTrdTrackFinderRobust();
	virtual ~CbmLitTrdTrackFinderRobust();

    virtual void Init();
    virtual Int_t DoFind(TClonesArray* hitArray,
                         TClonesArray* trackArray);
private:
	TClonesArray* fTrackSeedsArray;

	TrackPropagatorPtr fPropagatorToDet;

	void CreateTrackSeeds(
    		TClonesArray* trackArray,
    		TrackPtrVector& trackSeeds);

	ClassDef(CbmLitTrdTrackFinderRobust, 1);
};

#endif /*CBMLITTRDTRACKFINDERROBUST_H_*/
