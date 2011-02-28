/* CbmLitTrackSelectionCuts.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 *
 * Removes tracks based on the cuts.
 */
#ifndef CBMLITTRACKSELECTIONCUTS_H_
#define CBMLITTRACKSELECTIONCUTS_H_

#include "interface/CbmLitTrackSelection.h"

class CbmLitTrackSelectionCuts : public CbmLitTrackSelection
{
public:
	/* Constructor */
	CbmLitTrackSelectionCuts();

	/* Constructor */
	CbmLitTrackSelectionCuts(
			myf maxChiSq,
			int minLastPlaneId,
			int minNofHits,
			myf minMomentum);

	/* Destructor */
	virtual ~CbmLitTrackSelectionCuts();

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

	/* Returns last plane index cut */
	int GetMinLastPlaneId() const {return fMinLastPlaneId;}

	/* Returns number of hits cut */
	int GetMinNofHits() const {return fMinNofHits;}

	/* Returns momentum cut */
	myf GetMinMomentum() const {return fMinMomentum;}

	/* Sets chi square cut */
	void SetMaxChiSq(myf maxChiSq) {fMaxChiSq = maxChiSq;}

	/* Sets last plane index cut */
	void SetMinLastPlaneId(int minLastPlaneId) {fMinLastPlaneId = minLastPlaneId;}

	/* Sets number of hits cut */
	void SetMinNofHits(int minNofHits) {fMinNofHits = minNofHits;}

	/* Sets momentum cut */
	void SetMinMomentum(myf minMomentum) {fMinMomentum = minMomentum;}

private:
	/* Chi square cut */
	myf fMaxChiSq;
	/* Last plane index cut */
	int fMinLastPlaneId;
	/* Number of hits cut */
	int fMinNofHits;
	/* Momentum cut in [GeV/c] */
	myf fMinMomentum;
};

#endif /* CBMLITTRACKSELECTIONCUTS_H_ */
