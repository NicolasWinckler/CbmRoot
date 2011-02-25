#ifndef CBMLITMUCHTRACKFINDERNN_H_
#define CBMLITMUCHTRACKFINDERNN_H_

#include "finder/CbmLitTrackFinderNN.h"
#include "finder_cbm/CbmLitStsBasedTrackFinder.h"

#include "CbmMuchTrackFinder.h"

class TClonesArray;

class CbmLitMuchTrackFinderNN : public CbmMuchTrackFinder,
								private CbmLitTrackFinderNN,
								private CbmLitStsBasedTrackFinder

{
public:
	CbmLitMuchTrackFinderNN();
	virtual ~CbmLitMuchTrackFinderNN();

	virtual void Init();

    virtual Int_t DoFind(
            TClonesArray* trackArray);

    ClassDef(CbmLitMuchTrackFinderNN, 1);
};

#endif /* CBMLITMUCHTRACKFINDERNN_H_ */
