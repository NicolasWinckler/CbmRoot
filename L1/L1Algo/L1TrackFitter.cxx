/*
 *====================================================================
 *
 *  SIMD Kalman filter(KF) track fitter
 *
 *  Authors: M.Zyzak, I.Kulakov
 *  Primary authors: I.Kisel, S.Gorbunov
 *
 *  e-mail : I.Kisel@gsi.de, M.Zyzak@gsi.de
 *
 *====================================================================
 */



#include "L1Algo/L1Algo.h"
#include "L1Algo/L1TrackPar.h"
#include "L1Algo/L1Extrapolation.h"
#include "L1Algo/L1AddMaterial.h"
#include "L1Algo/L1Filtration.h" // for KFTrackFitter_simple

#include "TStopwatch.h"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;


namespace NS_L1TrackFitter{
  const fvec c_light = 0.000299792458, c_light_i = 1./c_light;
  const fvec ZERO = 0.;
  const fvec ONE = 1.;
  const fvec vINF = 0.1;
}
using namespace NS_L1TrackFitter;

  /// Fit reconstracted track like it fitted during the reconstruction.
void L1Algo::KFTrackFitter_simple()  // TODO: Add pipe.
{
//  cout << " Start KF Track Fitter " << endl;
  int start_hit = 0; // for interation in vRecoHits[]

  for(unsigned itrack = 0; itrack < vTracks.size(); itrack++)
  {
    L1Track &t =  vTracks[itrack]; // current track

    // get hits of current track
    std::vector<unsigned short int> hits; // array of indeses of hits of current track
    hits.clear();
    int nHits = t.NHits;
    for( int i = 0; i < nHits; i++ ){
      hits.push_back( vRecoHits[start_hit++]);
    }

    L1TrackPar T; // fitting parametr coresponding to current track

    fvec qp0 = 0;
    //fvec qp0 = 2./t.Momentum;
    for(int iter=0; iter<3; iter++ )
    {
      //cout<<" Back 1"<<endl;
      { // fit backward
        L1StsHit &hit0 = vStsHits[hits[nHits-1]];
        L1StsHit &hit1 = vStsHits[hits[nHits-2]];
        L1StsHit &hit2 = vStsHits[hits[nHits-3]];

        int ista0 = vSFlag[hit0.f]/4;
        int ista1 = vSFlag[hit1.f]/4;
        int ista2 = vSFlag[hit2.f]/4;

      //cout<<"back: ista012="<<ista0<<" "<<ista1<<" "<<ista2<<endl;
        L1Station &sta0 = vStations[ista0];
        L1Station &sta1 = vStations[ista1];
        L1Station &sta2 = vStations[ista2];

        fvec u0  = (fscal)vStsStrips[hit0.f];
        fvec v0  = (fscal)vStsStripsB[hit0.b];
        fvec x0,y0;
        StripsToCoor(u0, v0, x0, y0, sta1);
        fvec z0 = vStsZPos[hit0.iz];

        fvec u1  = (fscal)vStsStrips[hit1.f];
        fvec v1  = (fscal)vStsStripsB[hit1.b];
        fvec x1,y1;
        StripsToCoor(u1, v1, x1, y1, sta1);
        fvec z1 = vStsZPos[hit1.iz];

        fvec u2  = (fscal)vStsStrips[hit2.f];
        fvec v2  = (fscal)vStsStripsB[hit2.b];
        fvec x2,y2;
        StripsToCoor(u2, v2, x2, y2, sta2);
        fvec z2 = vStsZPos[hit2.iz];

        fvec dzi = 1./(z1-z0);

        const fvec vINF = .1;
        T.x  = x0;
        T.y  = y0;
        if( iter==0 ){
          T.tx = (x1-x0)*dzi;
          T.ty = (y1-y0)*dzi;
        }

        T.qp = qp0;
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

        int ista = ista2;
      //cout<<"\nfit, iter=:"<<iter<<endl;
        for( int i = nHits-2; i >= 0; i--){
  //  if( fabs(T.qp[0])>2. ) break;  // iklm. Don't know it need for
          L1StsHit &hit = vStsHits[hits[i]];
          ista = vSFlag[hit.f]/4;

          L1Station &sta = vStations[ista];
          
          L1Extrapolate( T, vStsZPos[hit.iz], qp0, fld );
//           T.L1Extrapolate( sta.z, qp0, fld );
//         L1Extrapolate( T, hit.z, qp0, fld );
          L1AddMaterial( T, sta.materialInfo, qp0 );
//         if (ista==NMvdStations-1) L1AddPipeMaterial( T, qp0);

          fvec u = (fscal)vStsStrips[hit.f];
          fvec v = (fscal)vStsStripsB[hit.b];
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

      // write received parametres in track
        t.TFirst[0] = T.x[0];
        t.TFirst[1] = T.y[0];
        t.TFirst[2] = T.tx[0];
        t.TFirst[3] = T.ty[0];
        t.TFirst[4] = T.qp[0];
        t.TFirst[5] = T.z[0];

        t.CFirst[0] = T.C00[0];
        t.CFirst[1] = T.C10[0];
        t.CFirst[2] = T.C11[0];
        t.CFirst[3] = T.C20[0];
        t.CFirst[4] = T.C21[0];
        t.CFirst[5] = T.C22[0];
        t.CFirst[6] = T.C30[0];
        t.CFirst[7] = T.C31[0];
        t.CFirst[8] = T.C32[0];
        t.CFirst[9] = T.C33[0];
        t.CFirst[10] = T.C40[0];
        t.CFirst[11] = T.C41[0];
        t.CFirst[12] = T.C42[0];
        t.CFirst[13] = T.C43[0];
        t.CFirst[14] = T.C44[0];

        t.chi2 = T.chi2[0];
        t.NDF = (int)T.NDF[0];
        qp0 = T.qp[0];
      } // fit backward

      // fit forward
      {
  //T.qp = first_trip->GetQpOrig(MaxInvMom);

        L1StsHit &hit0 = vStsHits[hits[0]];
        L1StsHit &hit1 = vStsHits[hits[1]];
        L1StsHit &hit2 = vStsHits[hits[2]];

        int ista0 = GetFStation( vSFlag[hit0.f] );
        int ista1 = GetFStation( vSFlag[hit1.f] );
        int ista2 = GetFStation( vSFlag[hit2.f] );

        L1Station &sta0 = vStations[ista0];
        L1Station &sta1 = vStations[ista1];
        L1Station &sta2 = vStations[ista2];

        fvec u0  = (fscal)vStsStrips[hit0.f];
        fvec v0  = (fscal)vStsStripsB[hit0.b];
        fvec x0,y0;
        StripsToCoor(u0, v0, x0, y0, sta1);
        fvec z0 = vStsZPos[hit0.iz];

        fvec u1  = (fscal)vStsStrips[hit1.f];
        fvec v1  = (fscal)vStsStripsB[hit1.b];
        fvec x1,y1;
        StripsToCoor(u1, v1, x1, y1, sta1);
        fvec z1 = vStsZPos[hit1.iz];

        fvec u2  = (fscal)vStsStrips[hit2.f];
        fvec v2  = (fscal)vStsStripsB[hit2.b];
        fvec x2,y2;
        StripsToCoor(u2, v2, x2, y2, sta2);
        fvec z2 = vStsZPos[hit2.iz];

        fvec dzi = 1./(z1-z0);

  //fvec qp0 = first_trip->GetQpOrig(MaxInvMom);

        const fvec vINF = .1;
        T.chi2 = 0.;
        T.NDF = 2.;
        T.x  = x0;
        T.y  = y0;
//         T.tx = (x1-x0)*dzi;
//         T.ty = (y1-y0)*dzi;
//         qp0 = 0;
        T.qp = qp0;
        T.z  = z0;
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
        fvec fz0 = sta1.z;
        fvec fz1 = sta2.z;
        fvec fz2 = sta0.z;

        sta1.fieldSlice.GetFieldValue( x1, y1, fB0 );
        sta2.fieldSlice.GetFieldValue( x2, y2, fB1 );
        sta0.fieldSlice.GetFieldValue( x0, y0, fB2 );

        fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );
        int ista = ista2;

        for( int i=1; i<nHits; i++){
          L1StsHit &hit = vStsHits[hits[i]];
          ista = vSFlag[hit.f]/4;
          L1Station &sta = vStations[ista];
          fvec u = (fscal)vStsStrips[hit.f];
          fvec v = (fscal)vStsStripsB[hit.b];

          L1Extrapolate( T, vStsZPos[hit.iz], qp0, fld );
//           T.L1Extrapolate( sta.z, qp0, fld );
//           L1Extrapolate( T, hit.z, qp0, fld );
          L1AddMaterial( T, sta.materialInfo, qp0 );
//           if (ista==NMvdStations) L1AddPipeMaterial( T, qp0);
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
        }

  // write received parametres in track
        t.TLast[0] = T.x[0];
        t.TLast[1] = T.y[0];
        t.TLast[2] = T.tx[0];
        t.TLast[3] = T.ty[0];
        t.TLast[4] = T.qp[0];
        t.TLast[5] = T.z[0];

        t.CLast[0] = T.C00[0];
        t.CLast[1] = T.C10[0];
        t.CLast[2] = T.C11[0];
        t.CLast[3] = T.C20[0];
        t.CLast[4] = T.C21[0];
        t.CLast[5] = T.C22[0];
        t.CLast[6] = T.C30[0];
        t.CLast[7] = T.C31[0];
        t.CLast[8] = T.C32[0];
        t.CLast[9] = T.C33[0];
        t.CLast[10] = T.C40[0];
        t.CLast[11] = T.C41[0];
        t.CLast[12] = T.C42[0];
        t.CLast[13] = T.C43[0];
        t.CLast[14] = T.C44[0];

        t.chi2 += T.chi2[0];
        t.NDF += (int)T.NDF[0];
      }
      qp0 = T.qp[0];
    }
  } // for(int itrack
}


void L1Algo::KFTrackFitter() // TODO: works only for same-z. Add pipe.
{
//  cout << " Start L1 Track Fitter " << endl;
  int start_hit = 0; // for interation in vRecoHits[]

  static L1FieldValue fB0, fB1, fB2 _fvecalignment;
  static L1FieldRegion fld _fvecalignment;

  static int nHits = NStations;
  int iVec=0, i=0;
  int nTracks_SIMD = fvecLen;
  L1TrackPar T; // fitting parametr coresponding to current track

  L1Track *t[fvecLen];

  int ista;
  L1Station *sta = vStations;
  fvec x[nHits], v[nHits], y[nHits], z[nHits];
  fvec w[nHits];
  fvec y_temp;
  fvec fz0, fz1, fz2, dz, z_start, z_end;
  L1FieldValue fB[nHits], fB_temp _fvecalignment;

  fvec ZSta[nHits];
  for(int iHit = 0; iHit<nHits; iHit++)
  {
    ZSta[iHit] = sta[iHit].z;
  }

  unsigned N_vTracks = vTracks.size();

  for(unsigned itrack = 0; itrack < N_vTracks; itrack+=fvecLen)
  {
    if(N_vTracks - itrack < fvecLen)
      nTracks_SIMD = N_vTracks - itrack;

    for(i=0; i<nTracks_SIMD; i++)
      t[i] = & vTracks[itrack+i]; // current track

    // get hits of current track
    for(i=0; i<nHits; i++)
    {
      w[i] = ZERO;
      z[i] = ZSta[i];
    }

    for(iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      int nHitsTrack = t[iVec]->NHits;
      for(i = 0; i < nHitsTrack; i++ )
      {
        L1StsHit &hit = vStsHits[vRecoHits[start_hit++]];
        ista = vSFlag[hit.f]/4;
        w[ista][iVec] = 1.;

        x[ista][iVec]  = vStsStrips[hit.f] ;
        v[ista][iVec]  = vStsStripsB[hit.b];
        y_temp         = sta[ista].yInfo.cos_phi*x[ista][iVec] + sta[ista].yInfo.sin_phi*v[ista][iVec];
        y[ista][iVec]  = y_temp[iVec];
//         z[ista][iVec]  = hit.z;
        z[ista][iVec]  = sta[ista].z[0];
        sta[ista].fieldSlice.GetFieldValue( x[ista], y[ista], fB_temp );
        fB[ista].x[iVec] = fB_temp.x[iVec];
        fB[ista].y[iVec] = fB_temp.y[iVec];
        fB[ista].z[iVec] = fB_temp.z[iVec];
//         if(i == 0) z_start[iVec] = hit.z;
//         if(i == nHitsTrack) z_end[iVec] = hit.z;
        if(i == 0) z_start[iVec] = sta[ista].z[0];
        if(i == nHitsTrack) z_end[iVec] = sta[ista].z[0];
      }
    }

//fit backward

    GuessVec( T, x, y, z, w, nHits);

    i= nHits-1;

    FilterFirst( T, x[i],y[i],w[i], sta[i] );
    L1AddMaterial( T, sta[i].materialInfo, T.qp );

    fz1 = z[i];
    sta[i].fieldSlice.GetFieldValue( T.x, T.y, fB1 );
    fB1.Combine( fB[i], w[i] );

    fz2 = z[i-2];
    dz = fz2-fz1;
    sta[i].fieldSlice.GetFieldValue( T.x + T.tx*dz, T.y + T.ty*dz, fB2 );
    fB2.Combine( fB[i-2], w[i-2] );
    fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );
    for( --i; i>=0; i-- )
    {
      fz0 = z[i];
      dz = (fz1-fz0);
      sta[i].fieldSlice.GetFieldValue( T.x - T.tx*dz, T.y - T.ty*dz, fB0 );
      fB0.Combine( fB[i], w[i] );
      fld.Set( fB0, fz0, fB1, fz1, fB2, fz2 );

      L1Extrapolate( T, z[i], T.qp, fld );
//       T.L1Extrapolate( z[i], T.qp, fld );
      L1AddMaterial( T, sta[i].materialInfo, T.qp );
      Filter( T, sta[i].frontInfo, x[i], w[i] );
      Filter( T, sta[i].backInfo,  v[i], w[i] );
      fB2 = fB1;
      fz2 = fz1;
      fB1 = fB0;
      fz1 = fz0;
    }

    for(iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      t[iVec]->TFirst[0] = T.x[iVec];
      t[iVec]->TFirst[1] = T.y[iVec];
      t[iVec]->TFirst[2] = T.tx[iVec];
      t[iVec]->TFirst[3] = T.ty[iVec];
      t[iVec]->TFirst[4] = T.qp[iVec];
      t[iVec]->TFirst[5] = z_start[iVec];
//      t[iVec]->TFirst[5] = T.z[iVec];

      t[iVec]->CFirst[0] = T.C00[iVec];
      t[iVec]->CFirst[1] = T.C10[iVec];
      t[iVec]->CFirst[2] = T.C11[iVec];
      t[iVec]->CFirst[3] = T.C20[iVec];
      t[iVec]->CFirst[4] = T.C21[iVec];
      t[iVec]->CFirst[5] = T.C22[iVec];
      t[iVec]->CFirst[6] = T.C30[iVec];
      t[iVec]->CFirst[7] = T.C31[iVec];
      t[iVec]->CFirst[8] = T.C32[iVec];
      t[iVec]->CFirst[9] = T.C33[iVec];
      t[iVec]->CFirst[10] = T.C40[iVec];
      t[iVec]->CFirst[11] = T.C41[iVec];
      t[iVec]->CFirst[12] = T.C42[iVec];
      t[iVec]->CFirst[13] = T.C43[iVec];
      t[iVec]->CFirst[14] = T.C44[iVec];

      t[iVec]->chi2 = T.chi2[iVec];
      t[iVec]->NDF = (int)T.NDF[iVec];
    }

    // fit forward

    i= 0;
    FilterLast( T, x[i],y[i],w[i], sta[i] );
    L1AddMaterial( T, sta[i].materialInfo, T.qp );

    fz1 = z[i];
    sta[i].fieldSlice.GetFieldValue( T.x, T.y, fB1 );
    fB1.Combine( fB[i], w[i] );

    fz2 = z[i+2];
    dz = fz2-fz1;
    sta[i].fieldSlice.GetFieldValue( T.x + T.tx*dz, T.y + T.ty*dz, fB2 );
    fB2.Combine( fB[i+2], w[i+2] );
    fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );

    for( ++i; i<nHits; i++ )
    {
      fz0 = z[i];
      dz = (fz1-fz0);
      sta[i].fieldSlice.GetFieldValue( T.x - T.tx*dz, T.y - T.ty*dz, fB0 );
      fB0.Combine( fB[i], w[i] );
      fld.Set( fB0, fz0, fB1, fz1, fB2, fz2 );

      L1Extrapolate( T, z[i], T.qp, fld );
//       T.L1Extrapolate( z[i], T.qp, fld );
      L1AddMaterial( T, sta[i].materialInfo, T.qp );
      Filter( T, sta[i].frontInfo, x[i], w[i] );
      Filter( T, sta[i].backInfo,  v[i], w[i] );

      fB2 = fB1;
      fz2 = fz1;
      fB1 = fB0;
      fz1 = fz0;
    }

    for(iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      t[iVec]->TLast[0] = T.x[iVec];
      t[iVec]->TLast[1] = T.y[iVec];
      t[iVec]->TLast[2] = T.tx[iVec];
      t[iVec]->TLast[3] = T.ty[iVec];
      t[iVec]->TLast[4] = T.qp[iVec];
      t[iVec]->TLast[5] = z_end[iVec];

      t[iVec]->CLast[0] = T.C00[iVec];
      t[iVec]->CLast[1] = T.C10[iVec];
      t[iVec]->CLast[2] = T.C11[iVec];
      t[iVec]->CLast[3] = T.C20[iVec];
      t[iVec]->CLast[4] = T.C21[iVec];
      t[iVec]->CLast[5] = T.C22[iVec];
      t[iVec]->CLast[6] = T.C30[iVec];
      t[iVec]->CLast[7] = T.C31[iVec];
      t[iVec]->CLast[8] = T.C32[iVec];
      t[iVec]->CLast[9] = T.C33[iVec];
      t[iVec]->CLast[10] = T.C40[iVec];
      t[iVec]->CLast[11] = T.C41[iVec];
      t[iVec]->CLast[12] = T.C42[iVec];
      t[iVec]->CLast[13] = T.C43[iVec];
      t[iVec]->CLast[14] = T.C44[iVec];

      t[iVec]->chi2 = T.chi2[iVec];
      t[iVec]->NDF = (int)T.NDF[iVec];
    }
  }
}

void L1Algo::GuessVec( L1TrackPar &t, fvec *xV, fvec *yV, fvec *zV, fvec *wV, int NHits )
{
  fvec A0, A1=ZERO, A2=ZERO, A3=ZERO, A4=ZERO, A5=ZERO, a0, a1=ZERO, a2=ZERO,
  b0, b1=ZERO, b2=ZERO;
  fvec z0, x, y, z, S, w, wz, wS;

  Int_t i=NHits-1;
  z0 = zV[i];
  w = wV[i];
  A0 = w;
  a0 = w*xV[i];
  b0 = w*yV[i];
  L1Station *st = vStations;
  for( i=0 ; i<NHits; i++, st++ ){
    x = xV[i];
    y = yV[i];
    w = wV[i];
    z = zV[i] - z0;
    S = st->Sy;
    wz = w*z;
    wS = w*S;
    A0+=w;
    A1+=wz;  A2+=wz*z;
    A3+=wS;  A4+=wS*z; A5+=wS*S;
    a0+=w*x; a1+=wz*x; a2+=wS*x;
    b0+=w*y; b1+=wz*y; b2+=wS*y;
  }

  fvec A3A3 = A3*A3;
  fvec A3A4 = A3*A4;
  fvec A1A5 = A1*A5;
  fvec A2A5 = A2*A5;
  fvec A4A4 = A4*A4;

  fvec det = rcp(-A2*A3A3 + A1*( A3A4+A3A4 - A1A5) + A0*(A2A5-A4A4));
  fvec Ai0 = ( -A4A4 + A2A5 );
  fvec Ai1 = (  A3A4 - A1A5 );
  fvec Ai2 = ( -A3A3 + A0*A5 );
  fvec Ai3 = ( -A2*A3 + A1*A4 );
  fvec Ai4 = (  A1*A3 - A0*A4 );
  fvec Ai5 = ( -A1*A1 + A0*A2 );

  fvec L, L1;
  t.x = (Ai0*a0 + Ai1*a1 + Ai3*a2)*det;
  t.tx = (Ai1*a0 + Ai2*a1 + Ai4*a2)*det;
  fvec txtx1 = 1.+t.tx*t.tx;
  L    = (Ai3*a0 + Ai4*a1 + Ai5*a2)*det *rcp(txtx1);
  L1 = L*t.tx;
  A1 = A1 + A3*L1;
  A2 = A2 + ( A4 + A4 + A5*L1 )*L1;
  b1+= b2 * L1;
  det = rcp(-A1*A1+A0*A2);

  t.y = (  A2*b0 - A1*b1 )*det;
  t.ty = ( -A1*b0 + A0*b1 )*det;
  t.qp = -L*c_light_i*rsqrt(txtx1 +t.ty*t.ty);
  t.z = z0;
}

void L1Algo::FilterFirst( L1TrackPar &track,fvec &x, fvec &y, fvec &w, L1Station &st )
{
  fvec w1 = ONE - w;
  fvec sigma21 = w*st.XYInfo.C00 + w1*vINF;
  fvec sigma22 = w*st.XYInfo.C10;
  fvec sigma23 = w*st.XYInfo.C11 + w1*vINF;
  // initialize covariance matrix
  track.C00= sigma21;
  track.C10= sigma22;      track.C11= sigma23;
/*  track.C00= vINF;
  track.C10= ZERO;      track.C11= vINF;*/
  track.C20= ZERO;         track.C21= ZERO;      track.C22= vINF;
  track.C30= ZERO;         track.C31= ZERO;      track.C32= ZERO;    track.C33= vINF;
  track.C40= ZERO;         track.C41= ZERO;      track.C42= ZERO;    track.C43= ZERO;     track.C44= vINF;

  track.x = w*x + w1*track.x;
  track.y = w*y + w1*track.y;
  track.NDF = -3.0;
  track.chi2 = ZERO;
}

void L1Algo::FilterLast( L1TrackPar &track,fvec &x, fvec &y, fvec &w, L1Station &st )
{
  fvec w1 = ONE - w;
  fvec sigma21 = w*st.XYInfo.C00 + w1*vINF;
  fvec sigma22 = w*st.XYInfo.C10;
  fvec sigma23 = w*st.XYInfo.C11 + w1*vINF;
  // initialize covariance matrix
  track.C00= sigma21;
  track.C10= sigma22;      track.C11= sigma23;
  track.C20= ZERO;         track.C21= ZERO;      track.C22= vINF;
  track.C30= ZERO;         track.C31= ZERO;      track.C32= ZERO;    track.C33= vINF;
  track.C40= ZERO;         track.C41= ZERO;      track.C42= ZERO;    track.C43= ZERO;     track.C44= ONE;

  track.x = w*x + w1*track.x;
  track.y = w*y + w1*track.y;
  track.NDF = -3.0;
  track.chi2 = ZERO;
}


void L1Algo::Filter( L1TrackPar &T, L1UMeasurementInfo &info, fvec &u , fvec &w)
{
  register fvec wi, zeta, zetawi, HCH;
  register fvec F0, F1, F2, F3, F4;
  register fvec K1, K2, K3, K4;

  zeta = info.cos_phi*T.x + info.sin_phi*T.y - u;

  // F = CH'
  F0 = info.cos_phi*T.C00 + info.sin_phi*T.C10;
  F1 = info.cos_phi*T.C10 + info.sin_phi*T.C11;

  HCH = ( F0*info.cos_phi + F1*info.sin_phi );

  F2 = info.cos_phi*T.C20 + info.sin_phi*T.C21;
  F3 = info.cos_phi*T.C30 + info.sin_phi*T.C31;
  F4 = info.cos_phi*T.C40 + info.sin_phi*T.C41;

  wi = 1./(info.sigma2 +HCH);

  wi *= w;

  zetawi = zeta *wi;
  T.chi2 +=  zeta * zetawi ;
  T.NDF  += w;

  K1 = F1*wi;
  K2 = F2*wi;
  K3 = F3*wi;
  K4 = F4*wi;

  T.x  -= F0*zetawi;
  T.y  -= F1*zetawi;
  T.tx -= F2*zetawi;
  T.ty -= F3*zetawi;
  T.qp -= F4*zetawi;

  T.C00-= F0*F0*wi;
  T.C10-= K1*F0;
  T.C11-= K1*F1;
  T.C20-= K2*F0;
  T.C21-= K2*F1;
  T.C22-= K2*F2;
  T.C30-= K3*F0;
  T.C31-= K3*F1;
  T.C32-= K3*F2;
  T.C33-= K3*F3;
  T.C40-= K4*F0;
  T.C41-= K4*F1;
  T.C42-= K4*F2;
  T.C43-= K4*F3;
  T.C44-= K4*F4;
}
/*
Int_t L1Algo::ExtrapolateRK4
(
 const fvec T_in [], // input track parameters (x,y,tx,ty,Q/p)
 const fvec C_in [], // input covariance matrix
 fvec       T_out[], // output track parameters
 fvec       C_out[], // output covariance matrix
 fvec       z_out  , // extrapolate to this z position
 fvec       qp0    , // use Q/p linearisation at this value
 FairField       *MF       // magnetic field
 )
{
  //
  // Forth-order Runge-Kutta method for solution of the equation
  // of motion of a particle with parameter qp = Q /P
  //              in the magnetic field B()
  //
  //        | x |          tx
  //        | y |          ty
  // d/dz = | tx| = ft * ( ty * ( B(3)+tx*b(1) ) - (1+tx**2)*B(2) )
  //        | ty|   ft * (-tx * ( B(3)+ty+b(2)   - (1+ty**2)*B(1) )  ,
  //
  //   where  ft = c_light*qp*sqrt ( 1 + tx**2 + ty**2 ) .
  //
  //  In the following for RK step  :
  //
  //     x=x[0], y=x[1], tx=x[3], ty=x[4].
  //
  //========================================================================
  //
  //  NIM A395 (1997) 169-184; NIM A426 (1999) 268-282.
  //
  //  the routine is based on LHC(b) utility code
  //
  //========================================================================

  const fvec c_light = 0.000299792458;

  static fvec a[4] = {0.0, 0.5, 0.5, 1.0};
  static fvec c[4] = {1.0/6.0, 1.0/3.0, 1.0/3.0, 1.0/6.0};
  static fvec b[4] = {0.0, 0.5, 0.5, 1.0};

  Int_t step4;
  fvec k[16],x0[4],x[4],k1[16];
  fvec Ax[4],Ay[4],Ax_tx[4],Ay_tx[4],Ax_ty[4],Ay_ty[4];

  //----------------------------------------------------------------

  fvec qp_in = T_in[4];
  fvec z_in  = T_in[5];
  fvec h     = z_out - z_in;
  fvec hC    = h * c_light;
  x0[0] = T_in[0]; x0[1] = T_in[1];
  x0[2] = T_in[2]; x0[3] = T_in[3];
  //
  //   Runge-Kutta step
  //

  Int_t step;
  Int_t i;

  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];
} else {
        x[i] = x0[i] + b[step] * k[step*4-4+i];
}
}

    fvec point[3] = { x[0], x[1], z_in  + a[step] * h };
    fvec B[3];
    if ( MF ) MF->GetFieldValue( point, B );
    else { B[0] = B[1] = B[2] = 0.; }

    fvec tx = x[2];
    fvec ty = x[3];
    fvec tx2 = tx * tx;
    fvec ty2 = ty * ty;
    fvec txty = tx * ty;
    fvec tx2ty21= 1.0 + tx2 + ty2;
    if( tx2ty21 > 1.e4 ) return 1;
    fvec I_tx2ty21 = 1.0 / tx2ty21 * qp0;
    fvec tx2ty2 = sqrt(tx2ty21 ) ;
    //   fvec I_tx2ty2 = qp0 * hC / tx2ty2 ; unsused ???
    tx2ty2 *= hC;
    fvec tx2ty2qp = tx2ty2 * qp0;
    Ax[step] = ( txty*B[0] + ty*B[2] - ( 1.0 + tx2 )*B[1] ) * tx2ty2;
    Ay[step] = (-txty*B[1] - tx*B[2] + ( 1.0 + ty2 )*B[0] ) * tx2ty2;

    Ax_tx[step] = Ax[step]*tx*I_tx2ty21 + (ty*B[0]-2.0*tx*B[1])*tx2ty2qp;
    Ax_ty[step] = Ax[step]*ty*I_tx2ty21 + (tx*B[0]+B[2])*tx2ty2qp;
    Ay_tx[step] = Ay[step]*tx*I_tx2ty21 + (-ty*B[1]-B[2])*tx2ty2qp;
    Ay_ty[step] = Ay[step]*ty*I_tx2ty21 + (-tx*B[1]+2.0*ty*B[0])*tx2ty2qp;

    step4 = step * 4;
    k[step4  ] = tx * h;
    k[step4+1] = ty * h;
    k[step4+2] = Ax[step] * qp0;
    k[step4+3] = Ay[step] * qp0;

}  // end of Runge-Kutta steps

  for(i=0; i < 4; ++i) {
    T_out[i]=x0[i]+c[0]*k[i]+c[1]*k[4+i]+c[2]*k[8+i]+c[3]*k[12+i];
}
  T_out[4]=T_in[4];
  T_out[5]=z_out;
  //
  //     Derivatives    dx/dqp
  //

  x0[0] = 0.0; x0[1] = 0.0; x0[2] = 0.0; x0[3] = 0.0;

  //
  //   Runge-Kutta step for derivatives dx/dqp

  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];
} else {
        x[i] = x0[i] + b[step] * k1[step*4-4+i];
}
}
    step4 = step * 4;
    k1[step4  ] = x[2] * h;
    k1[step4+1] = x[3] * h;
    k1[step4+2] = Ax[step] + Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
    k1[step4+3] = Ay[step] + Ay_tx[step] * x[2] + Ay_ty[step] * x[3];

}  // end of Runge-Kutta steps for derivatives dx/dqp

  fvec J[25];

  for (i = 0; i < 4; ++i ) {
    J[20+i]=x0[i]+c[0]*k1[i]+c[1]*k1[4+i]+c[2]*k1[8+i]+c[3]*k1[12+i];
}
  J[24] = 1.;
  //
  //      end of derivatives dx/dqp
  //

  //     Derivatives    dx/tx
  //

  x0[0] = 0.0; x0[1] = 0.0; x0[2] = 1.0; x0[3] = 0.0;

  //
  //   Runge-Kutta step for derivatives dx/dtx
  //

  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];
} else if ( i!=2 ){
        x[i] = x0[i] + b[step] * k1[step*4-4+i];
}
}
    step4 = step * 4;
    k1[step4  ] = x[2] * h;
    k1[step4+1] = x[3] * h;
    //    k1[step4+2] = Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
    k1[step4+3] = Ay_tx[step] * x[2] + Ay_ty[step] * x[3];

}  // end of Runge-Kutta steps for derivatives dx/dtx

  for (i = 0; i < 4; ++i ) {
    if(i != 2) {
      J[10+i]=x0[i]+c[0]*k1[i]+c[1]*k1[4+i]+c[2]*k1[8+i]+c[3]*k1[12+i];
}
}
  //      end of derivatives dx/dtx
  J[12] = 1.0;
  J[14] = 0.0;

  //     Derivatives    dx/ty
  //

  x0[0] = 0.0; x0[1] = 0.0; x0[2] = 0.0; x0[3] = 1.0;

  //
  //   Runge-Kutta step for derivatives dx/dty
  //

  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];           // ty fixed
} else if(i!=3) {
        x[i] = x0[i] + b[step] * k1[step*4-4+i];
}

}
    step4 = step * 4;
    k1[step4  ] = x[2] * h;
    k1[step4+1] = x[3] * h;
    k1[step4+2] = Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
    //    k1[step4+3] = Ay_tx[step] * x[2] + Ay_ty[step] * x[3];

}  // end of Runge-Kutta steps for derivatives dx/dty

  for (i = 0; i < 3; ++i ) {
    J[15+i]=x0[i]+c[0]*k1[i]+c[1]*k1[4+i]+c[2]*k1[8+i]+c[3]*k1[12+i];
}
  //      end of derivatives dx/dty
  J[18] = 1.;
  J[19] = 0.;

  //
  //    derivatives dx/dx and dx/dy

  for(i = 0; i < 10; ++i){ J[i] = 0.;}
  J[0] = 1.; J[6] = 1.;

  // extrapolate inverse momentum

  T_out[4] = qp_in;

  fvec dqp = qp_in - qp0;

{ for( Int_t i=0; i<4; i++ )
{
      T_out[i]+=J[5*4+i]*dqp;
}
}

  //          covariance matrix transport

  if ( C_in&&C_out ) CbmKFMath::multQtSQ( 5, J, C_in, C_out);
  return 0;
}    // end of RK4

void L1Algo::multQtSQ( Int_t N, const fvec Q[], const fvec S[], fvec S_out[] )
{
  fvec A[N*N];

  for( Int_t i=0, n=0; i<N; i++ ){
    for( Int_t j=0; j<N; j++, ++n ){
      A[n] = 0 ;
      for( Int_t k=0; k<N; ++k ) A[n]+= S[indexS(i,k)] * Q[ k*N+j];
}
}

  for( Int_t i=0; i<N; i++ ){
    for( Int_t j=0; j<=i; j++ ){
      Int_t n = indexS(i,j);
      S_out[n] = 0 ;
      for( Int_t k=0; k<N; k++ )  S_out[n] += Q[ k*N+i ] * A[ k*N+j ];
}
}
}*/
