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

#include "CbmKFParticlesFinder.h"
#include "CbmL1Def.h"

#include "CbmKFParticleInterface.h"
#include "CbmKFVertex.h"
#include "CbmStsTrack.h"

#include "TClonesArray.h"
#include "TStopwatch.h"
#include <iostream>

ClassImp(CbmKFParticlesFinder)

CbmKFParticlesFinder::CbmKFParticlesFinder(float cuts[2][3], const char *name, const char *title, Int_t iVerbose):
  FairTask(name,iVerbose),
  fCuts(),
  flistStsTracks(0),
  fPrimVtx(0),
  fParticles()
{
  if(cuts)
  {
    for(int i=0; i<2; ++i)
      for(int j=0; j<3; ++j)
        fCuts[i][j] = cuts[i][j];
  }
  else
  {
    fCuts[0][0] = 3.;
    fCuts[0][1] = 3.;
    fCuts[0][2] = -100.;
    fCuts[1][0] = 3.;
    fCuts[1][1] = 3.;
    fCuts[1][2] = -100.;
  }
}

CbmKFParticlesFinder::~CbmKFParticlesFinder()
{
}

void CbmKFParticlesFinder::SetParContainers()
{
}

InitStatus CbmKFParticlesFinder::ReInit()
{
  FairRootManager *fManger = FairRootManager::Instance();

  flistStsTracks = (TClonesArray *)  fManger->GetObject("StsTrack");
  fPrimVtx = (CbmVertex*) fManger->GetObject("PrimaryVertex");

  return kSUCCESS;
}

InitStatus CbmKFParticlesFinder::Init()
{
  return ReInit();
}

void CbmKFParticlesFinder::Exec(Option_t * option)
{
  if(!flistStsTracks) return;
  if(!fPrimVtx) return;

  TStopwatch timerSelect;

  fParticles.clear();

  vector<CbmStsTrack> vRTracks;
  int nTracks = flistStsTracks->GetEntries();
  vRTracks.resize(nTracks);
  for(int iTr=0; iTr<nTracks; iTr++)
    vRTracks[iTr] = *( (CbmStsTrack*) flistStsTracks->At(iTr));

//  CbmKFVertex kfVertex(*fPrimVtx);
  CbmKFVertex kfVertex;

  CbmKFParticleInterface::FindParticles(vRTracks,fParticles, kfVertex, fCuts);

  timerSelect.Stop();

  static int NEv=0;
  NEv++;
  static double timeSelectCPU=0.;
  static double timeSelectReal=0.;

  timeSelectCPU += timerSelect.CpuTime();
  timeSelectReal += timerSelect.RealTime();

  std::cout << " ---- Particle finder --- " << std::endl;
  std::cout << "KF Particle Finder Times:" <<" Real - "<< timeSelectReal/NEv << "   CPU - "<< timeSelectCPU/NEv << std::endl << std::endl;
}

void CbmKFParticlesFinder::Finish(){
}
