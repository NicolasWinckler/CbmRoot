/*
 * CbmMuchClusterAnalysis.h
 *
 *  Created on: Jul 2, 2009
 *      Author: M. Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *
 *  Performs some basic analysis of clusters
 */

#ifndef CBMMUCHCLUSTERANALYSIS_H_
#define CBMMUCHCLUSTERANALYSIS_H_ 1

#include "FairTask.h"

#include <vector>

using std::vector;

class CbmMuchGeoScheme;
class CbmMuchCluster;
class CbmMuchPad;
class TClonesArray;
class TH1F;

class CbmMuchClusterAnalysis: public FairTask {
  public:
    CbmMuchClusterAnalysis(const char* name, Int_t verbose);
    virtual ~CbmMuchClusterAnalysis();

    virtual InitStatus Init();
    virtual void Exec(Option_t * option);
    virtual void FinishTask();
    virtual void SetParContainers();

    /** Sets file name of the segmentation file. */
    void SetGeoFileName(TString fileName) { fGeoFileName = fileName; }


  private:
    Int_t fEvent; // Counter for events
    TClonesArray *fPoints; // Input MC points
    TClonesArray *fDigis; // Input digis
    TClonesArray *fDigiMatches; // Input digi matches
    TClonesArray *fClusters; // Input clusters
    TClonesArray *fHits; // Input hits
    TClonesArray *fMCTracks; // Input MC tracks
    TString fGeoFileName; // Input file with segmentation parameters
    CbmMuchGeoScheme *fGeoScheme; // Main object managing MUCH geometry


    // Histograms
    TH1F* fChargeTotal; // Charge on pads distribution for all clusters
    vector<TH1F*> fClusterSize; // Distributions of cluster sizes
    vector<vector<TH1F*> > fClusterPoints; // Distributions of number of points in the definite cluster sizes
    vector<vector<vector<TH1F*> > > fClusterPadsCharge; // Distributions of pad-charge for definite cluster sizes and number of contributed points
    vector<vector<vector<TH1F*> > > fClusterRadii; // Distributions of cluster radii for definite cluster sizes and number of contributed points
    vector<vector<vector<TH1F*> > > fClusterMeanEuclide; // Mean Euclidean distance between cluster pads

    /**
     * Mean Euclidean distance between pads in the cluster.
     */
    Double_t GetMeanDistance(CbmMuchCluster* cluster);
    /**
     * Euclidean distance between two given pads.
     */
    Double_t GetDistance(Int_t iDigi, Int_t jDigi);
    CbmMuchPad* GetPadByDigi(Int_t iDigi);

    ClassDef(CbmMuchClusterAnalysis,1)
};

#endif /* CBMMUCHCLUSTERANALYSIS_H_ */
