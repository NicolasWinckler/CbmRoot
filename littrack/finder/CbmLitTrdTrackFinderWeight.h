#ifndef CBMLITTRDTRACKFINDERROBUST_H_
#define CBMLITTRDTRACKFINDERROBUST_H_

#include "CbmTrdTrackFinder.h"
#include "CbmLitTrackFinderRobust.h"
#include "CbmLitStsBasedTrackFinder.h"

class TClonesArray;

class CbmLitTrdTrackFinderRobust : public CbmTrdTrackFinder,
							       private CbmLitTrackFinderRobust,
							       private CbmLitStsBasedTrackFinder
{
public:
	CbmLitTrdTrackFinderRobust();
	virtual ~CbmLitTrdTrackFinderRobust();

    virtual void Init();
    virtual Int_t DoFind(
    		TClonesArray* hitArray,
            TClonesArray* trackArray);

	ClassDef(CbmLitTrdTrackFinderRobust, 1);
};

#endif /*CBMLITTRDTRACKFINDERROBUST_H_*/
