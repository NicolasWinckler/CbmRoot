/* CbmLitTrackSelectionMuch.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 *
 * Track selection for MUCH.
 */

#ifndef CBMLITTRACKSELECTIONMUCH_H_
#define CBMLITTRACKSELECTIONMUCH_H_

#include "base/CbmLitPtrTypes.h"
#include "interface/CbmLitTrackSelection.h"

class CbmLitTrackSelectionMuch : public CbmLitTrackSelection
{
public:
	/* Constructor */
	CbmLitTrackSelectionMuch();

	/* Destructor */
	virtual ~CbmLitTrackSelectionMuch();

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
			TrackPtrVector& tracks);;

	/* Returns maximum number of shared hits */
	int GetNofSharedHits() const {return fNofSharedHits;}

	/* Returns minimum last plane index cut */
	int GetMinLastPlaneId() const {return fMinLastPlaneId;}

	/* Returns minimum number of hits cut */
	int GetMinNofHits() const {return fMinNofHits;}

	/* Sets maximum number of shared hits */
	void SetNofSharedHits(int nofHits) {fNofSharedHits = nofHits;}

	/* Sets minimum last plane index cut */
	void SetMinLastPlaneId(int minLastPlaneId) {fMinLastPlaneId = minLastPlaneId;}

	/* Sets minimum number of hits cut */
	void SetMinNofHits(int minNofHits) {fMinNofHits = minNofHits;}

private:
	/* Shared hits track selection tool */
	TrackSelectionPtr fSelectionSharedHits;
	/* Track selection tool that cuts on number of hits and last plane index */
	TrackSelectionPtr fSelectionCuts;

	/* Maximum number of shared hits */
	int fNofSharedHits;
	/* Minimum number of hits cuts */
	int fMinNofHits;
	/* Minimum last plane index cut */
	int fMinLastPlaneId;
};

#endif /*CBMLITTRACKSELECTIONMUCH_H_*/
