#ifndef CBMLITMUCHTRACKFINDERROBUST_H_
#define CBMLITMUCHTRACKFINDERROBUST_H_

#include "CbmMuchTrackFinder.h"
#include "CbmLitTrackFinderRobust.h"
#include "CbmLitStsBasedTrackFinder.h"

class TClonesArray;

class CbmLitMuchTrackFinderRobust : public CbmMuchTrackFinder,
									private CbmLitTrackFinderRobust,
									private CbmLitStsBasedTrackFinder
{
public:
	CbmLitMuchTrackFinderRobust();
	virtual ~CbmLitMuchTrackFinderRobust();

    virtual void Init();

    virtual Int_t DoFind(
    		TClonesArray* hitArray,
            TClonesArray* trackArray);

	ClassDef(CbmLitMuchTrackFinderRobust, 1);
};

#endif /*CBMLITMUCHTRACKFINDERROBUST_H_*/
