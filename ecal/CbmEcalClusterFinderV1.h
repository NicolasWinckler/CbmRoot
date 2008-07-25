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
class __CbmEcalPreCluster;

class CbmEcalClusterFinderV1: public CbmTask
{
public:
  /** Standard constructor **/
  CbmEcalClusterFinderV1(const char* name, const Int_t verbose);
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
  /** Clear a preclusters list **/
  void ClearPreClusters();
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

  /** A list of preclusters
   ** May be better use TClonesArray? **/
  std::list<__CbmEcalPreCluster*> fPreClusters;		//!

  ClassDef(CbmEcalClusterFinderV1, 1)
};

#endif

