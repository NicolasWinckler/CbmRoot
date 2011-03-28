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

#ifndef _CbmL1PFMCParticle_h_
#define _CbmL1PFMCParticle_h_

#include "CbmL1MCTrack.h"

#include <vector>

struct McVector
{
  vector< CbmL1MCTrack* >  mcTracks;
};

struct McTrackFind
{
  CbmL1MCTrack* mcTrack;
  int vNumber;
};

class CbmL1PFMCParticle
{
 public:
  CbmL1PFMCParticle();
  ~CbmL1PFMCParticle();

  void AddDaughterCandidates(vector< CbmL1MCTrack* >  &mcTracks);
  void AddDaughter(CbmL1MCTrack* mcTrack);
  void FindCommonMC();

  void SetPDG(int pdg) {PDG = pdg;}
  void SetMCTrackID(int id) {mcTrackID = id;}

  int  GetMCTrackID() {return mcTrackID;}
  int  GetPDG() {return PDG;}
  bool IsReconstructable() {return isReconstructable;}
  bool IsRecRec() {return isRecRec;}

  void CalculateIsReconstructable(unsigned int NDaughters = 2);
  void CalculateIsRecRec(unsigned int NDaughters = 2);

 private: //data
  vector< McVector >  mcDaughtersCandidates;
  vector< CbmL1MCTrack* > Daughters;
  bool isReconstructable;
  bool isRecRec; //all daughters are reconstructable and reconstructed
  int mcTrackID; //MC track, which corresponds to the particle
  int PDG;
};
#endif
