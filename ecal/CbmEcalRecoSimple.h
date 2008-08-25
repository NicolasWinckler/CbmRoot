/** A very simple version of reconstruction.
 ** Mainly for debugging.
 ** The tracks array can be imported from ROOT file. (see CbmEcalTrackExport. )
 ** The output can be exported to tree. 
 ** //Dr.Sys **/

#ifndef CBMECALRECOSIMPLE_H
#define CBMECALRECOSIMPLE_H

#include "CbmTask.h"
#include "TString.h"

class TChain;
class TClonesArray;
class TTree;
class CbmEcalStructure;
class CbmEcalCell;
class CbmEcalSCurveLib;
class CbmEcalInf;
class CbmEcalClusterV1;
class CbmEcalShowerLib;
class CbmEcalCalibration;
class TFormula;

class CbmEcalRecoSimple : public CbmTask
{
public:
  /** Default constructor. Requirement of ROOT system **/
  CbmEcalRecoSimple();

  /** Standard constructor **/
  CbmEcalRecoSimple(const char *name, const Int_t iVerbose=1, const char* configname="none");

  /** Destructor **/
  virtual ~CbmEcalRecoSimple();

  /** Init **/
  virtual InitStatus Init();
  virtual void Exec(Option_t* option);
  virtual void Finish();

  /** Tools for debugging **/
  inline void SetOutToTree(Bool_t totree=kTRUE) {fToTree=totree;}
  inline void SetRecoName(const char* name) {fRecoName=name;}
  inline TString GetRecoName() const {return fRecoName;}
private:
  /** Reconstruct photon from maximum **/
  void Reco(CbmEcalCell* cell, CbmEcalClusterV1* clstr);
  /** Calculate a chi2 for just reconstructed photons **/
  void CalculateChi2(CbmEcalClusterV1* cluster);
  /** Number of reconstructed tracks **/
  Int_t fN;
  Int_t fNOld;

  CbmEcalCell** fMaximums;	//!
  CbmEcalCell** fExcluded;	//!

  /** Event number **/
  Int_t fEventN;
  /** Calorimeter structure **/
  CbmEcalStructure* fStr;	//!
  /** An information about calorimeter **/
  CbmEcalInf* fInf;		//!
  /** S-curve library **/
  CbmEcalSCurveLib* fLib;	//!
  /** A shower library **/
  CbmEcalShowerLib* fShLib;	//!
  /** A calibration of the calorimeter **/
  CbmEcalCalibration* fCal;	//!

  /** Array of tracks **/
  TClonesArray* fTracks;	//!
  /** Array of reconstructed particles **/
  TClonesArray* fReco;		//!
  /** Array of clusters **/
  TClonesArray* fClusters;	//!
  /** Name of output tree **/
  TString fRecoName;

  /** A tool, to export reconstructed particles to file. Should be moved to
   ** a separate task in future. For debugging now. **/
  Bool_t fToTree;
  TTree* fOutTree;		//!
  void CreateTree();
  Short_t fType;
  Short_t fDiffType;
  Double_t fE;
  Double_t fE3x3;
  Double_t fE2x2;
  Double_t fPSE;
  Double_t fEReco;
  Double_t fXReco;
  Double_t fYReco;
  Double_t fAX;
  Double_t fAY;
  Double_t fCellX;
  Double_t fCellY;

  /** Constants used in reconstruction **/
  Double_t fC[10];
  /** Definition of sigma used for chi^2 calculation **/
  TFormula* fSigma;		//!
  TString fParNames[14];	//!
  TString fSigmaFormula;
  Int_t fParOffset;

  ClassDef(CbmEcalRecoSimple, 1)
};

#endif

