#ifndef CBMLITTRACKFINDER_H_
#define CBMLITTRACKFINDER_H_

#include "CbmLitTool.h"
#include "CbmLitTypes.h"

class CbmLitTrackFinder : public CbmLitTool
{
public:
	CbmLitTrackFinder();
	virtual ~CbmLitTrackFinder();

	virtual LitStatus DoFind(
			const HitPtrVector& hits,
			const TrackPtrVector& trackSeeds,
			TrackPtrVector& tracks) = 0;
};

#endif /*CBMLITTRACKFINDER_H_*/
