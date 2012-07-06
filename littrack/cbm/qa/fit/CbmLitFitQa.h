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

class CbmHistManager;
class CbmLitFitQaCalculator;

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

   /* Setters */
   void SetMvdMinNofHits(Int_t minNofHits) { fMvdMinNofHits = minNofHits; }
   void SetStsMinNofHits(Int_t minNofHits) { fStsMinNofHits = minNofHits; }
   void SetTrdMinNofHits(Int_t minNofHits) { fTrdMinNofHits = minNofHits; }
   void SetMuchMinNofHits(Int_t minNofHits) { fMuchMinNofHits = minNofHits; }
   void SetOutputDir(const string& dir) { fOutputDir = dir; }

private:

   void CreateSimulationReport(
         const string& title,
         const string& resultDirectory);

   Bool_t fIsFixedBounds; // if true than fixed bounds are used for histograms

   Int_t fMvdMinNofHits; // Cut on minimum number of hits in track in MVD
   Int_t fStsMinNofHits; // Cut on minimum number of hits in track in STS
   Int_t fTrdMinNofHits; // Cut on minimum number of hits in track in TRD
   Int_t fMuchMinNofHits; // Cut on minimum number of hits in track in MUCH

   string fOutputDir; // Output directory for images

   CbmHistManager* fHM; // Histogram manager
   CbmLitFitQaCalculator* fFitQa; // Implementation of track fit QA

   ClassDef(CbmLitFitQa, 1)
};

#endif /* CBMLITFITQA_H_ */
