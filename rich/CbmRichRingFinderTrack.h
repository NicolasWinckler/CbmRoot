// --------------------------------------------------------------------------------------
// -----                 CbmRichRingFinderTrack source file                         -----
// ----- implementation: Semen Lebedev (s.lebedev@gsi.de)  -----
// -------------------------------------------------------------------------

#ifndef CBM_RICH_RING_FINDER_TRACK_H
#define CBM_RICH_RING_FINDER_TRACK_H

#include "CbmRichRingFinderHoughBase.h"


class CbmRichRingFinderTrack : public CbmRichRingFinderHoughBase {

public:
  	CbmRichRingFinderTrack ( Int_t verbose, TString geometry);
	virtual ~CbmRichRingFinderTrack();

	virtual void HoughTransformReconstruction();

	ClassDef(CbmRichRingFinderTrack,1);
};
#endif // CBM_RICH_RING_FINDER_TRACK_H
