/** A slow version of reconstruction because of extensive Shower Library usage.
 ** Can be used for data production.
 ** The tracks array can be imported from ROOT file. (see CbmEcalTrackExport. )
 ** The output can be exported to tree. 
 ** //Dr.Sys **/

#ifndef CBMECALRECOSLOW_H
#define CBMECALRECOSLOW_H

#include "CbmTask.h"
#include "TString.h"
#include "TFitterMinuit.h"

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

class FCNEcalCluster : public ROOT::Minuit2::FCNGradientBase
{
public:
  FCNEcalCluster(CbmEcalCalibration* cal, CbmEcalShowerLib* shlib, TFormula* sigma, CbmEcalInf* inf)
    : fCal(cal), fShLib(shlib), fSigma(sigma), fInf(inf), fCluster(NULL), fEStep(1e-4), fCStep(1e-4), fErrorDef(1.0) {};
  
  CbmEcalClusterV1* GetCluster() const {return fCluster;}
  void SetCluster(CbmEcalClusterV1* cluster);

  void SetParticleSeeds(const std::vector<CbmEcalCell*> cells) {fCells=cells;}

  /** Get/Set number of photons in cluster **/
  void SetN(Int_t n);
  Int_t GetN() const {return fN;}

  /** Get/Set various steps **/
  void SetEStep(Double_t step) {fEStep=step;}
  void SetCStep(Double_t step) {fCStep=step;}
  Double_t GetEStep() const {return fEStep;}
  Double_t GetCStep() const {return fCStep;}

  /** The chi2 stuff **/
  /** par[n] --- energy, par[n+1] --- x, par[n+2] --- y **/
  virtual std::vector<Double_t> Gradient(const std::vector<Double_t>& par) const;
  virtual Double_t operator()(const std::vector<Double_t>& par) const;

  virtual Double_t Up() const {return fErrorDef;}
  void SetErrorDef(Double_t errordef) {fErrorDef=errordef;}

  ~FCNEcalCluster() {};
private:
  /** A calibration object **/
  CbmEcalCalibration* fCal; 		//!
  /** A shower library **/
  CbmEcalShowerLib* fShLib;		//!
  /** A sigma formula **/
  TFormula* fSigma;			//!
  /** Calorimeter information **/
  CbmEcalInf* fInf;			//!
  /** Calorimeter cluster **/
  CbmEcalClusterV1* fCluster;		//!

  /** Steps if gradient business **/
  Double_t fEStep;
  Double_t fCStep;
  /** A stuff from FCNBase **/
  Double_t fErrorDef;
  /** Energy of cluster **/
  Double_t fClusterEnergy;
  /** Response of cluster **/
  Double_t fClusterResp;
  /** Number of photons in cluster **/
  Int_t fN;
  /** Seed cells of cluster **/
  std::vector<CbmEcalCell*> fCells;	//!
};

class CbmEcalRecoSlow : public CbmTask
{
public:
  /** Default constructor. Requirement of ROOT system **/
  CbmEcalRecoSlow();

  /** Standard constructor **/
  CbmEcalRecoSlow(const char *name, const Int_t iVerbose=1, const char* configname="none");

  /** Destructor **/
  virtual ~CbmEcalRecoSlow();

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
  /** Fit a given cluster. A first approximation should be available **/
  void FitCluster(CbmEcalClusterV1* clstr);
  /** Calculate a chi2 for just reconstructed photons **/
  Double_t CalculateChi2(CbmEcalClusterV1* cluster);
  /** Fit a cluster. A**/
  Double_t FitCluster();
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
  /** A threshold for chi2. Is chi2<fChi2Th no fitting **/
  Double_t fChi2Th;
  Double_t fEStep;
  Double_t fCStep;
  Int_t fMaxIterations;
  FCNEcalCluster* fFCN;		//!
  TFitterMinuit* fFitter;	//!

  ClassDef(CbmEcalRecoSlow, 1)
};

#endif

