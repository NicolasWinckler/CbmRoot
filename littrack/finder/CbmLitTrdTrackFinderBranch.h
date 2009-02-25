#ifndef CBMLITTRDTRACKFINDERBRANCH_H_
#define CBMLITTRDTRACKFINDERBRANCH_H_

#include "CbmTrdTrackFinder.h"
#include "CbmLitTrackFinderBranch.h"
#include "CbmLitStsBasedTrackFinder.h"

#include <vector>

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
    virtual void SetIterationParameters(
    		Int_t iter);

	ClassDef(CbmLitTrdTrackFinderBranch, 1);
};

#endif /*CBMLITTRDTRACKFINDERBRANCH_H_*/
