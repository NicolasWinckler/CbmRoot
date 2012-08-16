/**
 * \file CbmMuchClusteringQaCalculator.h
 * \brief Performance calculator for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMMUCHCLUSTERINGQACALCULATOR_H_
#define CBMMUCHCLUSTERINGQACALCULATOR_H_

#include "FairTask.h"
#include "TClonesArray.h"

#include <vector>
#include <list>
#include <map>

class CbmMuchGeoScheme;
class CbmHistManager;
class TClonesArray;
class TH1;

/**
 * \class CbmLitClusteringQaCalculator
 * \brief Performance calculator for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmMuchClusteringQaCalculator
{
public:
   /**
    * \brief Constructor
    */
   CbmMuchClusteringQaCalculator(
         CbmHistManager* histManager);

   /**
    * \brief Destructor.
    */
   virtual ~CbmMuchClusteringQaCalculator();

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

   void CalculateClustersRelations();
   void CalculateAccuracy();
   void FillErrorsAndQualityByRadiusHistograms();
   //void FillQualityByRadiusHistograms();
   void FillEfficiencyByLayerHistograms();

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
   TClonesArray* fMuchHits;
   TClonesArray* fMuchDigis; // CbmMuchDigi array
   TClonesArray* fMuchDigiMatches;
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

   //------
   CbmMuchGeoScheme* fMuchGeoScheme;
   Int_t fNofClusters;
   Int_t fNofHits;
   Int_t fNofDigis;
   Int_t fNofPoints;
   Int_t fNofLayers;

   struct AccuracyStruct{
   		Float_t fErrorX;
   		Float_t fErrorY;
   		Float_t fErrorXY;
   		Int_t fHit;
   		Int_t fPoint;
   	};
   AccuracyStruct* fAccuracyArray;

   struct ClusterInformation{
   		//Int_t nofCluster;
   		Int_t fStation;
   		Int_t fLeyer;
   		Bool_t fSide;
   		Int_t fBestPoint;
   	   	Int_t fNofPoints;
   	   	std::vector<Int_t> fPointsInCluster;
   	   	std::vector<Int_t> fDigisByPoint;
   	   	std::vector<Float_t> fClusterToPointRelations;
   	   	std::vector<Float_t> fNDigisToClusterRelations;
   	};
   ClusterInformation* fClustersRelations;

   Int_t* fDigisByPoint;
};

#endif /* CBMMUCHCLUSTERINGQACALCULATOR_H_ */
