/*
 * CbmLitParallelTrackFitterTest.h
 *
 *  Created on: Sep 24, 2009
 *      Author: andrey
 */

#ifndef CBMLITPARALLELTRACKFITTERTEST_H_
#define CBMLITPARALLELTRACKFITTERTEST_H_

#include "CbmLitTrackFitter.h"
#include "CbmLitPixelHit.h"
#include "parallel/LitDetectorGeometry.h"
#include "parallel/LitHit.h"
#include "parallel/LitTypes.h"
#include "parallel/LitTrackParam.h"

class CbmLitTrack;
class CbmLitTrackParam;
//class LitTrackParam;

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

	int PlaneId(
			int stationGroup,
			int station,
			int substation,
			LitDetectorLayout<fscal>& layout) const;

	bool CheckHit(
			int stationGroup,
			int station,
			int substation,
			LitDetectorLayout<fscal>& layout,
			CbmLitTrack* track);

	void SerialHitToParallel(
			const CbmLitPixelHit& hit,
			LitPixelHit<fscal>& lhit);

	LitDetectorLayout<fscal> fLayout;
};

#endif /* CBMLITPARALLELTRACKFITTERTEST_H_ */
