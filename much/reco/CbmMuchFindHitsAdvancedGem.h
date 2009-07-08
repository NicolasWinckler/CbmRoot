/*
 * CbmMuchFindHitsAdvancedGem.h
 *
 *  Created on: Jun 17, 2007
 *      Author: M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *
 *  Produces hits in GEM-like modules using different
 *  clustering algorithms. Clustering algorithms can be chosen
 *  by the switch SetAlgorithm(Int_t iAlgorithm)
 *  Several clustering algorithms are supported
 *  0 - Spatial separation (clusters are sets of neighbor fired pads)
 *  1 - Simple
 *  2 - Ward (fuzzy clustering)
 *  3 - Simple + Ward
 *  4 - Divisive clustering
 *  5 - Simple + divisive clustering
 *
 */


#ifndef CBMMUCHFINDHITSADVANCEDGEM_H
#define CBMMUCHFINDHITSADVANCEDGEM_H 1


#include "TStopwatch.h"
#include "TClonesArray.h"
#include "TFile.h"
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

class CbmMuchFindHitsAdvancedGem : public FairTask
{

 public :

  /** Default constructor. */
  CbmMuchFindHitsAdvancedGem();

  /**
   * Standard constructor.
   * @param iVerbose   Verbosity level.
   */
  CbmMuchFindHitsAdvancedGem(Int_t iVerbose);

  /**
   * Constructor with task name.
   * @param name      Task name.
   * @param digiFile  Input digitization scheme file.
   * @param iVerbose  Verbosity level.
   */
  CbmMuchFindHitsAdvancedGem(const char* name, const char* digiFileName, Int_t iVerbose);

  /** Destructor. */
  virtual ~CbmMuchFindHitsAdvancedGem();

  /**
   * Sets the clustering algorithm to use.
   * 1 - Simple
   * 2 - Ward's fuzzy clustering
   * 3 - Simple + Ward's clustering
   * 4 - Divisive clustering
   * 5 - Simple + divisive clustering
   */
  void SetAlgorithm(Int_t iAlgorithm) {fAlgorithm = iAlgorithm;}

  /** Sets distance limit (value where recursive clustering stops). */
  void SetDistanceLimit(Double_t essLimit) {fDistanceLimit = essLimit;}

  /** Execution. */
  virtual void Exec(Option_t* opt);

//  void SetClusterCharges(Int_t stationNr, Int_t clusterSize, Int_t* padCharges);
//  void SetClusterDistanceLimits(Int_t stationNr, Int_t clusterSize, Double_t* distanceLimits);

//  void SetAnalysisStation(Int_t iStation) {fAnalysisStationIndex = iStation;}
//  void SetAnalysisClusterSize(Int_t nPads) {fAnalysisClusterSize = nPads;}
//  void SetAnalysisNPoints(Int_t nPoints) {fAnalysisNPoints = nPoints;}

 private:

  CbmMuchGeoScheme*                fGeoScheme;         // Geometry scheme
  TFile*                           fDigiFile;          // Digitization file
  TClonesArray*                    fHits;              // Output array of CbmMuchHit
  TClonesArray*                    fDigis;             // Input array of CbmMuchDigi
  TClonesArray*                    fDigiMatches;       // Input array of CbmMuchDigiMatch
  TClonesArray*                    fClusters;          // Output array of CbmMuchCluster objects
  map<pair<Int_t, Int_t>, Int_t>   fChannelDigiMap;    // Correspondence between unique channel id and digi index
  set<Int_t>                       fSelectedDigis;     // Digis already included in clusters
  Int_t                            fAlgorithm;         // Defines which algorithm to use
  TStopwatch                       fTimer;             // Timer
  Double_t                         fDistanceLimit;     // Limit for the ESS increment
  Int_t                            fChargeLimit;       // Limit of charge
//  map<Int_t, map<Int_t, vector<Double_t> > > fDistanceLimits; // Map from station index to the list of distance limits for each cluster size
//  map<Int_t, map<Int_t, vector<Int_t> > > fChargeLimits;     // Map from station index to the list of charge limits for each cluster size

//  // Analysis variables
//  map<Int_t, CbmMuchCluster*> fAnalysisClusters;  // Clusters to analyze
//  Int_t fAnalysisStationIndex; // Index of the station to analyze
//  Int_t fAnalysisClusterSize; // Size of the clusters to analyze
//  Int_t fAnalysisNPoints; // Number of points in the clusters to analyze
//  vector<Double_t> fFakes; // Fakes fraction
//  vector<Double_t> fLosts; // Losts fraction
//  Int_t fDebug;
//  Int_t fEvent;


  /** Get parameter containers. */
  virtual void SetParContainers();
  /** Initialization. */
  virtual InitStatus Init();
  /** Re-initialization. */
  virtual InitStatus ReInit();

  /** Groups neighbour digis into clusters (should be disentangled in the future)
   * and fills output fClusters array. */
  void FindClusters();


  // -------------------------------- Simple clustering --------------------------------------------------
  /** Creates correspondence between channel number and digi index.
   *  Fills fChannelDigiMap. */
  void FillChannelDigiMap();

  /**  **/
  void CreateCluster(Int_t iDigi, vector<Int_t> &digiIndices, Int_t &sumCharge, Int_t &qMax, Int_t qThreshold = 0);

  void ExecClusteringSimple(CbmMuchCluster* cluster, vector<CbmMuchCluster*> &clusters);
  // -----------------------------------------------------------------------------------------------------

  // -------------------------------- Ward's clustering --------------------------------------------------
  /**
   * Execution of fuzzy Ward's clustering.
   * @param cluster   Input cluster to disentangle.
   * @param clusters  Output list of disentangled clusters.
   */
  void ExecClusteringWard(CbmMuchCluster* cluster, vector<CbmMuchCluster*> &clusters);
  /**
   * Produces clusters according to the fuzzy Ward's clustering algorithm.
   * @param digiGroups  List of grouped digi indices.
   * @param essVector   List of ESS for each group (last element is the total ESS).
   */
  void GroupDigisWard(vector<vector<Int_t> > &digiGroups, vector<Double_t> &essVector);
  /**
   * Gets Error Sum of Squares for two given clusters if they are merged together.
   * @parameter digiGroup1  The first set of digi indices.
   * @parameter digiGroup2  The second set of digi indices.
   * @return                ESS for merged clusters.
   */
  Double_t GetESS(vector<Int_t> digiGroup1, vector<Int_t> digiGroup2);
  /**
   * Gets Error Sum of Squares for the given cluster.
   * @parameter digiGroup  The set of digi indices.
   * @return                ESS for the cluster.
   */
  Double_t GetESS(vector<Int_t> digiGroup);
  // -----------------------------------------------------------------------------------------------------

  // -------------------------------- Divisive clustering ------------------------------------------------
  /**
   * Execution of divisive clustering.
   * @param cluster   Input cluster to disentangle.
   * @param clusters  Output list of disentangled clusters.
   */
  void ExecClusteringDivisive(CbmMuchCluster* cluster, vector<CbmMuchCluster*> &clusters);
  /**
   * Produces list of groups of similar digis according to divisive algorithm.
   * @param digiGroups  List of grouped digi indices.
   * @param iGroup      Index of the group (0 on the first stage,
   *                    further it changes recursively).
   */
  void GroupDigisDivisive(vector<vector<Int_t> > &digiGroups, Int_t iGroup);
  /**
   * Whether the given group of digis should be further divided.
   * @param digiGroup  Group of digis to divide (if needed).
   */
  Bool_t ShouldDivideDivisive(vector<Int_t> digiGroup);
  /**
   * Moves digis from one group to another.
   * @param digiGroups List of groups of similar digis.
   * @param iGroup     Index of the initial group to move digis from.
   * @param jGroup     Index of the final group to move digis to.
   */
  void MoveDigisDivisive(vector<vector<Int_t> > &digiGroups, Int_t iGroup, Int_t jGroup);
  // -----------------------------------------------------------------------------------------------------


  /**
   * Gets mean Euclidean distance between the given digi
   * and digis in the supplied group.
   * @param digiGroup Group of digis.
   * @param iDigi     Index of the digi to calculate mean distance from.
   */
  Double_t GetMeanDistance(vector<Int_t> digiGroup, Int_t iDigi);
  /**
   * Gets mean Euclidean distance between the given digi
   * and digis in the supplied group.
   * @param digiGroup Group of digis.
   */
  Double_t GetMeanDistance(CbmMuchCluster* cluster);
  Double_t GetMeanDistance(vector<Int_t> digiGroup);
  /**
   * Gets the Euclidean distance between the two digis.
   * @param iDigi Index of the first digi.
   * @param jDigi Index of the second digi.
   */
  Double_t GetDistance(Int_t iDigi, Int_t jDigi);
  /**
   * Creates list of CbmMuchCluster objects from the given
   * list of grouped digis.
   * @param digiGroups List of grouped digis (their indices).
   */
  vector<CbmMuchCluster*> GetDigiGroupsClusters(vector<vector<Int_t> > digiGroups);
  /**
   * Creates hits from the given list of clusters.
   * @param clusters List of disentangled clusters.
   * @param iCluster Index of the parent cluster for clusters.
   */
  void CreateHits(vector<CbmMuchCluster*> clusters, Int_t iCluster);
  void CreateHits(CbmMuchCluster* cluster, Int_t iCluster);
  /**
   * Gets CbmMuchPad object corresponding to the given digi index.
   * @param digiIndex Index of the digi.
   * @param charge    Charge collected by the pad (output parameter).
   */
  CbmMuchPad* GetPadByDigi(Int_t digiIndex, Int_t &charge);

  void StatInfo();

  ClassDef(CbmMuchFindHitsAdvancedGem,1);
};

#endif
