#ifndef CBMLITTRDTRACKFINDERSTANDALONE_H_
#define CBMLITTRDTRACKFINDERSTANDALONE_H_

#include "CbmLitFloat.h"
#include "CbmTrdTrackFinder.h"
#include "CbmLitTrackFinderBranch.h"
#include "CbmLitTypes.h"

#include <vector>

class TClonesArray;

class CbmLitTrdTrackFinderStandalone : public CbmTrdTrackFinder,
							           private CbmLitTrackFinderBranch
{
public:
    CbmLitTrdTrackFinderStandalone();
    ~CbmLitTrdTrackFinderStandalone();

    virtual void Init();
    virtual Int_t DoFind(
    		TClonesArray* hitArray,
            TClonesArray* trackArray);

private:
    myf fMom;

    virtual void SetIterationParameters(int iter);

	void CreateTrackSeeds(
			const HitPtrVector& hits,
    		TrackPtrVector& trackSeeds);

	void CreateHits(
			TClonesArray* hitArray,
			HitPtrVector& hits);

	void CopyToOutput(
			TrackPtrVector& tracks,
			TClonesArray* trackArray);

	ClassDef(CbmLitTrdTrackFinderStandalone, 1);
};

#endif /*CBMLITTRDTRACKFINDERS_H_*/
