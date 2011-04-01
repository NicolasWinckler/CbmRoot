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

#include <vector>
using std::vector;

class CbmL1MCTrack;

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

  void AddDaughter( int i );
  int  NDaughters() const { return fDaughterIds.size(); };
  const vector<int>&  GetDaughterIds() const { return fDaughterIds; };
  void FindCommonMC();

  void SetPDG(int pdg) {fPDG = pdg;}
  void SetMCTrackID(int id) {fMCTrackID = id;}
  void SetMotherId(int id) {fMotherId = id;}
  
  int  GetMCTrackID()      const {return fMCTrackID;}
  int  GetMotherId()       const {return fMotherId;}
  int  GetPDG()            const {return fPDG;}
  bool IsReconstructable() const {return fIsReconstructable;}
  void SetAsReconstructable() { fIsReconstructable = 1;}
  
  void CalculateIsReconstructable(unsigned int NDaughters = 2);
  void CalculateIsRecRec(unsigned int NDaughters = 2);
  
 private: //data
  vector<int> fDaughterIds;
  int fMCTrackID; // sim id of MC track, which corresponds to the particle
  int fMotherId;  // index in L1 array of mother particle
  int fPDG;
  
  bool fIsReconstructable;
};

struct CbmL1TrackMatch // used for Reco to MC match as well as for MC to Reco
{
  bool IsMatched() const { return ids.size() != 0 || idsMI.size() != 0; };
  int  GetBestMatch() const { 
    if      (ids.size()   != 0) return ids[0];
    else if (idsMI.size() != 0) return idsMI[0];
    else return -1;
  };
  vector<int> ids;
  vector<int> idsMI; // matched but pdg is different - miss identification
};

#endif

