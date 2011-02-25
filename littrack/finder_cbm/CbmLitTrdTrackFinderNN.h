#ifndef CBMLITTRDTRACKFINDERNN_H_
#define CBMLITTRDTRACKFINDERNN_H_

#include "finder/CbmLitTrackFinderNN.h"
#include "finder_cbm/CbmLitStsBasedTrackFinder.h"

#include "CbmTrdTrackFinder.h"

class CbmLitTrdTrackFinderNN : public CbmTrdTrackFinder,
							   private CbmLitTrackFinderNN,
							   private CbmLitStsBasedTrackFinder
{
public:
	CbmLitTrdTrackFinderNN();
	virtual ~CbmLitTrdTrackFinderNN();

	virtual void Init();

    virtual Int_t DoFind(
    		TClonesArray* hitArray,
            TClonesArray* trackArray);

    ClassDef(CbmLitTrdTrackFinderNN, 1);
};

#endif /* CBMLITTRDTRACKFINDERNN_H_ */
