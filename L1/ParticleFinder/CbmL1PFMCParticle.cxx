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

#include "CbmL1PFMCParticle.h"
#include "CbmL1MCTrack.h"


CbmL1PFMCParticle::CbmL1PFMCParticle() :fDaughterIds(), fMCTrackID(-1), fMotherId(-1), fPDG(0), fIsReconstructable(0)
{
}

CbmL1PFMCParticle::~CbmL1PFMCParticle()
{
}

void CbmL1PFMCParticle::AddDaughter( int i )
{
  fDaughterIds.push_back(i);
}
