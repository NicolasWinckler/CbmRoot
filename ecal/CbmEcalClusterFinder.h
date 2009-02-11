// -------------------------------------------------------------------------
// -----                  CbmEcalClusterFinder header file             -----
// -----                 Created 12/07/06  by D.Konstantinov           -----
// -------------------------------------------------------------------------

/**  CbmEcalClusterFinder.h
 *@author D.Konstantinov 
 **
 ** Task for ECAL clusterization, which is a procedure to group the hits with
 ** the common edge
 **/

/* $Id: CbmEcalClusterFinder.h,v 1.3 2006/07/19 15:29:03 kharlov Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalClusterFinder.h,v $
 * Revision 1.3  2006/07/19 15:29:03  kharlov
 * Init geometry via constuctor, add cluster hit multiplicity, commenting
 *
 * Revision 1.2  2006/07/19 14:21:38  kharlov
 * Set W0 is included
 *
 * Revision 1.1  2006/07/12 14:22:56  kharlov
 * Adding ClusterFinder
 *
 */

#ifndef CBMECALCLUSTERFINDER_H
#define CBMECALCLUSTERFINDER_H


#include "CbmEcalInf.h"
#include "CbmEcalStructure.h"
#include "CbmEcal.h"
#include "CbmEcalHit.h"
#include "CbmEcalCluster.h"

#include "TClonesArray.h"

#include "CbmStack.h"
#include "FairTask.h"

class CbmEcalClusterFinder : public FairTask {


public:
 
  /** Default constructor **/
  CbmEcalClusterFinder();
  
  /** Standard constructor **/
  CbmEcalClusterFinder(const char *name, const Int_t iVerbose=1, 
		       const char *fileGeo="ecal_FullMC.geo");

  /** Destructor **/
  virtual ~CbmEcalClusterFinder();
  
  /** Initialization of the task **/  
  virtual InitStatus Init();

  /** Executed task **/ 
  virtual void Exec(Option_t* option);

  /** Finish task **/ 
  virtual void Finish();

  /** Add a new cluster to array of clusters **/
  void AddCluster(CbmEcalCluster * cl, Double32_t energy);

  /** Evaluate cluster parameters **/
  void Calculate(TClonesArray * hitsArray, CbmEcalCluster * cl);

  /** Set logarithmic weight threshold wzero **/
  void SetWzero(Float_t wzero) {fWzero = wzero;}

  /** Set hit threshold **/
  void SetHitThreshold(Float_t eMin) {fHitThreshold = eMin;}
private:

  /** ECAL structure **/
  CbmEcalStructure* fStr;

  /** ECAL geometry container **/
  CbmEcalInf* fInf;

  /** ECAL object **/
  CbmEcal* fEcal;

  /** Array of hits **/
  TClonesArray* fHitCollection;

  /** Array of clusters **/
  TClonesArray*fClusterCollection;

  /** Number of found clusters **/
  Int_t fNClusters;

  /** Internal event counter **/
  Int_t fEvent;

  /** logarithmic weight threshold **/
  Float_t fWzero;

  /** hit energy threshold **/
  Float_t fHitThreshold;

  /** Geo file to use **/
  TString fFileGeo;

  ClassDef(CbmEcalClusterFinder,1)

};

#endif
