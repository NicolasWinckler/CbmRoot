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

#include "CbmL1PFMCParticle.h"
#include "CbmL1PFFitter.h"

#include "TClonesArray.h"

#include <vector>

class CbmL1PFFitter;

class CbmL1ParticlesFinder
{
 public:
  CbmL1ParticlesFinder();
  ~CbmL1ParticlesFinder();

  void FindParticles(vector<CbmL1Track> &vRTracks);

  vector<CbmKFParticle>& GetParticles()  {return fParticles;}
  CbmKFVertex* GetPV() {return &fPrimVtx;}

 private:

  double GetChiToVertex( CbmKFTrackInterface &track );

  void Find2PDecay(vector<CbmL1Track> &vRTracks);
  void FindPV(vector<CbmL1Track> &vRTracks);

  CbmL1PFFitter fitter;
//for primary vertex
  CbmKFPrimaryVertexFinder fPVFinder;
  CbmKFVertex fPrimVtx;

  //selected tracks
  vector<CbmL1Track> fPos;
  vector<CbmL1Track> fNeg;

  vector<CbmKFParticle> fParticles;
};

#endif

