/** CbmLitParallelTrackFitterTestElectron.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 **/
#ifndef CBMLITPARALLELTRACKFITTERTESTELECTRON_H_
#define CBMLITPARALLELTRACKFITTERTESTELECTRON_H_

#include "base/CbmLitPtrTypes.h"
#include "data/CbmLitPixelHit.h"
#include "interface/CbmLitTrackFitter.h"
#include "parallel/electron/LitDetectorGeometryElectron.h"
#include "parallel/LitHit.h"

//class CbmLitField;
class CbmLitTrack;
class CbmLitTrackParam;

class CbmLitParallelTrackFitterTestElectron : public CbmLitTrackFitter {
public:
	CbmLitParallelTrackFitterTestElectron();
	virtual ~CbmLitParallelTrackFitterTestElectron();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual LitStatus Fit(
			CbmLitTrack *track,
			bool downstream = true);

private:

	unsigned char PlaneId(
			unsigned char stationGroup,
			unsigned char station,
			LitDetectorLayoutElectronScal& layout) const;

	bool CheckHit(
			unsigned char stationGroup,
			unsigned char station,
			LitDetectorLayoutElectron<fscal>& layout,
			CbmLitTrack* track);

	void SerialHitToParallel(
			const CbmLitPixelHit& hit,
			LitPixelHit<fscal>& lhit);

	LitDetectorLayoutElectronScal fLayout;

//	TrackExtrapolatorPtr fExtrapolator;
//	TrackPropagatorPtr fPropagator;
//	CbmLitField* fField;
};

#endif /* CBMLITPARALLELTRACKFITTERTESTELECTRON_H_ */
