/**
 * \file CbmLitPropagationQa.h
 *
 * \brief Track propagation and track fit QA.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 *
 **/

#ifndef CBMLITPROPAGATIONQA_H_
#define CBMLITPROPAGATIONQA_H_ 1

#include "base/CbmLitTypes.h"
#include "base/CbmLitPtrTypes.h"

#include "FairTask.h"
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

/**
 * \file CbmLitPropagationQa
 *
 * \brief Track propagation and track fit QA.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 *
 **/
class CbmLitPropagationQa : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitPropagationQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPropagationQa();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(
      Option_t* opt);

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void SetParContainers();

   /* Setters */
   void SetNofPlanes(Int_t nofPlanes) { fNofPlanes = nofPlanes; }
   void SetNofTrdHits(Int_t nofTrdHits) {fNofTrdHits = nofTrdHits;}
   void SetNofMuchHits(Int_t nofMuchHits) {fNofMuchHits = nofMuchHits;}
   void SetNofTofHits(Int_t nofTofHits) {fNofTofHits = nofTofHits;}
   void SetTestFastPropagation(Bool_t isTestFastPropagation) {fIsTestFastPropagation = isTestFastPropagation;}
   void SetPDGCode(Int_t pdgCode) {fPDGCode = pdgCode;}
   void SetOutputDir(const std::string& dir) { fOutputDir = dir;}
   void IsDrawPropagation(Bool_t drawPropagation) {fIsDrawPropagation = drawPropagation;}
   void IsDrawFilter(Bool_t drawFilter) {fIsDrawFilter = drawFilter;}
   void IsDrawSmoother(Bool_t drawSmoother) {fIsDrawSmoother = drawSmoother;}
   void IsCloseCanvas(Bool_t closeCanvas) {fIsCloseCanvas = closeCanvas;}
   void IsFixedBounds(Bool_t isFixedBounds) {fIsFixedBounds = isFixedBounds;};

private:
   /**
    * \brief Determine detector layout.
    */
   void DetermineSetup();

   /**
    * \brief Read and create data branches.
    */
   void ReadDataBranches();

   /**
    * \brief Create histograms.
    */
   void CreateHistograms();

   /**
    * \brief Create track arrays.
    */
   void CreateTrackArrays();

   /**
    * \brief Check global track acceptance.
    * \param[in] globalTrack Pointer to the global track.
    * \return True if global track is accepted.
    */
   Bool_t CheckAcceptance(
      const CbmGlobalTrack* globalTrack);

   /**
    * \brief Convert CbmGlobalTrack to CbmLitTrack.
    * \param[in] globalTrack Pointer to the global track to be converted.
    * \param[out] litTrack Output CbmLitTrack.
    */
   void GlobalTrackToLitTrack(
      const CbmGlobalTrack* globalTrack,
      CbmLitTrack* litTrack);

   /**
    * \brief Convert CbmGlobalTrack to Monte-Carlo CbmLitTrack.
    * \param[in] globalTrack Pointer to the global track to be converted.
    * \param[out] litTrack Output CbmLitTrack.
    */
   void GlobalTrackToMCLitTrack(
      const CbmGlobalTrack* globalTrack,
      CbmLitTrack* litTrack);

   /**
    * \brief Free memory and delete track arrays.
    */
   void DeleteTrackArrays();

   /**
    * \brief Run track propagation and track fit tests.
    */
   void RunTest();

   /**
    * \brief Test track propagation for a specified track.
    * \param[in] track Track to be propagated.
    * \param[in] mcTrack Corresponding MC track for comparison.
    */
   void TestPropagation(
      const CbmLitTrack* track,
      const CbmLitTrack* mcTrack);

   /**
    * \brief Test track fitter for a specified track.
    * \param[in,out] track Track to be fitted.
    * \param[in] mcTrack Corresponding MC track for comparison.
    */
   void TestFitter(
      CbmLitTrack* track,
      const CbmLitTrack* mcTrack);

   /**
    * \brief Fill pull/residual histograms for specified predicted track parameters.
    * \param[in] par Calculated track parameters.
    * \param[in] mcPar Monte-Carlo track parameters.
    * \param[in] hit Hit at plane where parameters are calculated.
    * \param[in] plane Plane number.
    */
   void FillHistosPropagation(
      const CbmLitTrackParam* par,
      const CbmLitTrackParam* mcPar,
      const CbmLitHit* hit,
      Int_t plane);

   /**
    * \brief Fill pull/residual histograms for specified updated track parameters.
    * \param[in] par Calculated track parameters.
    * \param[in] mcPar Monte-Carlo track parameters.
    * \param[in] hit Hit at plane where parameters are calculated.
    * \param[in] plane Plane number.
    * \param[in] chiSq Chi square value.
    */
   void FillHistosFilter(
      const CbmLitTrackParam* par,
      const CbmLitTrackParam* mcPar,
      const CbmLitHit* hit,
      Int_t plane,
      float chisq);

   /**
    *
    */
   void FillHistosFitter(
      const CbmLitTrack* track,
      const CbmLitTrack* mcTrack);

   /**
    *
    */
   void McPointToLitFitNode(
      FairMCPoint* point,
      CbmLitFitNode* node);

   /**
    *
    */
   std::vector<Double_t> CalcResidualsAndPulls(
      const CbmLitTrackParam* par,
      const CbmLitTrackParam* mcPar);

   /**
    *
    */
   void PrintStopwatchStatistics();

   /**
    *
    */
   void TestFastPropagation(
      CbmLitTrack* track,
      CbmLitTrack* mcTrack);

   /**
    *
    */
   void Draw();

   /**
    *
    */
   void DrawHistos(
      TCanvas* c[],
      Int_t v);

   /**
    *
    */
   void PrintResults(
      std::ostream& out,
      int v);

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
// TClonesArray* fStsTrackMatches; // CbmStsTrackMatch array
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
   static const Int_t NOF_PARAMS = 12; // number of parameters = 12

   //Time analysis
   TStopwatch fPropagationWatch; // for the propagation
   TStopwatch fFitterWatch; // for the track fitter
   TStopwatch fSmootherWatch; // for the track smoother

   Int_t fEvents; // Event counter
   Int_t fVerbose; // Verbose level

   Bool_t fIsTestFastPropagation;

   //Drawing options
   //If true than specified histograms are drawn.
   Bool_t fIsDrawPropagation; // for propagation
   Bool_t fIsDrawFilter; // for filter
   Bool_t fIsDrawSmoother; // for smoother

   Bool_t fIsCloseCanvas; // If true than canvas will be closed after drawing
   std::string fOutputDir; // Output directory for image files

   Bool_t fIsFixedBounds; // If true than fixed bounds are used for histograms

   // Vectors to store sigma and RMS value for different parameters.
   // [v][plane number][parameter]
   // v: 0-propagation, 1-filter, 2-smoother.
   // plane number: absolute plane number
   // parameter: see description above
   std::vector<std::vector<std::vector<Double_t> > > fSigma;
   std::vector<std::vector<std::vector<Double_t> > > fRms;

   ClassDef(CbmLitPropagationQa, 1);
};

#endif
