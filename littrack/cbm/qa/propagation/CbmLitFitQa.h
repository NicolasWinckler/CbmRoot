/**
 * \file CbmLitFitQa.h
 *
 * \brief Track fit QA for track reconstruction.
 *
 * Calculates residual and pull distributions for
 * first and last track parameters of MVD/STS, TRD and MUCH
 * for reconstructed tracks.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITFITQA_H_
#define CBMLITFITQA_H_

#include "FairTask.h"

class CbmLitMCTrackCreator;
class TH1F;
class FairTrackParam;
class CbmLitMCPoint;

/**
 * \class CbmLitFitQa
 *
 * \brief Track fit QA for track reconstruction.
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
      std::vector<TH1F*>& histos);

   void DrawHistos(
         const std::string& name,
         std::vector<TH1F*>& histos);

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

   // Histograms
   // histogram[parameter]
   // parameters:
   // [0-4] - residuals (x, y, tx, ty, q/p)
   // [5-9] - pulls (x, y, tx, ty, q/p)
   static const Int_t NOF_PARAMS = 10;
   std::vector<TH1F*> fStsHistosFirst; // STS first track parameter
   std::vector<TH1F*> fStsHistosLast; // STS last track parameter
   std::vector<TH1F*> fTrdHistosFirst; // TRD first track parameter
   std::vector<TH1F*> fTrdHistosLast; // TRD last track parameter
   std::vector<TH1F*> fMuchHistosFirst; // MUCH first track parameter
   std::vector<TH1F*> fMuchHistosLast; // MUCH last track parameter

   // MC track creator tool
   CbmLitMCTrackCreator* fMCTrackCreator;

   Bool_t fIsFixedBounds;

   ClassDef(CbmLitFitQa, 1)
};

#endif /* CBMLITFITQA_H_ */
