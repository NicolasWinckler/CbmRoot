#ifndef CBMLITPROPAGATIONANALYSIS_H
#define CBMLITPROPAGATIONANALYSIS_H 1

#include "FairTask.h"
#include "CbmLitTypes.h"
#include "CbmLitPtrTypes.h"

#include <vector>

class CbmLitTrackParam;
class CbmLitTrack;
class CbmLitHit;
class FairMCPoint;
class CbmMCTrack;
class CbmStsTrack;
class CbmTrdTrack;
class CbmMuchTrack;
class CbmGlobalTrack;
class TClonesArray;
class TH1F;

class CbmLitPropagationAnalysis : public FairTask
{
public:
	CbmLitPropagationAnalysis();
	virtual ~CbmLitPropagationAnalysis();

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
	void DetermineSetup();
	void ReadDataBranches();
	void CreateHistograms();
	void CreateTrackArrays();
	Bool_t CheckAcceptance(
			const CbmGlobalTrack* globalTrack);
	void GlobalTrackToLitTrack(
			const CbmGlobalTrack* globalTrack,
			CbmLitTrack* litTrack);
	void GlobalTrackToMCLitTrack(
			const CbmGlobalTrack* globalTrack,
			CbmLitTrack* litTrack);
	void DeleteTrackArrays();
	void RunTest();
	void TestPropagation(
			CbmLitTrack* track,
			CbmLitTrack* mcTrack);
	void TestFitter(
			CbmLitTrack* track,
			CbmLitTrack* mcTrack);
	void FillHistosPropagation(
			const CbmLitTrackParam* par,
			const CbmLitTrackParam* mcPar,
			const CbmLitHit* hit,
			Int_t plane);
	void FillHistosFitter(
			const CbmLitTrack* track,
			const CbmLitTrack* mcTrack);

	void McPointToLitFitNode(
			FairMCPoint* point,
			CbmLitFitNode* node);
	std::vector<Double_t> CalcResidualsAndPulls(
			const CbmLitTrackParam* par,
			const CbmLitTrackParam* mcPar);

	Bool_t fIsElectronSetup; // If "electron" setup detected than true
	Bool_t fIsSts; // If STS detected than true
	Bool_t fIsTrd; // If TRD detected than true
	Bool_t fIsMuch; // If MUCH detected than true
	Bool_t fIsTof; // If TOF detected than true

	TrackPtrVector fLitTracks; // array with reconstructed global tracks converted to LitTracks
	TrackPtrVector fLitMcTracks; // array with MC track converted to LitTracks
	Int_t fNofPlanes; // number of planes in the detector

	// Track acceptance parameters
	Int_t fNofTrdHits; // number of TRD hits
	Int_t fNofMuchHits; // number of MUCH hits
	Int_t fNofTofHits; // number of TOF hits

	// Pointers to data arrays
	TClonesArray* fMCTracks; // CbmMCTrack array
	TClonesArray* fGlobalTracks; // CbmGlobalTrack array
	TClonesArray* fStsTracks; // CbmStsTrack array
//	TClonesArray* fStsTrackMatches; // CbmStsTrackMatch array
	TClonesArray* fMuchTracks; // CbmMuchTracks array
	TClonesArray* fMuchHits; // CbmMuchHits array
	TClonesArray* fMuchPoints; // CbmMuchPoint array
	TClonesArray* fMuchTrackMatches; // CbmMuchTrackMatch array
	TClonesArray* fMuchDigiMatches; // CbmMuchDigiMatch array
	TClonesArray* fTrdTracks; // CbmTrdTrack array
	TClonesArray* fTrdHits; // CbmTrdHit array
	TClonesArray* fTrdPoints; // CbmTrdPoint array
	TClonesArray* fTrdTrackMatches; // CbmTrdTrackMatch array
	TClonesArray* fTofPoints; // CbmTofPoint array
	TClonesArray* fTofHits; // CbmTofHit array

	// Tools
	TrackPropagatorPtr fPropagator; // track propagation tool
	TrackUpdatePtr fFilter; // track update tool
	TrackFitterPtr fFitter; // track fitter tool
	TrackFitterPtr fSmoother; // track smoother tool

	// Histograms
	// histogram[plane number][parameter]
	// parameters:
	// [0-4] - residuals (x, y, tx, ty, q/p)
	// [5-9] - pulls (x, y, tx, ty, q/p)
	// [10] - relative momentum error in %
	// [11] - chi square
	std::vector<std::vector<TH1F*> > fPropagationHistos; //for propagation analysis
	std::vector<std::vector<TH1F*> > fFilterHistos; // for fitter analysis
	std::vector<std::vector<TH1F*> > fSmootherHistos; // for smoother analysis
	Int_t fNofParams; // number of parameters = 12

	Int_t fEvents; // Event counter
	Int_t fVerbose; // Verbose level

	ClassDef(CbmLitPropagationAnalysis,1);
};

#endif
