#ifndef CBMLITTRDTRACKFINDERBRANCH_H_
#define CBMLITTRDTRACKFINDERBRANCH_H_

#include "finder/CbmLitTrackFinderBranch.h"
#include "finder_cbm/CbmLitStsBasedTrackFinder.h"

#include "CbmTrdTrackFinder.h"

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

    ClassDef(CbmLitTrdTrackFinderBranch, 1);
};

#endif /*CBMLITTRDTRACKFINDERBRANCH_H_*/
