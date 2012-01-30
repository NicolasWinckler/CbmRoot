/**
 * \file CbmLitFitQa.h
 * \brief Track fit QA for track reconstruction.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITFITQA_H_
#define CBMLITFITQA_H_

#include "FairTask.h"
#include <string>

class CbmLitMCTrackCreator;
class TH1F;
class FairTrackParam;
class CbmLitMCPoint;
class CbmLitHistManager;

using std::string;

/**
 * \class CbmLitFitQa
 * \brief Track fit QA for track reconstruction.
 *
 * Calculate residual and pull distributions for
 * first and last track parameters of MVD/STS, TRD and MUCH
 * for reconstructed tracks.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitFitQa : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitFitQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFitQa();

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
   void SetMvdMinNofHits(Int_t minNofHits) { fMvdMinNofHits = minNofHits; }
   void SetStsMinNofHits(Int_t minNofHits) { fStsMinNofHits = minNofHits; }
   void SetTrdMinNofHits(Int_t minNofHits) { fTrdMinNofHits = minNofHits; }
   void SetMuchMinNofHits(Int_t minNofHits) { fMuchMinNofHits = minNofHits; }
   void SetOutputDir(const string& dir) { fOutputDir = dir; }

private:

   /**
    * \brief Reads data branches.
    */
   void ReadDataBranches();

   void CreateHistograms();

   void ProcessGlobalTracks();

   void ProcessStsTrack(
         Int_t trackId);

   void ProcessTrdTrack(
         Int_t trackId);

   void ProcessMuchTrack(
         Int_t trackId);

   void FillResidualsAndPulls(
      const FairTrackParam* par,
      const CbmLitMCPoint* mcPoint,
      const string& histName,
      const string& wrongName,
      Float_t wrongPar);

   void DrawHistos(
         const string& name,
         const string& histName,
         const string& wrongName);

   void CreateSimulationReport(
         const string& title,
         const string& resultDirectory);

   // Data branches
   TClonesArray* fGlobalTracks; // CbmGlobalTrack array
   TClonesArray* fStsTracks; // CbmStsTrack array
   TClonesArray* fStsTrackMatches; // CbmTrackMatch array
   TClonesArray* fStsHits; // CbmStsHit
   TClonesArray* fMvdHits; // CbmMvdHit
   TClonesArray* fTrdTracks; // CbmTrdTrack array
   TClonesArray* fTrdTrackMatches; // CbmTrackMatch array
   TClonesArray* fTrdHits; // CbmTrdHit array
   TClonesArray* fMuchTracks; // CbmStsTrack array
   TClonesArray* fMuchTrackMatches; // CbmTrackMatch array
   TClonesArray* fMuchPixelHits; // CbmMuchPixelHit array
   TClonesArray* fMuchStripHits; // CbmMuchStripHit array

   // Histograms for residuals and pulls. Number of parameters.
   // histogram[parameter]
   // parameters:
   // [0-4] - residuals (x, y, tx, ty, q/p)
   // [5-9] - pulls (x, y, tx, ty, q/p)
   static const Int_t NOF_PARAMS = 10;

   // Histograms for number of incorrect diagonal elements
   // histogram[parameter]
   // [0-4] number of wrong diagonal elements in covariance matrix in dependence on number of hits
   static const Int_t NOF_PARAMS_WRONG_COV = 5;

   // MC track creator tool
   CbmLitMCTrackCreator* fMCTrackCreator;

   Bool_t fIsFixedBounds; // if true than fixed bounds are used for histograms

   Int_t fMvdMinNofHits; // Cut on minimum number of hits in track in MVD
   Int_t fStsMinNofHits; // Cut on minimum number of hits in track in STS
   Int_t fTrdMinNofHits; // Cut on minimum number of hits in track in TRD
   Int_t fMuchMinNofHits; // Cut on minimum number of hits in track in MUCH

   Double_t fQuota; // percent of correctly attached hits

   std::string fOutputDir; // Output directory for images

   CbmLitHistManager* fHM; // Histogram manager

   ClassDef(CbmLitFitQa, 1)
};

#endif /* CBMLITFITQA_H_ */
