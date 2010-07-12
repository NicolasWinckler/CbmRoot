/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  L1 Monte Carlo information
 *
 *====================================================================
 */

#ifndef CbmL1MCTrack_H
#define CbmL1MCTrack_H

#include "CbmL1.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "CbmL1MCPoint.h"
#include <vector>
using std::vector;

class CbmL1Track;

class CbmL1MCTrack
{
 public:
  double mass, q, p, x, y, z, px, py, pz;
  int ID, mother_ID, pdg;
  vector<int> Points;  // indices of pints in L1::vMCPoints
  vector<int> StsHits; // indices of hits in algo->vStsHits or L1::vStsHits

  CbmL1MCTrack(){};
  CbmL1MCTrack(int _ID):ID(_ID){};
  CbmL1MCTrack(double mass, double q, TVector3 vr, TLorentzVector vp, int ID, int mother_ID, int pdg);
//   CbmL1MCTrack(TmpMCPoints &mcPoint, TVector3 vr, TLorentzVector vp, int ID, int mother_ID);

  bool IsPrimary(){return mother_ID < 0;};
  bool IsReconstructable(){return isReconstructable;};
  int  NStations(){return nStations;};
  
  void Init();

  void AddRecoTrack(CbmL1Track* rTr){rTracks.push_back(rTr);}
  vector< CbmL1Track* >&  GetRecoTracks(){ return rTracks;}
  int  GetNClones(){ return rTracks.size() - 1;}
  bool IsReconstructed(){ return rTracks.size(); }
  
  void AddTouchTrack(CbmL1Track* tTr){tTracks.push_back(tTr);}
  bool IsDisturbed(){ return tTracks.size(); }

  friend class CbmL1;
 private:
   int nMCContStations;   // number of consecutive stations with mcPoints
   int nHitContStations;  // number of consecutive stations with hits
   int maxNStaMC;         // max number of mcPoints on station
   int maxNSensorMC;         // max number of mcPoints with same z
   int maxNStaHits;       // max number of hits on station
   
   int nStations;         // number of stations with hits

   bool isReconstructable;
   
  void CalculateMCCont();
  void CalculateHitCont();
  void CalculateMaxNStaHits();
  void CalculateMaxNStaMC();
  void CalculateIsReconstructable();

    // next members filled and used in Performance
  vector< CbmL1Track* >  rTracks; // array of assosiated recoTracks
  vector< CbmL1Track* >  tTracks; // array of recoTracks wich aren't assosiated with this mcTrack, but use some hits from it.

};


#endif
