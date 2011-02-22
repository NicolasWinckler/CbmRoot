/* CbmLitTrackSelectionMomentum.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 *
 * Selects the track cutting on the minimum momentum in GeV.
 */

#ifndef CBMLITTRACKSELECTIONMOMENTUM_H_
#define CBMLITTRACKSELECTIONMOMENTUM_H_

#include "CbmLitTrackSelection.h"

class CbmLitTrackSelectionMomentum : public CbmLitTrackSelection
{
public:
	/* Constructor */
	CbmLitTrackSelectionMomentum();

	/* Destructor */
	virtual ~CbmLitTrackSelectionMomentum();

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

	/* Returns momentum cut */
	myf GetMinMomentum() const {return fMinMomentum;}

	/* Sets momentum cut */
	void SetMinMomentum(myf minMomentum) {fMinMomentum = minMomentum;}

private:
	/* Momentum cut in [GeV/c] */
	myf fMinMomentum;
};

#endif /*CBMLITTRACKSELECTIONMOMENTUM_H_*/
