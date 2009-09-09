#ifndef CBMLITSTSBASEDTRACKFINDER_H_
#define CBMLITSTSBASEDTRACKFINDER_H_

#include "base/CbmLitTypes.h"
#include "base/CbmLitPtrTypes.h"

#include "TObject.h"

class CbmLitTrackPropagator;
class CbmLitDetectorLayout;
class TClonesArray;

class CbmLitStsBasedTrackFinder
{
public:
	CbmLitStsBasedTrackFinder();
	virtual ~CbmLitStsBasedTrackFinder();

	void DefaultInit();

    int DefaultDoFind(
    		TClonesArray* hitArray,
            TClonesArray* trackArray);

    void DefaultCreateTrackSeeds(
    		TClonesArray* trackArray,
    		TrackPtrVector& trackSeeds,
    		const CbmLitDetectorLayout& layout,
    		int pdg);

    void SetPropagatorToDet(TrackPropagatorPtr propagatorToDet) { fPropagatorToDet = propagatorToDet;}

protected:
	TClonesArray* fTrackSeedsArray;

private:
	TrackPropagatorPtr fPropagatorToDet;

	ClassDef(CbmLitStsBasedTrackFinder, 1);
};

#endif /* CBMLITSTSBASEDTRACKFINDER_H_ */
