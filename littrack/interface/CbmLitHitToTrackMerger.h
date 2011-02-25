#ifndef CBMLITHITTOTRACKMERGER_H_
#define CBMLITHITTOTRACKMERGER_H_

#include "base/CbmLitTool.h"
#include "base/CbmLitTypes.h"

class CbmLitHitData;
class CbmLitDetectorLayout;

class CbmLitHitToTrackMerger : public CbmLitTool
{
public:
	CbmLitHitToTrackMerger() {};
	virtual ~CbmLitHitToTrackMerger() {};

	virtual LitStatus DoMerge(
			HitPtrVector& hits,
			TrackPtrVector& tracks) = 0;
};

#endif /* CBMLITHITTOTRACKMERGER_H_ */
