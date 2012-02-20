/**
 * \file CbmLitPropagationQaCalculator.h
 * \brief Implementation of propagation QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
#ifndef CBMLITPROPAGATIONQACALCULATOR_H_
#define CBMLITPROPAGATIONQACALCULATOR_H_

#include "TObject.h"
#include "base/CbmLitTypes.h"
#include "base/CbmLitPtrTypes.h"
class TClonesArray;
class CbmLitTrack;
class CbmGlobalTrack;
class CbmLitMCTrackCreator;
class CbmLitHistManager;
class CbmLitMCPoint;
class CbmLitMCTrack;

/**
 * \class CbmLitPropagationQaCalculator
 * \brief Implementation of propagation QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
class CbmLitPropagationQaCalculator
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitPropagationQaCalculator(
         CbmLitHistManager* histManager);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPropagationQaCalculator();

   /**
    * \brief Initialization. Has to be called in FairTask::Init.
    */
   void Init();

   /**
    * \brief Execution. Has to be called in FairTask::Exec.
    */
   void Exec();

   /**
    * \brief Finish. Has to be called in FairTask::Finish.
    */
   void Finish();

   /* Setters */
   void SetMinNofHits(Int_t nofHits) { fMinNofHits = nofHits; }
   void SetTestFastPropagation(Bool_t isTestFastPropagation) { fIsTestFastPropagation = isTestFastPropagation; }
   void SetPDGCode(Int_t pdgCode) { fPDGCode = pdgCode; }
   void SetNofPlanes(Int_t nofPlanes) { fNofPlanes = nofPlanes; }

private:
   /**
    * \brief Read and create data branches.
    */
   void ReadDataBranches();

   /**
    * \brief Create track arrays.
    */
   void CreateTrackArrays();

   /**
    * \brief Convert CbmGlobalTrack to CbmLitTrack.
    * \param[in] globalTrack Pointer to the global track to be converted.
    * \param[out] litTrack Output CbmLitTrack.
    */
   void GlobalTrackToLitTrack(
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
      const CbmLitTrack* track);

   /**
    * \brief Test track fitter for a specified track.
    * \param[in,out] track Track to be fitted.
    * \param[in] mcTrack Corresponding MC track for comparison.
    */
   void TestFitter(
      CbmLitTrack* track);

   /**
    * \brief Fill pull/residual histograms for specified predicted track parameters.
    * \param[in] par Calculated track parameters.
    * \param[in] mcPar Monte-Carlo track parameters.
    * \param[in] plane Plane number.
    */
   void FillHistosPropagation(
      const CbmLitTrackParam* par,
      const CbmLitMCPoint* mcPar,
      Int_t plane);

   /**
    * \brief Fill pull/residual histograms for specified updated track parameters.
    * \param[in] par Calculated track parameters.
    * \param[in] mcPar Monte-Carlo track parameters.
    * \param[in] plane Plane number.
    * \param[in] chiSq Chi square value.
    */
   void FillHistosFilter(
      const CbmLitTrackParam* par,
      const CbmLitMCPoint* mcPar,
      Int_t plane,
      float chisq);

   /**
    *
    */
   void FillHistosFitter(
      const CbmLitTrack* track,
      const CbmLitMCTrack* mcTrack);

   /**
    *
    */
   std::vector<Double_t> CalcResidualsAndPulls(
      const CbmLitTrackParam* par,
      const CbmLitMCPoint* mcPoint);

   /**
    *
    */
   void TestFastPropagation(
      CbmLitTrack* track);

   /**
    *
    */
   Int_t GetMcTrackId(
         const CbmGlobalTrack* track);

   /**
    *
    */
   CbmLitMCPoint GetMcPointByHit(
         const CbmLitHit* hit,
         const CbmLitMCTrack* mcTrack);

   // Pointers to data arrays
   TClonesArray* fGlobalTracks; // CbmGlobalTrack array
   TClonesArray* fStsTracks; // CbmStsTrack array
   TClonesArray* fStsTrackMatches; // CbmStsTrackMatch array
   TClonesArray* fMuchTracks; // CbmMuchTracks array
   TClonesArray* fMuchPixelHits; // CbmMuchPixelHits array
   TClonesArray* fMuchStrawHits; // CbmMuchStrawHits array
   TClonesArray* fMuchTrackMatches; // CbmTrackMatch array
   TClonesArray* fTrdTracks; // CbmTrdTrack array
   TClonesArray* fTrdHits; // CbmTrdHit array
   TClonesArray* fTrdTrackMatches; // CbmTrdTrackMatch array
   TClonesArray* fTofHits; // CbmTofHit array

   TrackPtrVector fLitTracks; // Array of reconstructed global tracks converted to LitTracks.

   // Tools
   TrackPropagatorPtr fPropagator; // track propagation tool
   TrackUpdatePtr fFilter; // track update tool
   TrackFitterPtr fFitter; // track fitter tool
   TrackFitterPtr fSmoother; // track smoother tool
   TrackFitterPtr fParallelFitter; // parallel track fitter tool

   Bool_t fIsTestFastPropagation; // True if parallel track propagation and fit tested
   CbmLitMCTrackCreator* fMCTrackCreator; // MC creator tool
   Int_t fNofPlanes; // Number of planes in detector
   Int_t fPDGCode; // PDG code for track propagation and fit
   Int_t fMinNofHits; // Cut on number of hits in track
   CbmLitHistManager* fHM; // Histogram manager
};

#endif /* CBMLITPROPAGATIONQACALCULATOR_H_ */
