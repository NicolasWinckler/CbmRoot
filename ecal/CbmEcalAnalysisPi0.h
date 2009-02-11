#ifndef CBMECALANALYSISPI0_H
#define CBMECALANALYSISPI0_H

#include "FairTask.h"

#include "TString.h"

#include <list>

class TTree;
class TClonesArray;
class CbmEcalStructure;

class CbmEcalAnalysisPi0 : public FairTask
{
public:
  CbmEcalAnalysisPi0(const char* name, const Int_t iVerbose=0);
public:
  /** Default constructor **/
  CbmEcalAnalysisPi0() {};

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalAnalysisPi0() {};
private:
  /** Initialize a tree **/
  void InitTree();
  /** Out tree **/
  TTree* fTree;			//!
  /** Number of found particles in this event **/
  Int_t fEntries;

  /** Event number **/
  Int_t fEvent;
  /** Information about first particle **/
  Double_t fE1;
  Double_t fPx1;
  Double_t fPy1;
  Double_t fPz1;
  Double_t fMCE1;
  Double_t fMCPx1;
  Double_t fMCPy1;
  Double_t fMCPz1;
  Double_t fX1;
  Double_t fY1;
  Double_t fCellX1;
  Double_t fCellY1;
  Double_t fR1;
  Double_t fChi21;
  Int_t fCellType1;
  Int_t fPdg1;

  /** Information about second particle **/
  Double_t fE2;
  Double_t fPx2;
  Double_t fPy2;
  Double_t fPz2;
  Double_t fMCE2;
  Double_t fMCPx2;
  Double_t fMCPy2;
  Double_t fMCPz2;
  Double_t fX2;
  Double_t fY2;
  Double_t fCellX2;
  Double_t fCellY2;
  Double_t fR2;
  Double_t fChi22;
  Int_t fCellType2;
  Int_t fPdg2;

  /** An invariant mass of pair **/
  Double_t fM;
  Double_t fMCM;
  /** A pdg ID of pair mother **/
  Int_t fMotherID;
  Double_t fMX;
  Double_t fMY;
  Double_t fMZ;
  Double_t fMPx;
  Double_t fMPy;
  Double_t fMPz;
  Double_t fME;

  /** An array of reconstructed particles **/
  TClonesArray* fReco;			//!
  /** MC tracks array **/
  TClonesArray* fMC;			//!
  /** Reconstructed tracks array **/
  TClonesArray* fTracks;		//!
  /** Array of found clusters **/
  TClonesArray* fClusters;		//!
  /** Calorimeter structure **/
  CbmEcalStructure* fStr;		//!

  ClassDef(CbmEcalAnalysisPi0,1)
};

#endif 

