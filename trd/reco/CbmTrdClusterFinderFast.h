// -------------------------------------------------------------------------
// -----          CbmTrdClusterFinderFast header file                  -----
// -----   Created 27/07/10  by Lorenz Quack and Cyrano Bergmann       -----
// -------------------------------------------------------------------------


#ifndef CBMTRDCLUSTERFINDERFAST_H
#define CBMTRDCLUSTERFINDERFAST_H 

#include "FairTask.h"

#include <vector>
#include <list>
#include <map>

class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdGeoHandler;
class TClonesArray;

typedef struct MyDigi
{
  Int_t digiId;
  Int_t rowId;
  Int_t colId;
  Int_t combiId;
  Float_t charge;
} MyDigi;

typedef std::list<MyDigi*> MyDigiList;
typedef std::vector<MyDigiList*> ClusterList;

class RowCluster
{
 public:
  RowCluster()
    : hasBeenVisited(false),
    minCol(-1),
    maxCol(-1),
    row(-1),
    digis(new MyDigiList),
    parents(),
    children()
    {
    }
  ~RowCluster()
    {
      if (digis) {
	delete digis;
	digis = NULL;
      }
    }
  Bool_t hasBeenVisited;
  Int_t minCol;
  Int_t maxCol;
  Int_t row;
  MyDigiList *digis;
  std::list<RowCluster*> parents;
  std::list<RowCluster*> children;
 private:
  RowCluster(const RowCluster&);
  RowCluster& operator=(const RowCluster&);
};


class CbmTrdClusterFinderFast : public FairTask
{

 public:

  /**
   * Default constructor.
   */
  CbmTrdClusterFinderFast();
  CbmTrdClusterFinderFast(Bool_t MultiHit, Bool_t NeighbourReadout, Bool_t RowClusterMerger, Double_t MinimumChargeTH);
  /**
   * Default destructor.
   */
  virtual ~CbmTrdClusterFinderFast();
 
  /** Initialisation **/
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();
  
  /** Executed task **/
  virtual void Exec(Option_t * option);

  /** Virtual method FinishEvent
   **
   ** Reset cluster collection at the
   ** end of the event.
   **/
  virtual void FinishEvent();

  /** Finish task **/
  virtual void Finish();
  void Register();

 private:
  //static const Bool_t fMultiHit = true;

  ClusterList *clusterModule(MyDigiList *digis, MyDigiList *neighbours);  
  void mergeRowCluster(RowCluster *currentCluster,
		       std::list<RowCluster*> *openList);
  ClusterList *findCluster(std::list<RowCluster*> *rowClusterList);
  void walkCluster(std::list<RowCluster*> *rowClusterList, 
		   RowCluster *currentRowCluster,
		   MyDigiList *cluster);
  void drawCluster(Int_t moduleId, ClusterList *clusterList);

  void addNeighbourDigis(Int_t nRow, ClusterList *clusters, MyDigiList *neighbours);

  void addCluster(std::map<Int_t, ClusterList*> fModClusterMap);

  TClonesArray*     fDigis;       /** Input array of CbmTrdDigi **/
  TClonesArray*     fClusters;    /** Output array of CbmTrdCluster **/

  CbmTrdDigiPar *fDigiPar;   //!
  CbmTrdModule  *fModuleInfo; //!
  
  CbmTrdGeoHandler* fGeoHandler; //!

  //static const Float_t minimumChargeTH = 5e-03;

  Int_t ClusterSum;
  Double_t fMinimumChargeTH;
  Bool_t fNeighbourReadout;
  Bool_t fRowClusterMerger;
  Bool_t fMultiHit;

  CbmTrdClusterFinderFast(const CbmTrdClusterFinderFast&);
  CbmTrdClusterFinderFast& operator=(const CbmTrdClusterFinderFast&);

  ClassDef(CbmTrdClusterFinderFast,2);
};
#endif
