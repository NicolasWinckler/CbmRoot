/** CbmLitTrackFinderNNParallel.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 **/

#ifndef CBMLITTRACKFINDERNNPARALLEL_H_
#define CBMLITTRACKFINDERNNPARALLEL_H_

#include "CbmLitTrackFinder.h"

class LitTrackFinderNNVecMuon;
class LitTrackFinderNNScalarElectron;
class LitTrackFinderNNVecElectron;
class LitScalPixelHit;
class LitScalTrack;

class CbmLitTrackFinderNNParallel : public CbmLitTrackFinder
{
public:
	/* Constructor */
	CbmLitTrackFinderNNParallel(const std::string& trackingType);

	/* Destructor */
	virtual ~CbmLitTrackFinderNNParallel();

	/* Inherited from CbmLitTrackFinder */
	virtual LitStatus DoFind(
			HitPtrVector& hits,
			TrackPtrVector& trackSeeds,
			TrackPtrVector& tracks);

	/* Inherited from CbmLitTool */
	virtual LitStatus Initialize();

	/* Inherited from CbmLitTool */
	virtual LitStatus Finalize();

private:
	void ConvertHits(
			HitPtrVector& hits,
			LitScalPixelHit* lhits[]);

	void ConvertSeeds(
			TrackPtrVector& seeds,
			LitScalTrack* lseeds[]);

	void ConvertTracks(
			LitScalTrack* ltracks[],
			unsigned int nofTracks,
			TrackPtrVector& tracks);

	LitTrackFinderNNVecMuon* fTFParallelMuon;

	LitTrackFinderNNScalarElectron* fTFScalElectron;

	LitTrackFinderNNVecElectron* fTFVecElectron;

	double fTime;

	int fEventNo;

	std::string fTrackingType;
};

#endif /* CBMLITTRACKFINDERNNPARALLEL_H_ */
