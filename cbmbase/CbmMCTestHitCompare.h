// -------------------------------------------------------------------------
// -----                 PNDMCMATCHSELECTORTASK header file             -----
// -----                  Created 18/01/10  by T.Stockmanns             -----
// -------------------------------------------------------------------------


/** PNDMCMATCHSELECTORTASK.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef CBMMCTESTHITCOMPARE_H
#define CBMMCTESTHITCOMPARE_H


// framework includes
#include "FairTask.h"
#include "CbmMCMatch.h"
#include "CbmDetectorList.h"


#include <vector>
#include <map>


#include "CbmDetectorList.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TList.h"
#include "TVector3.h"


class TClonesArray;

class CbmMCTestHitCompare : public FairTask
{
 public:

  /** Default constructor **/
	CbmMCTestHitCompare();

  /** Destructor **/
  virtual ~CbmMCTestHitCompare();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Create histograms **/
  void CreateHistos();

  /** Reset histograms and counters **/
  void Reset();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();



 private:
  CbmMCMatch* fMCMatch;
  TClonesArray* fStripHit;
  TClonesArray* fMCPoint;
  TH2F* fhHitPointCorrelation;
  TH2F* fhHitPointSCorrelationSmall;
  TH2F* fhHitPointSCorrelationLarge;
  TList* fHistoList;
  void Register();

  void ProduceHits();


  ClassDef(CbmMCTestHitCompare,1);

};

#endif
