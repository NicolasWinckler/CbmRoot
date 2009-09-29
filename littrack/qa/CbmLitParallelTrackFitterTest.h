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

class CbmLitTrack;
class CbmLitTrackParam;
class LitTrackParam;

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

	void SerialParamToParallel(
			const CbmLitTrackParam& par,
			LitTrackParam& lpar);


	void ParallelParamToSerial(
			const LitTrackParam& lpar,
			CbmLitTrackParam& par);

	int PlaneId(
			int stationGroup,
			int station,
			int substation,
			LitDetectorLayout& layout) const;

	bool CheckHit(
			int stationGroup,
			int station,
			int substation,
			LitDetectorLayout& layout,
			CbmLitTrack* track);

	void SerialHitToParallel(
			const CbmLitPixelHit& hit,
			LitPixelHit& lhit);

	LitDetectorLayout fLayout;
};

#endif /* CBMLITPARALLELTRACKFITTERTEST_H_ */
