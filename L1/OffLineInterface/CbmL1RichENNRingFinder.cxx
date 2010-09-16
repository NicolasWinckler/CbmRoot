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
// CBM includes
#include "CbmL1RichENNRingFinder.h"

#include "CbmRichHit.h"
#include "CbmRichRing.h"

#include "TClonesArray.h"
#include "TStopwatch.h"

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


ClassImp(CbmL1RichENNRingFinder)

CbmL1RichENNRingFinder::CbmL1RichENNRingFinder(Int_t verbose )
{
  fVerbose = verbose;
  fNEvents=0;
  fRecoTime = 0;
}

CbmL1RichENNRingFinder::~CbmL1RichENNRingFinder()
{
}


void CbmL1RichENNRingFinder::Init()
{ 
}

Int_t CbmL1RichENNRingFinder::DoFind( TClonesArray* HitArray, TClonesArray* ProjArray,
				      TClonesArray* RingArray )
{
  if( !RingArray || !HitArray ) return 0;

  RingArray->Clear();

  vector<ENNHit> Up;
  vector<ENNHit> Down;

  const Int_t nhits = HitArray->GetEntriesFast();  

  for ( register Int_t i = 0; i < nhits; ++i ){
    CbmRichHit * hit = (CbmRichHit*) HitArray->At( i );
    if ( !hit ) continue;
    ENNHit tmp;
    tmp.x = hit->GetX();
    tmp.y = hit->GetY();
    tmp.index = i;
    tmp.quality = 0;    
    if ( tmp.y > 0. ) Up.push_back(tmp);
    else Down.push_back(tmp);
  }
    
  sort( Up.begin(), Up.end(), CompareENNHits );
  sort( Down.begin(), Down.end(), CompareENNHits );

  vector< ENNRing > R;

  double HitSize = 1.;
  int MinRingHits = 5;
  double RMin = 2.; 
  double RMax = 7.; 

  fRecoTime = 0;
  TStopwatch timer;

  ENNRingFinder( Up, R, HitSize, MinRingHits, RMin, RMax );
  ENNRingFinder( Down, R, HitSize, MinRingHits, RMin, RMax );

  timer.Stop();
  
  Int_t NRings = 0;
  for( vector<ENNRing>::iterator i=R.begin(); i!=R.end(); ++i ){
    if( !i->on ) continue;
    new((*RingArray)[NRings]) CbmRichRing();
    CbmRichRing *ring = (CbmRichRing*) RingArray->At(NRings);
    NRings++;
    ring->SetCenterX ( i->x );
    ring->SetCenterY ( i->y );
    ring->SetRadius  ( i->r );
    for( vector<ENNHit*>::iterator j=i->Hits.begin(); j!=i->Hits.end(); j++ ){
      //CbmRichHit *hit = (CbmRichHit*)HitArray->At( );
      ring->AddHit((*j)->index ); 
    }
  }  

  fNEvents++;
  fRecoTime+= timer.CpuTime();
  cout.setf(ios::fixed);
  cout.setf(ios::showpoint);
  cout.precision(4);
  cout << "L1ENN Reco time all/this [ms]  : " 
       << fRecoTime*1000./fNEvents << "/" << timer.CpuTime()*1000.
       << endl;

  return NRings;
}


void CbmL1RichENNRingFinder::ENNRingFinder( vector<ENNHit> &Hits, vector<ENNRing> &Rings,
					    double HitSize, int MinRingHits, 
					    double RMin, double RMax ){  
 
  // INITIALIZATION
  
  const double Rejection = .5;
  const double ShadowSize = HitSize/4;
  const int    StartHitMaxQuality = 10;
  const int    SearchHitMaxQuality = 10;

  const double R2Min = RMin*RMin, R2Max = RMax*RMax;  
  const double HitSize2 = 2 * HitSize;
//  const double HitSize4 = 4 * HitSize;
//  const double HitSizeSq4 = HitSize2 * HitSize2;
  const double AreaSize  = 2 * RMax + HitSize;
  const double AreaSize2 = AreaSize * AreaSize;  

  typedef vector<ENNHit>::iterator iH;
  typedef vector<ENNHit*>::iterator iP;

  //Rings.clear();
  //sort( Hits.begin(), Hits.end(), CompareENNHits );

  int NInRings = Rings.size();

  // MAIN LOOP OVER HITS
    
  iH ileft = Hits.begin(), iright = ileft, i = ileft;
  int iih=0;
  for( ; i != Hits.end(); ++i, iih++ ){

    if( i->quality >= StartHitMaxQuality ) continue;  // already found hit
      
    double left  = i->x - AreaSize;
    double right = i->x + AreaSize;
      
    while( ileft->x < left ) ++ileft;
    while( iright != Hits.end() && iright->x < right ) ++iright;
         
    vector<ENNHit*> SearchArea;
    vector<ENNHit*> PickUpArea;   
      
    double X = 0, Y = 0, R = 0, R2 = 0;
    int    NRingHits = 1;
    double Dmax      = 0.;
    double S0, S1, S2, S3, S4, S5, S6, S7;
    int NAreaHits = 0;

    { // initialize hits in the search area

      S0 = S1 = S2 = S3 = S4 = 0.;
      
      for( iH j = ileft; j != iright; ++j ){
	if( j == i ) continue;	    
	j->ly  = j->y - i->y;
	if( fabs(j->ly) > AreaSize ) continue;	
	j->lx  = j->x - i->x;	
	j->S0  = j->lx * j->lx;
	j->S1  = j->ly * j->ly;
	j->lr2 = j->S0 + j->S1;	
	if( j->lr2 > AreaSize2 ) continue;
	NAreaHits++;	
	if( j->quality >= SearchHitMaxQuality ){
	  PickUpArea.push_back( &*j ); 
	  continue;
	}
	SearchArea.push_back( &*j );		 
	
	double &lr2 = j->lr2;
	double lr = sqrt(lr2);
	if( lr > Dmax ) Dmax = lr;	  
	  
	j->S2 = j->lx * j->ly;
	j->S3 = j->lx * lr2;
	j->S4 = j->ly * lr2;	    
	j->C = -lr/2;

	if( lr > 1.E-4 ){
	  double w = 1./lr, w2 = w*w;
	  j->Cx = w*j->lx;
	  j->Cy = w*j->ly;
	  S0 += w2*j->S0;
	  S1 += w2*j->S1;
	  S2 += w2*j->S2;
	  S3 += w2*j->S3;
	  S4 += w2*j->S4;
	}else {
	  j->Cx = j->Cy = 0;
	  S0 += j->S0;
	  S1 += j->S1;
	  S2 += j->S2;
	  S3 += j->S3;
	  S4 += j->S4;
	}
      }
      if(  NAreaHits+1 < MinRingHits  ) continue;
    }// end of initialization of the search area

    // loop for minimization of E and noise rejection
      
    do{
      // calculate parameters of the current ring
      double tmp = S0*S1-S2*S2;
      if( fabs(tmp) < 1.E-10 ) break;
      tmp = 0.5/tmp; 
      X = (S3*S1 - S4*S2)*tmp;
      Y = (S4*S0 - S3*S2)*tmp;
      R2 = X*X + Y*Y;
      R = sqrt( R2 );

      double Dcut      = Dmax * Rejection;            // cut for noise hits 
//      double RingCut   = HitSize4 * R ; // closeness
      S0 = S1 = S2 = S3 = S4 = 0.0;
      NRingHits = 1;
      NAreaHits = 0;
      Dmax = -1.;

      for( iP j = SearchArea.begin(); j != SearchArea.end(); ++j ){  
	double dx = (*j)->lx - X;
	double dy = (*j)->ly - Y;
	double d2 = dx*dx + dy*dy - R2 ;
	// oshibka pri d2=0
	//(*j)->on_ring = ( fabs( d2 - HitSizeSq4 ) <= RingCut );
	double d = fabs(sqrt(dx*dx + dy*dy) - R);
	(*j)->on_ring = ( d <= HitSize2 );

	if( (*j)->on_ring ){ 
	  NRingHits++;
	  S0 += (*j)->S0;
	  S1 += (*j)->S1;
	  S2 += (*j)->S2;
	  S3 += (*j)->S3;
	  S4 += (*j)->S4;
	}else {
	  double dp = fabs( (*j)->C + (*j)->Cx*X + (*j)->Cy*Y );
	  if( dp > Dcut ) continue;
	  if( dp > Dmax ) Dmax = dp;
	  NAreaHits++;	  
	  double w = 1./(1.e-5+fabs(d2));
	  S0 += w*(*j)->S0;
	  S1 += w*(*j)->S1;
	  S2 += w*(*j)->S2;
	  S3 += w*(*j)->S3;
	  S4 += w*(*j)->S4;
	}
      }            
    }while( Dmax > 0 && NRingHits + NAreaHits >= MinRingHits );

    // store the ring if it is found
      
    if( NRingHits < MinRingHits || R2 > R2Max || R2 < R2Min ) continue;
    
    { // final fit of 3 parameters (X,Y,R)      
      int n = 1;
      S0 = S1 = S2 = S3 = S4 = S5 = S6 = S7 = 0.0;
      for( iP j = SearchArea.begin(); j != SearchArea.end(); ++j ){
	if( !(*j)->on_ring ) continue;
	S0 += (*j)->S0;
	S1 += (*j)->S1;
	S2 += (*j)->S2;
	S3 += (*j)->S3;
	S4 += (*j)->S4;
	S5 += (*j)->lx;
	S6 += (*j)->ly;
	S7 += (*j)->lr2;
	n++;
      }
      double s0 = S6*S0-S2*S5;
      double s1 = S0*S1-S2*S2;
      double s2 = S0*S4-S2*S3;
      if( fabs(s0) < 1.E-6 || fabs(s1) < 1.E-6 ) continue;
      double tmp = s1*(S5*S5-n*S0)+s0*s0;
      double A = ( ( S0*S7-S3*S5 )*s1-s2*s0 ) / tmp;
      Y = (s2 + s0*A )/s1/2;
      X = ( S3 + S5*A - S2*Y*2 )/S0/2;
      R2 = X*X+Y*Y-A;
      if( R2 < 0 ) continue;
      R = sqrt( R2 );
    }// end of the final fit
      
    if( R2 > R2Max || R2 < R2Min ) continue;
      
    ENNRing tmp;
    Rings.push_back( tmp );
    ENNRing &ring = Rings.back();
    ring.x = i->x+X;
    ring.y = i->y+Y;
    ring.r = R;
    ring.Hits.push_back(&*i);
    ring.NHits = 1;
    ring.chi2  = 0;
    vector<ENNHit*> Shadow;
    Shadow.push_back(&*i);
    for( iP j = SearchArea.begin(); j != SearchArea.end(); ++j ){
      double dx = (*j)->x - ring.x;
      double dy = (*j)->y - ring.y;
      double d = fabs( sqrt(dx*dx+dy*dy) - R );
      if( d <= HitSize ){
	ring.chi2 += d*d;
	ring.Hits.push_back(*j);
	ring.NHits++;
      }
      if( d<= ShadowSize ) Shadow.push_back(*j);
    }	   
    for( iP j = PickUpArea.begin(); j != PickUpArea.end(); ++j ){
      double dx = (*j)->x - ring.x;
      double dy = (*j)->y - ring.y;
      double d = fabs( sqrt(dx*dx+dy*dy) - ring.r );
      if( d <= HitSize ){
	ring.chi2 += d*d;
	ring.Hits.push_back(*j);
	ring.NHits++;
      }
      if( d<= ShadowSize ) Shadow.push_back(*j);
    }
    if( ring.NHits < MinRingHits ){ 
      Rings.pop_back(); 
      continue; 
    }
    ring.chi2 = ring.chi2 / ( ring.NHits - 3)/.3/.3;
    double quality = ring.NHits;
    for( iP j = ring.Hits.begin(); j != ring.Hits.end(); ++j ){
      //if( (*j)->quality<quality ) (*j)->quality = quality;
    }    
    //quality *= ShadowOpacity;
    for( iP j = Shadow.begin(); j != Shadow.end(); ++j ){
      if( (*j)->quality<quality ) (*j)->quality = quality;
    }
  }// END OF THE MAIN LOOP OVER HITS

  
  // SELECTION OF RINGS

  typedef vector<ENNRing>::iterator iR;	
  iR Rbeg = Rings.begin() + NInRings;
  iR Rend = Rings.end();

  for( iH ih = Hits.begin(); ih != Hits.end(); ++ih ) ih->quality = 0;   
  for( iR ir = Rbeg; ir != Rend; ++ir ){
    ir->skip = ir->on = 0;  
    ir->NOwn = ir->NHits;
    if( ( ir->NHits < MinRingHits ) || 
	( ir->NHits <= 6 && ir->chi2  > .3 ) ) 
      ir->skip = 1;
  }

  do{
    iR best = Rend;
    int    bestOwn  = 0;
    double bestChi2 = 1.E20;    
    for( iR ir = Rbeg; ir != Rend; ++ir ){
      if( ir->skip ) continue;
      if( ( ir->NOwn < 1.0*MinRingHits ) || 
	  ( ir->NHits < 10 && ir->NOwn < 1.00*ir->NHits ) || 
	  ( ir->NOwn < .60*ir->NHits )
	  ){
	ir->skip = 1;
	continue;
      }
      if( ( ir->NOwn >  1.2*bestOwn ) || 
	  ( ir->NOwn >= 1.*bestOwn   &&  ir->chi2 < bestChi2 ) ){
	bestOwn  = ir->NOwn;//Hits;
	bestChi2 = ir->chi2;
	best = ir;
      }
    }
    if( best == Rend ) break;    
    best->skip = 1;
    best->on   = 1;
    for( iP ip = best->Hits.begin(); ip != best->Hits.end(); ++ip ) 
      (*ip)->quality = 1;    
    for( iR ir = Rbeg; ir != Rend; ++ir ){
      if( ir->skip ) continue;
      double dist = ir->r+best->r+HitSize2;
      if( fabs(ir->x-best->x) > dist || 
	  fabs(ir->y-best->y) > dist ) continue;
      ir->NOwn = 0;
      for( iP j = ir->Hits.begin(); j != ir->Hits.end(); ++j ){
	if( (*j)->quality==0 ) ir->NOwn++;
      }
    }
  }while(1);
}
