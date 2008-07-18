// -------------------------------------------------------------------------
// -----                  CbmMuchHitFinderQa header file               -----
// -----                  Created 16/11/07  by E. Kryshen              -----
// -------------------------------------------------------------------------

#ifndef CbmMuchHitFinderQa_H
#define CbmMuchHitFinderQa_H

#include "CbmTask.h"

#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TClonesArray.h"

class CbmMuchGeoScheme;
class TObjArray;

Double_t LandauMPV(Double_t *x, Double_t *par);

class CbmMuchHitFinderQa : public CbmTask {

public:
  CbmMuchHitFinderQa(const char* name = "MuchHitFinderQa", Int_t verbose = 1);
  virtual ~CbmMuchHitFinderQa();
  virtual InitStatus Init();
  virtual void Exec(Option_t * option);
  virtual void Finish();
  virtual void SetParContainers();
  void SetGeoFileName        (TString fileName) { fGeoFileName = fileName; }
  void SetPerformanceFileName(TString fileName) { fFileName    = fileName; }

protected:
  void PullsQa();
  void DigitizerQa();
  void OccupancyQa();
  void StatisticsQa();
  void ClusterDeconvQa();

private:
  CbmMuchGeoScheme* fGeoScheme;
  TString fGeoFileName;
  TString fFileName;
  Int_t fSignalPoints;           // Number of signal MC points
  Int_t fSignalHits;             // Number of signal hits
  Int_t fVerbose;
  Int_t fEvent;
  TClonesArray *fPoints;
  TClonesArray *fDigis;
  TClonesArray *fDigiMatches;
  TClonesArray *fClusters;
  TClonesArray *fHits;
  TClonesArray *fMCTracks;
  TClonesArray *fPointInfos;

  Int_t  fNstations;

  TObjArray* fChargeHistos;
  TH2D* fhChargeEnergyLog;
  TH2D* fhChargeEnergyLogPi;
  TH2D* fhChargeEnergyLogPr;
  TH2D* fhChargeEnergyLogEl;
  TH2D* fhChargeTrackLength;
  TH2D* fhChargeTrackLengthPi;
  TH2D* fhChargeTrackLengthPr;
  TH2D* fhChargeTrackLengthEl;
  TH1D* fhCharge;
  TH1D* fhChargeLog;
  TH1D* fhChargePr_1GeV_3mm;
  TH2D* fhNpadsVsS;
  
  TH1D** fhOccupancyR;
  TH1D** fhPadsTotalR;
  TH1D** fhPadsFiredR;
  TH1D** fhPullXpads1;
  TH1D** fhPullYpads1;
  TH1D** fhPullXpads2;
  TH1D** fhPullYpads2;
  TH1D** fhPullXpads3;
  TH1D** fhPullYpads3;
  Int_t  fnPadSizesX; 
  Int_t  fnPadSizesY; 
  
  TH1D* fhPullX;
  TH1D* fhPullY;

  TH1I* fhPointsInCluster;
  TH1I* fhDigisInCluster;
  TH1I* fhHitsInCluster;

  Int_t *fNall;         // number of all tracks at the first station
  Int_t *fNpr;          // number of protons at the first station
  Int_t *fNpi;          // number of pions at the first station
  Int_t *fNel;          // number of electrons at the first station
  Int_t *fNmu;          // number of muons at the first station
  Int_t *fNka;          // number of kaons at the first station
  Int_t *fNprimary;     // number of primary tracks at the first station
  Int_t *fNsecondary;   // number of secondary tracks at the first station

  Int_t fPointsTotal;
  Int_t fPointsUnderCounted;
  Int_t fPointsOverCounted;

  /** Defines whether the point with the given index is signal point. **/
  Bool_t IsSignalPoint(Int_t iPoint);

  ClassDef(CbmMuchHitFinderQa,1) 
};

#endif 

