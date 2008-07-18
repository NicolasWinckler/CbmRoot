#ifndef CBMLITTRDTRACKFINDERS_H_
#define CBMLITTRDTRACKFINDERS_H_

#include "CbmTrdTrackFinder.h"
#include "CbmLitTrackFinderImp.h"
#include "CbmLitDetectorLayout.h"

#include <vector>

class CbmLitTrackPropagator;
class CbmLitTrackExtrapolator;
class CbmLitTrackUpdate;
class CbmLitTrackFitter;
class CbmLitTrackSelection;
class CbmLitTrack;
class CbmLitHit;
class TClonesArray;


class CbmLitTrdTrackFinderS : public CbmTrdTrackFinder,
							   private CbmLitTrackFinderImp
{

public:

    CbmLitTrdTrackFinderS();
    ~CbmLitTrdTrackFinderS();
	
    virtual void Init();
    virtual Int_t DoFind(TClonesArray* hitArray,
                         TClonesArray* trackArray);

private:

    Double_t fMom;

	CbmLitTrackPropagator* fPropagator;
	CbmLitTrackExtrapolator* fExtrapolator;
	CbmLitTrackUpdate* fFilter;
	CbmLitTrackFitter* fFitter;
	CbmLitTrackSelection* fTrackSeedSelection;	
	CbmLitTrackSelection* fTrackSelectionStation;
	CbmLitTrackSelection* fTrackSelectionFinal;
	
	CbmLitDetectorLayout fLayout;
	
    Int_t fVerbose;
    
    virtual void SetIterationParameters(Int_t iter);
    
	void CreateTrackSeeds(
			const HitVector& hits,
    		TrackVector& trackSeeds);

	void CreateHits(
			TClonesArray* hitArray,
			HitVector& hits);
	
	void CopyToOutput(
			TrackVector& tracks,
			TClonesArray* trackArray);

    ClassDef(CbmLitTrdTrackFinderS, 1);
};


#endif /*CBMLITTRDTRACKFINDERS_H_*/


