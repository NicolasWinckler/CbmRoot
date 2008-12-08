/** A more smart cluster finder than CbmEcalClusterFinder
 ** Produces a CbmEcalClusterV1. **/

#ifndef CBMECALCLUSTERFINDERV1_H
#define CBMECALCLUSTERFINDERV1_H

#include "CbmTask.h"
#include <list>

class TClonesArray;
class CbmEcalStructure;
class CbmEcalCell;
class CbmEcalInf;
class CbmEcalCalibration;
class __CbmEcalPreCluster;

class CbmEcalClusterFinderV1: public CbmTask
{
public:
  /** Standard constructor **/
  CbmEcalClusterFinderV1(const char* name, const Int_t verbose, const char* cfg);
  /** Only to comply with frame work. **/
  CbmEcalClusterFinderV1();

  /** Destructor **/
  virtual ~CbmEcalClusterFinderV1();

  /** Finish a task **/
  virtual void Finish();

  /** Exec a task **/
  virtual void Exec(Option_t* option);

  /** Initialization **/
  virtual InitStatus Init();
private:
  /** Form clusters from precluster **/
  void FormClusters();
  /** Form a preclusters **/
  void FormPreClusters();
  /** Form a preclusters in PHENIX-like style **/
  void FormPreClustersPhenix();
  /** Form a preclusters in ALICE-like style **/
  void FormPreClustersAlice();
  /** Clear a preclusters list **/
  void ClearPreClusters();
  /** Attach cells to a precluster **/
  void AttachCells(std::list<CbmEcalCell*>& precluster, CbmEcalCell* max);
  /** Exclude maximums belong to charged tracks **/
  void ExcludeMaximums();
  /** Locate clusters not belong to charged tracks **/
  void FindMaximums();
  /** Current event **/
  Int_t fEv;

  /** Array of maximums in calorimeter.
   ** Maximums belong to charged tracks excluded. **/
  CbmEcalCell** fMaximums;		//!
  /** Array of maximums belong to charged tracks excluded. **/
  CbmEcalCell** fExcluded;		//!
  /** An array of tracks **/
  TClonesArray* fTracks;		//!
  /** An array of clusters **/
  TClonesArray* fClusters;		//!
  /** A calorimeter structure **/
  CbmEcalStructure* fStr;		//!
  /** An information about calorimeter **/
  CbmEcalInf* fInf;			//!
  /** A calibration **/
  CbmEcalCalibration* fCal;		//!

  /** A list of preclusters
   ** May be better use TClonesArray? **/
  std::list<__CbmEcalPreCluster*> fPreClusters;		//!

  /** Maximums belong to charged tracks should excluded? **/
  Int_t fRemoveCharged;
  /** Minimum precluster energy **/
  Double_t fMinClusterE;
  /** An algorithm for preclustering: 0 --- default, 1 --- PHENIX like,
   ** 2 --- ALICE like, 3 --- default, but remove low energy cells **/
  Int_t fPreClusterAlgo;
  /** Minimum cell energy **/
  Double_t fMinCellE;
  /** Minimum size of precluster **/
  Int_t fMinSize;
  /** Attach to cluster nearby cells with Edep>fMinCellE **/
  Double_t fAttachCells;
  /** Minimum energy of precluster maximum for consideration **/
  Double_t fMinMaxE;

  ClassDef(CbmEcalClusterFinderV1, 1)
};

#endif

