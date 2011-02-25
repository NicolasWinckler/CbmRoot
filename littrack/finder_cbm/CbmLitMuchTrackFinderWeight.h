#ifndef CBMLITMUCHTRACKFINDERWEIGHT_H_
#define CBMLITMUCHTRACKFINDERWEIGHT_H_

#include "finder/CbmLitTrackFinderWeight.h"
#include "finder_cbm/CbmLitStsBasedTrackFinder.h"

#include "CbmMuchTrackFinder.h"

class TClonesArray;

class CbmLitMuchTrackFinderWeight : public CbmMuchTrackFinder,
									private CbmLitTrackFinderWeight,
									private CbmLitStsBasedTrackFinder
{
public:
	CbmLitMuchTrackFinderWeight();
	virtual ~CbmLitMuchTrackFinderWeight();

    virtual void Init();

    virtual Int_t DoFind(
            TClonesArray* trackArray);

    ClassDef(CbmLitMuchTrackFinderWeight, 1);
};

#endif /*CBMLITMUCHTRACKFINDERWEIGHT_H_*/
