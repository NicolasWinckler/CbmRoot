/* CbmLitTrackSelectionTrd.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 *
 * Track selection for TRD.
 */

#ifndef CBMLITTRACKSELECTIONTRD_H_
#define CBMLITTRACKSELECTIONTRD_H_

#include "CbmLitTrackSelection.h"
#include "CbmLitPtrTypes.h"

class CbmLitTrackSelectionTrd : public CbmLitTrackSelection
{
public:
	/* Constructor */
	CbmLitTrackSelectionTrd();

	/* Destructor */
	virtual ~CbmLitTrackSelectionTrd();

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

	/* Returns maximum number of shared hits */
	int GetNofSharedHits() const {return fNofSharedHits;}

	/* Sets maximum number of shared hits */
	void SetNofSharedHits(int nofHits) {fNofSharedHits = nofHits;}

private:
	/* Shared hits track selection tool */
	TrackSelectionPtr fSharedHitsSelection;
	/* Same seed track selection tool */
	TrackSelectionPtr fSameSeedSelection;

	/* Maximum number of shared hits */
	int fNofSharedHits;
};

#endif /*CBMLITTRACKSELECTIONTRD_H_*/
