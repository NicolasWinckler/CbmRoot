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

#include "CbmL1ParticlesFinder.h"
#include "CbmL1Def.h"

#include "CbmKFMath.h"
#include "CbmKFTrack.h"

#include "CbmKFParticleInterface.h"
#include "CbmL1PFFitter.h"

#include "TStopwatch.h"
#include "L1Field.h"

#include <iostream>
CbmL1ParticlesFinder::CbmL1ParticlesFinder()
{
}

CbmL1ParticlesFinder::~CbmL1ParticlesFinder()
{
}

void CbmL1ParticlesFinder::FindPV(vector<CbmL1Track> &vRTracks)
{
  fPVFinder.Clear();
  for( unsigned short iTr=0; iTr<vRTracks.size(); iTr++ ){
    if( vRTracks[iTr].GetNOfHits() < 4 ) continue;
    if( vRTracks[iTr].GetRefChi2()<0. || vRTracks[iTr].GetRefChi2()>3.5*3.5*vRTracks[iTr].GetRefNDF() ) continue;
    if( !finite(vRTracks[iTr].GetTrack()[0]) || !finite(vRTracks[iTr].GetCovMatrix()[0])) continue;
    fPVFinder.AddTrack(&vRTracks[iTr]);
  }
  fPVFinder.Fit( fPrimVtx );
  std::cout << "!!!!!!!!!!!!!!!!!!!!!!!         Tracks " << fPrimVtx.GetRefNTracks() << std::endl;
}

double CbmL1ParticlesFinder::GetChiToVertex( CbmKFTrackInterface &track )
{
  CbmKFTrack T(track);
  T.Extrapolate( fPrimVtx.GetRefZ() );
  Double_t Cv[3] = { fPrimVtx.GetCovMatrix()[0], fPrimVtx.GetCovMatrix()[1], fPrimVtx.GetCovMatrix()[2] };

  Double_t dx = T.GetTrack()[0] - fPrimVtx.GetRefX();
  Double_t dy = T.GetTrack()[1] - fPrimVtx.GetRefY();
  Double_t c[3] = { T.GetCovMatrix()[0], T.GetCovMatrix()[1], T.GetCovMatrix()[2] };
  c[0]+= Cv[0];  c[1]+= Cv[1];  c[2]+= Cv[2];
  Double_t d = c[0]*c[2] - c[1]*c[1] ;

  if( fabs(d)<1.e-20 ) return 0;
  return sqrt( fabs( 0.5*(dx*dx*c[0]-2*dx*dy*c[1]+dy*dy*c[2])/d ));
}

void CbmL1ParticlesFinder::Find2PDecay(vector<CbmL1Track> &vRTracks)
{
  fitter.Fit(vRTracks);

  vector<float> ChiToPrimVtx;
  fitter.GetChiToVertex(vRTracks,ChiToPrimVtx,fPrimVtx);

  for(unsigned short iTr=0; iTr < vRTracks.size(); iTr++)
  {
    bool ok = 1;
    for(unsigned short iT=0; iT<5; iT++)
      ok = ok && finite(vRTracks[iTr].GetTrack()[iT]);
    for(unsigned short iC=0; iC<15; iC++)
      ok = ok && finite(vRTracks[iTr].GetCovMatrix()[iC]);
      ok = ok && (vRTracks[iTr].GetCovMatrix()[0] < 1. && vRTracks[iTr].GetCovMatrix()[0] > 0.)
              && (vRTracks[iTr].GetCovMatrix()[2] < 1. && vRTracks[iTr].GetCovMatrix()[2] > 0.)
              && (vRTracks[iTr].GetCovMatrix()[5] < 1. && vRTracks[iTr].GetCovMatrix()[5] > 0.)
              && (vRTracks[iTr].GetCovMatrix()[9] < 1. && vRTracks[iTr].GetCovMatrix()[9] > 0.)
              && (vRTracks[iTr].GetCovMatrix()[14] < 1. && vRTracks[iTr].GetCovMatrix()[14] > 0.);
      ok = ok && vRTracks[iTr].chi2 < 10*vRTracks[iTr].NDF;
    if(!ok) continue;

    if( ChiToPrimVtx[iTr] < 3. ) continue;

    if(vRTracks[iTr].T[4] >= 0.) fPos.push_back(vRTracks[iTr]);
    if(vRTracks[iTr].T[4] < 0.) fNeg.push_back(vRTracks[iTr]);
  }

  const Int_t PiPlusPDG = 211;
  const Int_t PiMinusPDG =-211;

  const Int_t PPlusPDG = 2212;
  const Int_t PMinusPDG =-2212;

  vector<L1FieldRegion> posB;
  vector<L1FieldRegion> negB;

  fitter.CalculateFieldRegion(fPos,posB);
  fitter.CalculateFieldRegion(fNeg,negB);

  for(unsigned short iTrP=0; iTrP < fPos.size(); iTrP++) {
    CbmKFParticle tmp = static_cast<CbmL1Track*>(&(fPos[iTrP]));
    tmp.SetPDG(211);
    tmp.SetId(fParticles.size());
    int entrSIMD = iTrP % fvecLen;
    int entrVec  = iTrP / fvecLen;
    posB[entrVec].GetOneEntry(tmp.fieldRegion,entrSIMD);
    fParticles.push_back(tmp);
  }
  for(unsigned short iTrN=0; iTrN < fNeg.size(); iTrN++) {
    CbmKFParticle tmp = static_cast<CbmL1Track*>(&(fNeg[iTrN]));
    tmp.SetPDG(-211);
    tmp.SetId(fParticles.size());
    int entrSIMD = iTrN % fvecLen;
    int entrVec  = iTrN / fvecLen;
    negB[entrVec].GetOneEntry(tmp.fieldRegion,entrSIMD);
    fParticles.push_back(tmp);
  }

  unsigned short NPositive = fPos.size();
  for(unsigned short iTrN=0; iTrN < fNeg.size(); iTrN++)
  {
    CbmKFParticle_simd vDaughters[2] = {CbmKFParticle_simd(fNeg[iTrN],0,&PiMinusPDG), CbmKFParticle_simd()};
    int entrSIMD = iTrN % fvecLen;
    int entrVec  = iTrN / fvecLen;
    vDaughters[0].SetField(negB[entrVec],1,entrSIMD);
    vDaughters[0].SetId(iTrN+NPositive);

    for(unsigned short iTrP=0; iTrP < NPositive; iTrP += fvecLen)
    {
      int NTracks = (iTrP + fvecLen < NPositive) ? fvecLen : (NPositive - iTrP);
      CbmKFTrackInterface* vfPos[fvecLen];

      for(unsigned short iv=0; iv<NTracks; iv++)
        vfPos[iv] = &fPos[iTrP+iv];
      vDaughters[1].Create(vfPos,NTracks,0,&PiPlusPDG);
      vDaughters[1].SetField(posB[iTrP/fvecLen]);
      fvec posId(iTrP,iTrP+1,iTrP+2,iTrP+3);
      vDaughters[1].SetId(posId);

      CbmKFParticleInterface Ks;
      Ks.SetPDG( 310 );
      Ks.Construct(vDaughters, 2, 0);

      for(int iv=0; iv<NTracks; iv++)
      {
        if(!finite(Ks.GetChi2()[iv])) continue;
        if(!(Ks.GetChi2()[iv] > 0.0f)) continue;
        if(!(Ks.GetChi2()[iv]==Ks.GetChi2()[iv])) continue;

        if( Ks.GetChi2()[iv]/Ks.GetNDF()[iv] < 3. )
        {
          CbmKFParticle Ks_temp;
          Ks.GetKFParticle(Ks_temp, iv);
          Ks_temp.SetId( fParticles.size() );
          fParticles.push_back(Ks_temp);
        }
      }

      vDaughters[1].Create(vfPos,NTracks,0,&PPlusPDG);
      vDaughters[1].SetId(posId);

      CbmKFParticleInterface Lambda;
      Lambda.SetPDG(3122);
      Lambda.Construct(vDaughters, 2, 0);

      for(int iv=0; iv<NTracks; iv++)
      {
        if(!finite(Lambda.GetChi2()[iv])) continue;
        if(!(Lambda.GetChi2()[iv] > 0.0f)) continue;
        if(!(Lambda.GetChi2()[iv]==Lambda.GetChi2()[iv])) continue;

        if( Lambda.GetChi2()[iv]/Lambda.GetNDF()[iv] < 3.)
        {
          CbmKFParticle Lambda_temp;
          Lambda.GetKFParticle(Lambda_temp, iv);
          Lambda_temp.SetId( fParticles.size() );
          fParticles.push_back(Lambda_temp);
        }
      }
    }
  }
}

void CbmL1ParticlesFinder::FindParticles(vector<CbmL1Track> &vRTracks)
{
  TStopwatch timerSelect, timerAll, timerPV;

  fPos.clear();
  fNeg.clear();
  fParticles.clear();

  timerAll.Start();
  timerPV.Start();

  FindPV(vRTracks);

  timerPV.Stop();
  timerSelect.Start();

  Find2PDecay(vRTracks);

  timerSelect.Stop();
  timerAll.Stop();

  static int NEv=0;
  NEv++;
  static double timeSelectCPU=0., timeAllCPU=0., timePVCPU=0.;
  static double timeSelectReal=0., timeAllReal=0., timePVReal=0.;

  timePVCPU += timerPV.CpuTime();
  timeSelectCPU += timerSelect.CpuTime();
  timeAllCPU += timerAll.CpuTime();

  timePVReal += timerPV.RealTime();
  timeSelectReal += timerSelect.RealTime();
  timeAllReal += timerAll.RealTime();

  std::cout << "Particle Finder Times:"<<std::endl;
  std::cout << "  PVFinder:        " <<" Real - "<< timePVReal/NEv << "   CPU - "<< timePVCPU/NEv << std::endl;
  std::cout << "  Select Candidates" <<" Real - "<< timeSelectReal/NEv << "   CPU - "<< timeSelectCPU/NEv << std::endl;
  std::cout << "  Total            " <<" Real - "<< timeAllReal/NEv << "   CPU - "<< timeAllCPU/NEv << std::endl;
}
