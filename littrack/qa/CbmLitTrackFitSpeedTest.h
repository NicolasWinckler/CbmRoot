/* CbmLitTrackFitSpeedTest.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 */

#ifndef CBMLITTRACKFITSPEEDTEST_H_
#define CBMLITTRACKFITSPEEDTEST_H_

#include "CbmLitPtrTypes.h"
#include "CbmLitTypes.h"
#include "CbmLitTask.h"

class CbmGlobalTrack;
class CbmLitTrack;

class CbmLitTrackFitSpeedTest : public CbmLitTask {
public:
	CbmLitTrackFitSpeedTest();
	virtual ~CbmLitTrackFitSpeedTest();

	virtual InitStatus Init();
	virtual void Exec(
			Option_t* opt);
	virtual void Finish();
	virtual void SetParContainers();

	void SetNofPlanes(Int_t nofPlanes) {fNofPlanes = nofPlanes;}
	void SetNofMuchHits(Int_t nofMuchHits) {fNofMuchHits = nofMuchHits;}
	void SetNofTrdHits(Int_t nofTrdHits) {fNofTrdHits = nofTrdHits;}

private:
	void RunTest();
	void ReadDataBranches();
	void CreateHistograms();
	void CreateTrackArrays();
	Bool_t CheckAcceptance(
			const CbmGlobalTrack* globalTrack);
	void GlobalTrackToLitTrack(
			const CbmGlobalTrack* globalTrack,
			CbmLitTrack* litTrack);

	Int_t fNofPlanes; // number of planes in the detector

	// Track acceptance parameters
	Int_t fNofMuchHits; // number of MUCH hits
	Int_t fNofTrdHits; // number of TRD hits

	// Pointers to data arrays
	TClonesArray* fGlobalTracks; // CbmGlobalTrack array
	TClonesArray* fStsTracks; // CbmStsTrack array
	TClonesArray* fMuchTracks; // CbmMuchTracks array
	TClonesArray* fMuchPixelHits; // CbmMuchPixelHits array
	TClonesArray* fMuchStrawHits; // CbmMuchStrawHits array
	TClonesArray* fTrdTracks; // CbmTrdTrack array
	TClonesArray* fTrdHits; // CbmTrdHit array
//	TClonesArray* fTofHits; // CbmTofHit array

	TrackPtrVector fLitTracks; // array with reconstructed global tracks converted to LitTracks

	TrackFitterPtr fFitterLitKalman; // track fitter tool
	TrackFitterPtr fFitterLitParallel; // parallel track fitter tool

	ClassDef(CbmLitTrackFitSpeedTest, 1);
};

#endif /* CBMLITTRACKFITSPEEDTEST_H_ */
