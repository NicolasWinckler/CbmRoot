/**
 * \file CbmLitClusteringQaCalculator.h
 * \brief Performance calculator for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITCLUSTERINGQACALCULATOR_H_
#define CBMLITCLUSTERINGQACALCULATOR_H_

class CbmHistManager;
class TClonesArray;
class TH1;

/**
 * \class CbmLitClusteringQaCalculator
 * \brief Performance calculator for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitClusteringQaCalculator
{
public:
   /**
    * \brief Constructor
    */
   CbmLitClusteringQaCalculator(
         CbmHistManager* histManager);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitClusteringQaCalculator();

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

private:

   /**
    * \brief Read data branches.
    */
   void ReadDataBranches();

   /**
    * \brief Calculate statistics for hits.
    */
   void ProcessHits();

   /**
    *
    */
   void IncreaseCounters();

   // Pointers to data arrays
   TClonesArray* fMvdPoints; // CbmMvdPoint array
   TClonesArray* fMvdDigis; // CbmMvdDigi array
   TClonesArray* fMvdClusters; // CbmMvdClusters array
   TClonesArray* fMvdHits; // CbmMvdHit array

   TClonesArray* fStsPoints; // CbmStsPoint array
   TClonesArray* fStsDigis; // CbmStsDigi array
   TClonesArray* fStsClusters; // CbmStsCluster array
   TClonesArray* fStsHits; // CbmStsHit array

   TClonesArray* fRichPoints; // CbmRichPoint array
   TClonesArray* fRichHits; // CbmRichHits array

   TClonesArray* fMuchPoints; // CbmMuchPoint array
   TClonesArray* fMuchDigis; // CbmMuchDigi array
   TClonesArray* fMuchClusters; // CbmMuchCluster array
   TClonesArray* fMuchPixelHits; // CbmMuchPixelHits array
   TClonesArray* fMuchStrawHits; // CbmMuchStrawHits array

   TClonesArray* fTrdPoints; // CbmTrdPoint array
   TClonesArray* fTrdDigis; // CbmTrdDigi array
   TClonesArray* fTrdClusters; // CbmTrdCluster array
   TClonesArray* fTrdHits; // CbmTrdHit array

   TClonesArray* fTofPoints; // CbmTofPoint array
   TClonesArray* fTofHits; // CbmTofHit array

   CbmHistManager* fHM; // Pointer to histogram manager
};

#endif /* CBMLITCLUSTERINGQACALCULATOR_H_ */
