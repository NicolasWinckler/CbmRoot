#ifndef CBMLITTRDTRACKFINDERS_H_
#define CBMLITTRDTRACKFINDERS_H_

#include "CbmTrdTrackFinder.h"
#include "CbmLitTrackFinderBranch.h"
#include "CbmLitTypes.h"

#include <vector>

class TClonesArray;

class CbmLitTrdTrackFinderS : public CbmTrdTrackFinder,
							  private CbmLitTrackFinderBranch
{
public:
    CbmLitTrdTrackFinderS();
    ~CbmLitTrdTrackFinderS();
	
    virtual void Init();
    virtual Int_t DoFind(
    		TClonesArray* hitArray,
            TClonesArray* trackArray);

private:
    Double_t fMom;

    virtual void SetIterationParameters(Int_t iter);
    
	void CreateTrackSeeds(
			const HitPtrVector& hits,
    		TrackPtrVector& trackSeeds);

	void CreateHits(
			TClonesArray* hitArray,
			HitPtrVector& hits);
	
	void CopyToOutput(
			TrackPtrVector& tracks,
			TClonesArray* trackArray);

    ClassDef(CbmLitTrdTrackFinderS, 1);
};

#endif /*CBMLITTRDTRACKFINDERS_H_*/
