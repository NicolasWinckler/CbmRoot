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

#ifndef _CbmL1PFFitter_h_
#define _CbmL1PFFitter_h_

#include "CbmL1Track.h"
#include "CbmL1Def.h"

#include <vector>

class L1TrackPar;
class L1UMeasurementInfo;
class L1MaterialInfo;
class L1Station;

class CbmL1PFFitter
{
 public:
  CbmL1PFFitter();
  ~CbmL1PFFitter();

  void AddMaterial( L1TrackPar &T, L1MaterialInfo &info, fvec qp0, fvec &mass2 );
  void AddPipeMaterial( L1TrackPar &T, fvec qp0, fvec &mass2 );
  void FilterFirst( L1TrackPar &track,fvec &x, fvec &y, fvec &w, L1Station &st );
  void FilterLast ( L1TrackPar &track,fvec &x, fvec &y, fvec &w, L1Station &st );
  void Filter( L1TrackPar &T, L1UMeasurementInfo &info, fvec &u , fvec &w);

  void Fit(vector<CbmL1Track> &Tracks, fvec mass2 = 0.1395679f*0.1395679f);
};
#endif
