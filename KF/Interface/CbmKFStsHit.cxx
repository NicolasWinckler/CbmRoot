
#include "CbmKFStsHit.h"

#include "CbmKF.h"
#include "CbmKFTrackInterface.h"
#include "CbmKFMaterial.h"

#include "CbmStsHit.h"
#include "CbmMvdHit.h"
#include "CbmStsAddress.h"
//#include <ios.h>
using std::vector;
using std::ios;

ClassImp( CbmKFStsHit );

static CbmKFTube st_tube;

void CbmKFStsHit::Create( CbmStsHit *h ){

  CbmKF *KF = CbmKF::Instance();
  int id = 1000+CbmStsAddress::GetElementId(h->GetAddress(), kStsStation);//h->GetStationNr();
  MaterialIndex =  KF->GetMaterialIndex( id );

  if(  MaterialIndex>=0 ) tube = (CbmKFTube*)KF->vMaterial[MaterialIndex];
  else{
    st_tube.z = st_tube.dz = st_tube.r = st_tube.R = st_tube.rr = st_tube.RR = 0;
    tube = &st_tube;
  }
  TVector3 pos, err;
  h->Position(pos);
  h->PositionError(err);
  
  FitPoint.x = pos.X();
  FitPoint.y = pos.Y();
  FitPoint.z = pos.Z();
#if 1
  FitPoint.V[0] = err.X() * err.X();
  FitPoint.V[1] = h->GetDxy();
  FitPoint.V[2] = err.Y() * err.Y();
#else
  FitPoint.V[0] = 3.000e-06;
  FitPoint.V[1] = -1.120e-05;
  FitPoint.V[2] = 8.357e-05;
#endif // 0

}


void CbmKFStsHit::Create( CbmMvdHit *h ){

  CbmKF *KF = CbmKF::Instance();
  int id = 1100+h->GetStationNr();
//   cout << " station " << h->GetStationNr() << " has id " << id << flush;
  MaterialIndex =  KF->GetMaterialIndex( id );
//   cout << " and material index = " << MaterialIndex << endl;

  if(  MaterialIndex>=0 ) tube = (CbmKFTube*)KF->vMaterial[MaterialIndex];
  else{
    st_tube.z = st_tube.dz = st_tube.r = st_tube.R = st_tube.rr = st_tube.RR = 0;
    tube = &st_tube;
  }
  TVector3 pos, err;
  h->Position(pos);
  h->PositionError(err);
  
  FitPoint.x = pos.X();
  FitPoint.y = pos.Y();
  FitPoint.z = pos.Z();

  FitPoint.V[0] = err.X() * err.X();
  FitPoint.V[1] = 0.;
  FitPoint.V[2] = err.Y() * err.Y();
}


Int_t CbmKFStsHit::Filter( CbmKFTrackInterface &track, Bool_t downstream, Double_t &QP0 ){
  Bool_t err = 0;
  Double_t zfst, zlst, zend;
  if( downstream )
    {
      zfst = tube->z - tube->ZThickness/4.;
      zlst = tube->z + tube->ZThickness/4.;
      zend = tube->z + tube->ZThickness/2.;
    }
  else
    {
      zfst = tube->z + tube->ZThickness/4.;
      zlst = tube->z - tube->ZThickness/4.;
      zend = tube->z - tube->ZThickness/2.;
   }
  Double_t zthick = tube->ZThickness/2.;

  err = err || tube->Pass( zfst, zthick, track, downstream, QP0 );
  err = err || track.Propagate( FitPoint.z, QP0 );
  err = err || FitPoint.Filter( track );
  err = err || tube->Pass( zlst, zthick, track, downstream, QP0 ); 
  err = err || track.Propagate( zend, QP0 );
  return err;
}


////////////////////////////////////////////////////////////////////////////
// 
// mathAddMeasurements: the implementation of the Probabilistic
//                      Data Association Filter for the MAPS
// 
//
// Author : Dmitry Emeliyanov, RAL, dmitry.emeliyanov@cern.ch
//     
/////////////////////////////////////////////////////////////////////////////

void CbmKFStsHit::FilterPDAF( CbmKFTrackInterface &track, 
			       vector<CbmKFStsHit*> &vpHits,
			       Bool_t downstream, Double_t *QP0,
			       double gateX, double gateY, int &best_hit_idx ){
  
  best_hit_idx=0;
  if( vpHits.empty() ) return;
  
  double qp0 =  (QP0)? *QP0 : track.GetTrack()[4];
 
  CbmKFStsHit* h=(*vpHits.begin());

  vector<CbmKFPixelMeasurement*> vm;
  vm.clear();
  for(vector<CbmKFStsHit*>::iterator phIt=vpHits.begin(); phIt!=vpHits.end();++phIt){
    vm.push_back(&((*phIt)->FitPoint));
  }
    
  CbmKFTube *tube = h->tube;
  
  Double_t zfst, zlst, zend;
  if( downstream )
    {
      zfst = tube->z - tube->ZThickness/4.;
      zlst = tube->z + tube->ZThickness/4.;
      zend = tube->z + tube->ZThickness/2.;
    }
  else
    {
      zfst = tube->z + tube->ZThickness/4.;
      zlst = tube->z - tube->ZThickness/4.;
      zend = tube->z - tube->ZThickness/2.;
    }
  Double_t zthick = tube->ZThickness/2.;

  tube->Pass( zfst, zthick, track, downstream, qp0 );
  track.Propagate( h->FitPoint.z, qp0 );

  vector<double> vProb;
  vProb.clear();
      
  CbmKFPixelMeasurement::FilterPDAF(track, vm, gateX, gateY, vProb );
  
  int idx=0;
  double bestProb=0.0;

  for(vector<double>::iterator probIt=vProb.begin(); probIt!=vProb.end();++probIt){
    if((*probIt)>bestProb){
      bestProb=(*probIt);
      best_hit_idx=idx;
    }
    idx++;
  }
  vProb.clear();

  tube->Pass( zlst, zthick, track, downstream, qp0 ); 
  track.Propagate( zend, qp0 );
  
  if( QP0 ) *QP0 = qp0;
}
