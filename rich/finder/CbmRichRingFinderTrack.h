// --------------------------------------------------------------------------------------
// -----                 CbmRichRingFinderTrack source file                         -----
// ----- implementation: Semen Lebedev (s.lebedev@gsi.de)  -----
// -------------------------------------------------------------------------

#ifndef CBM_RICH_RING_FINDER_TRACK_H
#define CBM_RICH_RING_FINDER_TRACK_H

#include "CbmRichRingFinder.h"


class CbmRichRingFinderTrack : public CbmRichRingFinder {

public:
  	CbmRichRingFinderTrack();

	virtual ~CbmRichRingFinderTrack();

	virtual void HoughTransformReconstruction();

	ClassDef(CbmRichRingFinderTrack,1);
};
#endif // CBM_RICH_RING_FINDER_TRACK_H
