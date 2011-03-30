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

  const vector<CbmL1PFMCParticle> & GetKsMC() const {return fKsMC;}
  const vector<CbmL1PFMCParticle> & GetLambdaMC() const {return fLambdaMC;}

  const vector<CbmKFParticle> & GetKs() const {return fKs;}
  const vector<CbmKFParticle> & GetLambda() const {return fLambda;}

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

  vector<CbmL1Track> fPPlus;
  vector<CbmL1Track> fPMinus;

  vector<CbmL1Track> fKPlus;
  vector<CbmL1Track> fKMinus;

  vector<CbmL1Track> fElectron;
  vector<CbmL1Track> fPozitron;

//output data
  vector<CbmKFParticle> fKs;
  vector<CbmL1PFMCParticle> fKsMC;
  vector<CbmKFParticle> fLambda;
  vector<CbmL1PFMCParticle> fLambdaMC;
};

#endif
