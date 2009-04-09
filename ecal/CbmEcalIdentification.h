#ifndef CBMECALIDENTIFICATION_H
#define CBMECALIDENTIFICATION_H

#include "FairTask.h"

#include "TString.h"

#include <list>

class TTree;
class TClonesArray;
class CbmEcalStructure;
class CbmEcalCalibration;
class CbmEcalTrackExtrapolation;
class FairTrackParam;
class CbmEcalCell;

class CbmEcalIdentification : public FairTask
{
public:
  CbmEcalIdentification(const char* name, const Int_t iVerbose=0, const char* config="$VMCWORKDIR/parameters/ecal/identification.cfg");
public:
  /** Default constructor **/
  CbmEcalIdentification() {};

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalIdentification() {};
private:
  /** Initialize a tree **/
  void InitTree();
  /** Identify a track **/
  void Identify(FairTrackParam* tr);
  /** Write information to the tree **/
  void WriteTree();
  /** Get energy of the cell **/
  Double_t GetEnergy(CbmEcalCell* cell);
  /** Get energy in PS  **/
  Double_t GetPSEnergy(CbmEcalCell* cell);
  TString fCfgName;
  /** Out tree **/
  TTree* fTree;			//!

  /** Coordinates of a track **/
  Double_t fX;
  Double_t fY;
  Double_t fZ;
  /** Coordinates of cell hitted by track **/
  Double_t fCellX;
  Double_t fCellY;
  /** Coordinates of maximum cell**/
  Double_t fMaxX;
  Double_t fMaxY;
  /** An energy and a shape of shower **/
  Double_t fE;
  Double_t fPSE;
  /** Energy deposition in preshower in cell hit by track **/
  Double_t fPSE2;
  /** PS energy in 2x2 matrix **/
  Double_t fPSEAll;
  Double_t fShape;
  /** Probability to be an electron **/
  Double_t fEProb;
  /** Current extrapolation **/
  Int_t fExtraNum;
  /** Number of global track, produce the particle **/
  Int_t fTrackNum;
  /** P of the track **/
  Double_t fTrackP;
  /** Number of MC track, produce the particle. **/
  Int_t fMCTrackNum;
  /** Should we dump debugging info to tree? **/
  Int_t fTreeOut;
  /** Use MC information to get MC track number **/
  Int_t fUseMC;
  /** Should we use hacks for hadrons? **/
  Int_t fUseHack;
  Double_t fHack;
  /** Event number **/
  Int_t fEvent;
  /** Number of the id particle **/
  Int_t fN;

  /** Hit index in TOF for given global track **/
  Int_t fNTOF;
  /** Track index in TDR for given global track **/
  Int_t fNTRD;
  /** Ring inxed in RICH for given global track **/
  Int_t fNRICH;
  /** \chi^2 of a givent global track **/
  Double_t fTrChi2;

  /** PDG code of the track by tracking system **/
  Int_t fPDG;
  /** MC PDG code **/
  Int_t fMCPDG;
  /** Energy of mother MC track**/
  Double_t fMCP;
  /** Distance between MC and predicted impact point of the track **/
  Double_t fR;
  /** Energy of the electron on the calorimeter surface **/
  Double_t fMCSurfE;
  /** Energy loss in TRD **/
  Double_t fTrdELoss;
  /** Energy in circle with radius fCircRad**/
  Double_t fMCCircE;
  /** Radius of circle for energy collection **/
  Double_t fCircRad;

  /** Resolution of the calorimeter. Need to calculate probability **/
  Double_t fEResolution;
  /** Array of identified particles **/
  TClonesArray* fId;
  /** Array of reconstructed particles **/
  TClonesArray* fTracks;
  /** Array of MC tracks. Used only for tree output and hack procedure (if any).**/
  TClonesArray* fMC;
  /** Global tracking information **/
  TClonesArray* fGlobal;
  /** TRD tracks for energy loss information **/
  TClonesArray* fTrdTracks;
  /** Sts info for track matching **/
//  TClonesArray* fStsTracks;
  TClonesArray* fStsTracksMatch;
  /** MC tracking on the calorimeter surface **/
  TClonesArray* fMCPoints;
  /** Calibration **/
  CbmEcalCalibration* fCal;
  /** Calorimeter structure **/
  CbmEcalStructure* fStr;
  /** Track extrapolation class. Need to map global track <-> track before the calorimeter **/
  CbmEcalTrackExtrapolation* fExtra;
  ClassDef(CbmEcalIdentification,1)
};

#endif 

