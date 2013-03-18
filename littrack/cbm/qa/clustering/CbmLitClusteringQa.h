/**
 * \file CbmLitClusteringQa.h
 * \brief FairTask for clustering performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITCLUSTERINGQA_H_
#define CBMLITCLUSTERINGQA_H_

#include "FairTask.h"
#include "../cbm/base/CbmLitDetectorSetup.h"
#include "CbmMuchGeoScheme.h"
#include <string>
#include <vector>
using std::string;
using std::vector;
using std::map;
class CbmHistManager;
class TClonesArray;

class CbmLitClusteringQa : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitClusteringQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitClusteringQa();

   /**
     * \brief Derived from FairTask.
     */
    virtual InitStatus Init();

    /**
     * \brief Derived from FairTask.
     */
    virtual void Exec(
       Option_t* opt);

    /**
     * \brief Derived from FairTask.
     */
    virtual void Finish();

    /** Setters */
    void SetOutputDir(const string& outputDir) { fOutputDir = outputDir; }

    void SetMuchDigiFile(const string& digiFileName) { fMuchDigiFile = digiFileName; }

    void MuchGeoSchemeInit(const TString& digiFileName);

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

    /**
     *
     */
    void CreateHistograms();

    /**
     *
     */
    void CreateH1F(
          const string& name,
          const string& xTitle,
          const string& yTitle,
          Int_t nofBins,
          Double_t minBin,
          Double_t maxBin);

    void CreateP1(
          const string& name,
          const string& xTitle,
          const string& yTitle,
          Int_t nofBins,
          Double_t minBin,
          Double_t maxBin);

    void CreateH2F(
          const string& name,
          const string& xTitle,
          const string& yTitle,
          const string& zTitle,
          Int_t nofBinsX,
          Double_t minBinX,
          Double_t maxBinX,
          Int_t nofBinsY,
          Double_t minBinY,
          Double_t maxBinY);

    void CreateP2(
          const string& name,
          const string& xTitle,
          const string& yTitle,
          const string& zTitle,
          Int_t nofBinsX,
          Double_t minBinX,
          Double_t maxBinX,
          Int_t nofBinsY,
          Double_t minBinY,
          Double_t maxBinY);
    /**
     *
     */
    void CreateNofObjectsHistograms(
          DetectorId detId,
          const string& detName);

    /**
     *
     */
    void CreateNofObjectsHistograms(
          DetectorId detId,
          const string& detName,
          const string& parameter,
          const string& xTitle);

    //void SetMuchDigiArray();
    void SetMuchClustersArray();
    //void CalculateMuchAcuracy();

    void CreateMuchHistograms();
    void CreateSingleLayerHistogram(Int_t nStation, Int_t nLayer,
    		const string& histName, const string& histNameX, const string& histNameY, Int_t nofBins, Double_t minX, Double_t maxX);

    void FillMuchHistograms();
    void FillMuchClusterQualityHistogram();
    void FillMuchClusterQuality2DHistogram();
    void FillMuchMCPointsInClusterHistogrm();
    void FillMuchMCPointsInCluster2DHistogrm();
    void FillMuchMCPointsByDigiHistogram();
    void FillMuchMCPointsByDigi2DHistogram();

    void FillMuchResidualHistograms();

    void FillMuchAccuracyHistograms();
    //void FillMuchRMSHistogram();
    //void FillMuchMeanHistogram();
    //void FillMuchSigmaHistogram();

    void FillMuchPixelHitsHistogram();
    void FillMuchPointsHistogram();
    void FillMuchClustersHistogram();
    void FillMuchDigisHistogram();

    void FillDigiByPointHistogrm();

    void FillDigiByPointDistributionHistogrm();
    void FillPointsInDigiDistributionHistogrm();
    void FillDigisInClusterDistributionHistogrm();

    Float_t CalculateMuchClusterToPointRatio(Int_t nCluster, Int_t nPoint);
    Float_t CalculateMuchDigisToClusterRatio(Int_t nCluster, Int_t nPoint);
    Int_t GetNofPointsInCluster(Int_t nCluster);


    CbmHistManager* fHM; // Histogram manager
    string fOutputDir; // Output directory for results
    CbmLitDetectorSetup fDet; // For detector setup determination

    // Pointers to data arrays
    TClonesArray* fMCTracks;

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
    TClonesArray* fMuchDigiMatches;
    TClonesArray* fMuchClusters; // CbmMuchCluster array
    TClonesArray* fMuchPixelHits; // CbmMuchPixelHits array
    TClonesArray* fMuchStrawHits; // CbmMuchStrawHits array
    TClonesArray* fMuchTrackMatches;

    TClonesArray* fTrdPoints; // CbmTrdPoint array
    TClonesArray* fTrdDigis; // CbmTrdDigi array
    TClonesArray* fTrdClusters; // CbmTrdCluster array
    TClonesArray* fTrdHits; // CbmTrdHit array

    TClonesArray* fTofPoints; // CbmTofPoint array
    TClonesArray* fTofHits; // CbmTofHit array



    CbmMuchGeoScheme* fMuchGeoScheme;
    TString fMuchDigiFile;
    Int_t fNofMuchClusters;
    Int_t fNofMuchHits;
    Int_t fNofMuchDigis;
    Int_t fNofMuchPoints;
    Int_t fNofMuchLayers;

    //Int_t* fMuchDigisByPoint;

    /*struct AccuracyStruct{
    	Float_t fErrorX;
    	Float_t fErrorY;
    	Float_t fErrorXY;
    	Int_t fHit;
    	Int_t fPoint;
    };
    AccuracyStruct* fAccuracyArray;*/

    std::map<Int_t, Int_t> fBestPoints;
    std::map<Int_t, Int_t> fBestPointsForHits;

    /*struct ClusterInformation{
    	//Int_t nofCluster;
    	Int_t fDetId;
   		Int_t fBestPoint;
   	   	Int_t fNofDigis;
   	   	std::vector<Int_t> fPointsInCluster;
   	   	std::vector<Int_t> fDigisByPoint;
   	   	std::vector<Float_t> fClusterToPointRatio;
   	   	std::vector<Float_t> fNDigisToClusterRatio;
    };
    ClusterInformation* fClustersArray;*/

    /*struct ActiveDigi{
	   Int_t fDigi;
	   Int_t fDetId;
	   std::vector<Int_t> fPoints;
    };
    ActiveDigi* fDigiArray;*/

    ClassDef(CbmLitClusteringQa, 1);
};

#endif /* CBMLITCLUSTERINGQA_H_ */
