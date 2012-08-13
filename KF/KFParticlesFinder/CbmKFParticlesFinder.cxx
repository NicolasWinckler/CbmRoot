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
#include "CbmKFTrack.h"
#include "CbmStsTrack.h"

#include "TClonesArray.h"
#include "TStopwatch.h"
#include <iostream>

#include "CbmTrackMatch.h"
#include "CbmMCTrack.h"

#include "CbmL1PFFitter.h"


ClassImp(CbmKFParticlesFinder)

CbmKFParticlesFinder::CbmKFParticlesFinder(float cuts[2][3], Bool_t useMCPID, const char *name, const char *title, Int_t iVerbose):
  FairTask(name,iVerbose),
  fCuts(),
  fUseMCPID(useMCPID),
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
  //for the particle id
  flistStsTracksMatch = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsTrackMatch") );
  flistMCTracks = dynamic_cast<TClonesArray*>( fManger->GetObject("MCTrack") );

  return kSUCCESS;
}

InitStatus CbmKFParticlesFinder::Init()
{
  return ReInit();
}

void CbmKFParticlesFinder::Exec(Option_t * option)
{
  if(!flistStsTracks) return;
//   if(!fPrimVtx) return;

  if(!flistStsTracksMatch) return;
  if(!flistMCTracks) return;

  vector<CbmStsTrack> vRTracks;
  int nTracks = flistStsTracks->GetEntries();
  vRTracks.resize(nTracks);
  for(int iTr=0; iTr<nTracks; iTr++)
    vRTracks[iTr] = *( (CbmStsTrack*) flistStsTracks->At(iTr));

  CbmKFVertex kfVertex;
  if(fPrimVtx)
    kfVertex = CbmKFVertex(*fPrimVtx);
  
  vector<int> vTrackPDG(vRTracks.size(), -1);
  if(fUseMCPID)
  {
    for(int iTr=0; iTr<nTracks; iTr++)
    {
      CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*)flistStsTracksMatch->At(iTr);
      if(stsTrackMatch -> GetNofMCTracks() == 0) continue;
      const int mcTrackId = stsTrackMatch->GetMCTrackId();
      CbmMCTrack* mcTrack = (CbmMCTrack*)flistMCTracks->At(mcTrackId);
      vTrackPDG[iTr] = mcTrack->GetPdgCode();
    }
  }
/*
  for(int iTr=0; iTr<nTracks; iTr++)
  {
    if(vRTracks[iTr].GetParamFirst()->GetQp()<0) vTrackPDG[iTr] *= -1;
  }*/
  
  TStopwatch timerSelect;

  fParticles.clear();

  CbmL1PFFitter fitter;

//  fitter.Fit(vRTracks); //assumed, that the initial fit should be fixed and must return good results!!!

  vector<L1FieldRegion> vField;
  vector<float> ChiToPrimVtx;
  fitter.GetChiToVertex(vRTracks, vField, ChiToPrimVtx, kfVertex, 3);

//  fitter.CalculateFieldRegion(vRTracks, vField);

  vector<CbmKFTrack> vKFTrack(nTracks);
  for(int iTr=0; iTr<nTracks; iTr++)
    vKFTrack[iTr] = CbmKFTrack(vRTracks[iTr]);

  CbmKFParticleInterface::FindParticles(vKFTrack, ChiToPrimVtx, vField, fParticles, kfVertex, vTrackPDG, fCuts);

  timerSelect.Stop();

  static int NEv=0;
  NEv++;
  static double timeSelectCPU=0.;
  static double timeSelectReal=0.;

  timeSelectCPU += timerSelect.CpuTime();
  timeSelectReal += timerSelect.RealTime();

  if(NEv%10==0)
  {
    std::cout.setf(ios::fixed);
    std::cout.setf(ios::showpoint);
    std::cout.precision(9);
    std::cout << " ---- Particle finder --- " << std::endl;
    std::cout << "KF Particle Finder Times:" <<" Real - "<< timeSelectReal/NEv 
                                            << "   CPU - "<< timeSelectCPU/NEv << std::endl << std::endl;
  }
}

void CbmKFParticlesFinder::Finish(){
}
