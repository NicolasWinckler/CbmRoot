#ifndef CBMLITTRDTRACKFINDERWEIGHT_H_
#define CBMLITTRDTRACKFINDERWEIGHT_H_

#include "finder/CbmLitTrackFinderWeight.h"
#include "finder_cbm/CbmLitStsBasedTrackFinder.h"

#include "CbmTrdTrackFinder.h"

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
