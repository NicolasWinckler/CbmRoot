#ifndef CBMLITTRACKFINDER_H_
#define CBMLITTRACKFINDER_H_

#include "CbmTool.h"
#include "CbmLitTypes.h"

class CbmLitTrackFinder : public CbmTool
{
public:
	
	CbmLitTrackFinder();
	virtual ~CbmLitTrackFinder();
	
	virtual LitStatus DoFind(
			const HitVector& hits,
			const TrackVector& trackSeeds,
			TrackVector& tracks) = 0;
	
    ClassDef(CbmLitTrackFinder, 1);
};

#endif /*CBMLITTRACKFINDER_H_*/
