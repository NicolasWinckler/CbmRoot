/** CbmLitParallelTrackFitterTest.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **/
#ifndef CBMLITPARALLELTRACKFITTERTEST_H_
#define CBMLITPARALLELTRACKFITTERTEST_H_

#include "CbmLitTrackFitter.h"
#include "CbmLitPixelHit.h"
#include "parallel/muon/LitDetectorGeometryMuon.h"
#include "parallel/LitHit.h"

class CbmLitTrack;
class CbmLitTrackParam;

class CbmLitParallelTrackFitterTest : public CbmLitTrackFitter {
public:
	CbmLitParallelTrackFitterTest();
	virtual ~CbmLitParallelTrackFitterTest();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual LitStatus Fit(
			CbmLitTrack *track,
			bool downstream = true);

private:

	unsigned char PlaneId(
			unsigned char stationGroup,
			unsigned char station,
			unsigned char substation,
			LitDetectorLayoutMuon<fscal>& layout) const;

	bool CheckHit(
			unsigned char stationGroup,
			unsigned char station,
			unsigned char substation,
			LitDetectorLayoutMuon<fscal>& layout,
			CbmLitTrack* track);

	void SerialHitToParallel(
			const CbmLitPixelHit& hit,
			LitPixelHit<fscal>& lhit);

	LitDetectorLayoutMuon<fscal> fLayout;
};

#endif /* CBMLITPARALLELTRACKFITTERTEST_H_ */
