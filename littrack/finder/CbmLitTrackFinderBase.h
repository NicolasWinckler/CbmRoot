/** CbmLitTrackFinderBase.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 *
 * Class implements basic functionality for track finders.
 **/

#ifndef CBMLITTRACKFINDERBASE_H_
#define CBMLITTRACKFINDERBASE_H_

#include "interface/CbmLitTrackFinder.h"
#include "finder/CbmLitGating.h"
#include "base/CbmLitFloat.h"
#include "base/CbmLitTypes.h"
#include "base/CbmLitDetectorLayout.h"
#include "base/CbmLitHitData.h"
#include "base/CbmLitPtrTypes.h"
#include "base/CbmLitTrackFinderSettings.h"

#include <set>

class CbmLitTrackFinderBase : public CbmLitTrackFinder,
                              public CbmLitGating
{
public:
	/* Constructor */
	CbmLitTrackFinderBase();

	/* Destructor */
	virtual ~CbmLitTrackFinderBase();

	/* Sets detector layout */
	void SetLayout(CbmLitDetectorLayout layout) {fLayout = layout;}

	/* Sets settings for tracking */
	void SetSettings(const CbmLitTrackFinderSettings& settings) { fSettings = settings; }

	/* Sets verbosity level */
    void SetVerbose(int verbose) {fVerbose = verbose;}

protected:
    /* Sets number of iterations for tracking */
	void SetNofIter(int nofIter) { fNofIter = nofIter; }

	/* Sets maximum acceptable number of missing hits */
	void SetMaxNofMissingHits(int maxNofMissingHits) { fMaxNofMissingHits = maxNofMissingHits;}

	/* Sets particle hypothesis for tracking */
	void SetPDG(int pdg) { fPDG = pdg; }

protected:
    /* Parameters for each tracking iteration has to be set in this function
     * @param iter Iteration number */
    virtual void SetIterationParameters(
    		int iter){}

    /* Arranges hits by stations
     * @param itBegin Iterator of the first hit
     * @param itEnd Iterator of the last hit */
    void ArrangeHits(
    		HitPtrIterator itBegin,
    		HitPtrIterator itEnd);

    /* Writes already used hits to a used hits set
     * @param Iterator to the first track
     * @param Iterator to the last track */
    void RemoveHits(
    		TrackPtrIterator itBegin,
    		TrackPtrIterator itEnd);

    /* Copies tracks to output array
     * @param Iterator to the first track
     * @param Iterator to the last track
     * @param Output track array */
    void CopyToOutput(
    		TrackPtrIterator itBegin,
    		TrackPtrIterator itEnd,
    		TrackPtrVector& tracks);

protected:
    /* Detector layout */
    CbmLitDetectorLayout fLayout;
    /* Hit storage */
    CbmLitHitData fHitData;
    /* Sets with hits that have been used */
    std::set<int> fUsedHitsSet;
    /* Set with track seeds that have been used */
    std::set<int> fUsedSeedsSet;
    /* Settings for the track finder */
    CbmLitTrackFinderSettings fSettings;
    /* Number of iterations for tracking */
	int fNofIter;
	/* Maximum number of acceptable missing hits */
	int fMaxNofMissingHits;
	/* Particle hypothesis for tracking */
	int fPDG;
	/* Verbosity level */
    int fVerbose;
    /* Event counter */
    int fEventNo;
};

#endif /*CBMLITTRACKFINDERBASE_H_*/
