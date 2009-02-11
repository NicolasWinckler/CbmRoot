/** CbmMuchFindHits.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 22.03.07
 *@version 1.0
 **
 ** CBM task class for finding hits in the MUCH
 ** Task level RECO
 ** Produces objects of type CbmMuchHits out of CbmMuchDigi.
 **/


#ifndef CBMMUCHFINDHITS_H
#define CBMMUCHFINDHITS_H 1


#include "FairTask.h"

#include "TStopwatch.h"

#include <map>
#include <set>
#include <vector>


class TClonesArray;
class CbmGeoMuchPar;
class CbmMuchDigiPar;
class CbmMuchDigiScheme;
class CbmMuchSector;
class CbmMuchStation;
class CbmMuchPad;
class CbmMuchCluster;

using std::map;
using std::pair;
using std::set;
using std::vector;


class CbmMuchFindHits : public FairTask
{


 public :

  /** Default constructor **/
  CbmMuchFindHits();

  /** Standard constructor **/
  CbmMuchFindHits(Int_t iVerbose);

  /** Constructor with task name **/
  CbmMuchFindHits(const char* name, Int_t iVerbose);

  /** Destructor **/
  virtual ~CbmMuchFindHits();

  /** Execution **/
  virtual void Exec(Option_t* opt);

  inline void SetUseClustering(Int_t useClustering) {fUseClustering = useClustering;}


 private:
  Int_t                            fUseClustering;     // Switch showing whether to use clustering algorithm
  CbmGeoMuchPar*                   fGeoPar;            // Geometry parameters
  CbmMuchDigiPar*                  fDigiPar;           // Digitization parameters
  CbmMuchDigiScheme*               fDigiScheme;        // Digitization scheme
  TClonesArray*                    fDigis;             // Input array of CbmMuchDigi
  TClonesArray*                    fDigiMatches;       // Input array of CbmMuchDigiMatch
  TClonesArray*                    fHits;              // Output array of CbmMuchHit
  TClonesArray*                    fPrimaryClusters;   // Output array of primary CbmMuchCluster objects
  map<pair<Int_t, Int_t>, Int_t>   fChannelDigiMap;    // Map from channel id to index of a digi
  map<CbmMuchSector*, set<Int_t> > fDigiMap;           // sector digis
  TStopwatch fTimer;                                   // Timer
  set<Int_t> fSelectedDigis;


  
  /** Execution which uses simple algorithm for creating hits,
   * one hit for one fired pad.
   **/
  void ExecSimple(Bool_t &warn);

  /** Execution which uses clustering algorithm for creating hits. **/
  void ExecClusters();

  /** Hit producer in straw tubes. **/
  void ExecStraws(Int_t begStation);

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Intialisation **/
  virtual InitStatus Init();

  /** Reinitialisation **/
  virtual InitStatus ReInit();

  /** Make sectorwise sets for digis  **/
  void MakeSets();

  /** Sort digis sectorwise  **/
  void SortDigis();

  /** Gets CbmMuchPad object corresponding to given digi index 
   *@param digiIndex     Index of a digi
   *@param charge        Charge from the digi
   **/
  CbmMuchPad* GetPadByDigi(Int_t digiIndex, Int_t &charge);

 /** Fills corresponding map with data. **/
  void FillChannelDigiMap();

  /** Match digis to the parent cluster **/
  void SetDigiClusterMatch(vector<Int_t> digiIndices, Int_t clusterIndex);

  /** Find clusters of fired pads in the MUCH system**/
  void FindClusters();

  /** Creates a cluster 
   *@param iDigi             Index of a digi which is contained in the cluster
   *@param digiIndices       Indices of digis which form the cluster
   *@param sumCharge         Summary charge collected by the cluster
   *@param qMax              Maximal charge collected by a pad in the cluster
   *@param qThreshold        Charge threshold to apply while searching a cluster
   **/
  void CreateCluster(Int_t iDigi, vector<Int_t> &digiIndices, Int_t &sumCharge, Int_t &qMax, Int_t qThreshold = 0);

  /** Applies a threshold to a cluster in order to divide it on several clusters
   * if it contains multiple tracks
   *@param cluster     CbmMuchCluster to divide
   *@param newClusters A list of clusters on which the former one was divided
   **/
  void ApplyThreshold(CbmMuchCluster* cluster, std::vector<CbmMuchCluster*> &newClusters);

  /** Find hits in one sector **/
  Int_t FindHits(CbmMuchStation* station, CbmMuchSector* sector,
		 std::set<Int_t>& digiSet);



  ClassDef(CbmMuchFindHits,1);
};

#endif
