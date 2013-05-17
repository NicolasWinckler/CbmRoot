/**
 * \file CbmLitTofQa.h
 * \brief Task for TOF QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */

#ifndef CBMLITTOFQA_H_
#define CBMLITTOFQA_H_

#include "FairTask.h"
#include "CbmStsKFTrackFitter.h"
#include <string>

class CbmHistManager;
class CbmVertex;
class FairTrackParam;

using std::string;

/**
 * \class CbmLitTofQa
 * \brief  Task for TOF QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */
class CbmLitTofQa : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTofQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTofQa();

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

   /* Setters */
   void SetOutputDir(const string& dir) { fOutputDir = dir; }

   /**
    * \brief Set momentum range and number of bins for histograms.
    */
   void SetPRange(Int_t bins, Int_t min, Int_t max) {
      fPRangeBins = bins;
      fPRangeMin = min;
      fPRangeMax = max;
   }

private:
   /**
   * \brief Reads data branches.
   */
   void ReadDataBranches();

   /**
    * \brief Creates histograms.
    */
   void CreateHistograms();

   void ProcessGlobalTracks();

   Bool_t fIsFixedBounds; // if true than fixed bounds are used for histograms

   string fOutputDir; // Output directory for images

   Double_t fPRangeMin; // Min momentum
   Double_t fPRangeMax; // Max momentum
   Int_t fPRangeBins; // Number of bins

   CbmHistManager* fHM; // Histogram manager

   // Data branches
   TClonesArray* fGlobalTracks; // CbmGlobalTrack array
   TClonesArray* fStsTracks; // CbmStsTrack array
   TClonesArray* fStsTrackMatches; // CbmTrackMatch array
   TClonesArray* fTofHits; // CbmTofHit array
   TClonesArray* fTofPoints; // CbmTofPoint array
   TClonesArray* fMCTracks; // CbmMCTrack array

   CbmVertex* fPrimVertex; // Pointer to the primary vertex
   CbmStsKFTrackFitter fKFFitter; // Pointer to the Kalman Filter Fitter algorithm

   ClassDef(CbmLitTofQa, 1)
};

#endif /* CBMLITTOFQA_H_ */

