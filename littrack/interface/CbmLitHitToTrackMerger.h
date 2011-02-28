/** CbmLitHitToTrackMerger.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2008
 *
 * Interface for hit-to-track merging algorithms.
 **/

#ifndef CBMLITHITTOTRACKMERGER_H_
#define CBMLITHITTOTRACKMERGER_H_

#include "base/CbmLitTool.h"
#include "base/CbmLitTypes.h"

class CbmLitHitData;
class CbmLitDetectorLayout;

class CbmLitHitToTrackMerger : public CbmLitTool
{
public:
	/* Constructor */
	CbmLitHitToTrackMerger() {};

	/* Destructor */
	virtual ~CbmLitHitToTrackMerger() {};

	/* Main function to be implemented for concrete hit-to-track merging algorithm
	 * @param hits Vector of hits that have to be merged with tracks
	 * @param tracks Vector of tracks that have to be merged with hits
	 * @return Status code */
	virtual LitStatus DoMerge(
			HitPtrVector& hits,
			TrackPtrVector& tracks) = 0;
};

#endif /* CBMLITHITTOTRACKMERGER_H_ */
