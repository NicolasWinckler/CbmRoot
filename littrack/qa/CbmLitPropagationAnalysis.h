/** CbmLitPropagationAnalysis.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 2.0
 **
 ** CBM task class for analysis of the track propagation and track fit.
 **/

#ifndef CBMLITPROPAGATIONANALYSIS_H_
#define CBMLITPROPAGATIONANALYSIS_H_ 1

#include "FairTask.h"
#include "CbmLitTypes.h"
#include "CbmLitPtrTypes.h"

#include "TStopwatch.h"

#include <vector>
#include <iostream>

class CbmLitTrackParam;
class CbmLitTrack;
class CbmLitHit;
class FairMCPoint;
class CbmMCTrack;
class CbmGlobalTrack;
class TClonesArray;
class TH1F;
class TCanvas;

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
	void SetTestFastPropagation(Bool_t isTestFastPropagation){fIsTestFastPropagation = isTestFastPropagation;}
	void SetPDGCode(Int_t pdgCode) {fPDGCode = pdgCode;}

	/**
     * Sets the output directory for images.
     * @param dir Directory name.
     */
	void SetOutputDir(const std::string& dir) { fOutputDir = dir;}

	void IsDrawPropagation(Bool_t drawPropagation) {fIsDrawPropagation = drawPropagation;}
	void IsDrawFilter(Bool_t drawFilter) {fIsDrawFilter = drawFilter;}
	void IsDrawSmoother(Bool_t drawSmoother) {fIsDrawSmoother = drawSmoother;}
	void IsCloseCanvas(Bool_t closeCanvas) {fIsCloseCanvas = closeCanvas;}

	void IsFixedBounds(Bool_t isFixedBounds) {fIsFixedBounds = isFixedBounds;};

	void SetPlaneNoPhd(Int_t planeNo) {fPlaneNoPhd = planeNo;}

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
	void FillHistosFilter(
			const CbmLitTrackParam* par,
			const CbmLitTrackParam* mcPar,
			const CbmLitHit* hit,
			Int_t plane,
			float chisq);

	void FillHistosFitter(
			const CbmLitTrack* track,
			const CbmLitTrack* mcTrack);

	void McPointToLitFitNode(
			FairMCPoint* point,
			CbmLitFitNode* node);
	std::vector<Double_t> CalcResidualsAndPulls(
			const CbmLitTrackParam* par,
			const CbmLitTrackParam* mcPar);

	void PrintStopwatchStatistics();

	void TestFastPropagation(
			CbmLitTrack* track,
			CbmLitTrack* mcTrack);

	void Draw();

	void DrawHistos(
			TCanvas* c[],
			Int_t v);

//	void DrawText(
//			Int_t index,
//			Double_t sigma,
//			Double_t rms);

	void PrintResults(
			std::ostream& out,
			int v);

	void DrawForPhd();

	void DrawForPhd(
			TCanvas* canvas,
			Int_t v);

	Bool_t fIsElectronSetup; // If "electron" setup detected than true
	Bool_t fIsSts; // If STS detected than true
	Bool_t fIsTrd; // If TRD detected than true
	Bool_t fIsMuch; // If MUCH detected than true
	Bool_t fIsTof; // If TOF detected than true

	TrackPtrVector fLitTracks; // array with reconstructed global tracks converted to LitTracks
	TrackPtrVector fLitMcTracks; // array with MC track converted to LitTracks
	Int_t fNofPlanes; // number of planes in the detector

	Int_t fPDGCode; // PDG code for the track fit

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
	TClonesArray* fMuchPixelHits; // CbmMuchPixelHits array
	TClonesArray* fMuchStrawHits; // CbmMuchStrawHits array
	TClonesArray* fMuchPoints; // CbmMuchPoint array
	TClonesArray* fMuchTrackMatches; // CbmTrackMatch array
	TClonesArray* fMuchPixelDigiMatches; // CbmMuchDigiMatch array for pixel hits
	TClonesArray* fMuchStrawDigiMatches; // CbmMuchDigiMatch array for straw hits
	TClonesArray* fMuchClusters; // CbmMuchCluster array
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
	TrackFitterPtr fParallelFitter; // parallel track fitter tool

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

	// Histograms for first and last STS parameters
	std::vector<std::vector<TH1F*> > fStsHistos;

	//Time analysis
	TStopwatch fPropagationWatch; // for the propagation
	TStopwatch fFitterWatch; // for the track fitter
	TStopwatch fSmootherWatch; // for the track smoother

	Int_t fEvents; // Event counter
	Int_t fVerbose; // Verbose level

	Bool_t fIsTestFastPropagation;

	//Drawing options
	//If true than specified histograms are drawn.
	Bool_t fIsDrawPropagation;
	Bool_t fIsDrawFilter;
	Bool_t fIsDrawSmoother;

	Bool_t fIsCloseCanvas; // If true than canvas will be closed after drawing
	std::string fOutputDir; // Output directory for image files

	Bool_t fIsFixedBounds;

	Int_t fPlaneNoPhd; // Number of the plane fo Phd drawing

	// Vectors to store sigma and RMS value for different parameters.
	// [v][plane number][parameter]
	// v: 0-propagation, 1-filter, 2-smoother.
	// plane number: absolute plane number
	// parameter: see description above
	std::vector<std::vector<std::vector<Double_t> > > fSigma;
	std::vector<std::vector<std::vector<Double_t> > > fRms;

	ClassDef(CbmLitPropagationAnalysis, 1);
};

#endif
