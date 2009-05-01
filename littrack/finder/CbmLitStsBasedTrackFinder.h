#ifndef CBMLITSTSBASEDTRACKFINDER_H_
#define CBMLITSTSBASEDTRACKFINDER_H_

#include "CbmLitTypes.h"
#include "CbmLitPtrTypes.h"
#include "TObject.h"

class CbmLitTrackPropagator;
class CbmLitDetectorLayout;
class TClonesArray;

class CbmLitStsBasedTrackFinder //: public TObject
{
public:
	CbmLitStsBasedTrackFinder();
	virtual ~CbmLitStsBasedTrackFinder();

	void DefaultInit();

    Int_t DefaultDoFind(
    		TClonesArray* hitArray,
            TClonesArray* trackArray);

    void DefaultCreateTrackSeeds(
    		TClonesArray* trackArray,
    		TrackPtrVector& trackSeeds,
    		const CbmLitDetectorLayout& layout,
    		Int_t pdg);

    void SetPropagatorToDet(TrackPropagatorPtr propagatorToDet) { fPropagatorToDet = propagatorToDet;}

protected:
	TClonesArray* fTrackSeedsArray;

private:
	TrackPropagatorPtr fPropagatorToDet;

	ClassDef(CbmLitStsBasedTrackFinder, 1);
};

#endif /* CBMLITSTSBASEDTRACKFINDER_H_ */
