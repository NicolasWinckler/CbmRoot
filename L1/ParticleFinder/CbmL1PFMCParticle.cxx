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


CbmL1PFMCParticle::CbmL1PFMCParticle() :isReconstructable(0),isRecRec(0),mcTrackID(-1),PDG(0),fIsReconstructable(0)
{
}

CbmL1PFMCParticle::~CbmL1PFMCParticle()
{
}

void CbmL1PFMCParticle::AddDaughter(CbmL1MCTrack* mcTrack)
{
  Daughters.push_back(mcTrack);
}

void CbmL1PFMCParticle::AddDaughter( int i )
{
  fDaughterIds.push_back(i);
}

void CbmL1PFMCParticle::CalculateIsReconstructable(unsigned int nDaughters)
{
  if(Daughters.size() < nDaughters) {
    isReconstructable = 0;
  }
  else
  {
    isReconstructable = 1;
    for(unsigned int iV=0; iV<Daughters.size(); iV++)
      isReconstructable = isReconstructable && Daughters[iV]->IsReconstructable();
  }
}

void CbmL1PFMCParticle::CalculateIsRecRec(unsigned int nDaughters)
{
  CalculateIsReconstructable(nDaughters);
  isRecRec = isReconstructable;
  if(Daughters.size() != 0)
    for(unsigned int iV=0; iV<Daughters.size(); iV++)
      isRecRec = isRecRec && Daughters[iV]->IsReconstructed();
}

void CbmL1PFMCParticle::AddDaughterCandidates(vector< CbmL1MCTrack* >  &mcTracks)
{
  McVector mcVectorTemp;
  mcVectorTemp.mcTracks = mcTracks;
  mcDaughtersCandidates.push_back(mcVectorTemp);
}

void CbmL1PFMCParticle::FindCommonMC()
{
/*  std::cout << "pervyj track  ";
  for(unsigned int iMC=0; iMC<mcDaughtersCandidates[0].mcTracks.size(); iMC++)
    std::cout << mcDaughtersCandidates[0].mcTracks[iMC]->mother_ID<<"  ";
  std::cout<<std::endl;
  std::cout << "vtoroj track  ";
  for(unsigned int iMC=0; iMC<mcDaughtersCandidates[1].mcTracks.size(); iMC++)
    std::cout << mcDaughtersCandidates[1].mcTracks[iMC]->mother_ID<<"  ";
  std::cout<<std::endl;*/
  vector<McTrackFind> AllCandidates;
  int nVectors = mcDaughtersCandidates.size();
  int nStart=0;
  for(int iV=0; iV<nVectors; iV++)
  {
    for(unsigned int iMC=0; iMC<mcDaughtersCandidates[iV].mcTracks.size(); iMC++)
    {
      McTrackFind tempCandidat;
      tempCandidat.mcTrack = mcDaughtersCandidates[iV].mcTracks[iMC];
      tempCandidat.vNumber = iV;
      bool CorrectMC = 1;
      for(unsigned int i=nStart; i<AllCandidates.size(); i++)
        if(AllCandidates[i].mcTrack == tempCandidat.mcTrack) CorrectMC = 0;
      if(CorrectMC) AllCandidates.push_back(tempCandidat);
    }
    nStart += mcDaughtersCandidates[iV].mcTracks.size();
  }
  for(unsigned int i=0; i<AllCandidates.size(); i++)
  {
    int mother_id = AllCandidates[i].mcTrack->mother_ID;
    int vect = AllCandidates[i].vNumber;

    vector<int> MotherIDs;
    MotherIDs.resize(nVectors);
    vector<CbmL1MCTrack*> Daughters_temp;
    Daughters_temp.resize(nVectors);
    for(int iV=0; iV<nVectors; iV++) 
    {
      MotherIDs[iV] = -1;
      Daughters_temp[iV] = 0;
    }

    for(unsigned int j=0; j<AllCandidates.size(); j++)
    {
      if(AllCandidates[j].mcTrack->mother_ID == mother_id)
      {
        MotherIDs[AllCandidates[j].vNumber] = mother_id;
        MotherIDs[vect] = mother_id;
      }
    }
    bool IsSame = 1;
    for(int iV=1; iV<nVectors; iV++)
      if(MotherIDs[iV] != MotherIDs[0]) IsSame = 0;
    if(IsSame) 
    {
      mcTrackID = MotherIDs[0];
      for(unsigned int iV=0; iV<Daughters_temp.size(); iV++)
        Daughters.push_back(Daughters_temp[iV]);
    }
  }
}

