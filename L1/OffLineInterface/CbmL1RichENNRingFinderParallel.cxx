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
 *  Standalone RICH ring finder based on the Elastic Neural Net
 *
 *====================================================================
 */

//#define PRINT_TIMING

// CBM includes
#include "CbmL1RichENNRingFinderParallel.h"

#include "CbmRichHit.h"
#include "CbmRichRing.h"

#include "TClonesArray.h"
#include "TStopwatch.h"

#include "assert.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>

using std::vector;
using std::cout;
using std::endl;
using std::ios;
using std::sqrt;
using std::fabs;

CbmL1RichENNRingFinderParallel::CbmL1RichENNRingFinderParallel(Int_t verbose ):fRecoTime(0),fNEvents(0)
{
  fVerbose = verbose;
#ifdef  PRINT_TIMING  
  TString name_tmp[NTimers] = {

    "All",
    
    "Ring finding",
    "Ring finding: Prepare data",
    "Ring finding: Init of param",
    "Ring finding: Hit selection",
    "Ring finding: Final fit",
    "Ring finding: Store ring",
    
    "Selection",
    
    "Sort" ,
    "Repack",
    "Copy data"
  };
  for (int i = 0; i < NTimers; i++) {
    fTimersNames[i] = name_tmp[i];
  }
#endif // PRINT_TIMING  
}

CbmL1RichENNRingFinderParallel::~CbmL1RichENNRingFinderParallel()
{
}


void CbmL1RichENNRingFinderParallel::Init()
{ 
}

Int_t CbmL1RichENNRingFinderParallel::DoFind( TClonesArray* HitArray, TClonesArray* ProjArray,
				      TClonesArray* RingArray )
{
  if( !RingArray || !HitArray ) return 0;

  RingArray->Clear();
#ifdef  PRINT_TIMING
  for (int i = 0; i < NTimers; i++) {
    fTimers[i].Reset();
  }
#endif // PRINT_TIMING

#ifdef  PRINT_TIMING
  GetTimer("Copy data").Start(0);
#endif // PRINT_TIMING   
  vector<ENNRingHit> Up;
  vector<ENNRingHit> Down;
    
  const Int_t nhits = HitArray->GetEntriesFast();  

  for ( register Int_t i = 0; i < nhits; ++i ){
    CbmRichHit * hit = L1_DYNAMIC_CAST<CbmRichHit*>( HitArray->At( i ) );
    if ( !hit ) continue;
    ENNRingHit tmp;
    tmp.x = hit->GetX();
    tmp.y = hit->GetY();
    tmp.outIndex = i;
    tmp.quality = 0;    
    if ( tmp.y > 0. ){
      Up.push_back(tmp);
    }
    else{
      Down.push_back(tmp);
    }
  }

    
#ifdef  PRINT_TIMING
  GetTimer("Copy data").Stop();
#endif // PRINT_TIMING
  
#ifdef  PRINT_TIMING
  GetTimer("Sort").Start(0);
#endif // PRINT_TIMING 
  sort( Up.begin(),   Up.end(),   ENNHit::Compare );
  sort( Down.begin(), Down.end(), ENNHit::Compare );

    // save local-out indices correspondece
  vector<Int_t> outIndicesUp; // indices in HitArray indexed by index in Up
  vector<Int_t> outIndicesDown; // indices in HitArray indexed by index in Down
  outIndicesUp.resize(Up.size());
  outIndicesDown.resize(Down.size());
  for (THitIndex k = 0; k < Up.size();   k++) { Up[k].localIndex = k;   outIndicesUp[k]   = Up[k].outIndex;   }
  for (THitIndex k = 0; k < Down.size(); k++) { Down[k].localIndex = k; outIndicesDown[k] = Down[k].outIndex; }

#ifdef  PRINT_TIMING
  GetTimer("Sort").Stop();
#endif // PRINT_TIMING
    
#ifdef  PRINT_TIMING
  GetTimer("Repack").Start(0);
#endif // PRINT_TIMING 
  
    // save local-out indices correspondece
  nsL1vector<ENNHitV>::TSimd UpV;
  nsL1vector<ENNHitV>::TSimd DownV;
  UpV  .resize((Up  .size() + fvecLen-1)/fvecLen);
  DownV.resize((Down.size() + fvecLen-1)/fvecLen);
  for (THitIndex k = 0; k < Up.size();   k++) {
    int k_4 = k%fvecLen, k_V = k/fvecLen;
    ENNHitV& hits = UpV[k_V]; // TODO change on ENNHitV
    hits.CopyHit(Up[k], k_4);
  }
  for (THitIndex k = 0; k < Down.size();   k++) {
    int k_4 = k%fvecLen, k_V = k/fvecLen;
    ENNHitV& hits = DownV[k_V];
    hits.CopyHit(Down[k], k_4);
  }

#ifdef  PRINT_TIMING
  GetTimer("Repack").Stop();
#endif // PRINT_TIMING
  
  vector< ENNRing > R;

  float HitSize = 1.;
  THitIndex MinRingHits = 5;
  float RMin = 2.; 
  float RMax = 7.; 

  TStopwatch timer;

  ENNRingFinder( Up.size(),   UpV,   R, HitSize, MinRingHits, RMin, RMax );
  ENNRingFinder( Down.size(), DownV, R, HitSize, MinRingHits, RMin, RMax );

  timer.Stop();
  
#ifdef  PRINT_TIMING
  GetTimer("Copy data").Start(0);
#endif // PRINT_TIMING  
  Int_t NRings = 0;
  for( vector<ENNRing>::iterator i=R.begin(); i!=R.end(); ++i ){
    if( !i->on ) continue;
    new((*RingArray)[NRings]) CbmRichRing();
    CbmRichRing *ring = L1_DYNAMIC_CAST<CbmRichRing*>( RingArray->At(NRings) );
    NRings++;
    ring->SetCenterX ( i->x );
    ring->SetCenterY ( i->y );
    ring->SetRadius  ( i->r );
    const THitIndex NHits = i->localIHits.size();
    for( THitIndex j = 0; j < NHits; j++ ){
      if ( i->y > 0. ) ring->AddHit( outIndicesUp  [i->localIHits[j]] );
      else             ring->AddHit( outIndicesDown[i->localIHits[j]] ); 
    }
  }
#ifdef  PRINT_TIMING
  GetTimer("Copy data").Stop();
#endif // PRINT_TIMING  

  fNEvents++;
  fRecoTime += timer.RealTime();
  cout.setf(ios::fixed);
  cout.setf(ios::showpoint);
  cout.precision(4);
  cout << "L1ENN Reco time stat/this [ms]  : " 
       << fRecoTime*1000./fNEvents << "/" << timer.RealTime()*1000.
       << endl;
  
#ifdef PRINT_TIMING
  static float timeStat[NTimers];
  static bool firstCall = 1;
  if (firstCall) {
    for (int i = 0; i < NTimers; i++) {
      timeStat[i] = 0;
    }
    firstCall = 0;
  }
  
  cout.width(30);
  cout << "Timing statEvents / curEvent[ms] | statEvents[%]  : " << endl;
  cout.flags(ios::left|ios::oct|ios::showpoint|ios::fixed);
  cout.precision(3);
  for (int i = 0; i < NTimers; i++) {
    timeStat[i] += fTimers[i].RealTime();
    cout.width(30);
    cout << fTimersNames[i] << " timer = "
        << timeStat[i]*1000./fNEvents << " / "
        << fTimers[i].RealTime()*1000. << " | "
        << timeStat[i]/timeStat[1]*100 << endl;
  }
#endif // PRINT_TIMING  
  return NRings;
}


void CbmL1RichENNRingFinderParallel::ENNRingFinder( const int NHits, nsL1vector<ENNHitV>::TSimd &HitsV, vector<ENNRing> &Rings,
					    float HitSize, THitIndex MinRingHits, 
					    fvec RMin, fvec RMax ){  
#ifdef  PRINT_TIMING
  GetTimer("All").Start(0);
#endif // PRINT_TIMING 
  // INITIALIZATION

  const fvec MinRingHits_v = MinRingHits;
  const fvec Rejection = .5;
  const float ShadowSize = HitSize/4;
  const int    StartHitMaxQuality = 10;
  const int    SearchHitMaxQuality = 10;

  const fvec R2Min = RMin*RMin, R2Max = RMax*RMax;  
  const fvec HitSize2 = 2 * HitSize;
  const fvec HitSize4 = 4 * HitSize;
  const fvec HitSizeSq4 = HitSize2 * HitSize2;
  const float AreaSize  = 2 * RMax[0] + HitSize;
  const float AreaSize2 = AreaSize * AreaSize;  

  typedef vector<ENNRingHit>::iterator iH;
  typedef vector<ENNRingHit*>::iterator iP;

  THitIndex NInRings = Rings.size();

  // MAIN LOOP OVER HITS
#ifdef  PRINT_TIMING  
  GetTimer("Ring finding").Start(0);
#endif // PRINT_TIMING
  nsL1vector<ENNSearchHitV>::TSimd SearchArea;
  nsL1vector<ENNHitV>::TSimd       PickUpArea;
  const int MaxAreaHits = 200; // TODO take into account NHits
  
  SearchArea.resize(MaxAreaHits,ENNSearchHitV());
  PickUpArea.resize(MaxAreaHits);

 // TODO 1
#if 0
  nsL1vector<ENNRingV>::TSimd rings_tmp; // simd hits for tmp store
  rings_tmp.resize(100); // TODO use NRings
  int nRings_tmp = 0;
#endif
  
//  ENNRingHit* ileft = &(Hits[0]), *iright = ileft;//, i_main = ileft;
  int ileft = 0, iright = ileft;
  // int ileft[fvecLen] = {0, 0, 0, 0};
  // int iright[fvecLen] = {0, 0, 0, 0};

#ifndef CBM_KF_F32vec1_H  
  THitIndex i_mains[fvecLen] = {0, 0, 0, 0};
#else
  THitIndex i_mains[fvecLen] = {0};
#endif // CBM_KF_F32vec1_H  
  
  THitIndex i_main_array[NHits]; // need for proceed in paralled almost independent areas
  for (THitIndex i = 0; i < NHits; i++) {
    i_main_array[i] = i; // TODO
  }


  for( THitIndex ii_main = 0; ii_main < NHits; ) {
    
#ifdef  PRINT_TIMING    
    GetTimer("Ring finding: Prepare data").Start(0);
#endif // PRINT_TIMING    


    fvec S0, S1, S2, S3, S4, S5, S6, S7;
    fvec X = 0, Y = 0, R = 0, R2 = 0;

    fvec validRing = (fvec(0) <= fvec(0)); // mask of the valid rings

    fvec SearchAreaSize = 0; // number of hits to fit and search ring
    fvec PickUpAreaSize = 0;
        
    for( int i_4 = 0; (i_4 < fvecLen) && (ii_main < NHits); ii_main++) {
      const THitIndex i_main = i_main_array[ii_main];
      const int i_main_4 = i_main%fvecLen, i_main_V = i_main/fvecLen;
      ENNHitV* i = &HitsV[i_main_V];
      if( i->quality[i_main_4] >= StartHitMaxQuality ) continue;  // already found hit
      
      i_mains[i_4] = i_main;

      float left  = i->x[i_main_4] - AreaSize;
      float right = i->x[i_main_4] + AreaSize;

        // while(                          (HitsV[ileft[i_4]/fvecLen] .x[ileft[i_4]%fvecLen] < left ) ) ++ileft[i_4];
        // while( (iright[i_4] < NHits) && (HitsV[iright[i_4]/fvecLen].x[ileft[i_4]%fvecLen] < right) ) ++iright[i_4];
        // for( int j = ileft[i_4]; j < iright[i_4]; ++j ){
      while(                     (HitsV[ileft/fvecLen] .x[ileft%fvecLen] < left ) ) ++ileft; // TODO SIMDize
      while( (iright < NHits) && (HitsV[iright/fvecLen].x[ileft%fvecLen] < right) ) ++iright;

      for( int j = ileft; j < iright; ++j ){
        const int j_4 = j%fvecLen, j_V = j/fvecLen;
        ENNSearchHitV& sHit = SearchArea[int(SearchAreaSize[i_4])];
        sHit.CopyHit(HitsV[j_V], j_4, i_4);
        
        sHit.ly[i_4]  = sHit.y[i_4] - i->y[i_main_4];
          //	if( fabs(sHit.ly) > AreaSize ) continue;
        sHit.lx[i_4]  = sHit.x[i_4] - i->x[i_main_4];
        sHit.S0  = sHit.lx * sHit.lx;
        sHit.S1  = sHit.ly * sHit.ly;
        sHit.lr2 = sHit.S0 + sHit.S1;
          // if(( sHit.lr2 > AreaSize2 ) || ( j == i )) continue; // no difference in speed
        if( sHit.lr2[i_4] > AreaSize2 ) continue;
        if( ISUNLIKELY( j == i_main ) ) continue;

        if( sHit.quality[i_4] >= SearchHitMaxQuality ) { // CHECKME do we really need PickUpArea
          PickUpArea[PickUpAreaSize[i_4]++].CopyHit( HitsV[j_V], j_4, i_4 );
          continue;
        }
        
        SearchAreaSize[i_4]++;
      } // hits
      
        // if(  NAreaHits+1 < MinRingHits  ) continue;
      if ( SearchAreaSize[i_4] < MinRingHits - 1 ) continue;
      i_4++;
    } // i_4
    
    ENNHitV iHit;
    int MaxSearchAreaSize = 0;
    int MaxPickUpAreaSize = 0;
    for( int i_4 = 0; i_4 < fvecLen; i_4++) {
      iHit.CopyHit(HitsV[i_mains[i_4]/fvecLen], i_mains[i_4]%fvecLen, i_4 ); 
      MaxSearchAreaSize = (MaxSearchAreaSize < SearchAreaSize[i_4]) ? int(SearchAreaSize[i_4]) : MaxSearchAreaSize;
      MaxPickUpAreaSize = (MaxPickUpAreaSize < PickUpAreaSize[i_4]) ? int(PickUpAreaSize[i_4]) : MaxPickUpAreaSize;
    }
#ifdef PRINT_TIMING    
    GetTimer("Ring finding: Prepare data").Stop();
      //      assert( MaxSearchAreaSize[i_4] <= MaxAreaHits );
#endif // PRINT_TIMING

#ifdef  PRINT_TIMING    
    GetTimer("Ring finding: Init of param").Start(0);
#endif // PRINT_TIMING   
    for (int isa = 0; isa < MaxSearchAreaSize; isa++){ // TODO don't work w\o this because of nan in wights
      ENNSearchHitV& sHit = SearchArea[isa];
      const fvec validHit = (fvec(isa) < SearchAreaSize) & validRing;
      sHit.lx  = if3( validHit, sHit.lx,  0);
      sHit.ly  = if3( validHit, sHit.ly,  0);
      sHit.lr2 = if3( validHit, sHit.lr2, 0);
    }

      // initialize hits in the search area
    fvec Dmax = 0.;
    S0 = S1 = S2 = S3 = S4 = 0.;
    for( int ih = 0; ih < MaxSearchAreaSize; ih++ ){
      ENNSearchHitV& sHit = SearchArea[ih];
      const fvec validHit = (fvec(ih) < SearchAreaSize) & validRing;
        
      fvec &lr2 = sHit.lr2;
      fvec lr = sqrt(lr2);
      Dmax = if3( (lr > Dmax) & validHit, lr, Dmax );
	  
      sHit.S2 = sHit.lx * sHit.ly;
      sHit.S3 = sHit.lx * lr2;
      sHit.S4 = sHit.ly * lr2;	    
      sHit.C = -lr*.5;

      const fvec w  = if3( validHit, if3( lr > fvec(1.E-4), 1./lr, 1), 0);
      const fvec w2 = w*w;
      sHit.Cx = w*sHit.lx;
      sHit.Cy = w*sHit.ly;
      S0 += w2*sHit.S0;
      S1 += w2*sHit.S1;
      S2 += w2*sHit.S2;
      S3 += w2*sHit.S3;
      S4 += w2*sHit.S4;

    }

      // end of initialization of the search area
#ifdef PRINT_TIMING    
    GetTimer("Ring finding: Init of param").Stop();
      //      assert( MaxSearchAreaSize[i_4] <= MaxAreaHits );
#endif // PRINT_TIMING
    
#ifdef PRINT_TIMING  
      // loop for minimization of E and noise rejection
    GetTimer("Ring finding: Hit selection").Start(0);
#endif // PRINT_TIMING
    do{
        // calculate parameters of the current ring
      fvec tmp = S0*S1-S2*S2;

        //      if( fabs(tmp) < 1.E-10 ) break; // CHECKME
      tmp = 0.5/tmp; 
      X = (S3*S1 - S4*S2)*tmp;
      Y = (S4*S0 - S3*S2)*tmp;
      R2 = X*X + Y*Y;
      R = sqrt( R2 );

      const fvec Dcut      = Dmax * Rejection;            // cut for noise hits 
        //      float RingCut   = HitSize4 * R ; // closeness
      S0 = S1 = S2 = S3 = S4 = 0.0;
      Dmax = -1.;

      for( THitIndex ih = 0; ih < MaxSearchAreaSize; ih++){
        const fvec validHit = (fvec(ih) < SearchAreaSize) & validRing;
          //        ENNHit *j = &(SearchArea[ih]);
        ENNSearchHitV& sHit = SearchArea[ih];
        const fvec dx = sHit.lx - X;
        const fvec dy = sHit.ly - Y;
        const fvec d2 = dx*dx + dy*dy - R2;
          // oshibka pri d2=0
          // sHit.on_ring = ( fabs( d2 - HitSizeSq4 ) <= RingCut );
        const fvec d = fabs(sqrt(dx*dx + dy*dy) - R);
        sHit.on_ring = ( d <= HitSize2 ) & validHit;
        const fvec dp = if3( sHit.on_ring , -1, fabs( sHit.C + sHit.Cx*X + sHit.Cy*Y ) );
        Dmax = if3((( dp <= Dcut ) & ( dp > Dmax )), dp, Dmax);
             
        fvec w = if3(( sHit.on_ring ), 1, 1./(1.e-5+fabs(d2)));
        w = if3( (dp <= Dcut) & validHit , w, 0);
        S0 += w*sHit.S0;
        S1 += w*sHit.S1;
        S2 += w*sHit.S2;
        S3 += w*sHit.S3;
        S4 += w*sHit.S4;
      }

    }while( NotEmpty( Dmax > fvec(0.) ) );

      
#ifdef  PRINT_TIMING    
    GetTimer("Ring finding: Hit selection").Stop();
      // store the ring if it is found

    GetTimer("Ring finding: Final fit").Start(0);
#endif // PRINT_TIMING

    fvec NRingHits = 1;
    { // final fit of 3 parameters (X,Y,R)      
      S0 = S1 = S2 = S3 = S4 = S5 = S6 = S7 = 0.0;
      for( THitIndex ih = 0; ih < MaxSearchAreaSize; ih++){
        ENNSearchHitV &sHit = SearchArea[ih];
        const fvec w = bool2int( SearchArea[ih].on_ring );
        S0 += w*sHit.S0;
        S1 += w*sHit.S1;
        S2 += w*sHit.S2;
        S3 += w*sHit.S3;
        S4 += w*sHit.S4;
        S5 += w*sHit.lx;
        S6 += w*sHit.ly;
        S7 += w*sHit.lr2;
        NRingHits += w;
      }
      const fvec s0 = S6*S0-S2*S5;
      const fvec s1 = S0*S1-S2*S2;
      const fvec s2 = S0*S4-S2*S3;
        //        if( fabs(s0) < 1.E-6 || fabs(s1) < 1.E-6 ) continue; // CHECKME
      const fvec tmp = s1*(S5*S5-NRingHits*S0)+s0*s0;
      const fvec A = ( ( S0*S7-S3*S5 )*s1-s2*s0 ) / tmp;
      Y = ( s2 + s0*A )/s1*0.5;
      X = ( S3 + S5*A - S2*Y*2 )/S0*0.5;
      R2 = X*X+Y*Y-A;
        // if( R2 < 0 ) continue; // will be rejected letter by R2 > R2Min
      R = sqrt( R2 );
    }// end of the final fit

    validRing = !( (NRingHits < MinRingHits_v) | (R2 > R2Max) | (R2 < R2Min) );
//    cout << validRing << endl;
#ifdef PRINT_TIMING    
    GetTimer("Ring finding: Final fit").Stop();
#endif // PRINT_TIMING


#ifdef PRINT_TIMING       
    GetTimer("Ring finding: Store ring").Start(0);
#endif // PRINT_TIMING
    if ( ISUNLIKELY ( Empty(validRing) ) ) continue;
 
      ///////////
#if 0 // TODO 1
    {
      ENNRingV &ringV = rings_tmp[nRings_tmp++];
    
      ringV.localIHits.reserve(25);
      ringV.x = iHit.x + X;
      ringV.y = iHit.y + Y;
      ringV.r = R;
      ringV.localIHits.push_back(iHit.localIndex);
      ringV.NHits = 1;
      ringV.chi2  = 0;
      nsL1vector<fvec>::TSimd Shadow; // save hit indices of hits, which's quality will be changed
      Shadow.reserve(25);
      Shadow.push_back(iHit.localIndex);
      for( THitIndex ih = 0; ih < MaxSearchAreaSize; ih++){
        fvec validHit = ih < SearchAreaSize;
      
        ENNSearchHitV &sHit = SearchArea[ih];
        const fvec dx = sHit.x - ringV.x;
        const fvec dy = sHit.y - ringV.y;
        const fvec d = fabs( sqrt(dx*dx+dy*dy) - ringV.r );
        validHit = validHit & ( d <= HitSize );
        ringV.chi2 += d*d;
        ringV.localIHits.push_back( if3( validHit, sHit.localIndex, -1 ) );
        ringV.NHits += bool2int(validHit);
        validHit = validHit & ( d <= ShadowSize );
        if ( Empty (validHit) ) continue; // CHECKME
        Shadow.push_back( if3( validHit, sHit.localIndex, -1 ) );
      }
      for( int ipu = 0; ipu < MaxPickUpAreaSize; ipu++ ) {
        fvec validHit = ipu < PickUpAreaSize;
      
        ENNHitV &puHit = PickUpArea[ipu];
        const fvec dx = puHit.x - ringV.x;
        const fvec dy = puHit.y - ringV.y;
        const fvec d = fabs( sqrt(dx*dx+dy*dy) - ringV.r );
        validHit = validHit & ( d <= HitSize );
        if ( Empty (validHit) ) continue;
        ringV.chi2 += d*d;
        ringV.localIHits.push_back( if3( validHit, puHit.localIndex, -1 ) );
        ringV.NHits += bool2int(validHit);
        validHit = validHit & ( d <= ShadowSize );
        if ( Empty (validHit) ) continue; // CHECKME
        Shadow.push_back( if3( validHit, puHit.localIndex, -1 ) );
      }

      ringV.chi2 = ringV.chi2 / ( ringV.NHits - 3)/.3/.3;
      const fvec quality = ringV.NHits;

      
        // for( iP j = ringV.Hits.begin(); j != ringV.Hits.end(); ++j ){
        //   if( (*j)->quality<quality ) (*j)->quality = quality;
        // } 
   
        //quality *= ShadowOpacity;
      for( int i_4 = 0; (i_4 < fvecLen); i_4++) { 
        const int NShadow = Shadow.size();
        for( int is = 0; is < NShadow; is++ ) { // CHECKME change loops to speed up?
          cout << i_4 << Shadow[is] << endl;
           float ih_f = (Shadow[is])[i_4]-200;
          if (ih_f == -1) continue;
           int ih = static_cast<int>(ih_f);  // TODO ! problem in conversion...
           float ih_f2 =  static_cast<float>(ih);
          cout << ih_f << " " << ih << " " << ih_f2 << " " << ih%fvecLen << " " << ih/fvecLen << endl;

          const THitIndex ih_4 = ih%fvecLen;
          ENNHitV & hitV = HitsV[ih/fvecLen];

            //          hitV.quality[ih_4] = ( hitV.quality[ih_4] < quality[i_4] ) ? quality[i_4] : hitV.quality[ih_4];
            //        shHit->quality = if3( shHit->quality < quality, quality, shHit->quality );
        }
      } // i_4
    }
#endif // 0
      ////////////////
    
    for( int i_4 = 0; (i_4 < fvecLen); i_4++) { 
        //      if( NRingHits < MinRingHits || R2 > R2Max || R2 < R2Min ) continue;
      
      if ( /*ISUNLIKELY*/ ( !validRing[i_4] ) ) continue;
            
      ENNRing voidRing;
      Rings.push_back( voidRing );
      ENNRing &ring = Rings.back();
      ring.localIHits.reserve(15);
      ring.x = iHit.x[i_4]+X[i_4];
      ring.y = iHit.y[i_4]+Y[i_4];
      ring.r = R[i_4];
      ring.localIHits.push_back(iHit.localIndex[i_4]);
      ring.NHits = 1;
      ring.chi2  = 0;
      vector<THitIndex> Shadow; // save hit indices of hits, which's quality will be changed
      Shadow.reserve(15);
      Shadow.push_back(iHit.localIndex[i_4]);
      for( THitIndex ih = 0; ih < SearchAreaSize[i_4]; ih++){
        ENNSearchHitV &sHit = SearchArea[ih];
        const float dx = sHit.x[i_4] - ring.x;
        const float dy = sHit.y[i_4] - ring.y;
        const float d = fabs( sqrt(dx*dx+dy*dy) - R[i_4] );
        if( ISLIKELY ( d <= HitSize ) ){
          ring.chi2 += d*d;
          ring.localIHits.push_back(int(sHit.localIndex[i_4]));
          ring.NHits++;
          if( d <= ShadowSize ) Shadow.push_back(sHit.localIndex[i_4]);
        }
      }	   
      for( int ipu = 0; ipu < PickUpAreaSize[i_4]; ipu++ ) {
        ENNHitV &puHit = PickUpArea[ipu];
        const float dx = puHit.x[i_4] - ring.x;
        const float dy = puHit.y[i_4] - ring.y;
        const float d = fabs( sqrt(dx*dx+dy*dy) - ring.r );
        if( ISLIKELY ( d <= HitSize ) ){
          ring.chi2 += d*d;
          ring.localIHits.push_back(puHit.localIndex[i_4]);
          ring.NHits++;
          if( d <= ShadowSize ) Shadow.push_back(puHit.localIndex[i_4]);
        }
      }
      if( ISUNLIKELY( ring.NHits < MinRingHits ) ){ 
        Rings.pop_back(); 
        continue; 
      }
      ring.chi2 = ring.chi2 / ( ring.NHits - 3)/.3/.3;
      int quality = ring.NHits;

        // for( iP j = ring.Hits.begin(); j != ring.Hits.end(); ++j ){
        //   if( (*j)->quality<quality ) (*j)->quality = quality;
        // } 
   
        //quality *= ShadowOpacity;
      const int NShadow = Shadow.size();
      for( int is = 0; is < NShadow; is++ ) {
        const THitIndex ih = Shadow[is];
        const THitIndex ih_4 = ih%fvecLen;
        ENNHitV & hitV = HitsV[ih/fvecLen];

        hitV.quality[ih_4] = ( hitV.quality[ih_4] < quality ) ? quality : hitV.quality[ih_4];
//        shHit->quality = if3( shHit->quality < quality, quality, shHit->quality );
      }
    } // i_4

    
#ifdef PRINT_TIMING     
      GetTimer("Ring finding: Store ring").Stop();
#endif // PRINT_TIMING
  } // i_main
#ifdef PRINT_TIMING  
  GetTimer("Ring finding").Stop();
  
    // SELECTION OF RINGS
  GetTimer("Selection").Start(0);
#endif // PRINT_TIMING
  typedef vector<ENNRing>::iterator iR;	
  iR Rbeg = Rings.begin() + NInRings;
  iR Rend = Rings.end();

  const int NHitsV = HitsV.size();
  for( int ih = 0; ih < NHitsV; ih++ ) HitsV[ih].quality = 0.;
  for( iR ir = Rbeg; ir != Rend; ++ir ){
    ir->on = 0;
    ir->NOwn = ir->NHits;
    ir->skip = ( ( ir->NHits < MinRingHits ) || 
                 ( ir->NHits <= 6 && ir->chi2  > .3 ) );
  }

  do{
    iR best = Rend;
    THitIndex bestOwn  = 0;
    float bestChi2 = 1.E20;    
    for( iR ir = Rbeg; ir != Rend; ++ir ){
        // const bool skip = ir->skip || ( ( ir->NOwn  < 1.0*MinRingHits ) || 
        //                                 ( ir->NHits < 10 && ir->NOwn < 1.00*ir->NHits ) || 
        //                                 ( ir->NOwn  < .60*ir->NHits ) );
      if( ir->skip ) continue; // faster with if
      const bool skip = ( ( ir->NOwn  < 1.0*MinRingHits ) || 
                          ( ir->NHits < 10 && ir->NOwn < 1.00*ir->NHits ) || 
                          ( ir->NOwn  < .60*ir->NHits ) );
      ir->skip = skip;
      const bool isBetter = !skip & ( ( ir->NOwn >  1.2*bestOwn ) || 
                                      ( ir->NOwn >= 1.*bestOwn && ir->chi2 < bestChi2 ) );
      bestOwn  = (isBetter) ? ir->NOwn : bestOwn;//Hits;
      bestChi2 = (isBetter) ? ir->chi2 : bestChi2;
      best     = (isBetter) ? ir       : best;
    }
    if( best == Rend ) break;    
    best->skip = 1;
    best->on   = 1;
    const THitIndex NHitsBest = best->localIHits.size();
    for( THitIndex iih = 0; iih < NHitsBest; iih++ ) {
      const THitIndex ih = best->localIHits[iih];
      HitsV[ih/fvecLen].quality[ih%fvecLen] = 1;
    }
    for( iR ir = Rbeg; ir != Rend; ++ir ){
      if( ir->skip ) continue;
      float dist = ir->r + best->r + HitSize2[0];
      if( fabs(ir->x - best->x) > dist || 
	  fabs(ir->y - best->y) > dist ) continue;
      ir->NOwn = 0;
      const THitIndex NHitsCur = ir->localIHits.size();
      for( THitIndex iih = 0; iih < NHitsCur; iih++ ) {
        const THitIndex ih = ir->localIHits[iih];
        ir->NOwn += ( HitsV[ih/fvecLen].quality[ih%fvecLen]==0 );
      }
    }
  }while(1);
#ifdef PRINT_TIMING
  GetTimer("Selection").Stop();
  GetTimer("All").Stop();
#endif // PRINT_TIMING
}

TStopwatch& CbmL1RichENNRingFinderParallel::GetTimer( TString t ){
  int i = 0;
  for( ; (i < NTimers) && (fTimersNames[i] != t); i++);
  assert(i < NTimers);
  return fTimers[i];
}
