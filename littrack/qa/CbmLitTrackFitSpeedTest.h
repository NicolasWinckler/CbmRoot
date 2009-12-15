/* CbmLitTrackFitSpeedTest.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 */

#ifndef CBMLITTRACKFITSPEEDTEST_H_
#define CBMLITTRACKFITSPEEDTEST_H_

#include "CbmLitPtrTypes.h"
#include "CbmLitTypes.h"
#include "FairTask.h"

class CbmGlobalTrack;
class CbmLitTrack;

class CbmLitTrackFitSpeedTest : public FairTask {
public:
	CbmLitTrackFitSpeedTest();
	virtual ~CbmLitTrackFitSpeedTest();

	virtual InitStatus Init();
	virtual void Exec(
			Option_t* opt);
	virtual void Finish();
	virtual void SetParContainers();

	void SetNofPlanes(Int_t nofPlanes) {fNofPlanes = nofPlanes;}
	void SetNofTrdHits(Int_t nofTrdHits) {fNofTrdHits = nofTrdHits;}
	void SetNofMuchHits(Int_t nofMuchHits) {fNofMuchHits = nofMuchHits;}
	void SetNofTofHits(Int_t nofTofHits) {fNofTofHits = nofTofHits;}

private:
	void RunTest();
	void DetermineSetup();
	void ReadDataBranches();
	void CreateHistograms();
	void CreateTrackArrays();
	Bool_t CheckAcceptance(
			const CbmGlobalTrack* globalTrack);
	void GlobalTrackToLitTrack(
			const CbmGlobalTrack* globalTrack,
			CbmLitTrack* litTrack);

	Bool_t fIsElectronSetup; // If "electron" setup detected than true
	Bool_t fIsSts; // If STS detected than true
	Bool_t fIsTrd; // If TRD detected than true
	Bool_t fIsMuch; // If MUCH detected than true
	Bool_t fIsTof; // If TOF detected than true

	Int_t fNofPlanes; // number of planes in the detector

	// Track acceptance parameters
	Int_t fNofTrdHits; // number of TRD hits
	Int_t fNofMuchHits; // number of MUCH hits
	Int_t fNofTofHits; // number of TOF hits

	// Pointers to data arrays
//	TClonesArray* fMCTracks; // CbmMCTrack array
	TClonesArray* fGlobalTracks; // CbmGlobalTrack array
	TClonesArray* fStsTracks; // CbmStsTrack array
//	TClonesArray* fStsTrackMatches; // CbmStsTrackMatch array
	TClonesArray* fMuchTracks; // CbmMuchTracks array
	TClonesArray* fMuchPixelHits; // CbmMuchPixelHits array
	TClonesArray* fMuchStrawHits; // CbmMuchStrawHits array
//	TClonesArray* fMuchPoints; // CbmMuchPoint array
//	TClonesArray* fMuchTrackMatches; // CbmTrackMatch array
//	TClonesArray* fMuchPixelDigiMatches; // CbmMuchDigiMatch array for pixel hits
//	TClonesArray* fMuchStrawDigiMatches; // CbmMuchDigiMatch array for straw hits
//	TClonesArray* fMuchClusters; // CbmMuchCluster array
	TClonesArray* fTrdTracks; // CbmTrdTrack array
	TClonesArray* fTrdHits; // CbmTrdHit array
//	TClonesArray* fTrdPoints; // CbmTrdPoint array
//	TClonesArray* fTrdTrackMatches; // CbmTrdTrackMatch array
//	TClonesArray* fTofPoints; // CbmTofPoint array
	TClonesArray* fTofHits; // CbmTofHit array

	TrackPtrVector fLitTracks; // array with reconstructed global tracks converted to LitTracks

	TrackFitterPtr fFitterLitKalman; // track fitter tool
	TrackFitterPtr fFitterLitParallel; // parallel track fitter tool

	Int_t fEvents; // Event counter
	Int_t fVerbose; // Verbose level

	ClassDef(CbmLitTrackFitSpeedTest, 1);
};

#endif /* CBMLITTRACKFITSPEEDTEST_H_ */
