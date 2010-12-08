#ifndef CBMECALIDENTIFICATION2_H
#define CBMECALIDENTIFICATION2_H

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
class TFormula;
class CbmEcalMaximum;

class CbmEcalIdentification2 : public FairTask
{
public:
  CbmEcalIdentification2(const char* name, const Int_t iVerbose=0, const char* config="$VMCWORKDIR/parameters/ecal/identification.cfg");
public:
  /** Default constructor **/
  CbmEcalIdentification2() {};

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalIdentification2() {};
private:
  /** Initialize a tree **/
  void InitTree();
  /** Init variables before writing to tree **/
  void InitVar();
  /** Identify a track **/
  void Identify(FairTrackParam* tr);
  /** Write information to the tree **/
  void WriteTree();
  void WriteTreeLight();
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
  Double_t fTheta;
  Double_t fPhi;
  /** Coordinates of cell hitted by track **/
  Double_t fCellX;
  Double_t fCellY;
  /** Coordinates of maximum cell**/
  Double_t fMaxX;
  Double_t fMaxY;
  /** MC Coordinates of the track **/
  Double_t fMCX;
  Double_t fMCY;
  /** An energy and a shape of shower **/
  Double_t fE;
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
  /** Type of cell **/
  Int_t fCellType;
  /** Number of the id particle **/
  Int_t fN;

  /** \chi^2 of a given STS track **/
  Double_t fTrChi2;
  /** NDF of a given STS track **/
  Int_t fTrNDF;

  /** PDG code of the track by tracking system **/
  Int_t fPDG;
  /** MC PDG code **/
  Int_t fMCPDG;
  /** MC PDG code of mother of the track **/
  Int_t fMotherMCPDG;
  /** Energy of mother MC track**/
  Double_t fMCP;
  /** Momentum of energy distribution by particle on the calorimeter surface**/
  Double_t fMCM;
  /** Distance between MC and predicted impact point of the track **/
  Double_t fR;
  /** Energy of the electron on the calorimeter surface **/
  Double_t fMCSurfE;
  /** Energy in circle with radius fCircRad**/
  Double_t fMCCircE;
  /** Radius of circle for energy collection **/
  Double_t fCircRad;


  /** Array of constants for maximums location **/
  Double_t* fF;				//!
  TFormula* fForm[6];			//!
  Double_t GetPL(Double_t e, Double_t theta) const;
  Double_t GetPH(Double_t e, Double_t theta) const;
  Double_t fQL;
  Double_t fQH;
  Double_t fCPL;
  Double_t fCPH;
  Double_t fPH;
  Double_t fPL;
  Int_t CheckMaximum(CbmEcalMaximum* max);

  /** Cluster construction stuff **/
  std::list<CbmEcalCell*> fCluster;	//!
  Int_t fRegions;
  Int_t fRegion;
  Double_t* fThetaB;			//!
  Int_t* fAlgo;				//!
  Double_t* fPar1;			//!
  Double_t* fPar2;			//!
  Double_t* fPar3;			//!
  Int_t* fParI;				//!
  Int_t fIntSteps;

  void ConstructCluster(CbmEcalMaximum* max);
  Double_t SolveEllipse(CbmEcalCell* cell, Double_t cx, Double_t cy, Int_t reg);

  /** Shower shape analysis **/
  void ShapeAnalysis(CbmEcalMaximum* max);
  Double_t fEMax;
  Double_t fE2;
  Double_t fE3;
  Double_t fECluster;
  Double_t fSE[5];
  Double_t fP2;
  Double_t fQ2;
  Double_t fCellE;
  Double_t fCellE2;
  Double_t fAngle;
  Double_t fP;
  Double_t fQ;
  Double_t fPC;
  Double_t fPR;

  void CalculateShape(CbmEcalMaximum* max);
  Double_t fQCH;
  Double_t fQCL;

  /** Resolution of the calorimeter. Need to calculate probability **/
  TFormula* fEResolution;		//!
  /** Array of identified particles **/
  TClonesArray* fId;
  /** Array of reconstructed particles **/
  TClonesArray* fTracks;
  /** Array of MC tracks. Used only for tree output and hack procedure (if any).**/
  TClonesArray* fMC;
  /** Sts tracks for energy loss information **/
  TClonesArray* fStsTracks;
  /** Sts info for track matching **/
  TClonesArray* fStsTracksMatch;
  /** MC tracking on the calorimeter surface **/
  TClonesArray* fMCPoints;
  /** Maximums **/
  TClonesArray* fMaximums;
  /** Calibration **/
  CbmEcalCalibration* fCal;			//!
  /** Calorimeter structure **/
  CbmEcalStructure* fStr;			//!
  /** Track extrapolation class. Need to map global track <-> track before the calorimeter **/
  CbmEcalTrackExtrapolation* fExtra;
  ClassDef(CbmEcalIdentification2,1)
};

#endif 

