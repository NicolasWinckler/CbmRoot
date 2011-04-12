/** A simple and modern version of CbmEcalClusterFinderV3
 ** Produces a CbmEcalClusterV1. **/

#ifndef CBMECALCLUSTERFINDERV3_H
#define CBMECALCLUSTERFINDERV3_H

#include "FairTask.h"
#include <list>

class TClonesArray;
class CbmEcalStructure;
class CbmEcalCell;
class CbmEcalInf;
class CbmEcalCalibration;
class CbmEcalPreCluster;
class CbmEcalMaximum;

class CbmEcalClusterFinderV3: public FairTask
{
public:
  /** Standard constructor **/
  CbmEcalClusterFinderV3(const char* name, const Int_t verbose, const char* cfg);
  /** Only to comply with frame work. **/
  CbmEcalClusterFinderV3();

  /** Destructor **/
  virtual ~CbmEcalClusterFinderV3();

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
  /** Current event **/
  Int_t fEv;

  /** Array of maximums in calorimeter.
   ** Maximums belong to charged tracks excluded. **/
  TClonesArray* fMaximums;		//!
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
  std::list<CbmEcalPreCluster*> fPreClusters;		//!

  /** Minimum precluster energy **/
  Double_t fMinClusterE;
  /** Minimum cell energy **/
  Double_t fMinCellE;
  /** Minimum energy of precluster maximum for consideration **/
  Double_t fMinMaxE;

  ClassDef(CbmEcalClusterFinderV3, 1)
};

#endif

