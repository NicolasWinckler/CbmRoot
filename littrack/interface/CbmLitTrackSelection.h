/** CbmLitTrackSelection.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2008
 *
 * Interface for track selection algorithms.
 **/

#ifndef CBMLITTRACKSELECTION_H_
#define CBMLITTRACKSELECTION_H_

#include "base/CbmLitTool.h"
#include "base/CbmLitTypes.h"

class CbmLitTrackSelection : public CbmLitTool
{
public:
	/* Constructor */
	CbmLitTrackSelection(){};

	/* Destructor */
	virtual ~CbmLitTrackSelection(){};

	/* Main function to be implemented for the concrete track selection algorithm
	 * @param itBegin Iterator to the first track
	 * @param itEnd Iterator to the last track
	 * @return Status code */
	virtual LitStatus DoSelect(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd) = 0;

	/* Main function to be implemented for the concrete track selection algorithm
	 * @param tracks Input track array
	 * @return Status code */
	virtual LitStatus DoSelect(
			TrackPtrVector& tracks) = 0;
};

#endif /*CBMLITTRACKSELECTION_H_*/
