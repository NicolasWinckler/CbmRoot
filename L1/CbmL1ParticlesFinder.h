/*
 *=====================================================
 *
 *  CBM Level 1 Reconstruction
 *
 *  Authors: M.Zyzak
 *
 *  e-mail :
 *
 *=====================================================
 *
 *  Finds Particles: Lambdas, K0
 *
 */

#ifndef _CbmL1ParticlesFinder_h_
#define _CbmL1ParticlesFinder_h_

#include "CbmL1Track.h"

#include "CbmKFParticle.h"
#include "CbmKFVertex.h"
#include "CbmKFPrimaryVertexFinder.h"

#include "ParticleFinder/CbmL1PFMCParticle.h"
#include "ParticleFinder/CbmL1PFFitter.h"

#include "TClonesArray.h"

#include <vector>

class CbmL1PFFitter;

class CbmL1ParticlesFinder
{
 public:
  CbmL1ParticlesFinder();
  ~CbmL1ParticlesFinder();

  void FindParticles(vector<CbmL1Track> &vRTracks,TClonesArray *listMCTracks);
  vector<CbmL1PFMCParticle>& GetKsMC() {return fKsMC;}
  vector<CbmL1PFMCParticle>& GetLambdaMC() {return fLambdaMC;}
  vector<CbmKFParticle>& GetKs()         {return fKs;}
  vector<CbmKFParticle>& GetLambda()     {return fLambda;}
  vector<CbmKFParticle>& GetPionPlusP()  {return fPionPlusP;}
  vector<CbmKFParticle>& GetPionMinusP() {return fPionMinusP;}
  vector<CbmKFParticle>& GetPPlusP()     {return fPPlusP;}
  vector<CbmKFParticle>& GetPMinusP()    {return fPMinusP;}
  
 private:

  double GetChiToVertex( CbmKFTrackInterface &track );

  void SelectCandidates(vector<CbmL1Track> &vRTracks);
  void FindPV(vector<CbmL1Track> &vRTracks);
  void FindKs(TClonesArray *listMCTracks);
  void FindLambda();

 private:

 CbmL1PFFitter fitter;
//for primary vertex
  CbmKFPrimaryVertexFinder fPVFinder;
  CbmKFVertex fPrimVtx;

//input data
  vector<CbmL1Track> fPionPlus;
  vector<CbmL1Track> fPionMinus;

  vector<CbmL1Track> fPPlus;    // proton
  vector<CbmL1Track> fPMinus;

  vector<CbmL1Track> fKPlus;
  vector<CbmL1Track> fKMinus;

  vector<CbmL1Track> fElectron;
  vector<CbmL1Track> fPozitron;

  vector<CbmKFParticle> fPionPlusP; // fPionPlus Particle
  vector<CbmKFParticle> fPionMinusP;

  vector<CbmKFParticle> fPPlusP; // fPPlus Particle
  vector<CbmKFParticle> fPMinusP;
  
//output data
  vector<CbmKFParticle> fKs;
  vector<CbmL1PFMCParticle> fKsMC;
  vector<CbmKFParticle> fLambda;
  vector<CbmL1PFMCParticle> fLambdaMC;
};

#endif

