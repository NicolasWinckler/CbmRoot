/*
 *=====================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *=====================================================
 *
 *  Finds tracks using the Cellular Automaton algorithm
 *
 */
#include "L1Algo/L1Algo.h"
#include "L1Algo/L1TrackPar.h"
#include "L1Algo/L1Branch.h"
#include "L1Algo/L1Extrapolation.h"
#include "L1Algo/L1Filtration.h"
#include "L1Algo/L1AddMaterial.h"

#include "TStopwatch.h"

#include <iostream>
#include <map>
#include <list>

using std::cout;
using std::endl;
using std::pair;
using std::vector;
using std::map;

void L1Algo::CATrackFinder()
{

#ifdef DRAW
  InitL1Draw();
#endif

#ifdef DEBUG_CA
  InitL1Histo();
#endif

  //cout.setf(ios::fixed);
  //cout.setf(ios::showpoint);
  cout.precision(6);
   
  //static double 
    //stat_time = 0.0, stat_time_find = 0.0, stat_time_fit = 0.0, 
    //stat_time_fit_bra = 0.0, stat_time_fit_fin = 0.0,
    //stat_time_find_10 = 0.0, stat_time_find_20 = 0.0, stat_time_find_30 = 0.0, stat_time_find_40 = 0.0,
    //stat_time_find_50 = 0.0,
    //stat_time_find_11 = 0.0, stat_time_find_21 = 0.0, stat_time_find_31 = 0.0;
    //static unsigned int stat_N = 0;
  //double 
  //time_find = 0.0, time_fit = 0.0, time_fit_bra = 0.0, time_fit_fin = 0.0,
  //time_find_10 = 0.0, time_find_20 = 0.0, time_find_30 = 0.0, time_find_40 = 0.0, time_find_50 = 0.0,
  //time_find_11 = 0.0, time_find_21 = 0.0, time_find_31 = 0.0;

  const int FIRSTCASTATION = 0;

  TStopwatch c_time;

  //cout << endl << "===> CA Track Finder ..." << endl;
  
  vTracks.clear();
  vRecoHits.clear();
  
  // Arrays of duplets 
  const int MaxArrSize = 200000;
  static unsigned short int 
    vecA0[MaxArrSize], vecB0[MaxArrSize], vecA[MaxArrSize], vecB[MaxArrSize],
    *mrDuplets_hits  = vecA, // right hits of middle-right duplets
    *mrDuplets_start = vecA0,// first right hit in mrDuplets_hits, array paralel to middle hits 
    *lmDuplets_hits  = vecB, // same for left-middle duplets 
    *lmDuplets_start = vecB0;

  //

  static bool vecFakeA[MaxArrSize], vecFakeB[MaxArrSize], 
    *vFake_m = vecFakeA, *vFake_l = vecFakeB; 

  static pair<unsigned short int, unsigned short int> vecTripA0[MaxArrSize], vecTripB0[MaxArrSize], 
    *vTriplets0_m = vecTripA0, *vTriplets0_l = vecTripB0;

  pair<unsigned short int, unsigned short int> dup_init(0,0);


  //(*vA).clear(); (*vB).clear();
  
  for( vector<unsigned char>::iterator is= vSFlag.begin(); is!=vSFlag.end(); ++is) SetFUnUsed(*is);
  for( vector<unsigned char>::iterator is= vSFlagB.begin(); is!=vSFlagB.end(); ++is) SetFUnUsed(*is);

  fvec vINF = 10.0;
  unsigned int 
    stat_max_n_trip = 0,
    stat_max_trip_size = 0,
    stat_max_n_dup = 0,
    stat_max_BranchSize = 0,
    stat_max_n_branches = 0;
  
#ifdef DRAW
  Draw1();
#endif

  //for (int isec = 0; isec <= 0; isec++){ // D0 !!!
  //for (int isec = 0; isec <= 1; isec++){
  for (int isec = 0; isec <= 2; isec++){ // all finder
  //for (int isec = 0; isec <= fTrackingLevel; isec++){ // 
    
    unsigned int stat_n_trip = 0;
    
    //cout << "isec " << isec << " : ";
    //cout<<" total STS hits "<<vStsHits.size()<<endl;
    //cout<<" total STS hits, strips, back strips "
    //<<vStsHits.size()<<" "<<vStsStrips.size()<<" "<<vStsStripsB.size()<<endl;
    for( vector<unsigned char>::iterator is= vSFlag.begin(); is!=vSFlag.end(); ++is) SetFUnUsedD(*is);
    for( vector<unsigned char>::iterator is= vSFlagB.begin(); is!=vSFlagB.end(); ++is) SetFUnUsedD(*is);
    
    //TStopwatch c_time_find;
    
    TRACK_CHI2_CUT = 10.0; // SG
 
    double Pick_m = 3.0;
    double Pick_r = 5.0;
    double Pick_gather = 5.0; //!!!
    if (isec == 0){
      Pick_m =  2.0;
    }

    //double MaxInvMom = 1.0/1.;
    double MaxInvMom = 1.0/0.5;
    //if (isec == 1) MaxInvMom =  1.0/0.2;
    if (isec == 1) MaxInvMom =  1.0/0.1;
    //if (isec == 2) MaxInvMom =  1.0/0.2;
    if (isec == 2) MaxInvMom =  1.0/0.1;
    if (isec == fTrackingLevel )  MaxInvMom = 1./fMomentumCutOff;

    int nlevel[NStations];
    for (int il = 0; il < NStations; ++il) nlevel[il] = 0;    
   
    double SigmaTargetX, SigmaTargetY; // target constraint

    static L1FieldValue targB _fvecalignment;
    fvec targX= 0, targY= 0, targZ= 0;

    if (isec <= 1){// target
      targB = vtxFieldValue;
      if (isec ==-1) SigmaTargetX = SigmaTargetY = 0.01; // target    
      else           SigmaTargetX = SigmaTargetY = 0.1;
    }
    else{//use outer radius of the 1st station as a constraint
      L1Station &st = vStations[0];
      SigmaTargetX = SigmaTargetY = 10;//st.Rmax[0];
      targZ = 0.;//-1.; 
      st.fieldSlice.GetFieldValue( 0, 0, targB );
    }
 
    static L1XYMeasurementInfo TargetXYInfo _fvecalignment;
    TargetXYInfo.C00 = SigmaTargetX * SigmaTargetX;
    TargetXYInfo.C10 = 0;
    TargetXYInfo.C11 = SigmaTargetY * SigmaTargetY;
    
    //cout<<"Triplets found " << endl;

    //create triplets and find neighbors
    vTriplets.clear();

    int nh = StsHitsStopIndex[NStations-1] - StsHitsStartIndex[NStations-1] + 1;
    L1StsHit *vStsHits_l = &(vStsHits[0]) + StsHitsStartIndex[NStations-1];
    for (int ih =0 ; ih<nh; ih++){
      L1StsHit &hit = vStsHits_l[ih];
      vFake_m[ih] = GetFUsed( vSFlag[hit.f] | vSFlagB[hit.b] );
      vTriplets0_m[ih] = dup_init;
    }

    //for (int istal = NStations-2; istal >= 0; istal--){//  //start downstream chambers
    for (int istal = NStations-2; istal >= FIRSTCASTATION; istal--){//  //start downstream chambers
      //cout << "istal " << istal << " : " << endl;

      TripStartIndex[istal] = TripStopIndex[istal] = vTriplets.size();
      int nstaltriplets = 0;
      
      int istam = istal+1;
      int istar = istal+2;
           
      unsigned int nDuplets_lm = 0;
      
      L1Station &stal = vStations[istal];
      L1Station &stam = vStations[istam];
      L1Station &star = vStations[istar];

      fvec qp1 = MaxInvMom; 
      if( isec>=2 ) qp1 = 1;
      fvec zl = stal.z[0];
      fvec dzli = 1./(zl-targZ[0]);
      fvec Infqp = MaxInvMom/3.*MaxInvMom/3.;

      const int Portion = 16;

      const int MaxPortionHit = Portion;
      const int MaxPortionDup = 10000;
      const int MaxPortionTrip = 10000;

      static L1FieldRegion fld_1[2000];
      static L1FieldRegion fld_2[20000];
      static L1TrackPar T_1[20000];
      static L1TrackPar T_2[20000];
      static L1TrackPar *T_3 = T_1;

      static unsigned short int hitsl_2[MaxPortionDup];
      static unsigned short int hitsl_3[MaxPortionTrip];
      static unsigned short int *hitsl_1 = hitsl_3;

      static unsigned short int hitsm_2[10000];
      static unsigned short int hitsm_3[10000];
      static unsigned short int hitsr_3[10000];
      
      static fvec u_front[20000], u_back[20000];
      static fvec x_minusV[20000], x_plusV[20000], y_minusV[20000], y_plusV[20000];
      static fscal *x_minus = (fscal*) x_minusV;
      static fscal *x_plus  = (fscal*) x_plusV;
      static fscal *y_minus = (fscal*) y_minusV;
      static fscal *y_plus  = (fscal*) y_plusV;
 
      int stat_nhits=0, stat_nduplets=0, stat_n_triplet_c=0, stat_n_triplets=0; 


      L1StsHit *vStsHits_l = &(vStsHits[0]) + StsHitsStartIndex[istal];
      L1StsHit *vStsHits_m = &(vStsHits[0]) + StsHitsStartIndex[istam];
      L1StsHit *vStsHits_r = 0;
      int NHits_l = StsHitsStopIndex[istal] - StsHitsStartIndex[istal] + 1;
      int NHits_m = StsHitsStopIndex[istam] - StsHitsStartIndex[istam] + 1;
      int NHits_r = 0;
      if( istar < NStations ){
	vStsHits_r = &(vStsHits[0]) + StsHitsStartIndex[istar];
	NHits_r = StsHitsStopIndex[istar] - StsHitsStartIndex[istar] + 1;
      }

      //hit on the middle plane to start
      int start_mhit = 0;
      
      for( int start_lh=0; start_lh<NHits_l; start_lh+=Portion ){

	int end_lh = start_lh + Portion;
	if( end_lh>NHits_l ) end_lh = NHits_l;

	int n1 = 0, n1_V=0, n1_4=0;

	// prepare the portion of left hits

	//TStopwatch c_time_find_10;

	for (int ilh=start_lh; ilh <end_lh; ilh++){
	  lmDuplets_start[ilh] = nDuplets_lm;
	  vTriplets0_l[ilh] = dup_init;

	  //vFake_l[ilh]=1;
	  
	  L1StsHit &hitl = vStsHits_l[ilh];
	  if( GetFUsed( vSFlag[hitl.f] | vSFlagB[hitl.b] ) ) continue; // if used

	  hitsl_1[n1] = ilh;
	  u_front[n1_V][n1_4] = vStsStrips[hitl.f];
	  u_back [n1_V][n1_4] = vStsStripsB[hitl.b];
	  
	  n1++;
	  n1_V= n1/fvecLen;
	  n1_4= n1%fvecLen;
	}
	if( n1_4>0 ){
	  for( int i1_4=n1_4-1; n1_4<fvecLen; n1_4++ ){
	    u_front[n1_V][n1_4] = u_front[n1_V][i1_4];
	    u_back [n1_V][n1_4] = u_back [n1_V][i1_4];
	  }	
	  n1_V+=1;
	}

	//c_time_find_10.Stop();    
	//time_find_10 += double(c_time_find_10.CpuTime()); //ms
	
	//cout<<"n1 = "<<n1<<", n1_V="<<n1_V<<", n1_4="<<n1_4<<endl;
	stat_nhits+=n1;

	// construct T_1:  target->left hit-> Z of middle station 

	//TStopwatch c_time_find_11;

	for( int i1_V=0; i1_V<n1_V; i1_V++ ){
	  fvec &x = u_front[i1_V];
	  fvec &v = u_back [i1_V];
	  fvec y  = stal.yInfo.cos_phi*x + stal.yInfo.sin_phi*v;
	  //cout<<"x="<<x<<endl;
	  //cout<<"y="<<y<<endl;
	  fvec tx = (x-targX)*dzli;
	  fvec ty = (y-targY)*dzli;
	  static L1FieldValue l_B, m_B _fvecalignment;
	  static L1FieldRegion fld;
	  stal.fieldSlice.GetFieldValue( x, y, l_B );
	  stam.fieldSlice.GetFieldValue( tx*stam.z, ty*stam.z, m_B );
	  if( istal>0 ){
	    L1FieldValue centB;
	    L1Station &st = vStations[istal/2];
	    st.fieldSlice.GetFieldValue( tx*st.z, ty*st.z, centB );
	    fld.Set( l_B, stal.z, centB, st.z, targB, targZ );
	    fld_1[i1_V].Set( m_B, stam.z, l_B, stal.z, centB, st.z  );
	  }
	  else{
	    fld.Set( l_B, stal.z, targB, targZ );
	    fld_1[i1_V].Set( m_B, stam.z, l_B, stal.z, targB, targZ  );
	  }
	  L1TrackPar &T = T_1[i1_V];

	  T.chi2 = 0.;
	  T.NDF = 2.;
	  if( isec==2) T.NDF = 0;
	  T.x  = x;
	  T.y  = y;
	  T.tx = tx;
	  T.ty = ty;
	  T.qp = 0.;
	  T.z  = zl;
	  T.C00 = stal.XYInfo.C00;
	  T.C10 = stal.XYInfo.C10;
	  T.C11 = stal.XYInfo.C11;
	  T.C22 = T.C33 = vINF; T.C44 = Infqp;
	  //add the target
	  {
	    fvec eX, eY, J04, J14;
	    fvec dz; 
	    dz = targZ - zl;
	    L1ExtrapolateJXY0(T.tx, T.ty, dz, fld, eX, eY, J04, J14 );
	    fvec J[6];
	    J[0]= dz; J[1] = 0; J[2]= J04;
	    J[3] = 0; J[4]= dz; J[5]= J14;
	    L1FilterVtx( T, targX, targY, TargetXYInfo, eX, eY, J );
	  }

	  L1AddMaterial( T, stal.materialInfo, qp1 );	  
	  L1Extrapolate0( T, stam.z, fld );

	  fvec dxm_est = Pick_m*sqrt(fabs(T.C00+stam.XYInfo.C00));
	  fvec dym_est = Pick_m*sqrt(fabs(T.C11+stam.XYInfo.C11));

	  x_minusV[i1_V] = T.x - dxm_est;
	  x_plusV [i1_V] = T.x + dxm_est;
	  y_minusV[i1_V] = T.y - dym_est;
	  y_plusV [i1_V] = T.y + dym_est;

	}// i1_V
	//cout<<20<<endl;
	
	//c_time_find_11.Stop();
	//time_find_11 += double(c_time_find_11.CpuTime()); //ms

	//cout<<"left filtration ok"<<endl;

	// make duplets, prepare the portion of triplet candidates


	//TStopwatch c_time_find_20;
	
	int n2=0, n2_V=0, n2_4=0 ;

	for (int i1=0; i1<n1; i1++){

	  int i1_V = i1/fvecLen;
	  int i1_4 = i1 % fvecLen;
	  //cout<<i1<<":1"<<endl;
	  for (; start_mhit <= NHits_m; start_mhit++){	    
	    if( vFake_m[start_mhit] ) continue;
	    L1StsHit &hitm = vStsHits_m[start_mhit];
	    fscal &x = vStsStrips[hitm.f];
	    fscal &v = vStsStripsB[hitm.b];
	    //cout<<"x,v="<<x<<" "<<v<<endl;
	    fscal y  = (stam.yInfo.cos_phi*x + stam.yInfo.sin_phi*v)[0];
	    //cout<<"="<<y<<endl;
	    if ( y >= y_minus[i1]) break;	    
	  }
	  //cout<<i1<<":2"<<endl;
	  
	  lmDuplets_start[hitsl_1[i1]] = nDuplets_lm;

	  for (int imh = start_mhit; imh < NHits_m; imh++){	  
	    if( n2>= MaxPortionDup ) break;
	    //if( imh>=10000 ) cout<<"imh="<<imh<<endl;
	    if( vFake_m[imh] ) continue;
	    L1StsHit &hitm = vStsHits_m[imh];
	    fscal &x = vStsStrips[hitm.f];
	    fscal &v = vStsStripsB[hitm.b];
	    fscal y  = (stam.yInfo.cos_phi*x + stam.yInfo.sin_phi*v)[0];
	    if ( y > y_plus[i1] ) break;
	    if ( (x < x_minus[i1] ) || (x > x_plus[i1]) ) continue;

	    lmDuplets_hits[nDuplets_lm++]=imh;

	    if (istar >= NStations) continue;

	    if( mrDuplets_start[imh] >= mrDuplets_start[imh+1] ) continue;

	    L1TrackPar &T1 = T_1[i1_V];
	    L1TrackPar &T2 = T_2[n2_V];
	    L1FieldRegion &f1 = fld_1[i1_V];
	    L1FieldRegion &f2 = fld_2[n2_V];
	    u_front[n2_V][n2_4] = x;
	    u_back [n2_V][n2_4] = v;
	    for( fvec *i=&T2.x, *j=&T1.x; i<=&T2.NDF; i++,j++ ) (*i)[n2_4]=(*j)[i1_4];
	    for( fvec *i=&f2.cx0, *j=&f1.cx0; i<=&f2.z0; i++,j++ ) (*i)[n2_4]=(*j)[i1_4];
	    hitsl_2[n2] = hitsl_1[i1];
	    hitsm_2[n2] = imh;
	    n2++;
	    n2_V = n2/fvecLen;
	    n2_4 = n2 % fvecLen;	  
	  }
	  //cout<<i1<<":3"<<endl;
	  lmDuplets_start[hitsl_1[i1]+1] = nDuplets_lm;

	  if( lmDuplets_start[hitsl_1[i1]] < nDuplets_lm){
	    L1StsHit &hitl = vStsHits_l[hitsl_1[i1]];
	    vFake_l[hitsl_1[i1]] = 0;
	    SetFUsedD( vSFlag[hitl.f] );
	    SetFUsedD( vSFlagB[hitl.b] );
	  }
	  //cout<<i1<<":4"<<endl;
  
	}//i1
	if( n2_4>0 ){
	  L1TrackPar &T2 = T_2[n2_V];
	  L1FieldRegion &f2 = fld_2[n2_V];
	  for( int i2_4=n2_4-1; n2_4<fvecLen; n2_4++ ){
	    u_front[n2_V][n2_4] = u_front[n2_V][i2_4];
	    u_back [n2_V][n2_4] = u_back [n2_V][i2_4];
	    for( fvec *i=&T2.x; i<=&T2.NDF; i++ ) (*i)[n2_4]=(*i)[i2_4];
	    for( fvec *i=&f2.cx0; i<=&f2.z0; i++ ) (*i)[n2_4]=(*i)[i2_4];
	  }	
	  n2_V+=1;
	}
	//cout<<30<<endl;

	stat_n_triplet_c+=n2; 
	//cout<<"n2 = "<<n2<<endl;

	//c_time_find_20.Stop();    
	//time_find_20 += double(c_time_find_20.CpuTime()); //ms

	//add the middle hit and extrapolate to right station
      	//TStopwatch c_time_find_21;

	for( int i2_V=0; i2_V<n2_V; i2_V++){
	  L1TrackPar &T2 = T_2[i2_V];
	  L1FieldRegion &f2 = fld_2[i2_V];
	  L1Filter( T2, stam.frontInfo, u_front[i2_V] );
	  L1Filter( T2, stam.backInfo,  u_back[i2_V] );
	  L1AddMaterial( T2, stam.materialInfo, T2.qp );
	  L1Extrapolate( T2, star.z, T2.qp, f2 );	
	  fvec dxm_est = Pick_r*sqrt(fabs(T2.C00+star.XYInfo.C00));
	  fvec dym_est = Pick_r*sqrt(fabs(T2.C11+star.XYInfo.C11));	
	  x_minusV[i2_V] = T2.x - dxm_est;
	  x_plusV [i2_V] = T2.x + dxm_est;	  
	  y_minusV[i2_V] = T2.y - dym_est;
	  y_plusV [i2_V] = T2.y + dym_est;
	}
	//cout<<" middle fit ok"<<endl;
	//c_time_find_21.Stop();    
	//time_find_21 += double(c_time_find_21.CpuTime()); //ms

	// prepare triplets 

	//TStopwatch c_time_find_30;

	int n3=0, n3_V=0, n3_4=0;
	//cout<<40<<endl;

	for( int i2=0; i2<n2; i2++){
	  int i2_V = i2/fvecLen;
	  int i2_4 = i2%fvecLen;
	  unsigned short int duplet_b = mrDuplets_start[hitsm_2[i2]];
	  unsigned short int duplet_e = mrDuplets_start[hitsm_2[i2]+1];

	  for (int irh_index = duplet_b; irh_index < duplet_e; irh_index++){
	    if( n3>= MaxPortionTrip ) break;
	    int irh = mrDuplets_hits[irh_index];
	    //if( irh>=10000 ) cout<<"irh="<<irh<<endl;
	    L1StsHit &hitr = vStsHits_r[irh];
	    //double yr = hitr.y;
	    double xr = vStsStrips[hitr.f];
	    double vr = vStsStripsB[hitr.b];
	    fscal yr  = (star.yInfo.cos_phi*xr + star.yInfo.sin_phi*vr)[0];
	    if (yr < y_minus[i2]) continue;
	    if (yr > y_plus [i2] ) break;	      
	    if ((xr < x_minus[i2]) || (xr > x_plus[i2])) continue;

	    L1TrackPar &T3 = T_3[n3_V];
	    L1TrackPar &T2 = T_2[i2_V];
	  
	    hitsl_3[n3] = hitsl_2[i2];
	    hitsm_3[n3] = hitsm_2[i2];
	    hitsr_3[n3] = irh;

	    if( n3_4==0 ){
	      u_front[n3_V] = vStsStrips[hitr.f];
	      u_back [n3_V] = vStsStripsB[hitr.b];
	      for( fvec *i=&T3.x, *j=&T2.x; i<=&T3.NDF; i++,j++ ) *i=(*j)[i2_4];
	    }else{
	      u_front[n3_V][n3_4] = vStsStrips[hitr.f];
	      u_back [n3_V][n3_4] = vStsStripsB[hitr.b];
	      for( fvec *i=&T3.x, *j=&T2.x; i<=&T3.NDF; i++,j++ )  (*i)[n3_4]=(*j)[i2_4];
	    }
	    n3++;
	    n3_V = n3/fvecLen;
	    n3_4 = n3 % fvecLen;
	  }
	}// i2
	if( n3_4>0 ){
	  for( int i3_4=n3_4-1; n3_4<fvecLen; n3_4++ ){
	    L1TrackPar &T3 = T_3[n3_V];
	    u_front[n3_V][n3_4] = u_front[n3_V][i3_4];
	    u_back [n3_V][n3_4] = u_back [n3_V][i3_4];
	    for( fvec *i=&T3.x, *j=&T3.x; i<=&T3.NDF; i++,j++ )  (*i)[n3_4]=(*j)[i3_4];
	  }	
	  n3_V+=1;
	}
	
	//cout<<"n3 = "<<n3<<endl;

      	//c_time_find_30.Stop();    
	//time_find_30 += double(c_time_find_30.CpuTime()); //ms

	stat_n_triplets+=n3; 
 
	//add the right hit to triplets
	//TStopwatch c_time_find_31;

	for( int i3_V=0; i3_V<n3_V; i3_V++){
	  L1Filter( T_3[i3_V], star.frontInfo, u_front[i3_V] );
	  L1Filter( T_3[i3_V], star.backInfo,  u_back [i3_V] );
	}
	//c_time_find_31.Stop();    
	//time_find_31 += double(c_time_find_31.CpuTime()); //ms

	//cout<<" right fit ok"<<endl;
	//cout<<50<<endl;

	//TStopwatch c_time_find_40;
	
	for( int i3=0; i3<n3; i3++){
	  int i3_V = i3/fvecLen;
	  int i3_4 = i3%fvecLen;

	  fscal chi2 = T_3[i3_V].chi2[i3_4];

	  if ( !finite(chi2) || chi2<0 ||chi2 > TRACK_CHI2_CUT * (T_3[i3_V].NDF[i3_4]-5)) continue;
	  //cout<<510<<" "<<chi2<<endl;

	  L1Triplet trip;

	  fscal tx = fabs(T_3[i3_V].tx[i3_4]);
	  if( tx>1 ) continue;
	  fscal ty = fabs(T_3[i3_V].ty[i3_4]);
	  if( ty>1 ) continue;
	  fscal qp = MaxInvMom + T_3[i3_V].qp[i3_4];
	  if( qp<0 ) qp = 0;
	  if( qp> MaxInvMom*2 ) qp = MaxInvMom*2;
	  fscal Cqp = 5.*sqrt(fabs(T_3[i3_V].C44[i3_4]));
	  //cout<<520<<endl;
	  fscal scale = 255/(MaxInvMom*2);
	  //cout<<qp*scale<<" "<<Cqp*scale<<endl;
	  qp    = ((unsigned int)(qp*scale) )%256;
	  Cqp    = ((unsigned int)(Cqp*scale))%256;
	  Cqp+= 1;
	  //cout<<525<<endl;
	  if( Cqp<0   ) Cqp= 0;
	  if( Cqp>255 ) Cqp= 255;
	  if( Cqp>20 ) Cqp= 20;
	  qp = (unsigned char) qp;
	  //cout<<"qp "<<T_3[i3_V].qp[i3_4]<<" Cqp "<<T_3[i3_V].C44[i3_4]
	  //<<", qqp="<<qp<<", Cqqp="<<Cqp<<endl;
	  //if( Cqp>30 ) cout<<"track: "<<
	  //T_3[i3_V].x[i3_4]<<" "<<T_3[i3_V].y[i3_4]<<" "<<T_3[i3_V].z[i3_4]<<" "<<T_3[i3_V].tx[i3_4]<<" "<<T_3[i3_V].ty[i3_4]<<" "<<T_3[i3_V].qp[i3_4]<<endl;
	  //cout<<530<<endl;

	  unsigned char level  = 0;
	  
	  unsigned short int ihitl = hitsl_3[i3] + StsHitsStartIndex[istal];
	  unsigned short int ihitm = hitsm_3[i3] + StsHitsStartIndex[istam];
	  unsigned short int ihitr = hitsr_3[i3] + StsHitsStartIndex[istar];

	  //if( ihitl<0 || ihitl>60000) 
	  //cout<<"ihitl="<<ihitl<<" "<<hitsl_3[i3]<<" "<<StsHitsStartIndex[istal]
	  //<<" i3="<<i3<<" n3="<<n3<<endl;

	  unsigned short int first_neighbour = 0;
	  unsigned short int end_neighbour = 0;

	  if (istar != NStations-1){ //find the highest level among neighbors

	    int offset_m = TripStartIndex[istam];
	    int offset_r = TripStartIndex[istar];
	    unsigned int first_triplet = vTriplets0_m[hitsm_3[i3]].first;
	    unsigned int last_triplet  = vTriplets0_m[hitsm_3[i3]].second;
	    
	    for (unsigned int index = first_triplet; index < last_triplet; ++index){ 
	      
	      L1Triplet *ptrip = &vTriplets[offset_m+index];

	      unsigned short int jhitm;
	      if( ptrip->GetLevel()==0 ) jhitm = ptrip->GetMHit();	      
	      else jhitm = vTriplets[offset_r+ptrip->GetFirstNeighbour()].GetLHit();	      
	      if ( ihitr != jhitm) continue;
	      fscal qp1 = qp;
	      fscal qp2 = ptrip->GetQp();
	      fscal Cqp = trip.Cqp;
	      fscal Cqp1 = ptrip->Cqp;
	      if ( fabs(qp1 - qp2) > ( Cqp + Cqp1 ) ) continue;
	      
	      //OK, modify the level

	      if( end_neighbour == 0 ){
		if( index > ptrip->GetMaxFirstNeighbour() ) break;
		first_neighbour = index;
	      }
	      if( index + 1 - first_neighbour >= ptrip->GetMaxNNeighbours() ) break;
	      end_neighbour = index + 1;	  
	      unsigned char jlevel = ptrip->GetLevel();
	      if( level<=jlevel) level = jlevel + 1 ;		
	    }
	    //cout<<530<<endl;

	  }	 
	  //cout<<540<<endl;
	  trip.Set( ihitl, ihitm, ihitr, 
		    first_neighbour, end_neighbour-first_neighbour, 
		    level, (unsigned char) qp, chi2);
	  //cout << "Triplet Qp " << trip.GetQpOrig(MaxInvMom) << " mom " << 1.0/trip.GetQpOrig(MaxInvMom) << endl;

	  //if( trip.GetLHit()<0 || trip.GetLHit()>60000) cout<<"trip.GetLHit()="<<trip.GetLHit()<<endl;
	  trip.Cqp   = (unsigned char) Cqp;
	 	
	  //count statistics
	  nlevel[level]++;
	
	  vTriplets.push_back(trip);
	  TripStopIndex[istal]++;

	  stat_n_trip++;  
	  //modify hit pointers
	  if ( vTriplets0_l[hitsl_3[i3]].second == 0 ) vTriplets0_l[hitsl_3[i3]].first = nstaltriplets;
	  vTriplets0_l[hitsl_3[i3]].second = ++nstaltriplets;
	  //cout<<540<<endl;

	}//i3
	//c_time_find_40.Stop();    
	//time_find_40 += double(c_time_find_40.CpuTime()); //ms
	//cout<<60<<endl;
      }//portion of left hits
      //cout<<2<<endl;

      //TStopwatch c_time_find_50;

      for( int ilh=0; ilh<NHits_l; ilh++ ){
	L1StsHit &hitl = vStsHits_l[ilh];
	if( GetFUsed( vSFlag[hitl.f] | vSFlagB[hitl.b]) ){ // if used
	  vFake_l[ilh] = 1;
	  continue;
	}        
	if( vFake_l[ilh] ){
	  L1StsHit &hitl = vStsHits_l[ilh];
	  if( !GetFUsedD( vSFlag[hitl.f] | vSFlagB[hitl.b]) ) vFake_l[ilh] = 0;
	  vFake_l[ilh] = 0;//!!!
	}
      }

      //c_time_find_50.Stop();    
      //time_find_50 += double(c_time_find_50.CpuTime()); //ms
    
      stat_nduplets=nDuplets_lm;

#ifdef DEBUG_CA
      h_nduplets[istal][isec]->Fill(nDuplets_lm);
#endif
   
      if( stat_max_n_dup<nDuplets_lm ) stat_max_n_dup = nDuplets_lm ;

      unsigned short int *ptmp = mrDuplets_hits; 
      mrDuplets_hits = lmDuplets_hits; 
      lmDuplets_hits = ptmp;
 
      ptmp = mrDuplets_start; 
      mrDuplets_start = lmDuplets_start; 
      lmDuplets_start = ptmp;

      pair<unsigned short int, unsigned short int> *pttmp = vTriplets0_m; 
      vTriplets0_m = vTriplets0_l; 
      vTriplets0_l = pttmp;

      bool * Ftmp = vFake_m;
      vFake_m = vFake_l; 
      vFake_l = Ftmp;

      //cout << "*** stal " << istal << " : " << nstaltriplets << endl;
      //cout<<" supressed hits = "<<nsuppressed<<" of "<<nfreetot<<" free"<<endl;
      //cout<<"n hits, dup, tr_c, trip: "<<stat_nhits<<" "<<stat_nduplets<<" "<<stat_n_triplet_c<<" "<<stat_n_triplets<<endl; 

    }// stations

    //cout<<endl;
    
    //c_time_find.Stop();    
    //time_find += double(c_time_find.CpuTime()); //ms
    



    //==================================================================
    //=                                                                =
    //=                   Collect track candidates                     =
    //=                                                                =
    //==================================================================

    //cout<<"Collect track candidates..."<<endl;
    
    //TStopwatch c_time_fit;
    
    int min_level = 0;//1; //remove pile-up in MAPS //!!!
    if (isec == 1) min_level = 0; 
    if (isec == 2) min_level = 2; //only the longest tracks
    if (isec == -1) min_level = NStations-3 - 3; //only the longest tracks
    //min_level = 1; //!!!
    
    for (int ilev = NStations-3; ilev >= min_level; ilev--){
      //for (int ilev = NStations-3; ilev >= 0; ilev--){
 
      //TStopwatch c_time_fit_bra;

      static vector<L1Branch> vtrackcandidate; vtrackcandidate.clear();
      
      //how many levels to check
      int nlevel = (NStations-2)-ilev+1;
      int ntracks = 0;

      for( int istaF = 0; istaF<= NStations-3-ilev; istaF++ ){
	
	//cout<<"istaF="<<istaF<<", max= "<<NStations-3-ilev<<endl;
	
	if( isec==2 && istaF>=4 ) break; // ghost supression !!!
	
	int sta_first = TripStartIndex[istaF];
	int sta_end   = TripStopIndex[istaF];
	//cout<<" level "<<ilev<<", station "<<istaF<<", triplets "<<sta_end-sta_first
	//<<", trip indices ("<<sta_first<<"->"<<sta_end<<")"<<endl;
	//cout<<vStsHits.size()<<endl;
	for( int itrip=sta_first; itrip<sta_end; itrip++ ){	  	
	  L1Triplet *first_trip = &vTriplets[itrip];
	  int lv=first_trip->GetLevel();
	  int jst= GetFStation( vSFlag[vStsHits[first_trip->GetLHit()].f] );
	  int fl = GetFUsed( vSFlag[vStsHits[first_trip->GetLHit()].f] | vSFlagB[vStsHits[first_trip->GetLHit()].b] );

	  if ( lv != ilev) continue;
	  if (ilev == 0){ //remove pile-up -> collect only MAPS triplets
	    if ( jst != 0) continue;
	  }
	  if( fl ) continue; // if used	  
	  
	  /*
	  //check the level	  
	  if (first_trip->GetLevel() != ilev) continue;
	  if (ilev == 0){ //remove pile-up -> collect only MAPS triplets
	    if ( GetFStation(vSFlag[vStsHits[first_trip->GetLHit()].f]) != 0) continue;
	  }

	  //check if all hits are not used => clear start
	  
	  if( GetFUsed( vSFlag[vStsHits[first_trip->GetLHit()].f] | vSFlagB[vStsHits[first_trip->GetLHit()].b] )
	      ) continue; // if used	  
	  */

	  //cout<<itrip<<" "<<sta_end<<endl;
	  
	  //OK, search the best track candidate in the branches starting from the first_trip

	  L1Branch curr_tr;
	
	  curr_tr.StsHits.push_back(first_trip->GetLHit());	  
	  unsigned char curr_L = 1;	
	  fscal curr_chi2 = first_trip->GetChi2();
	  //fscal curr_Qp = first_trip->GetQpOrig(MaxInvMom);
	  //cout << "Triplet Qp " << trip.GetQpOrig(MaxInvMom) << " mom " << 1.0/trip.GetQpOrig(MaxInvMom) << endl;

	  L1Branch best_tr = curr_tr;
	  fscal best_chi2 = curr_chi2;
	  unsigned char best_L = curr_L;
	  int NCalls = 1;
	  //cout<<"CAFindTrack, triplet N "<<itrip-sta_first<<" of "<<sta_end-sta_first<<endl;
	  CAFindTrack(istaF, first_trip, best_tr, best_L, best_chi2, curr_tr, curr_L, curr_chi2, NCalls);
	  //cout<<"CAFindTrack ok, L="<<best_L<<", chi2="<<best_chi2/(best_L*2-5)
	  //<<", Ncalls = "<<NCalls<<endl;
	  //if(sta_first==34218 && ilev==1) cout<<4<<endl;
	  if (best_L < ilev+2) continue; // maximum one hit missing

	  int ndf = best_L*2-5;
	  best_chi2 = best_chi2/ndf; //normalize
	
	  if (best_chi2 > TRACK_CHI2_CUT) continue;

	  // fit track candidate

	  if( best_L <3 ) continue;

	  static L1TrackPar T _fvecalignment;
	  int nGathered = 0;
	  
	  if(0){ // fit forward

	    Int_t nHits = best_tr.StsHits.size();

	    //T.qp = first_trip->GetQpOrig(MaxInvMom);
	    //FitTrack( T, &(best_tr.StsHits[0]), best_tr.StsHits.size(), 1 );
	    
	    L1StsHit &hit0 = vStsHits[best_tr.StsHits[0]];
	    L1StsHit &hit1 = vStsHits[best_tr.StsHits[1]];
	    L1StsHit &hit2 = vStsHits[best_tr.StsHits[2]];

	    int ista0 = GetFStation( vSFlag[hit0.f] );
	    int ista1 = GetFStation( vSFlag[hit1.f] );
	    int ista2 = GetFStation( vSFlag[hit2.f] );

	    L1Station &sta0 = vStations[ista0];
	    L1Station &sta1 = vStations[ista1];
	    L1Station &sta2 = vStations[ista2];

	    fvec x0  = vStsStrips[hit0.f];
	    fvec v0  = vStsStripsB[hit0.b];
	    fvec y0  = sta0.yInfo.cos_phi*x0 + sta0.yInfo.sin_phi*v0;
	    fvec z0 = sta0.z;

	    fvec x1  = vStsStrips[hit1.f];
	    fvec v1  = vStsStripsB[hit1.b];
	    fvec y1  = sta1.yInfo.cos_phi*x1 + sta1.yInfo.sin_phi*v1;
	    fvec z1 = sta1.z;

	    fvec x2  = vStsStrips[hit2.f];
	    fvec v2  = vStsStripsB[hit2.b];
	    fvec y2  = sta2.yInfo.cos_phi*x2 + sta2.yInfo.sin_phi*v2;
	    fvec z2 = sta2.z;
    
	    fvec dzi = 1./(z1-z0);
 
	    fvec qp0 = first_trip->GetQpOrig(MaxInvMom);

	    T.chi2 = 0.;
	    T.NDF = 0.;
	    T.x  = x0;
	    T.y  = y0;
	    T.tx = (x1-x0)*dzi;
	    T.ty = (y1-y0)*dzi;
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
	      L1StsHit &hit = vStsHits[best_tr.StsHits[i]];	      
	      ista = vSFlag[hit.f]/4;
	      L1Station &sta = vStations[ista];
	      fvec x = vStsStrips[hit.f];
	      fvec v = vStsStripsB[hit.b];
	      fvec y  = sta.yInfo.cos_phi*x + sta.yInfo.sin_phi*v;
	      L1Extrapolate( T, sta.z, qp0, fld );	
	      L1Filter( T, sta.frontInfo, x );
	      L1Filter( T, sta.backInfo,  v );
	      L1AddMaterial( T, sta.materialInfo, qp0 );
	      fB0 = fB1;
	      fB1 = fB2;
	      fz0 = fz1;
	      fz1 = fz2;
	      sta.fieldSlice.GetFieldValue( x, y, fB2 );
	      fz2 = sta.z;
	      fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );
	    }
	  
	    // gather missed hits
	    
	    if(1)for( ista+=2; ista<NStations; ista++ ){
	      L1Station &sta = vStations[ista];
	      L1Extrapolate( T, sta.z, qp0, fld );

	      fscal dxm_est = ( Pick_gather*sqrt(fabs(T.C00+sta.XYInfo.C00)) )[0];
	      fscal dym_est = ( Pick_gather*sqrt(fabs(T.C11+sta.XYInfo.C11)) )[0];	
	      fscal x_minus = T.x[0] - dxm_est;
	      fscal x_plus  = T.x[0] + dxm_est;	  
	      fscal y_minus = T.y[0] - dym_est;
	      fscal y_plus  = T.y[0] + dym_est;

	      double D2 = 1.e8;
	      int ibest = -1;
	      for( int ih = StsHitsStartIndex[ista]; ih<=StsHitsStopIndex[ista]; ih++ ){

		L1StsHit &hit = vStsHits[ih];
		if( GetFUsed( vSFlag[hit.f] | vSFlagB[hit.b] ) ) continue; // if used

		fscal x = vStsStrips[hit.f];
		fscal v = vStsStripsB[hit.b];
		fscal y  = (sta.yInfo.cos_phi*x + sta.yInfo.sin_phi*v)[0];
		if (y < y_minus) continue;
		if (y > y_plus ) break;	      
		if ((x < x_minus) || (x > x_plus)) continue;
		double dx = x-T.x[0];
		double dy = y-T.y[0];
		double d2 = dx*dx + dy*dy;
		if( d2>D2 ) continue;
		D2 = d2;
		ibest = ih;
	      }
	      if( ibest <0 ) break;
	      
	      best_tr.StsHits.push_back(ibest);
	      best_L++;
	      nGathered ++;
	      L1StsHit &hit = vStsHits[ibest];
	      fvec x = vStsStrips[hit.f];
	      fvec v = vStsStripsB[hit.b];
	      fvec y  = sta.yInfo.cos_phi*x + sta.yInfo.sin_phi*v;
	      L1Filter( T, sta.frontInfo, x );
	      L1Filter( T, sta.backInfo,  v );
	      L1AddMaterial( T, sta.materialInfo, qp0 );
	      fB0 = fB1;
	      fB1 = fB2;
	      fz0 = fz1;
	      fz1 = fz2;
	      sta.fieldSlice.GetFieldValue( x, y, fB2 );
	      fz2 = sta.z;
	      fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );
	    }
	  }

	  if(0){ // fit backward
	    cout<<" back 1"<<endl;
	    Int_t nHits = best_tr.StsHits.size();

	    L1StsHit &hit0 = vStsHits[best_tr.StsHits[nHits-1]];
	    L1StsHit &hit1 = vStsHits[best_tr.StsHits[nHits-2]];
	    L1StsHit &hit2 = vStsHits[best_tr.StsHits[nHits-3]];

	    int ista0 = vSFlag[hit0.f]/4;
	    int ista1 = vSFlag[hit1.f]/4;
	    int ista2 = vSFlag[hit2.f]/4;

	    cout<<"back: ista012="<<ista0<<" "<<ista1<<" "<<ista2<<endl;
	    L1Station &sta0 = vStations[ista0];
	    L1Station &sta1 = vStations[ista1];
	    L1Station &sta2 = vStations[ista2];

	    fvec x0  = vStsStrips[hit0.f];
	    fvec v0  = vStsStripsB[hit0.b];
	    fvec y0  = sta0.yInfo.cos_phi*x0 + sta0.yInfo.sin_phi*v0;
	    fvec z0 = sta0.z;

	    fvec x1  = vStsStrips[hit1.f];
	    fvec v1  = vStsStripsB[hit1.b];
	    fvec y1  = sta1.yInfo.cos_phi*x1 + sta1.yInfo.sin_phi*v1;
	    fvec z1 = sta1.z;

	    fvec x2  = vStsStrips[hit2.f];
	    fvec v2  = vStsStripsB[hit2.b];
	    fvec y2  = sta2.yInfo.cos_phi*x2 + sta2.yInfo.sin_phi*v2;
	    fvec z2 = sta2.z;
    
	    fvec dzi = 1./(z1-z0);
 
	    cout<<"qp0 ="<<T.qp<<endl;
	    fvec qp0 = first_trip->GetQpOrig(MaxInvMom);

	    T.x  = x0;
	    T.y  = y0;
	    T.tx = (x1-x0)*dzi;
	    T.ty = (y1-y0)*dzi;
	    T.qp = qp0;
	    T.z  = z0;
	    T.chi2 = 0.;
	    T.NDF = 0.;
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
	    cout<<" back 2"<<endl;
	    int ista = ista2;
	    for( int i=nHits-2; i>=0; i--){
	      if( fabs(T.qp[0])>2. ) break;
	      L1StsHit &hit = vStsHits[best_tr.StsHits[i]];
	      ista = vSFlag[hit.f]/4;
	      cout<<i<<" :ista="<<ista<<endl;
	      L1Station &sta = vStations[ista];
	      fvec x = vStsStrips[hit.f];
	      fvec v = vStsStripsB[hit.b];
	      fvec y  = sta.yInfo.cos_phi*x + sta.yInfo.sin_phi*v;
	      cout<<i<<" :"<<x<<" "<<y<<" "<<v<<endl;
	      L1Extrapolate( T, sta.z, qp0, fld );	
	      cout<<i<<":1 "<<endl;
	      L1Filter( T, sta.frontInfo, x );
	      cout<<i<<":2 "<<endl;
	      L1Filter( T, sta.backInfo,  v );
	      cout<<i<<":3 "<<T.tx<<" "<<T.ty<<endl;
	      L1AddMaterial( T, sta.materialInfo, qp0 );
	      cout<<i<<":4  "<<endl;
	      fB0 = fB1;
	      fB1 = fB2;
	      fz0 = fz1;
	      fz1 = fz2;
	      sta.fieldSlice.GetFieldValue( x, y, fB2 );
	      fz2 = sta.z;
	      fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );
	      cout<<i<<" ok "<<endl;
	    }
	  
	    cout<<" back 3"<<endl;
	    // gather missed hits
	    
	    if(1)for( ista-=2; ista>=0; ista-- ){

	      L1Station &sta = vStations[ista];

	      L1Extrapolate( T, sta.z, qp0, fld );

	      fscal dxm_est = ( Pick_gather*sqrt(fabs(T.C00+sta.XYInfo.C00)) )[0];
	      fscal dym_est = ( Pick_gather*sqrt(fabs(T.C11+sta.XYInfo.C11)) )[0];	
	      fscal x_minus = T.x[0] - dxm_est;
	      fscal x_plus  = T.x[0] + dxm_est;	  
	      fscal y_minus = T.y[0] - dym_est;
	      fscal y_plus  = T.y[0] + dym_est;

	      double D2 = 1.e8;
	      int ibest = -1;
	      for( int ih = StsHitsStartIndex[ista]; ih<=StsHitsStopIndex[ista]; ih++ ){

		L1StsHit &hit = vStsHits[ih];
		if( GetFUsed( vSFlag[hit.f] | vSFlagB[hit.b] ) ) continue; // if used

		fscal x = vStsStrips[hit.f];
		fscal v = vStsStripsB[hit.b];
		fscal y  = (sta.yInfo.cos_phi*x + sta.yInfo.sin_phi*v)[0];
		if (y < y_minus) continue;
		if (y > y_plus ) break;	      
		if ((x < x_minus) || (x > x_plus)) continue;
		double dx = x-T.x[0];
		double dy = y-T.y[0];
		double d2 = dx*dx + dy*dy;
		if( d2>D2 ) continue;
		D2 = d2;
		ibest = ih;
	      }
	      if( ibest <0 ) break;

	      best_tr.StsHits.push_back(ibest);
	      best_L++;
	      nGathered ++;
	      L1StsHit &hit = vStsHits[ibest];
	      fvec x = vStsStrips[hit.f];
	      fvec v = vStsStripsB[hit.b];
	      fvec y  = sta.yInfo.cos_phi*x + sta.yInfo.sin_phi*v;
	      L1Filter( T, sta.frontInfo, x );
	      L1Filter( T, sta.backInfo,  v );
	      L1AddMaterial( T, sta.materialInfo, qp0 );
	      fB0 = fB1;
	      fB1 = fB2;
	      fz0 = fz1;
	      fz1 = fz2;
	      sta.fieldSlice.GetFieldValue( x, y, fB2 );
	      fz2 = sta.z;
	      fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );
	    }
	  }
	  //if(  nGathered>0 ) cout<<nGathered<<" "<<ndf<<" "<<T.chi2[0]<<endl;
	  //ndf = best_L*2-5;
	  //if( !finite(T.chi2[0]) ) continue;
	  //best_chi2 = T.chi2[0]/ndf; //normalize
	  //if ( T.chi2[0] > TRACK_CHI2_CUT *ndf ) continue;
	  //if ( fabs(T.qp[0]) > MaxInvMom ) continue;
	  
	  if( best_L < min_level+3 ) continue;
       
	  if( fGhostSuppression ){//suppress ghost	    
	    if( best_L <3 ) continue;
	    if( best_L == 3 ){
	      //if( isec == 2 ) continue; // too short secondary track
	      if( isec==2 && istaF != 0 ) continue; // too short non-MAPS track
	      if( (isec<2)&&(best_chi2>5.0) ) continue; 
	    } 
	  }
	  // track candidate is OK, store

	  best_tr.Set( istaF, best_L, best_chi2, first_trip->GetQpOrig(MaxInvMom));

	  vtrackcandidate.push_back(best_tr);
	  ntracks++;	
	}
	//cout<<" level "<<ilev<<", station "<<ista<<" ok"<<endl;
      }
      
      //cout <<"Level "<< ilev <<" track candidates "<< ntracks << ", ";
      //cout << " total number of track candidates " << vtrackcandidate.size() <<endl;
      
      if (--nlevel == 0) break;    
      
      //c_time_fit_bra.Stop();    
      //time_fit_bra += double(c_time_fit_bra.CpuTime());

      //make competition between tracks of the same length
 
      //TStopwatch c_time_fit_fin;
 
      // 1. sort tracks
      static vector<L1Branch*> vptrackcandidate; vptrackcandidate.clear();

      for (vector<L1Branch>::iterator trIt = vtrackcandidate.begin();
	   trIt != vtrackcandidate.end(); ++trIt){
	vptrackcandidate.push_back(&(*trIt));
      }
      //if (isec <= 1) 
	sort(vptrackcandidate.begin(), vptrackcandidate.end(), L1Branch::comparePChi2);
	//else
	//sort(vptrackcandidate.begin(), vptrackcandidate.end(), L1Branch::comparePMomentum);
       
      ntracks = 0;
      
      for (vector<L1Branch*>::iterator trIt = vptrackcandidate.begin();
	   trIt != vptrackcandidate.end(); ++trIt){
	
	//count number of used hits
	
	L1Branch *tr = *trIt;

	//cout << "Branch isec " << isec << " Nhits " << tr->StsHits.size() << " Quality " << tr->Quality  << " chi2 " << tr-> chi2 << " momentum " << tr->Momentum << endl;
	int nused = 0;
	for (vector<unsigned short int>::iterator phIt = tr->StsHits.begin();
	     phIt != tr->StsHits.end(); ++phIt){
	  L1StsHit &h = vStsHits[*phIt];
	  if ( GetFUsed( vSFlag[h.f] | vSFlagB[h.b] ) ) nused++; 
	}
	
	if (nused != 0) continue;
	
	//=======================================================
 	// gather MAPS hits using the Kalman filter
	// was here, skipped for a moment
	//=======================================================
	
	//store track
	for (vector<unsigned short int>::iterator phitIt = tr->StsHits.begin();
	     phitIt != tr->StsHits.end(); ++phitIt){	  
	  L1StsHit &h = vStsHits[*phitIt];
	  SetFUsed( vSFlag[h.f] );
	  SetFUsed( vSFlagB[h.b] );
	  vRecoHits.push_back(*phitIt);
	}
	
	//store track
	L1Track t;
	t.NHits = tr->StsHits.size();
	t.Momentum = tr->Momentum;
	for( int i=0; i<6; i++) t.TFirst[i] = t.TLast[i]=0; 
	for( int i=0; i<15; i++ ) t.CFirst[i] = t.CLast[i] = 10;
	t.chi2 = 100;

	vTracks.push_back(t);
	ntracks++;   

      }
      //c_time_fit_fin.Stop();    
      //time_fit_fin += double(c_time_fit_fin.CpuTime());

      unsigned int Bsize=0, nb=0;
      for (vector<L1Branch*>::iterator trIt = vptrackcandidate.begin();
	   trIt != vptrackcandidate.end(); ++trIt){
	L1Branch *tr = *trIt;
	Bsize+= sizeof(L1Branch) + sizeof(unsigned short int) * tr->StsHits.size();
	nb++;
      }
      if( Bsize>stat_max_BranchSize ) stat_max_BranchSize = Bsize;       
      if( nb>stat_max_n_branches ) stat_max_n_branches = nb;
      
      //for (vector<L1StsHit>::iterator ih = vStsHits.begin(); ih != vStsHits.end(); ++ih){
      //if( ih->strip_f->used || ih->strip_b->used) ih->used = 1;
      //}
    
      //cout<< " track candidates stored: "<< ntracks <<endl;
    }//ilev
    
    //cout<<"End of collecting track candidates "<<endl;

    //c_time_fit.Stop();    
    //time_fit += double(c_time_fit.CpuTime());

    if( stat_max_n_trip<stat_n_trip ) stat_max_n_trip = vTriplets.size();
    unsigned int tsize = vTriplets.size()*sizeof(L1Triplet);
    if( stat_max_trip_size < tsize ) stat_max_trip_size = tsize;

  } // isec
  
  //cout<< "Total number of tracks found " << vTracks.size() <<endl;
  
  //==================================
  
  c_time.Stop();    
  CATime = (double(c_time.CpuTime()));

#ifdef XXX
  /*
  cout<<"CA Track Finder: " << (double(c_time.CpuTime()))
      <<" (" << time_find <<" [" 
      << time_find_10 << "/"<< time_find_11<< "+"
      << time_find_20 << "/"<< time_find_21<< "+"
      << time_find_30 << "/"<< time_find_31<< "+"
      << time_find_40 << "+"
      << time_find_50 << "+"
      << "]"
      << " + " << time_fit <<" [" << time_fit_bra << "/" << time_fit_fin << "]"
      << " ) s/ev"<<endl;
  */
  stat_N++;
  stat_time_find += time_find;
  stat_time_find_10 += time_find_10;
  stat_time_find_20 += time_find_20;
  stat_time_find_30 += time_find_30;
  stat_time_find_40 += time_find_40;
  stat_time_find_50 += time_find_50;
  stat_time_find_11 += time_find_11;
  stat_time_find_21 += time_find_21;
  stat_time_find_31 += time_find_31;

  stat_time_fit += time_fit;
  stat_time_fit_bra += time_fit_bra;
  stat_time_fit_fin += time_fit_fin;
  stat_time     += CATime;
  
  cout<<"CA stat time: " << (stat_time/stat_N)
      <<" (" << stat_time_find/stat_N <<" [" 
      << stat_time_find_10/stat_N  <<"/" << stat_time_find_11/stat_N <<"+" 
      << stat_time_find_20/stat_N  <<"/" << stat_time_find_21/stat_N <<"+" 
      << stat_time_find_30/stat_N  <<"/" << stat_time_find_31/stat_N <<"+" 
      << stat_time_find_40/stat_N <<"+" 
      << stat_time_find_50/stat_N 
      << "]"
      << " + " << stat_time_fit/stat_N 
      <<" [" << stat_time_fit_bra/stat_N << "/" << stat_time_fit_fin/stat_N << "]"
      << ") s/ev"<<endl;
  
  
  static long int NTimes =0, NHits=0, HitSize =0, NStrips=0, StripSize =0, NStripsB=0, StripSizeB =0,
    NDup=0, DupSize=0, NTrip=0, TripSize=0, NBranches=0, BranchSize=0, NTracks=0, TrackSize=0 ;

  NTimes++;
  NHits += vStsHits.size();
  HitSize += vStsHits.size()*sizeof(L1StsHit);
  NStrips+= vStsStrips.size();
  StripSize += vStsStrips.size()*sizeof(fscal) + vSFlag.size()*sizeof(unsigned char);
  NStripsB+= vSFlagB.size();
  StripSizeB += vStsStripsB.size()*sizeof(fscal) + vSFlagB.size()*sizeof(unsigned char);
  NDup += stat_max_n_dup;
  DupSize += stat_max_n_dup*sizeof(short int);
  NTrip += stat_max_n_trip;
  TripSize += stat_max_trip_size;
  
  NBranches += stat_max_n_branches;
  BranchSize += stat_max_BranchSize;
  NTracks += vTracks.size();
  TrackSize += sizeof(L1Track)*vTracks.size() + sizeof(unsigned short int)*vRecoHits.size();
  int k = 1024*NTimes;
  
  cout<<"L1 Event size: \n"
      <<HitSize/k<<"kB for "<<NHits/NTimes<<" hits, \n"
      <<StripSize/k<<"kB for "<<NStrips/NTimes<<" strips, \n"
      <<StripSizeB/k<<"kB for "<<NStripsB/NTimes<<" stripsB, \n"
      <<DupSize/k<<"kB for "<<NDup/NTimes<<" duplets, \n"
      <<TripSize/k<<"kB for "<<NTrip/NTimes<<" triplets\n"
      <<BranchSize/k<<"kB for "<<NBranches/NTimes<<" branches, \n"
      <<TrackSize/k<<"kB for "<<NTracks/NTimes<<" tracks. "<<endl;
  cout<<" L1 total event size = "
      <<( HitSize + StripSize +  StripSizeB + DupSize + TripSize + BranchSize + TrackSize )/k
      <<" Kb"<<endl;
  
  //cout << "===> ... CA Track Finder    " << endl << endl;
#endif

#ifdef DRAW
  DrawAsk();
#endif 
  
}

//1234567890123456789012345678901234567890123456789012345678901234567890


void L1Algo::CAFindTrack( int ista, const L1Triplet* curr_trip, 
			  L1Branch& best_tr, unsigned char &best_L, fscal &best_chi2,
			  L1Branch &curr_tr, unsigned char &curr_L, fscal &curr_chi2,
			  int &NCalls ){

  /****************************************************************
   *                                                              *
   *     The routine performs recusrsive search for tracks        *
   *                                                              *
   *     I. Kisel                                    06.03.05     *
   *                                                              *
   ***************************************************************/

  //  cout << "CA track finder " << curr_trip->level << endl;

  if (curr_trip->GetLevel() == 0){ // the last triplet -> check and store    
    if ((curr_L > best_L )||
	((curr_L == best_L )&&(curr_chi2<best_chi2))){
      best_tr = curr_tr;
      best_chi2 = curr_chi2;
      best_L = curr_L;
    } 
  } 
  else {// curr_trip->level != 0
    
    //store hits & triplets of the current track
    L1Branch temp_tr = curr_tr;
    unsigned char temp_L = curr_L;
    fscal temp_chi2 = curr_chi2;
    int offset = TripStartIndex[ista+1];
    int first_neighbour = offset + curr_trip->GetFirstNeighbour();
    int end_neighbour = first_neighbour + curr_trip->GetNNeighbours();

    for(int index = first_neighbour; index < end_neighbour; ++index){

      L1Triplet *new_trip = &vTriplets[index];      

      if (curr_trip->GetLevel() != new_trip->GetLevel()+1) continue;
      fscal qp1 = curr_trip->GetQp();
      fscal qp2 = new_trip->GetQp();
      fscal dqp = fabs(qp1 - qp2);
      fscal Cqp = curr_trip->Cqp; 
      Cqp      += new_trip->Cqp;
      if ( dqp > Cqp  ) continue; //bad neighbour
      
      //no used hits allowed
      if ( GetFUsed( vSFlag[vStsHits[new_trip->GetLHit()].f] | vSFlagB[vStsHits[new_trip->GetLHit()].b]
		     ) )
	{
	  if((curr_L > best_L)||
	     (curr_L == best_L)&&(curr_chi2<best_chi2)){
	    best_tr  = curr_tr;
	    best_chi2 = curr_chi2;	    
	    best_L = curr_L;
	  } //compare and store track
	}
      else{//add this triplet to the current track

	// restore current track 

	curr_tr = temp_tr;
	curr_chi2 = temp_chi2;
	curr_L = temp_L;
	curr_tr.StsHits.push_back(new_trip->GetLHit()); 
	curr_L+= 1;
	dqp = dqp/Cqp*5.;
	curr_chi2 += dqp*dqp;
	if( new_trip->GetLevel()==0 ){
	  unsigned short int ihitm = new_trip->GetMHit();
	  unsigned short int ihitr = new_trip->GetRHit();
	  if( !GetFUsed( vSFlag[vStsHits[ihitm].f] |  vSFlagB[vStsHits[ihitm].b] )  ){
	    curr_tr.StsHits.push_back(ihitm);
	    curr_L+= 1;
	  }
	  if( !GetFUsed( vSFlag[vStsHits[ihitr].f] | vSFlagB[vStsHits[ihitr].b] ) ){
	    curr_tr.StsHits.push_back(ihitr);
	    curr_L+= 1;
	  }
	}

	if( curr_chi2 <= TRACK_CHI2_CUT * (curr_L) ){ // go further      
	  CAFindTrack(ista+1,new_trip, best_tr, best_L, best_chi2, curr_tr, curr_L, curr_chi2,NCalls);
	  NCalls++;
	}
      }//good triplet added
    }//first_triplet --- last_triplet
  }//level != 0

  //cout << "Track finder finished " << endl;
  return;
}

//========================================================
