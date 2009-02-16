#ifndef CBMLITTRDTRACKFINDERBRANCH_H_
#define CBMLITTRDTRACKFINDERBRANCH_H_

#include "CbmTrdTrackFinder.h"
#include "CbmLitTrackFinderBranch.h"
#include "CbmLitStsBasedTrackFinder.h"

#include <vector>

class CbmLitTrackPropagator;
class CbmLitTrackExtrapolator;
class TClonesArray;

class CbmLitTrdTrackFinderBranch: public CbmTrdTrackFinder,
							   private CbmLitTrackFinderBranch,
							   private CbmLitStsBasedTrackFinder
{
public:
	CbmLitTrdTrackFinderBranch();
	virtual ~CbmLitTrdTrackFinderBranch();

    virtual void Init();

    virtual Int_t DoFind(
    		TClonesArray* hitArray,
            TClonesArray* trackArray);

private:
//	TClonesArray* fTrackSeedsArray;
//
//	CbmLitTrackPropagator* fPropagatorToDet;
//	CbmLitTrackExtrapolator* fExtrapolatorToDet;

    virtual void SetIterationParameters(
    		Int_t iter);

//	void CreateTrackSeeds(
//    		TClonesArray* trackArray,
//    		TrackPtrVector& trackSeeds);

	ClassDef(CbmLitTrdTrackFinderBranch, 1);
};

#endif /*CBMLITTRDTRACKFINDERBRANCH_H_*/
