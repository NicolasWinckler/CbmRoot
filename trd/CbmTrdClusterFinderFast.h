// -------------------------------------------------------------------------
// -----          CbmTrdClusterFinderFast header file                  -----
// -----   Created 27/07/10  by Lorenz Quack and Cyrano Bergmann       -----
// -------------------------------------------------------------------------


#ifndef CBMTRDCLUSTERFINDERFAST_H
#define CBMTRDCLUSTERFINDERFAST_H 

#include "FairTask.h"
#include "CbmTrdDetectorId.h"

#include <vector>
#include <list>
#include <map>

class CbmTrdDigiPar;
class CbmTrdModule;
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
    {
      hasBeenVisited = false;
      digis = new MyDigiList;
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
};


class CbmTrdClusterFinderFast : public FairTask
{

 public:

  /**
   * Default constructor.
   */
  CbmTrdClusterFinderFast();
  
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

  /** Virtual method EndOfEvent
   **
   ** Reset cluster collection at the
   ** end of the event.
   **/
  virtual void EndOfEvent();

  /** Finish task **/
  virtual void Finish();
  void Register();

 private:
  ClusterList *clusterModule(Bool_t rowClusterMerger, MyDigiList *digis, MyDigiList *neighbours);  
  void mergeRowCluster(RowCluster *currentCluster,
		       std::list<RowCluster*> *openList);
  ClusterList *findCluster(std::list<RowCluster*> *rowClusterList);
  void walkCluster(std::list<RowCluster*> *rowClusterList, 
		   RowCluster *currentRowCluster,
		   MyDigiList *cluster);
  void drawCluster(Int_t moduleId, ClusterList *clusterList);

  void addCluster(std::map<Int_t, ClusterList*> fModClusterMap);

  TClonesArray*     fDigis;       /** Input array of CbmTrdDigi **/
  TClonesArray*     fClusters;    /** Output array of CbmTrdCluster **/

  CbmTrdDigiPar *fDigiPar;   //!
  CbmTrdModule  *fModuleInfo; //!
  
  CbmTrdDetectorId fTrdId; //!

  //static const Float_t minimumChargeTH = 5e-03;

  Int_t ClusterSum;

  ClassDef(CbmTrdClusterFinderFast,1);
};
#endif
