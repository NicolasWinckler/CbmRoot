// -------------------------------------------------------------------------
// -----                  CbmMuchPhysicsQa header file                 -----
// -----                Created **/01/08  by A. Kiseleva               -----
// -------------------------------------------------------------------------

#ifndef CBMMUCHPHYSICSQA_H
#define CBMMUCHPHYSICSQA_H

#include "CbmTask.h"
#include "CbmMCTrack.h"
#include "CbmGeoNode.h"
#include "TH1.h"
#include "TH2.h"
#include "TLorentzVector.h"

class TClonesArray;
class CbmGeoMuchPar;
class CbmVertex;

class CbmMuchPhysicsQa : public CbmTask {

public:
  CbmMuchPhysicsQa(const char *name="MuchPhysics", Int_t verbose = 1);
  
  virtual ~CbmMuchPhysicsQa();
  virtual InitStatus Init();
  virtual void Exec(Option_t * option);
  virtual void ExecMC(); //--- MC data 
  virtual void ExecReco(); //--- RECO data
  virtual void Finish();

//---------------- Sets  
  virtual void SetParContainers();
//--- Type of Qa: 0 - MC, 1 - RECO
  virtual void SetQaType(Int_t type); 
//--- Set name of PS file for histograms  
  virtual void SetQaFileName(const  char* QaFileName); 
//--- Type of pID in MuCh: 0 - 70% true hits, 1 - all hits behind last absorber
  virtual void SetMuchPIdType(Int_t type); 
  
//----------------   
  Double_t ReadGeo(CbmGeoNode *node); // retutn Z position detector or absorber
  
//---------------- Design of histograms  
  virtual void FillHistogram(Int_t i); //--- fill QA histograms
  virtual void DrawStatText(TH1D *h1, TH1D *h2);
  virtual void DrawStatText(TH2D *h);
  virtual void DrawHistograms(TH1D *h1, TH1D *h2);
//---------------
  
  virtual void ParticleCounter(Int_t i, DetectorId detId); //--- Calculation of particle numbers
 
 
private:
  Int_t fVerbose;
  TClonesArray *fMuchTracks, *fMuchHits, *fMuchHitMatch, *fMuchPoints, *fMCTracks, *fStsTracks, *fStsTrackMatch;
  CbmVertex *fPrimVtx;  
  
  CbmGeoMuchPar*     fGeoPar; //--- Geometry parameter container

  Int_t  fNofEvents, fNofDetectors, fNofLastDetectors, fNofAbsorbers, fQaType, fMuchPIdType;
  Int_t  fNofPi, fNofMu, fNofK, fNofP; //--- particle ID of MC or RECO tracks: 70% true hits in STS
  Int_t  fNofPiMuch, fNofMuMuch, fNofKMuch, fNofPMuch, fNofEMuch; //--- particle ID of RECO tracks: 70% true hits in MuCh
  
  Double_t fZofLastAbsorber;
  
  char    fQaFileName[256];
  char    fInFileName[256];
  
  TH1D* MuonMCTrackID;
//---------------- Histograms:
//---------------- Bg - background, S - signal (if there is)
  TH1D *fRapidityBg, *fRapidityS, *fMomentumBg, *fMomentumS, *fTransMomentumBg, *fTransMomentumS, *fNofTracksBg, *fNofTracksS;
  TH2D *fMomentumThetaBg, *fMomentumThetaS; 
  
  ClassDef(CbmMuchPhysicsQa,1) 
};

#endif 

