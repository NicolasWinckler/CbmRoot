/* CbmLitTrackSelectionD.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 *
 * Cuts tracks on number of hits and last reached plane
 */

#ifndef CBMLITTRACKSELECTIOND_H_
#define CBMLITTRACKSELECTIOND_H_

#include "CbmLitTrackSelection.h"

class CbmLitTrackSelectionD : public CbmLitTrackSelection
{
public:
	/* Constructor */
	CbmLitTrackSelectionD();

	/* Destructor */
	virtual ~CbmLitTrackSelectionD();

	/* Derived from CbmLitTool */
	virtual LitStatus Initialize();

	/* Derived from CbmLitTool */
	virtual LitStatus Finalize();

	/* Derived from CbmLitTrackSelection */
	virtual LitStatus DoSelect(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);

	/* Derived from CbmLitTrackSelection */
	virtual LitStatus DoSelect(
			TrackPtrVector& tracks);

	/* Returns last plane index cut */
	int GetMinLastPlaneId() const {return fMinLastPlaneId;}

	/* Returns number of hits cut */
	int GetMinNofHits() const {return fMinNofHits;}

	/* Sets last plane index cut */
	void SetMinLastPlaneId(int minLastPlaneId) {fMinLastPlaneId = minLastPlaneId;}

	/* Sets number of hits cut */
	void SetMinNofHits(int minNofHits) {fMinNofHits = minNofHits;}

private:
	/* Last plane index cut */
	int fMinLastPlaneId;
	/* Number of hits cut */
	int fMinNofHits;
};

#endif /*CBMLITTRACKSELECTIOND_H_*/
