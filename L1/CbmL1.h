/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  CbmL1 header file
 *
 *====================================================================
 */

#ifndef _CbmL1_h_
#define _CbmL1_h_
 
#include "CbmL1MC.h"
#include "CbmL1Track.h"
#include "CbmL1Vtx.h"

#include "FairTask.h"
#include "FairRootManager.h"
#include "FairDetector.h"
#include "CbmMCTrack.h"
#include "FairMCApplication.h"
#include "CbmStsPoint.h"
#include "CbmStsHit.h"
#include "CbmMvdPoint.h"
#include "CbmMvdHit.h"
#include "CbmStsDigiScheme.h"

#include "TClonesArray.h"
#include "TRefArray.h"
#include "TParticle.h"
#include "TRandom.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoTube.h"
#include "TGeoCone.h"
#include "TGeoPcon.h"
#include "TGeoBBox.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>

class L1Algo;


class CbmL1HitStore{
 public:
  int ExtIndex;
  int iStation;
  double x, y;
};

class CbmL1 : public FairTask 
{
 public:

  L1Algo *algo;
  CbmStsDigiScheme StsDigi;

  static CbmL1 *Instance(){ return fInstance; }

  void SetParContainers();
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  void Exec(Option_t * option);
  

  std::vector<CbmL1MCPoint> vMCPoints;
  std::vector<CbmL1MCTrack> vMCTracks;

  std::vector<CbmL1Track> vRTracks;
  std::vector<int> vHitMCRef;

  CbmL1Vtx PrimVtx;
  
  ~CbmL1();
  
  CbmL1(const char *name, Int_t iVerbose = 1 );
  CbmL1();
  void Finish();

  void SetTrackingLevel( Int_t iLevel ){ fTrackingLevel = iLevel; }
  void MomentumCutOff( Double_t cut ){ fMomentumCutOff = cut; }
  void SetGhostSuppression( Bool_t b ){ fGhostSuppression= b; }
  void SetDetectorEfficiency( Double_t eff ){ fDetectorEfficiency = eff; }

  TClonesArray *listStsPts; // Sts MC points
  TClonesArray *listStsHits;
  TClonesArray *listMvdPts; // Mvd MC points
  TClonesArray *listMvdHits;
  TClonesArray *listMvdHitMatches;
  TClonesArray *listMCTracks ;

  int NStation, NMvdStations, NStsStations;
  Int_t fTrackingLevel;
  Double_t fMomentumCutOff;
  Bool_t fGhostSuppression;
  Bool_t fUseMVD;
  Double_t fDetectorEfficiency;
  
  TDirectory *histodir;

  void ReadEvent();

  std::vector<CbmL1HitStore> vHitStore;

  void Performance();

  void IdealTrackFinder(); // just copy MCTrack into RecoTracks.

  bool ReadMCPoint( CbmL1MCPoint *MC, int iPoint, bool MVD );

  static void writedir2current( TObject *obj );


 private:

  static CbmL1 *fInstance;

  void InputPerformance();
  void WriteHistos();
  
  ClassDef(CbmL1,1);

};

#endif //_CbmL1_h_
