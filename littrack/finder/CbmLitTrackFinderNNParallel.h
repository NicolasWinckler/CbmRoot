/** CbmLitTrackFinderNNParallel.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 **/

#ifndef CBMLITTRACKFINDERNNPARALLEL_H_
#define CBMLITTRACKFINDERNNPARALLEL_H_

#include "CbmLitTrackFinder.h"

#include "parallel/LitTrackFinderNNParallel.h"

class CbmLitTrackFinderNNParallel : public CbmLitTrackFinder
{
public:
	/* Constructor */
	CbmLitTrackFinderNNParallel();

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
			ScalPixelHitVector& lhits);

	void ConvertSeeds(
			TrackPtrVector& seeds,
			TrackVector& lseeds);

	void ConvertTracks(
			TrackVector& ltracks,
			TrackPtrVector& tracks);

	LitTrackFinderNNParallel fTrackFinder;

	int fEventNo;
};

#endif /* CBMLITTRACKFINDERNNPARALLEL_H_ */
