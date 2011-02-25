#ifndef CBMLITTOFMERGER_H_
#define CBMLITTOFMERGER_H_

#include "base/CbmLitTypes.h"
#include "base/CbmLitDetectorLayout.h"
#include "base/CbmLitHitData.h"
#include "finder/CbmLitNearestHitToTrackMerger.h"

#include "CbmTofMerger.h"

class TClonesArray;

class CbmLitTofMerger : public CbmTofMerger,
                        private CbmLitNearestHitToTrackMerger
{
public:
	CbmLitTofMerger();
	virtual ~CbmLitTofMerger();

	virtual void Init();

    virtual Int_t DoMerge(
    		TClonesArray* glbTracks,
    		TClonesArray* tofHits);
private:

	void ArrangeHits(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd,
			CbmLitHitData& hitData);

	void GlobalTrackArrayToTrackVector(
			const TClonesArray* trdTracks,
			TrackPtrVector& litTracks,
			TClonesArray* trdHits,
			TClonesArray* globalTracks);

	void AddTofHitToGlobalTrack(
			const TrackPtrVector& litTracks,
			TClonesArray* globalTracks);

	TClonesArray* fTrdTracks;
	TClonesArray* fTrdHits;
	HitPtrVector fLitHits;
	TrackPtrVector fLitTracks;

	CbmLitDetectorLayout fLayout;
	CbmLitHitData fHitData;

	ClassDef(CbmLitTofMerger, 1);
};

#endif /* CBMLITTOFMERGER_H_ */
