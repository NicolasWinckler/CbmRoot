#ifndef L1Algo_h
#define L1Algo_h 1

#include "L1Field.h"
#include "L1Station.h"
#include "L1StsHit.h"
#include "L1Triplet.h"
#include "L1Branch.h"

#include <iostream>
#include <vector>

class L1Algo{

 public:

  L1Station vStations[20] _fvecalignment;
  L1FieldRegion vtxFieldRegion _fvecalignment;
  L1FieldValue  vtxFieldValue _fvecalignment;

  

  void CATrackFinder();
  void KFTrackFitter();

  void FitTrack( L1TrackPar &T, short unsigned int* vHits, int NHits, bool downstream );

  void CAFindTrack( int ista, const L1Triplet* ptrip, 
		    L1Branch& newtrack, unsigned char &new_L, fscal &new_chi2, 
		    L1Branch &currenttrack, unsigned char &curr_L, fscal &curr_chi2,
		    int &NCalls);

  void Init( fscal geo[] );
   //local copy of measurements arranged vs station/planes

  int NStations, fTrackingLevel, fGhostSuppression, bla3;
  double TRACK_CHI2_CUT;
  double CATime;
  double fMomentumCutOff;
  double bla;

  std::vector< fscal > vStsStrips, vStsStripsB;
  std::vector< L1StsHit   > vStsHits;
  std::vector< unsigned char > vSFlag;  // = iStation*4 + used*2 + used_by_duplets;
  std::vector< unsigned char > vSFlagB;
  std::vector< unsigned short int > vRecoHits;
  std::vector< L1Track > vTracks;

  std::vector <L1Triplet> vTriplets;
  
  int StsHitsStartIndex[20], StsHitsStopIndex[20];
  int TripStartIndex[20], TripStopIndex[20];

  unsigned char GetFStation( unsigned char flag ){ return flag/4; }
  bool GetFUsed   ( unsigned char flag ){ return (flag&0x02)!=0; }
  bool GetFUsedD  ( unsigned char flag ){ return (flag&0x01)!=0; }

  void SetFStation ( unsigned char &flag, unsigned iStation ){ flag = iStation*4 + (flag%4); }
  void SetFUsed    ( unsigned char &flag ){ flag |= 0x02; }
  void SetFUsedD   ( unsigned char &flag ){ flag |= 0x01; }
  void SetFUnUsed  ( unsigned char &flag ){ flag &= 0xFC; }
  void SetFUnUsedD ( unsigned char &flag ){ flag &= 0xFE; }

} _fvecalignment;



inline void L1Algo::Init( fscal geo[] )
{
  int ind=0;
  {
    L1FieldValue B[3];
    fvec z[3];
    for( int i=0; i<3; i++){
      z[i] = geo[ind++]; B[i].x = geo[ind++];   B[i].y = geo[ind++];  B[i].z = geo[ind++]; 
      std::cout<<"L1Algo Input Magnetic field:"<<z[i][0]<<" "<<B[i].x[0]<<" "<<B[i].y[0]<<" "<<B[i].z[0]<<std::endl;
    }
    vtxFieldRegion.Set(B[0], z[0], B[1], z[1], B[2], z[2] );
    vtxFieldValue = B[0];
  }
  //vStations.clear();
  NStations = (int) geo[ind++];
  std::cout<<"L1Algo Input "<<NStations<<" Stations:"<<std::endl;
  for( int i=0; i<NStations; i++ ){
    L1Station &st = vStations[i];
    st.z = geo[ind++];
    st.materialInfo.thick = geo[ind++];
    st.Rmin =  geo[ind++];
    st.Rmax =  geo[ind++];
    st.materialInfo.RL    = geo[ind++];
    st.materialInfo.RadThick = st.materialInfo.thick/st.materialInfo.RL;
    st.materialInfo.logRadThick = log(st.materialInfo.RadThick);
    
    double f_phi   = geo[ind++];
    double f_sigma = geo[ind++];
    double b_phi   = geo[ind++];
    double b_sigma = geo[ind++];
    double c_f = cos(f_phi);
    double s_f = sin(f_phi);
    double c_b = cos(b_phi);
    double s_b = sin(b_phi);

    st.frontInfo.cos_phi = c_f;
    st.frontInfo.sin_phi = s_f;
    st.frontInfo.sigma2 = f_sigma*f_sigma;

    st.backInfo.cos_phi = c_b;
    st.backInfo.sin_phi = s_b;
    st.backInfo.sigma2 = b_sigma*b_sigma;

    double det = c_f*s_b - s_f*c_b;
    det *=det;
    st.XYInfo.C00 = ( s_b*s_b*f_sigma*f_sigma + s_f*s_f*b_sigma*b_sigma )/det;
    st.XYInfo.C10 =-( s_b*c_b*f_sigma*f_sigma + s_f*c_f*b_sigma*b_sigma )/det;
    st.XYInfo.C11 = ( c_b*c_b*f_sigma*f_sigma + c_f*c_f*b_sigma*b_sigma )/det;

    if( fabs(b_phi-f_phi)<.1 ){
      double th = b_phi-f_phi;
      double det = cos(th);
      det *=det;
      st.XYInfo.C00 = ( s_b*s_b*f_sigma*f_sigma + s_f*s_f*b_sigma*b_sigma )/det;
      st.XYInfo.C10 =-( s_b*c_b*f_sigma*f_sigma + s_f*c_f*b_sigma*b_sigma )/det;
      st.XYInfo.C11 = ( c_b*c_b*f_sigma*f_sigma + c_f*c_f*b_sigma*b_sigma )/det;
    }

    st.xInfo.cos_phi = c_f/(c_f*s_b - c_b*s_f);
    st.xInfo.sin_phi =-c_b/(c_f*s_b - c_b*s_f);
    st.xInfo.sigma2 = st.XYInfo.C00;

    st.yInfo.cos_phi = c_b/(c_b*s_f - c_f*s_b);
    st.yInfo.sin_phi =-c_f/(c_b*s_f - c_f*s_b);
    st.yInfo.sigma2 = st.XYInfo.C11;

    int N= (int) geo[ind++];
    for( int i=0; i<N; i++ ) st.fieldSlice.cx[i] = geo[ind++];
    for( int i=0; i<N; i++ ) st.fieldSlice.cy[i] = geo[ind++];
    for( int i=0; i<N; i++ ) st.fieldSlice.cz[i] = geo[ind++];
    std::cout<<"    "<<st.z[0] <<" "<<st.materialInfo.thick[0]<<" "<<st.materialInfo.RL[0]<<", "
	<<N<<" field coeff."<<std::endl;
    std::cout<<"       "<<f_phi<<" "<<f_sigma <<" "<<b_phi<<" "<<b_sigma <<std::endl;
    //vStations.push_back(st);
  }  

  fTrackingLevel = (int) geo[ind++];
  fMomentumCutOff = geo[ind++];
  fGhostSuppression = (int) geo[ind++];

  {
    fvec By0 = vStations[NStations-1].fieldSlice.cy[0];
    fvec z0  = vStations[NStations-1].z;
    fvec sy = 0., Sy = 0.;
    for( int i=NStations-1; i>=0; i-- ){
      L1Station &st = vStations[i];
      fvec dz = st.z-z0;
      fvec By = vStations[i].fieldSlice.cy[0];
      Sy += dz*sy + dz*dz*By/2.;
      sy += dz*By;
      st.Sy = Sy;
      z0 = st.z;
    }
  }
  std::cout<<"L1Algo initialized"<<std::endl;
}


#endif
