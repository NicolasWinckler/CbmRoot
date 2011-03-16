/** CbmLitFindMvdTracks.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 **/
#ifndef CBMLITFINDMVDTRACKS_H_
#define CBMLITFINDMVDTRACKS_H_

#include "base/CbmLitTypes.h"
#include "base/CbmLitPtrTypes.h"
#include "FairTask.h"

class CbmLitFindMvdTracks : public FairTask
{
public:
	/* Constructor */
	CbmLitFindMvdTracks();

	/* Destructor */
	virtual ~CbmLitFindMvdTracks();

    /* Inherited from FairTask. */
	virtual InitStatus Init();

	/* Inherited from FairTask. */
	virtual void Exec(Option_t* opt);

	/* Inherited from FairTask. */
	virtual void SetParContainers();

	/* Sets the tracking algorithm to be used.
     * @param trackingType Name of the tracking algorithm.
     * "branch" - branching tracking
     * "nn" - nearest neighbor tracking
     * "weight" - weighting tracking */
	void SetTrackingType(const std::string& trackingType) { fTrackingType = trackingType;}

private:
    /* Inherited from FairTask. */
	virtual void Finish();

    /* Determines the CBM detector setup, based on TGeoManager stored in the input MC file. */
	void DetermineSetup();

    /* Reads necessary data branches from the input data files and
     * creates branches for CbmGlobalTrack, CbmTrdTrack, CbmMuchTrack */
	void ReadAndCreateDataBranches();

    /* Converts input data from CBMROOT data classes to LIT data classes. */
	void ConvertInputData();

    /* Converts output data LIT data classes to CBMROOT data classes. */
	void ConvertOutputData();

    /* Runs the track reconstruction. */
	void RunTrackReconstruction();

	/* Create and initializes track finder and track merger objects. */
	void InitTrackReconstruction();

    /* Clear arrays and frees the memory. */
	void ClearArrays();

	Bool_t fIsMvd; // If MVD detected than true
	Bool_t fIsSts; // If MVD detected than true

	// Pointers to data arrays
	TClonesArray* fStsTracks; // CbmStsTrack array
	TClonesArray* fStsHits; // CbmStsHit
	TClonesArray* fMvdHits; // CbmMvdHits

	// LIT data arrays
	TrackPtrVector fLitStsTracks; // STS tracks
	HitPtrVector fLitMvdHits; // MVD
	TrackPtrVector fLitOutputTracks; // output Lit tracks

	// Tools
	TrackFinderPtr fFinder; // track finder
//	HitToTrackMergerPtr fMerger; // hit-to-track merger
//	TrackFitterPtr fFitter; // track fitter

	// Settings
	// Tracking method to be used
	// "branch" - branching method
	// "nn" - nearest neighbor method
	// "weight" - weighting method
	std::string fTrackingType;

	// stopwatches
//	TStopwatch fTrackingWatch; // stopwatch for tracking
//	TStopwatch fMergerWatch; // stopwatch for merger

	Int_t fEventNo; // event counter

	ClassDef(CbmLitFindMvdTracks, 1);
};

#endif /* CBMLITFINDMVDTRACKS_H_ */
