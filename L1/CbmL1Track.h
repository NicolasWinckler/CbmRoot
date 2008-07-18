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
 *  L1 track class
 *
 *====================================================================
 */

#ifndef CbmL1Track_H
#define CbmL1Track_H

#include "CbmL1TrackPar.h"

#include <vector>

class L1Triplet;
class L1StsHit;

struct CbmL1Track: public CbmL1TrackPar
{

  int    GetNOfHits(){ return StsHits.size(); }

  std::vector<int> StsHits;

  double TLast[6], CLast[15];

  int nStations;

  bool is_long;

  double Ttmp[6], Ctmp[15]; // temporary variables

  static bool compareChi2(const CbmL1Track &a, const CbmL1Track &b){
    return (a.chi2 < b.chi2);
  }
  static bool comparePChi2(const CbmL1Track *a, const CbmL1Track *b){
    return (a->chi2 < b->chi2);
  }
};

#endif
