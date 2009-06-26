#ifndef CBMLITMUCHTRACKFINDERNN_H_
#define CBMLITMUCHTRACKFINDERNN_H_

#include "CbmLitTrackFinderNN.h"
#include "CbmMuchTrackFinder.h"
#include "CbmLitStsBasedTrackFinder.h"

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
