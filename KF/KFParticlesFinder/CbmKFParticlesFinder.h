/*
 *=====================================================
 *
 *  CBM Level 1 Reconstruction
 *
 *  Authors: M.Zyzak
 *
 *  e-mail : m.zyzak@gsi.de
 *
 *=====================================================
 *
 *  Finds Particles: Lambdas, K0
 *
 */

#ifndef _CbmKFParticlesFinder_h_
#define _CbmKFParticlesFinder_h_

#include "FairTask.h"
#include "CbmKFParticle.h"

#include <vector>

class TClonesArry;
class CbmVertex;

class CbmKFParticlesFinder: public FairTask
{
 public:
  CbmKFParticlesFinder(float cuts[2][3] = 0,
                       const char *name ="CbmKFParticlesFinder",
                       const char *title="Cbm KF Particles Finder",
                       Int_t iVerbose = 1);
  ~CbmKFParticlesFinder();

  void SetParContainers();
  InitStatus ReInit();
  InitStatus Init();
  void Exec(Option_t * option);
  void Finish();

  vector<CbmKFParticle>& GetParticles()  {return fParticles;}

  ClassDef(CbmKFParticlesFinder,1);

 private:

  float fCuts[2][3];
  TClonesArray* flistStsTracks;
  CbmVertex* fPrimVtx;

  vector<CbmKFParticle> fParticles;
};

#endif

