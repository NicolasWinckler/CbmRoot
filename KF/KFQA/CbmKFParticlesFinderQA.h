/*
 *====================================================================
 *
 *  CBM KF Track Quality
 *  
 *  Authors: M.Zyzak
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  KF Particles Finder performance 
 *
 *====================================================================
 */

#ifndef _CbmKFParticlesFinderQA_h_
#define _CbmKFParticlesFinderQA_h_

#include "CbmL1PFMCParticle.h"
#include "CbmKFTrErrMCPoints.h"
#include "FairTask.h"
#include <vector>

class CbmKFParticlesFinder;
class TClonesArray;
class CbmVertex;
class TDirectory;

class CbmKFParticlesFinderQA :public FairTask
{
 public:

  CbmKFParticlesFinderQA(CbmKFParticlesFinder *pf=0, Int_t iVerbose = 1, int findParticlesMode = 1, int perf = 3,
                         const char *name ="CbmKFParticlesFinderQA",
                         const char *title="Cbm KF Particles Finder QA");
  ~CbmKFParticlesFinderQA();

  void SetOutFile(TString outname) {outfileName = outname;}

  virtual InitStatus ReInit();
  virtual InitStatus Init();
  void Exec(Option_t * option);
  void Finish();

 private:
 
  void StsHitMatch();
  void WriteHistos( TObject *obj );
  void GetMCParticles();
  void FindReconstructableMCParticles();
  void CheckMCParticleIsReconstructable(CbmL1PFMCParticle &part);
  void MatchParticles();
  void PartEffPerformance();
  void PartHistoPerformance();
  
  // performance modes and definitions
  const int fFindParticlesMode;
  const int fPerformance;
  const int fMinNStations;
  const float fMinRecoMom;
// CbmKFParticlesFinder object
  CbmKFParticlesFinder *fPF;

  vector<CbmKFTrErrMCPoints> fMCTrackPoints;
  vector<CbmL1PFMCParticle> vMCParticles;  // MC particles
  vector<CbmL1TrackMatch> MCtoRParticleId; // array for match
  vector<CbmL1TrackMatch> RtoMCParticleId; 
// Arrays of points, tracks, etc

  TClonesArray* flistStsPts;
  TClonesArray* flistMvdPts;
  TClonesArray* flistMCTracks;
  TClonesArray* flistStsTracksMatch;
  TClonesArray* flistStsTracks;
  TClonesArray* flistStsHits;
  TClonesArray* flistMvdHits;
  TClonesArray* flistMvdHitMatches;
  TClonesArray* flistStsClusters;
  TClonesArray* flistStsDigi;
  CbmVertex*    fPrimVtx;

// Names of files
  TString outfileName;
  TDirectory* histodir;

  std::vector<int> vStsHitMatch;
  std::vector<int> vStsPointMatch;
  std::vector<int> vMvdPointMatch;
  std::vector<int> vMCTrackMatch;

  ClassDef(CbmKFParticlesFinderQA,1);
 private:
  CbmKFParticlesFinderQA(const CbmKFParticlesFinderQA&);
  void operator=(const CbmKFParticlesFinderQA&);
};

#endif // _CbmKFParticlesFinderQA_h_
