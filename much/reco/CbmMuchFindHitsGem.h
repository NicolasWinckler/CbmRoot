/*
 * CbmMuchFindHitsGem.h
 *
 *  Created on: June 17, 2007
 *      Author: M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *
 *  Produces hits in GEM-like modules using different
 *  clustering algorithms. Clustering algorithms can be chosen
 *  by the switch SetAlgorithm(Int_t iAlgorithm)
 *  Several clustering algorithms are supported
 *  0 - Spatial separation (clusters are sets of neighbor fired pads)
 *  1 - Simple
 *  2 - Simple (threshold is set for each station separately)
 *  3 - Peaks algorithm
 *
 */

#ifndef CBMMUCHFINDHITSADVANCEDGEM_H
#define CBMMUCHFINDHITSADVANCEDGEM_H 1

#include "TStopwatch.h"
#include "TClonesArray.h"
#include "TString.h"
#include "CbmMuchGeoScheme.h"
#include "FairTask.h"

#include <map>
#include <vector>
#include <set>

using std::map;
using std::pair;
using std::vector;
using std::set;

class CbmMuchPad;
class CbmMuchCluster;

class CbmMuchFindHitsGem: public FairTask {

  public:

    /** Default constructor. */
    CbmMuchFindHitsGem();

    /**
     * Standard constructor.
     * @param iVerbose   Verbosity level.
     */
    CbmMuchFindHitsGem(Int_t iVerbose);

    /**
     * Constructor with task name.
     * @param name      Task name.
     * @param digiFile  Input digitization scheme file.
     * @param iVerbose  Verbosity level.
     */
    CbmMuchFindHitsGem(const char* name, const char* digiFileName,
        Int_t iVerbose);

    /** Destructor. */
    virtual ~CbmMuchFindHitsGem();

    /** Sets number of stations for the current setup. */
    void SetNStations(Int_t nStations);

    /** Sets ratio to multiply on maximal pad-charge in a cluster to obtain
     * charge threshold (appropriate only for simple clustering). */
    void SetThresholdRatio(Double_t thresholdRatio) {
      fThresholdRatio = thresholdRatio;
    }

    /** Sets ratios to multiply on maximal pad-charge in a cluster to obtain
     * charge threshold for each station (appropriate only for algorithm 6). */
    void SetThresholdRatios(Double_t* thresholdRatios);

    /**
     * Sets the clustering algorithm to use.
     * 0 - Spatial separation (clusters are sets of neighbor fired pads)
     * 1 - Simple
     * 2 - Simple (threshold is set for each station separately)
     * 3 - Peaks algorithm
     */
    void SetAlgorithm(Int_t iAlgorithm) {
      fAlgorithm = iAlgorithm;
    }

    void SetEpoch(Bool_t epoch) { fEpoch=epoch; }
    void SetClusterSeparationTime(Double_t time) { fClusterSeparationTime = time; }
    
    
    /** Execution. */
    virtual void Exec(Option_t* opt);

    //  void SetAnalysisStation(Int_t iStation) {fAnalysisStationIndex = iStation;}
    //  void SetAnalysisClusterSize(Int_t nPads) {fAnalysisClusterSize = nPads;}
    //  void SetAnalysisNPoints(Int_t nPoints) {fAnalysisNPoints = nPoints;}

  private:

    CbmMuchGeoScheme* fGeoScheme;                       // Geometry scheme
    TString fDigiFile;                                  // Digitization file
    Int_t fNStations;                                   // Number of stations
    TClonesArray* fHits;                                // Output array of CbmMuchHit
    TClonesArray* fDigis;                               // Input array of CbmMuchDigi
    TClonesArray* fDigiMatches;                         // Input array of CbmMuchDigiMatch
    TClonesArray* fClusters;                            // Output array of CbmMuchCluster objects
    TClonesArray* fPoints;                              // Input array of MC points
    TClonesArray* fMCTracks;                            // Input array of MC tracks
    map<pair<Int_t, Long64_t> , Int_t> fChannelDigiMap; // Correspondence between unique channel id and digi index
    set<Int_t> fSelectedDigis;                          // Digis already included in clusters
    Double_t fThresholdRatio;                           // Multiplied by maximal pad-charge gives the charge ratio (for simple clustering only)
    vector<Double_t> fThresholdRatios;                  // Charge thresholds for each station
    Int_t fAlgorithm;                                   // Defines which algorithm to use
    TStopwatch fTimer;                                  // Timer
    Bool_t fEpoch;                                      // Epoch
    Double_t fClusterSeparationTime;                    // Minimum required time between two clusters
    
    // Analysis variables
    vector<Double_t> fFakes; // Fakes fraction
    vector<Double_t> fLosts; // Losts fraction
    map<Int_t, set<Int_t> > fSignalMuonsAll; // Map from station index to the list of all signal muons per event
    map<Int_t, set<Int_t> > fSignalMuons; // Map from station index to the list signal muons formed one cluster per event
    map<Int_t, vector<Double_t> > fStationEff; // Map from station index to the list of station efficiencies
    Int_t fEvent;


    //  map<Int_t, CbmMuchCluster*> fAnalysisClusters;  // Clusters to analyze
    //  Int_t fAnalysisStationIndex; // Index of the station to analyze
    //  Int_t fAnalysisClusterSize; // Size of the clusters to analyze
    //  Int_t fAnalysisNPoints; // Number of points in the clusters to analyze
    //  Int_t fDebug;


    /** Get parameter containers. */
    virtual void SetParContainers();
    /** Initialization. */
    virtual InitStatus Init();
    /** Re-initialization. */
    virtual InitStatus ReInit();
    /** Finish task. */
    virtual void FinishTask();

    /** Groups neighbour digis into clusters (should be disentangled in the future)
     * and fills output fClusters array. */
    void FindClusters();

    // -------------------------------- Simple clustering --------------------------------------------------
    /** Creates correspondence between channel number and digi index.
     *  Fills fChannelDigiMap. */
    void FillChannelDigiMap();

    /**  **/
    void CreateCluster(Int_t iDigi, vector<Int_t> &digiIndices,
        UInt_t &sumCharge, UInt_t &qMax, UInt_t qThreshold = 0);

    void ExecClusteringSimple(CbmMuchCluster* cluster,
        vector<CbmMuchCluster*> &clusters);
    // -----------------------------------------------------------------------------------------------------

    void ExecClusteringPeaks(CbmMuchCluster* cluster,
        vector<CbmMuchCluster*> &clusters);
    /**
     * Creates hits from the given list of clusters.
     * @param clusters List of disentangled clusters.
     * @param iCluster Index of the parent cluster for clusters.
     */
    void CreateHits(vector<CbmMuchCluster*> clusters, Int_t iCluster);
    /**
     * Creates a hit from the given cluster.
     * @param cluster  Cluster to create a hit from.
     * @param iCluster Index of the cluster, which is parent for the supplied one.
     */
    void CreateHits(CbmMuchCluster* cluster, Int_t iCluster);
    /**
     * Gets CbmMuchPad object corresponding to the given digi index.
     * @param digiIndex Index of the digi.
     * @param charge    Charge collected by the pad (output parameter).
     */
    CbmMuchPad* GetPadByDigi(Int_t digiIndex, UInt_t &charge);

    void StatInfo();
    Bool_t IsCornerBorder(CbmMuchPad *pad, CbmMuchPad *neighbourPad);
    Bool_t CornersNotFired(CbmMuchPad *pad, CbmMuchPad *neighbourPad, Double_t deltaX, Double_t deltaY);

    CbmMuchFindHitsGem(const CbmMuchFindHitsGem&);
    CbmMuchFindHitsGem& operator=(const CbmMuchFindHitsGem&);

    ClassDef(CbmMuchFindHitsGem,1);
};

#endif
