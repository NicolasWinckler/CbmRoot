#include "L1Algo.h"
#include "L1TrackPar.h"
#include "L1Branch.h"
#include "L1Track.h"
#include "L1Extrapolation.h"
#include "L1Filtration.h"
#include "L1AddMaterial.h"
#include "L1HitPoint.h"

#include <iostream>

// using namespace std;
using std::cout;
using std::endl;
using std::vector;



   /// Fit track
   /// t - track with hits
   /// T - track params
   /// dir - 0 - forward, 1 - backward
   /// qp0 - momentum for extrapolation
   /// initialize - should be params ititialized. 1 - yes.
void L1Algo::BranchFitterFast(const L1Branch &t, L1TrackPar& T, const bool dir, const fvec qp0, const bool initParams)
{
  L1_assert(t.StsHits.size() >= 3);
  
    // get hits of current track
  const std::vector<THitI>& hits = t.StsHits; // array of indeses of hits of current track
  const int nHits = t.StsHits.size();

  const signed short int step = -2*static_cast<int>(dir) + 1; // increment for station index
  const int iFirstHit = (dir) ? nHits-1 : 0;
  const int iLastHit  = (dir) ? 0 : nHits-1;
  
  L1StsHit &hit0 = vStsHits[hits[iFirstHit]];
  L1StsHit &hit1 = vStsHits[hits[iFirstHit + step]];
  L1StsHit &hit2 = vStsHits[hits[iFirstHit + 2*step]];

  int ista0 = GetFStation(vSFlag[hit0.f]);
  int ista1 = GetFStation(vSFlag[hit1.f]);
  int ista2 = GetFStation(vSFlag[hit2.f]);

  L1Station &sta0 = vStations[ista0];
  L1Station &sta1 = vStations[ista1];
  L1Station &sta2 = vStations[ista2];

  fvec u0  = static_cast<fscal>( vStsStrips[hit0.f] );
  fvec v0  = static_cast<fscal>( vStsStripsB[hit0.b] );
  fvec x0,y0;
  StripsToCoor(u0, v0, x0, y0, sta1);
  fvec z0 = vStsZPos[hit0.iz];

  fvec u1  = static_cast<fscal>( vStsStrips[hit1.f] );
  fvec v1  = static_cast<fscal>( vStsStripsB[hit1.b] );
  fvec x1,y1;
  StripsToCoor(u1, v1, x1, y1, sta1);
  fvec z1 = vStsZPos[hit1.iz];

  fvec u2  = static_cast<fscal>( vStsStrips[hit2.f] );
  fvec v2  = static_cast<fscal>( vStsStripsB[hit2.b] );
  fvec x2,y2;
  StripsToCoor(u2, v2, x2, y2, sta2);
  fvec z2 = vStsZPos[hit2.iz];

  fvec dzi = 1./(z1-z0);

  const fvec vINF = .1;
  T.x  = x0;
  T.y  = y0;
  if( initParams ){
    T.tx = (x1-x0)*dzi;
    T.ty = (y1-y0)*dzi;
    T.qp = qp0;
  }

  T.z  = z0;
  T.chi2 = 0.;
  T.NDF = 2.;
  T.C00 = sta0.XYInfo.C00;
  T.C10 = sta0.XYInfo.C10;
  T.C11 = sta0.XYInfo.C11;

  T.C20 = T.C21 = 0;
  T.C30 = T.C31 = T.C32 = 0;
  T.C40 = T.C41 = T.C42 = T.C43 = 0;
  T.C22 = T.C33 = vINF;
  T.C44 = 1.;

  static L1FieldValue fB0, fB1, fB2 _fvecalignment;
  static L1FieldRegion fld _fvecalignment;
  fvec fz0 = sta1.z; // suppose field is smoth
  fvec fz1 = sta2.z;
  fvec fz2 = sta0.z;


  sta1.fieldSlice.GetFieldValue( x1, y1, fB0 );
  sta2.fieldSlice.GetFieldValue( x2, y2, fB1 );
  sta0.fieldSlice.GetFieldValue( x0, y0, fB2 );

  fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );

  int ista_prev = ista1;
  int ista = ista2;

  for( int i = iFirstHit + step; step*i <= step*iLastHit; i += step){
    L1StsHit &hit = vStsHits[hits[i]];
    ista_prev = ista;
    ista = GetFStation(vSFlag[hit.f]);

    L1Station &sta = vStations[ista];
          
    L1Extrapolate( T, vStsZPos[hit.iz], qp0, fld );
    L1AddMaterial( T, sta.materialInfo, qp0 );
    if ( (step*ista <= step*(NMvdStations + (step+1)/2 - 1)) && (step*ista_prev >= step*(NMvdStations + (step+1)/2 - 1 - step)) )
      L1AddPipeMaterial( T, qp0 );
  
    fvec u = static_cast<fscal>( vStsStrips[hit.f] );
    fvec v = static_cast<fscal>( vStsStripsB[hit.b] );
    L1Filter( T, sta.frontInfo, u );
    L1Filter( T, sta.backInfo,  v );
    fB0 = fB1;
    fB1 = fB2;
    fz0 = fz1;
    fz1 = fz2;
    fvec x,y;
    StripsToCoor(u, v, x, y, sta);
    sta.fieldSlice.GetFieldValue( x, y, fB2 );

    fz2 = sta.z;
    fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );
  } // i

} // void L1Algo::BranchFitterFast

  /// like BranchFitterFast but more precise
void L1Algo::BranchFitter(const L1Branch &t, L1TrackPar& T, const bool dir, const fvec qp0, const bool initParams)
{
  BranchFitterFast (t, T, dir, qp0, initParams);
  for (int i = 0; i < 1; i++) {
    BranchFitterFast (t, T, !dir, T.qp, false);
    BranchFitterFast (t, T, dir, T.qp, false);  
  }
} // void L1Algo::BranchFitter

   /// Find additional hits for existing track
   /// t - track with hits
   /// T - track params
   /// dir - 0 - forward, 1 - backward
   /// qp0 - momentum for extrapolation
   /// initialize - should be params ititialized. 1 - yes.
void L1Algo::FindMoreHits(L1Branch &t, L1TrackPar& T, const bool dir, const fvec qp0) // TODO take into account pipe
{
  std::vector<THitI> newHits;
  newHits.clear();
  
  const signed short int step = - 2*static_cast<int>(dir) + 1; // increment for station index
  const int iFirstHit = (dir) ? 2 : t.StsHits.size()-3;
//  int ista = GetFStation(vSFlag[vStsHits[t.StsHits[iFirstHit]].f]) + 2*step; // current station. set to the end of track

  const L1StsHit &hit0 = vStsHits[t.StsHits[iFirstHit         ]]; // optimize
  const L1StsHit &hit1 = vStsHits[t.StsHits[iFirstHit + step  ]];
  const L1StsHit &hit2 = vStsHits[t.StsHits[iFirstHit + 2*step]];

  const int ista0 = GetFStation(vSFlag[hit0.f]);
  const int ista1 = GetFStation(vSFlag[hit1.f]);
  const int ista2 = GetFStation(vSFlag[hit2.f]);

  const L1Station &sta0 = vStations[ista0];
  const L1Station &sta1 = vStations[ista1];
  const L1Station &sta2 = vStations[ista2];

  fvec u0  = static_cast<fscal>( vStsStrips[hit0.f] );
  fvec v0  = static_cast<fscal>( vStsStripsB[hit0.b] );
  fvec x0,y0;

  StripsToCoor(u0, v0, x0, y0, sta1);
  fvec z0 = vStsZPos[hit0.iz];

  fvec u1  = static_cast<fscal>( vStsStrips[hit1.f] );
  fvec v1  = static_cast<fscal>( vStsStripsB[hit1.b] );
  fvec x1,y1;
  StripsToCoor(u1, v1, x1, y1, sta1);
  fvec z1 = vStsZPos[hit1.iz];

  fvec u2  = static_cast<fscal>( vStsStrips[hit2.f] );
  fvec v2  = static_cast<fscal>( vStsStripsB[hit2.b] );
  fvec x2,y2;
  StripsToCoor(u2, v2, x2, y2, sta2);
  fvec z2 = vStsZPos[hit2.iz];

  fvec dzi = 1./(z1-z0);

  static L1FieldValue fB0, fB1, fB2 _fvecalignment;
  static L1FieldRegion fld _fvecalignment;
  fvec fz0 = sta1.z;
  fvec fz1 = sta2.z;
  fvec fz2 = sta0.z;

  sta1.fieldSlice.GetFieldValue( x1, y1, fB0 );
  sta2.fieldSlice.GetFieldValue( x2, y2, fB1 );
  sta0.fieldSlice.GetFieldValue( x0, y0, fB2 );

  fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );
  
  for( int ista = ista2 + 2*step; (ista < NStations) && (ista >= 0); ista += step ){ // CHECKME why ista2?

    L1Station &sta = vStations[ista];
          
    L1Extrapolate( T, sta.z, qp0, fld );

    fscal r2_best = 1e8; // best distance to hit
    int iHit_best = -1;  // index of the best hit
    
      // find first hit
    int start = StsHitsUnusedStartIndex[ista];
    {
      int end = StsHitsUnusedStopIndex[ista];
      for( THitI ih = start; end - start >= 3; ih++ ){ // optimize
        int middle = (start + end)/2;
        L1StsHit &hit = (*vStsHitsUnused)[middle];

        fscal x, y, z;
        GetHitCoor(hit, x, y, z, sta);
      
        L1TrackPar T_new = T;
        L1ExtrapolateShort( T_new, z, qp0, fld);

        fscal dym_est = ( Pick_gather*sqrt(fabs(T_new.C11+sta.XYInfo.C11)) )[0];
        fscal y_minus_new = T_new.y[0] - dym_est;
        if (y < y_minus_new) start = middle;
        else end = middle;
      }
    }
    for( THitI ih = start; ih < StsHitsUnusedStopIndex[ista]; ih++ ){ // optimize
      L1StsHit &hit = (*vStsHitsUnused)[ih];

      if( GetFUsed( vSFlag[hit.f] | vSFlagB[hit.b] ) ) continue; // if used

      fscal x, y, z;
      GetHitCoor(hit, x, y, z, sta);
      
      L1TrackPar T_new = T;
      L1ExtrapolateShort( T_new, z, qp0, fld);

      fscal dym_est = ( Pick_gather*sqrt(fabs(T_new.C11+sta.XYInfo.C11)) )[0];
      fscal y_minus_new = T_new.y[0] - dym_est;
      if (y < y_minus_new) continue;  // CHECKME tack into account overlaping?
      fscal y_plus_new = T_new.y[0] + dym_est;
      if (y > y_plus_new ) break;
      
      fscal dxm_est = ( Pick_gather*sqrt(fabs(T_new.C00+sta.XYInfo.C00)) )[0];
      fscal x_minus_new = T_new.x[0] - dxm_est;
      fscal x_plus_new  = T_new.x[0] + dxm_est;
      if ((x < x_minus_new) || (x > x_plus_new)) continue;
      fscal dx = x - T_new.x[0];
      fscal dy = y - T_new.y[0];
      fscal d2 = dx*dx + dy*dy;
      if( d2 > r2_best ) continue;
      r2_best = d2;
      iHit_best = ih;
    }
    if( iHit_best < 0 ) break;
    
    newHits.push_back(RealIHit[iHit_best]);

    L1StsHit &hit = (*vStsHitsUnused)[iHit_best];
    fvec u = static_cast<fvec>(vStsStrips[hit.f]);
    fvec v = static_cast<fvec>(vStsStripsB[hit.b]);
    fvec x, y, z;
    StripsToCoor(u, v, x, y, sta);
    z = vStsZPos[hit.iz];
      
    L1ExtrapolateShort( T, z, qp0, fld);
    L1AddMaterial( T, sta.materialInfo, qp0 );
    L1Filter( T, sta.frontInfo, u );
    L1Filter( T, sta.backInfo,  v );

    fB0 = fB1;
    fB1 = fB2;
    fz0 = fz1;
    fz1 = fz2;
    sta.fieldSlice.GetFieldValue( x, y, fB2 );
    fz2 = sta.z;
    fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );
  }

    // save hits
  if (dir) { // backward
    std::vector<THitI> oldHits;
    oldHits.clear();
    oldHits.resize(t.StsHits.size());
    for (unsigned int i = 0; i < t.StsHits.size(); i++) { 
      oldHits[i] = t.StsHits[i];
    }
    t.StsHits.clear();
    for (int i = newHits.size()-1; i >= 0 ; i--) { 
      t.StsHits.push_back(newHits[i]);
    }
    for (unsigned int i = 0; i < oldHits.size(); i++) { 
      t.StsHits.push_back(oldHits[i]);
    }
  }
  else { // forward
    for (unsigned int i = 0; i < newHits.size(); i++) { 
      t.StsHits.push_back(newHits[i]);
    }
  }

} // void L1Algo::FindMoreHits

  /// Try to extrapolate and find additional hits on other stations
fscal L1Algo::BranchExtender(L1Branch &t) // TODO Simdize
{
  //  const unsigned int minNHits = 3;
  
  L1TrackPar T;

    // forward
  bool dir = 0;

  BranchFitter (t, T, dir);
  // BranchFitterFast (t, T, dir, 0, true);
  
//  if (t.StsHits.size() < minNHits) return T.chi2[0];
  FindMoreHits(t, T, dir, T.qp);

    // backward
  dir = 1;
  BranchFitterFast (t, T, dir, T.qp, false); // 577


  FindMoreHits(t, T, dir, T.qp);

  return T.chi2[0];
}
