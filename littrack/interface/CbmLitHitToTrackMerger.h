#ifndef CBMLITHITTOTRACKMERGER_H_
#define CBMLITHITTOTRACKMERGER_H_

#include "CbmLitTool.h"
#include "CbmLitTypes.h"

class CbmLitHitData;
class CbmLitDetectorLayout;

class CbmLitHitToTrackMerger : public CbmLitTool
{
public:
	CbmLitHitToTrackMerger();
	virtual ~CbmLitHitToTrackMerger();

	virtual LitStatus DoMerge(
			HitPtrVector& hits,
			TrackPtrVector& tracks) = 0;

	ClassDef(CbmLitHitToTrackMerger, 1);
};

#endif /* CBMLITHITTOTRACKMERGER_H_ */
