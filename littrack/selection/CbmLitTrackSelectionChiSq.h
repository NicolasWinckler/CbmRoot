/* CbmLitTrackSelectionChiSq.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 *
 * Removes tracks that do not pass chi square criterion.
 */

#ifndef CBMLITTRACKSELECTIONCHISQ_H_
#define CBMLITTRACKSELECTIONCHISQ_H_

#include "CbmLitTrackSelection.h"

class CbmLitTrackSelectionChiSq : public CbmLitTrackSelection
{
public:
	/* Constructor */
	CbmLitTrackSelectionChiSq();

	/* Destructor */
	virtual ~CbmLitTrackSelectionChiSq();

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

	/* Return chi square cut */
	myf GetMaxChiSq() const {return fMaxChiSq;}

	/* Sets chi square cut */
	void SetMaxChiSq(myf maxChiSq) {fMaxChiSq = maxChiSq;}

private:
	/* Chi square cut */
	myf fMaxChiSq;
};

#endif /*CBMLITTRACKSELECTIONCHISQ_H_*/
