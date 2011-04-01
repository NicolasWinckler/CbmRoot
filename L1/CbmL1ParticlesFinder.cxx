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

void CbmL1ParticlesFinder::SelectCandidates(vector<CbmL1Track> &vRTracks)
{
  fitter.Fit(vRTracks);
  
  vector<CbmL1Track> Pos;
  vector<CbmL1Track> Neg;

//  std::cout << std::endl << "all tracks " << vRTracks.size();

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

    if( GetChiToVertex( vRTracks[iTr] ) < 3. ) continue;
//    if( vRTracks[iTr].GetNOfHits() < 4 ) continue;

    if(vRTracks[iTr].T[4] >= 0.) Pos.push_back(vRTracks[iTr]);
    if(vRTracks[iTr].T[4] < 0.) Neg.push_back(vRTracks[iTr]);
  }

//  std::cout << " Pos1 " << Pos.size() << " Neg1 " << Neg.size() << std::endl;

  vector<bool> IsSelectedPos;
  vector<bool> IsSelectedNeg;
  IsSelectedPos.resize(Pos.size());
  IsSelectedNeg.resize(Neg.size());
  for(unsigned short iTrP=0; iTrP < Pos.size(); iTrP++)
    IsSelectedPos[iTrP] = 0;
  for(unsigned short iTrN=0; iTrN < Neg.size(); iTrN++)
    IsSelectedNeg[iTrN] = 0;

//scalar
/*  for(unsigned short iTrN=0; iTrN < Neg.size(); iTrN++)
  {
    unsigned short NPositive = Pos.size();
    for(unsigned short iTrP=0; iTrP < NPositive; iTrP ++)
    {
      vector<CbmKFTrackInterface*> t1;
      t1.push_back(&Pos[iTrP]);
      t1.push_back(&Neg[iTrN]);
      CbmKFParticle Temp;
      Temp.Construct(t1, 0);
      if( Temp.Chi2/Temp.NDF < 3.)
      {
        IsSelectedPos[iTrP] = 1;
        IsSelectedNeg[iTrN] = 1;
      }
    }
  }*/

  CbmKFVertexInterface* pvIntrface[fvecLen];
  for(unsigned short iv=0; iv<fvecLen; iv++ )
    pvIntrface[iv] = &fPrimVtx;
  for(unsigned short iTrN=0; iTrN < Neg.size(); iTrN++)
  {
    unsigned short NPositive = Pos.size();
    for(unsigned short iTrP=0; iTrP < NPositive; iTrP += fvecLen)
    {
      CbmKFParticle_simd vDaughters[2] = {CbmKFParticle_simd(Neg[iTrN]), CbmKFParticle_simd()};
      int NTracks = (iTrP + fvecLen < NPositive) ? fvecLen : (NPositive - iTrP);
      CbmKFTrackInterface* vPos[fvecLen];
      for(unsigned short iv=0; iv<NTracks; iv++)
        vPos[iv] = &Pos[iTrP+iv];
      vDaughters[1].Create(vPos,NTracks);
      CbmKFParticleInterface Temp;
      Temp.Construct(vDaughters, 2, 0);
      // CbmKFParticleInterface Temp_topo;
      // Temp_topo.Construct(vDaughters, 2, pvIntrface);

      for(int iv=0; iv<4; iv++)
      {

        if(!finite(Temp.GetChi2()[iv]) || !(Temp.GetChi2()[iv] > 0.0f) || !(Temp.GetChi2()[iv]==Temp.GetChi2()[iv])) continue;
        // if(!finite(Temp_topo.GetChi2()[iv]) || !(Temp_topo.GetChi2()[iv] > 0.0f) || !(Temp_topo.GetChi2()[iv]==Temp_topo.GetChi2()[iv])) continue;

        if(Temp.GetChi2()[iv]/Temp.GetNDF()[iv] < 3. /*&& Temp_topo.GetChi2()[iv]/Temp_topo.GetNDF()[iv] < 3.*/ && Temp.GetZ()[iv] > 3. && iTrP+iv < NPositive)
        {
          IsSelectedPos[iTrP+iv] = 1;
          IsSelectedNeg[iTrN] = 1;
        }
      }
    }
  }

  for(unsigned short iTrP=0; iTrP < Pos.size(); iTrP++) {
    if(IsSelectedPos[iTrP]) {
      fPionPlus.push_back(Pos[iTrP]);
      CbmKFParticle tmp(CbmKFParticle(&(Pos[iTrP])));
      tmp.SetPDG(211);
      tmp.SetId(fParticles.size());
      fParticles.push_back(tmp);
    }
  }
  for(unsigned short iTrN=0; iTrN < Neg.size(); iTrN++) {
    if(IsSelectedNeg[iTrN]) {
      fPionMinus.push_back(Neg[iTrN]);
      CbmKFParticle tmp(CbmKFParticle(&(Neg[iTrN])));
      tmp.SetPDG(-211);
      tmp.SetId(fParticles.size());
      fParticles.push_back(tmp);
    }
  }

  const fvec massP = 0.938272f;
  
  fPPlus  = fPionPlus;
  fPMinus = fPionMinus;
  fitter.Fit(fPPlus, massP);
  fitter.Fit(fPMinus, massP);

  for(unsigned short iTrP=0; iTrP < fPPlus.size(); iTrP++) {
    CbmKFParticle tmp(CbmKFParticle(&(fPPlus[iTrP])));
    tmp.SetPDG(2212);
    tmp.SetId(fParticles.size());
    fParticles.push_back(tmp);
  }
  for(unsigned short iTrN=0; iTrN < fPMinus.size(); iTrN++) {
    CbmKFParticle tmp(CbmKFParticle(&(fPMinus[iTrN])));
    tmp.SetPDG(-2212);
    tmp.SetId(fParticles.size());
    fParticles.push_back(tmp);
  }
  

//  std::cout << " Pos2 " << fPionPlus.size() << " Neg2 " << fPionMinus.size() << std::endl;
}

void CbmL1ParticlesFinder::FindKs() // pions should be consequtive
{
  CbmKFVertexInterface* pvIntrface[fvecLen];
  for(unsigned short iv=0; iv<fvecLen; iv++ )
    pvIntrface[iv] = &fPrimVtx;
  const int NParticles = fParticles.size();
  for(unsigned short iTrN=0; iTrN < NParticles; iTrN++)
  {
    if ( fParticles[iTrN].GetPDG() != -211 ) continue;
    unsigned short iTrP=0;
    for(; iTrP < NParticles; iTrP++)
      if ( fParticles[iTrP].GetPDG() == 211 ) break;
    for(; iTrP < NParticles; iTrP += fvecLen)
    {
      if ( fParticles[iTrP].GetPDG() != 211 ) continue;
      CbmKFParticle_simd vDaughters[2] = {CbmKFParticle_simd(fParticles[iTrN]), CbmKFParticle_simd()};
      CbmKFParticle* vPos[fvecLen];
      int nEntries = 0;
      for(; iTrP+nEntries < NParticles && nEntries < fvecLen; nEntries++ ) {
        if ( fParticles[iTrP+nEntries].GetPDG() != 211 ) break;
        vPos[nEntries] = &fParticles[iTrP+nEntries];
      }
      vDaughters[1].Create(vPos,nEntries);
      CbmKFParticleInterface Ks;
      Ks.SetPDG( 310 );
      Ks.Construct(vDaughters, 2, 0);

      // CbmKFParticleInterface Ks_topo;
      // Ks_topo.SetPDG( 310 );
      // Ks_topo.Construct(vDaughters, 2, pvIntrface);

      for(int iv=0; iv<nEntries; iv++)
      {
        if(!finite(Ks.GetChi2()[iv])) continue;
        if(!(Ks.GetChi2()[iv] > 0.0f)) continue;
        if(!(Ks.GetChi2()[iv]==Ks.GetChi2()[iv])) continue;
        // if(!finite(Ks_topo.GetChi2()[iv]) || !(Ks_topo.GetChi2()[iv] > 0.0f) || !(Ks_topo.GetChi2()[iv]==Ks_topo.GetChi2()[iv])) continue;

        if( Ks.GetChi2()[iv]/Ks.GetNDF()[iv] < 3. /*&& Ks_topo.GetChi2()[iv]/Ks_topo.GetNDF()[iv] < 3.*/ && Ks.GetZ()[iv] > 3.)
        {
          CbmKFParticle Ks_temp;
          Ks.GetKFParticle(Ks_temp, iv);
          Ks_temp.SetId( fParticles.size() );
          fParticles.push_back(Ks_temp);
        }
      }
    }
  }

}

void CbmL1ParticlesFinder::FindLambda() // p and pions should be consequtive 
{
  CbmKFVertexInterface* pvIntrface[fvecLen];
  for(unsigned short iv=0; iv<fvecLen; iv++ )
    pvIntrface[iv] = &fPrimVtx;
  const int NParticles = fParticles.size();
  for(unsigned short iTrN=0; iTrN < NParticles; iTrN++)
  {
    if ( fParticles[iTrN].GetPDG() != -211 ) continue;
    unsigned short iTrP=0;
    for(; iTrP < NParticles; iTrP++)
      if ( fParticles[iTrP].GetPDG() == 2212 ) break;
    for(; iTrP < NParticles; iTrP += fvecLen)
    {
      if ( fParticles[iTrP].GetPDG() != 2212 ) break;
      CbmKFParticle_simd vDaughters[2] = {CbmKFParticle_simd(fParticles[iTrN]), CbmKFParticle_simd()};
      CbmKFParticle* vPos[fvecLen];
      int nEntries = 0;
      for(; iTrP+nEntries < NParticles && nEntries < fvecLen; nEntries++ ) {
        if ( fParticles[iTrP+nEntries].GetPDG() != 2212 ) break;
        vPos[nEntries] = &fParticles[iTrP+nEntries];
      }
      vDaughters[1].Create(vPos, nEntries);
      CbmKFParticleInterface Lambda;
      Lambda.SetPDG(3122);
      Lambda.Construct(vDaughters, 2, 0);
	//      CbmKFParticleInterface Lambda_topo;
        //      Lambda_topo.SetPDG(3122);
	//      Lambda_topo.Construct(vDaughters, 2, pvIntrface);
      
      for(int iv=0; iv<nEntries; iv++)
      {
        if(!finite(Lambda.GetChi2()[iv])) continue;
        if(!(Lambda.GetChi2()[iv] > 0.0f)) continue;
        if(!(Lambda.GetChi2()[iv]==Lambda.GetChi2()[iv])) continue;
          //        if(!finite(Lambda_topo.GetChi2()[iv]) || !(Lambda_topo.GetChi2()[iv] > 0.0f) || !(Lambda_topo.GetChi2()[iv]==Lambda_topo.GetChi2()[iv])) continue;
	
        if( Lambda.GetChi2()[iv]/Lambda.GetNDF()[iv] < 3. /*&& Lambda_topo.GetChi2()[iv]/Lambda_topo.GetNDF()[iv] < 3. && Lambda.GetZ()[iv] > 3.*/)
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
  TStopwatch timerSelect, timerAll, timerPV, timerFindKs, timerFindLambda;

  timerAll.Start();
  timerPV.Start();

  FindPV(vRTracks);

  timerPV.Stop();
  timerSelect.Start();

  fPionPlus.clear();
  fPionMinus.clear();
  fPPlus.clear();
  fPMinus.clear();
  fKPlus.clear();
  fKMinus.clear();
  fElectron.clear();
  fPozitron.clear();
  fParticles.clear();
  SelectCandidates(vRTracks);

  timerSelect.Stop();
  timerFindKs.Start();

  FindKs();

  timerFindKs.Stop();
  timerFindLambda.Start();

  FindLambda();

  timerFindLambda.Stop();
  timerAll.Stop();

  static int NEv=0;
  NEv++;
  static double timeSelectCPU=0., timeAllCPU=0., timePVCPU=0., timeFindKsCPU=0., timeFindLambdaCPU=0.;
  static double timeSelectReal=0., timeAllReal=0., timePVReal=0., timeFindKsReal=0., timeFindLambdaReal=0.;

  timePVCPU += timerPV.CpuTime();
  timeSelectCPU += timerSelect.CpuTime();
  timeFindKsCPU += timerFindKs.CpuTime();
  timeFindLambdaCPU += timerFindLambda.CpuTime();
  timeAllCPU += timerAll.CpuTime();

  timePVReal += timerPV.RealTime();
  timeSelectReal += timerSelect.RealTime();
  timeFindKsReal += timerFindKs.RealTime();
  timeFindLambdaReal += timerFindLambda.RealTime();
  timeAllReal += timerAll.RealTime();

  std::cout << "Particle Finder Times:"<<std::endl;
  std::cout << "  PVFinder:        " <<" Real - "<< timePVReal/NEv << "   CPU - "<< timePVCPU/NEv << std::endl;
  std::cout << "  Select Candidates" <<" Real - "<< timeSelectReal/NEv << "   CPU - "<< timeSelectCPU/NEv << std::endl;
  std::cout << "  Find K0s         " <<" Real - "<< timeFindKsReal/NEv << "   CPU - "<< timeFindKsCPU/NEv << std::endl;
  std::cout << "  Find Lambda      " <<" Real - "<< timeFindLambdaReal/NEv << "   CPU - "<< timeFindLambdaCPU/NEv << std::endl;
  std::cout << "  Total            " <<" Real - "<< timeAllReal/NEv << "   CPU - "<< timeAllCPU/NEv << std::endl;

}
