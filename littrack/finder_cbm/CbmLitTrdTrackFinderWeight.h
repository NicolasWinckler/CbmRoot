#ifndef CBMLITTRDTRACKFINDERWEIGHT_H_
#define CBMLITTRDTRACKFINDERWEIGHT_H_

#include "CbmTrdTrackFinder.h"
#include "CbmLitTrackFinderWeight.h"
#include "CbmLitStsBasedTrackFinder.h"

class TClonesArray;

class CbmLitTrdTrackFinderWeight : public CbmTrdTrackFinder,
							       private CbmLitTrackFinderWeight,
							       private CbmLitStsBasedTrackFinder
{
public:
	CbmLitTrdTrackFinderWeight();
	virtual ~CbmLitTrdTrackFinderWeight();

    virtual void Init();
    virtual Int_t DoFind(
    		TClonesArray* hitArray,
            TClonesArray* trackArray);

    ClassDef(CbmLitTrdTrackFinderWeight, 1);
};

#endif /*CBMLITTRDTRACKFINDERWEIGHT_H_*/
