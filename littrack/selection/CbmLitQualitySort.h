/* CbmLitQualitySort.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 *
 * Performs sort of the track array by the quality criterion.
 */

#ifndef CBMLITQUALITYSORT_H_
#define CBMLITQUALITYSORT_H_

#include "CbmLitEnums.h"
#include "CbmLitTypes.h"

class CbmLitQualitySort {
public:
	/* Constructor */
	CbmLitQualitySort();

	/* Destructor */
	virtual ~CbmLitQualitySort();

	/* Sorts track array by quality */
	virtual LitStatus DoSort(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);

	/* Sorts track array by quality */
	virtual LitStatus DoSort(
			TrackPtrVector& tracks);

private:

	/* Sorts track array by quality
	 * using number of hits and chi2 */
	void SortNofHits(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);

	/* Sorts track array by quality
	 * using last plane id and chi2 */
	void SortLastPlaneId(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);
};

#endif /* CBMLITQUALITYSORT_H_ */
