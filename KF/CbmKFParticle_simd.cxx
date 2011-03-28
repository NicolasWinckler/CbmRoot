//3456789012345678901234567890123456789012345678901234567890123456789012
/** Implementation of the CbmKFParticle_simd class
 *
 * @author  S.Gorbunov, I.Kisel
 * @version 1.0
 * @since   06.02.06
 *
 */

#include "CbmKFParticle_simd.h"
#include "CbmKF.h"
#include "CbmKFMath.h"
#include "CbmKFTrack.h"
#include "CbmStsKFTrackFitter.h"
#include "TMath.h"

#include "CbmKFHit.h"
#include "CbmStsHit.h"

//#include "CbmKFField.h"

#include "TStopwatch.h"
//#include "MySimdTest.h"

#include <cmath>
#include <vector>

#define cnst static const fvec

#include <new>
namespace new_obj { 
    //template<typename T> static T *allocate(size_t n=1) { return static_cast<T*>(_mm_malloc(sizeof(T)*n, 16)); }
    //template<class T> static T *allocate(size_t n=1) { return static_cast<T*>(_mm_malloc(sizeof(T)*n, 16)); }
    //template<typename T> static void liberate(T *p) { _mm_free(p); }
    //template<typename T> static T *construct(size_t n=1) { return n > 1 ? new (allocate<T>(n)) T[n] : new (allocate<T>(n)) T; }
    // etc...
}

using namespace std;

CbmKFParticle_simd::CbmKFParticle_simd()
{
  cnst ZERO = 0.0, ONE = 1.;

  NDF = ZERO;
  Q = ZERO;

  for(int i=0; i<8; i++)
    r[i] = ZERO;
  for(int i=0; i<36; i++)
    C[i] = ZERO;
  for(int i=0; i<3; i++)
  {
    fVtxGuess[i] = ZERO;
    fVtxErrGuess[i] = ONE;
  }
  fIsVtxGuess = 0;
  fIsVtxErrGuess = 0;
}

CbmKFParticle_simd::CbmKFParticle_simd(CbmKFTrackInterface &Track)
{
  fvec m[6];
  fvec V[15];
  fvec TrMass;

  CbmKFTrack Tr(Track);
  for(unsigned int i=0; i<6; i++)
    m[i] = Tr.GetTrack()[i];
  for(unsigned int i=0; i<15; i++)
    V[i] = Tr.GetCovMatrix()[i];
  TrMass = Tr.GetMass();
  NDF  = Tr.GetRefNDF();
  Chi2 = Tr.GetRefChi2();

  fvec a = m[2], b = m[3], qp = m[4];
  
  //* convert the track to (x,y,px,py,pz,e,t/p) parameterization

  fvec c2 = 1./(1. + a*a + b*b);
  fvec pq = 1./qp;
  fvec p2 = pq*pq;
  fvec pz = sqrt(p2*c2);
  fvec px = a*pz;
  fvec py = b*pz;
  fvec E = sqrt( TrMass*TrMass + p2 );

  fvec H[3] = { -px*c2, -py*c2, -pz*pq };
  fvec HE = -pq*p2/E;
	  
  r[0] = m[0];
  r[1] = m[1];
  r[2] = m[5];
  r[3] = px;
  r[4] = py;
  r[5] = pz;
  r[6] = E;
  r[7] = 0;

  fvec cxpz = H[0]*V[ 3] + H[1]*V[ 6] + H[2]*V[10];
  fvec cypz = H[0]*V[ 4] + H[1]*V[ 7] + H[2]*V[11];
  fvec capz = H[0]*V[ 5] + H[1]*V[ 8] + H[2]*V[12];
  fvec cbpz = H[0]*V[ 8] + H[1]*V[ 9] + H[2]*V[13];
  fvec cqpz = H[0]*V[12] + H[1]*V[13] + H[2]*V[14];
  fvec cpzpz = H[0]*H[0]*V[5] +H[1]*H[1]*V[9] + H[2]*H[2]*V[14] 
    + 2*( H[0]*H[1]*V[8] +H[0]*H[2]*V[12] +H[1]*H[2]*V[13]);

  C[ 0] = V[0];
  C[ 1] = V[1];
  C[ 2] = V[2];
  C[ 3] = 0;
  C[ 4] = 0;
  C[ 5] = 0;
  C[ 6] = V[3]*pz + a*cxpz; 
  C[ 7] = V[4]*pz + a*cypz; 
  C[ 8] = 0;
  C[ 9] = V[5]*pz*pz + 2*a*pz*capz + a*a*cpzpz;
  C[10] = V[6]*pz+b*cxpz; 
  C[11] = V[7]*pz+b*cypz; 
  C[12] = 0;
  C[13] = V[8]*pz*pz + a*pz*cbpz + b*pz*capz + a*b*cpzpz;
  C[14] = V[9]*pz*pz + 2*b*pz*cbpz + b*b*cpzpz;
  C[15] = cxpz; 
  C[16] = cypz; 
  C[17] = 0;
  C[18] = capz*pz + a*cpzpz;
  C[19] = cbpz*pz + b*cpzpz;
  C[20] = cpzpz;
  C[21] = HE*V[10];
  C[22] = HE*V[11];
  C[23] = 0;
  C[24] = HE*( V[12]*pz + a*cqpz );
  C[25] = HE*( V[13]*pz + b*cqpz );
  C[26] = HE*cqpz;
  C[27] = HE*HE*V[14];	    
  C[28] = C[29] = C[30] = C[31] = C[32] = C[33] = C[34] = 0;
  C[35] = 1.;

  for(int j=0; j<fvecLen; j++)
    Q[j] = (qp[j]>0.) ?1 :( (qp[j]<0) ?-1 :0);
  AtProductionVertex = 1;
  fIsVtxGuess = false;
  fIsVtxErrGuess = false;

//  SetField(Track);
//  SetField();

}

CbmKFParticle_simd::CbmKFParticle_simd( CbmKFTrackInterface* Track[])
{
  Create(Track);
}

void CbmKFParticle_simd::Create(CbmKFTrackInterface* Track[], int NTracks)
{
  fvec m[6];
  fvec V[15];
  fvec TrMass;

  double *m1[fvecLen];
  double *V1[fvecLen];
  CbmKFTrack *Tr[fvecLen];
  for (int j=0; j<NTracks; j++)
  {
//cout << "rrrrrrrrrrrrrrrr   " << Track[j].GetTrack()[0] << endl;

    //CbmKFTrackInterface tr_temp = *Track[j];
 ///   Tr[j] = (CbmKFTrack*) _mm_malloc(sizeof(CbmKFTrack), 16);
    Tr[j] = new CbmKFTrack(*Track[j]);
//cout << "rrrrrrrrrrrrrrrr   " << Tr[j]->GetTrack()[0] << endl;
    m1[j] = Tr[j]->GetTrack();
    V1[j] = Tr[j]->GetCovMatrix();
    TrMass[j] = Tr[j]->GetMass();
    NDF[j] = Tr[j]->GetRefNDF();
    Chi2[j] = Tr[j]->GetRefChi2();

    for(int i = 0; i < 6; i++) m[i][j] = m1[j][i];
    for(int i = 0; i < 15; i++) V[i][j] = V1[j][i];

  ///  _mm_free(Tr[j]);
  }

  fvec a = m[2], b = m[3], qp = m[4];
  
  //* convert the track to (x,y,px,py,pz,e,t/p) parameterization

  fvec c2 = 1./(1. + a*a + b*b);
  fvec pq = 1./qp;
  fvec p2 = pq*pq;
  fvec pz = sqrt(p2*c2);
  fvec px = a*pz;
  fvec py = b*pz;
  fvec E = sqrt( TrMass*TrMass + p2 );
    
  fvec H[3] = { -px*c2, -py*c2, -pz*pq };
  fvec HE = -pq*p2/E;
	  
  r[0] = m[0];
  r[1] = m[1];
  r[2] = m[5];
  r[3] = px;
  r[4] = py;
  r[5] = pz;
  r[6] = E;
  r[7] = 0;

  fvec cxpz = H[0]*V[ 3] + H[1]*V[ 6] + H[2]*V[10];
  fvec cypz = H[0]*V[ 4] + H[1]*V[ 7] + H[2]*V[11];
  fvec capz = H[0]*V[ 5] + H[1]*V[ 8] + H[2]*V[12];
  fvec cbpz = H[0]*V[ 8] + H[1]*V[ 9] + H[2]*V[13];
  fvec cqpz = H[0]*V[12] + H[1]*V[13] + H[2]*V[14];
  fvec cpzpz = H[0]*H[0]*V[5] +H[1]*H[1]*V[9] + H[2]*H[2]*V[14] 
    + 2*( H[0]*H[1]*V[8] +H[0]*H[2]*V[12] +H[1]*H[2]*V[13]);

  C[ 0] = V[0];
  C[ 1] = V[1];
  C[ 2] = V[2];
  C[ 3] = 0.f;
  C[ 4] = 0.f;
  C[ 5] = 0.f;
  C[ 6] = V[3]*pz + a*cxpz; 
  C[ 7] = V[4]*pz + a*cypz; 
  C[ 8] = 0.f;
  C[ 9] = V[5]*pz*pz + 2*a*pz*capz + a*a*cpzpz;
  C[10] = V[6]*pz+b*cxpz; 
  C[11] = V[7]*pz+b*cypz; 
  C[12] = 0.f;
  C[13] = V[8]*pz*pz + a*pz*cbpz + b*pz*capz + a*b*cpzpz;
  C[14] = V[9]*pz*pz + 2*b*pz*cbpz + b*b*cpzpz;
  C[15] = cxpz; 
  C[16] = cypz; 
  C[17] = 0.f;
  C[18] = capz*pz + a*cpzpz;
  C[19] = cbpz*pz + b*cpzpz;
  C[20] = cpzpz;
  C[21] = HE*V[10];
  C[22] = HE*V[11];
  C[23] = 0.f;
  C[24] = HE*( V[12]*pz + a*cqpz );
  C[25] = HE*( V[13]*pz + b*cqpz );
  C[26] = HE*cqpz;
  C[27] = HE*HE*V[14];	    
  C[28] = C[29] = C[30] = C[31] = C[32] = C[33] = C[34] = 0;
  C[35] = 1.f;

  for(int j=0; j<fvecLen; j++)
    Q[j] = (qp[j]>0.) ?1 :( (qp[j]<0) ?-1 :0);
  AtProductionVertex = 1;
  fIsVtxGuess = false;
  fIsVtxErrGuess = false;

//  SetField(Track);
//  SetField();

  for(int j=0; j<NTracks; j++)
    if(Tr[j]) delete Tr[j];

}

void CbmKFParticle_simd::SetField(CbmKFTrackInterface *Track[])
{
/*  fvec r1[3], r2[3], r3[3];

  for(int j=0; j<fvecLen; j++)
  {
    CbmKFHit *kfhit1 = Track[j]->GetHit(0);
    CbmKFHit *kfhit2 = Track[j]->GetHit(1);
    CbmKFHit *kfhit3 = Track[j]->GetHit(2);

    r1[0][j] = ((CbmKFStsHit*)kfhit1)->FitPoint.x;
    r2[0][j] = ((CbmKFStsHit*)kfhit2)->FitPoint.x;
    r3[0][j] = ((CbmKFStsHit*)kfhit3)->FitPoint.x;

    r1[1][j] = ((CbmKFStsHit*)kfhit1)->FitPoint.y;
    r2[1][j] = ((CbmKFStsHit*)kfhit2)->FitPoint.y;
    r3[1][j] = ((CbmKFStsHit*)kfhit3)->FitPoint.y;

    r1[2][j] = ((CbmKFStsHit*)kfhit1)->FitPoint.z;
    r2[2][j] = ((CbmKFStsHit*)kfhit2)->FitPoint.z;
    r3[2][j] = ((CbmKFStsHit*)kfhit3)->FitPoint.z;

//cout <<"opana!"<<endl;
//cout << "WWWWWWWWWWWWWWWWW  "<< r1[2][j] << endl;
//cout << "WWWWWWWWWWWWWWWWW  "<< r2[2][j] << endl;
//cout << "WWWWWWWWWWWWWWWWW  "<< r3[2][j] << endl;
//    cout << r1[2][j] << "  ";
  }
//cout << "kaka"<<endl;

//cout << r1[2] << endl;

  fvec x1 = r1[2];
  fvec x2 = r2[2];
  fvec x3 = r3[2];

  fvec Y1[3],Y2[3],Y3[3]; //y = B

  CbmKFField *KFF = CbmKF::Instance()->GetKFMagneticField();
  KFF->GetField(r1,Y1);
  KFF->GetField(r2,Y2);
  KFF->GetField(r3,Y3);

//cout << "Nhits "<<Track[0]->GetNOfHits()<<"  "<<Track[1]->GetNOfHits()<<"  "<<Track[2]->GetNOfHits()<<"  "<<Track[3]->GetNOfHits()<<endl;
//cout <<"r   "<< r1[0] <<"           "<< r1[1]<< "          "<< r1[2]<< endl;
//cout <<"B   "<< Y1[0] <<"           "<< Y1[1]<< "          "<< Y1[2]<< endl;

  fvec ndr;

  for(int i=0; i<3; i++)
  {
    ndr = x3*(Y2[i] - Y1[i]) + x2*Y1[i] - x1*Y2[i];
    ndr = Y3[i] - ndr/(x2-x1);

    aB[i] = ndr/(x3*(x3-x1-x2)+x1*x2);
    bB[i] = (Y2[i] - Y1[i])/(x2 - x1) - aB[i]*(x1+x2);
    cB[i] = (x2*Y1[i] - x1*Y2[i])/(x2-x1) + aB[i]*x1*x2;
  }

 // fvec B2X = aB[0] * x2*x2 + bB[0] * x2 + cB[0];
 // cout << "!!!!!!!!!!!!!!!!!  P->z = " << GetZ() << " x2 = " << x2 << endl;
 // cout << "!!!!!!!!!!!!!!!!!  B2X  = " << Y2[0] <<"      "<< B2X << endl;*/
}


void CbmKFParticle_simd::SetField()
{
  fvec Tx = GetTx();
  fvec Ty = GetTy();

  fvec r1[3], r2[3], r3[3];

  r1[2] = GetZ() - 1.;
  r2[2] = GetZ();
  r3[2] = GetZ() + 1.;

  r1[0] = GetX() - Tx*1.;
  r2[0] = GetX();
  r3[0] = GetX() + Tx*1.;

  r1[1] = GetY() - Ty*1.;
  r2[1] = GetY();
  r3[1] = GetY() + Ty*1.;

  fvec x1 = r1[2];
  fvec x2 = r2[2];
  fvec x3 = r3[2];

  fvec Y1[3],Y2[3],Y3[3]; //y = B
  FairField *MF = CbmKF::Instance()->GetMagneticField();
  {
    double BB[3], r01[8];
    for(int j = 0; j<fvecLen; j++)
    {
      for(int i = 0; i<8; i++)
        r01[i] = r1[i][j];
      MF->GetFieldValue( r01, BB );
      for(int i = 0; i<3; i++) Y1[i][j] = BB[i];

      for(int i = 0; i<8; i++)
        r01[i] = r2[i][j];
      MF->GetFieldValue( r01, BB );
      for(int i = 0; i<3; i++) Y2[i][j] = BB[i];

      for(int i = 0; i<8; i++)
        r01[i] = r3[i][j];
      MF->GetFieldValue( r01, BB );
      for(int i = 0; i<3; i++) Y3[i][j] = BB[i];
    }
  }

  fvec ndr;

  for(int i=0; i<3; i++)
  {
    ndr = x3*(Y2[i] - Y1[i]) + x2*Y1[i] - x1*Y2[i];
    ndr = Y3[i] - ndr/(x2-x1);

    aB[i] = ndr/(x3*(x3-x1-x2)+x1*x2);
    bB[i] = (Y2[i] - Y1[i])/(x2 - x1) - aB[i]*(x1+x2);
    cB[i] = (x2*Y1[i] - x1*Y2[i])/(x2-x1) + aB[i]*x1*x2;
  }

  fvec B2X = aB[0] * x2*x2 + bB[0] * x2 + cB[0];
//  cout << "!!!!!!!!!!!!!!!!!  P->z = " << GetZ() << " x2 = " << x2 << endl;
//  cout << "!!!!!!!!!!!!!!!!!  B2X  = " << Y2[0] <<"      "<< B2X << endl;
}

void CbmKFParticle_simd::GetKFTrack( CbmKFTrackInterface** Track)
{
  fvec T[6],Cov[15];

  fvec px=r[3], py=r[4],  pz=r[5];
  fvec p = sqrt( px*px + py*py + pz*pz );
  fvec pzi = 1/pz;
  fvec qp=1/p;

  for(int j=0; j<fvecLen; j++)
    if( Q[j]!=0 ) qp[j] = Q[j]*qp[j];

  T[0] = r[0];   // dX = dx -T[2]*dz
  T[1] = r[1];   // dY = dy -T[3]*dz
  T[2] = px*pzi; // dtx= (dpx - T[2]*dpz)/pz
  T[3] = py*pzi; // dty= (dpy - T[3]*dpz)/pz
  T[4] = qp;     // dq = (px*dpx + py*dpy + pz*dpz)*(-q/p^3)
  T[5] = r[2];
  
  fvec qp3 = qp*qp*qp;
  fvec pzi2= pzi*pzi;

  fvec cXpz= C[15]-T[2]*C[17];
  fvec cYpz= C[16]-T[3]*C[17];
  fvec cqpx = -qp3*( px*C[ 9] + py*C[13] + pz*C[18] );
  fvec cqpy = -qp3*( px*C[13] + py*C[14] + pz*C[19] );
  fvec cqpz = -qp3*( px*C[18] + py*C[19] + pz*C[20] );

  Cov[ 0]= C[0] + T[2]*( - 2*C[3] + T[2]*C[5] );
  Cov[ 1]= C[1] - T[2]*C[4] + T[3]*( -C[3] + T[2]*C[5] );
  Cov[ 2]= C[2] + T[3]*( - 2*C[4] + T[3]*C[5]);
  Cov[ 3]= pzi*( C[6] - T[2]*(C[8]     + cXpz) );
  Cov[ 4]= pzi*( C[7] - T[3]*C[8] - T[2]*cYpz );
  Cov[ 5]= pzi2*( C[9] + T[2]*( -2*C[18] + T[2]*C[20]) );
  Cov[ 6]= pzi*( C[10]-T[2]*C[12]-T[3]*cXpz );
  Cov[ 7]= pzi*( C[11]-T[3]*(C[12]    +cYpz) );
  Cov[ 8]= pzi2*( C[13] - T[2]*C[19] -T[3]*C[18] + T[2]*T[3]*C[20] );
  Cov[ 9]= pzi2*( C[14] + T[3]*( -2*C[19] + T[3]*C[20]) );
  Cov[10]=-qp3*(px*C[6]+py*C[10]+pz*C[15]) - T[2]*cqpz;
  Cov[11]=-qp3*(px*C[7]+py*C[11]+pz*C[16]) - T[3]*cqpz;
  Cov[12]= pzi*( cqpx - T[2]*cqpz );
  Cov[13]= pzi*( cqpy - T[3]*cqpz );
  Cov[14]=-qp3*( px*cqpx + py*cqpy + pz*cqpz );

  for(int j=0; j<fvecLen; j++)
  {
    for(int i=0;i<6;i++) Track[j]->GetTrack()[i] =  T[i][j];
    for(int i=0;i<15;i++) Track[j]->GetCovMatrix()[i] = Cov[i][j];

    Track[j]->GetRefNDF() = (int) NDF[j];
    Track[j]->GetRefChi2() = Chi2[j];
  }
}


void CbmKFParticle_simd::GetMomentum( fvec &P, fvec &Error )
{
  fvec x = r[3];
  fvec y = r[4];
  fvec z = r[5];
  fvec x2 = x*x;
  fvec y2 = y*y;
  fvec z2 = z*z;
  fvec p2 = x2+y2+z2;
  P = sqrt(p2);
  Error = sqrt( (x2*C[9]+y2*C[14]+z2*C[20] 
		 + 2*(x*y*C[13]+x*z*C[18]+y*z*C[19]) )/p2 );
}

void CbmKFParticle_simd::GetMass( fvec &M, fvec &Error )
{
  // S = sigma^2 of m2/2

  fvec S = (  r[3]*r[3]*C[9] + r[4]*r[4]*C[14] + r[5]*r[5]*C[20] 
		  + r[6]*r[6]*C[27] 
		+2*( + r[3]*r[4]*C[13] + r[5]*(r[3]*C[18] + r[4]*C[19]) 
		     - r[6]*( r[3]*C[24] + r[4]*C[25] + r[5]*C[26] )   )
		 );
  fvec m2 = r[6]*r[6] - r[3]*r[3] - r[4]*r[4] - r[5]*r[5];
  for(int j=0; j<fvecLen; j++)
  {
    M[j]     = (            m2[j]>1.e-20 ) ? sqrt(m2[j])      :0.  ;
    Error[j] = ( S[j]>=0 && m2[j]>1.e-20 ) ? sqrt(S[j]/m2[j]) :1.e4;
  }
}


void CbmKFParticle_simd::GetDecayLength( fvec &L, fvec &Error )
{
  fvec x = r[3];
  fvec y = r[4];
  fvec z = r[5];
  fvec t = r[7];
  fvec x2 = x*x;
  fvec y2 = y*y;
  fvec z2 = z*z;
  fvec p2 = x2+y2+z2;
  L = t*sqrt(p2);
  Error = sqrt( p2*C[35] + t*t/p2*(x2*C[9]+y2*C[14]+z2*C[20]
				   + 2*(x*y*C[13]+x*z*C[18]+y*z*C[19]) )
		+ 2*t*(x*C[31]+y*C[32]+z*C[33]) 
		);
}

void CbmKFParticle_simd::GetLifeTime( fvec &TauC, fvec &Error ){
  fvec m, dm;
  GetMass( m, dm );
  fvec cTM = (-r[3]*C[31] - r[4]*C[32] - r[5]*C[33] + r[6]*C[34]);
  TauC = r[7]*m;
  Error = sqrt( m*m*C[35] + 2*r[7]*cTM + r[7]*r[7]*dm*dm);
}

void CbmKFParticle_simd::SetVtxGuess( fvec &x, fvec &y, fvec &z )
{
  fVtxGuess[0] = x;
  fVtxGuess[1] = y;
  fVtxGuess[2] = z;
  fIsVtxGuess = 1;
}

void CbmKFParticle_simd::SetVtxErrGuess( fvec &dx, fvec &dy, fvec &dz )
{
  fVtxErrGuess[0] = dx;
  fVtxErrGuess[1] = dy;
  fVtxErrGuess[2] = dz;
  fIsVtxErrGuess = 1;
}

///*                      16.09.2009
void CbmKFParticle_simd::DecayVtxAprox(vector<CbmKFParticle_simd*> vPart, fvec &XX, fvec &YY, fvec &ZZ)
{
/*
  fvec fR[(int) vPart.size()][3];
  fvec fP[(int) vPart.size()][3];
  fvec fdR[(int) vPart.size()][3];
  fvec fdP[(int) vPart.size()][3];
  fvec qb[(int) vPart.size()];



  fvec vert_x=0,vert_y=0,vert_z=0;
  fvec weight_x=0,weight_y=0,weight_z=0;

  Int_t Nvect=0;
  Int_t Nel=0;
  Int_t nv=0;

  fvec www;
  fvec Z_min[3]={0.,0.,1000.};
  Int_t nPart_min=-1;
  
  const fvec c_light =  0.000299792458;

  for( vector<CbmKFParticle_simd*>::iterator tr=vPart.begin(); tr!=vPart.end(); ++tr )
  {
    CbmKFParticle_simd Part(**tr);
    if(Part.GetZ()<Z_min[2])
    {
       Z_min[2] = Part.GetZ();
       nPart_min = nv;
    }
    nv++;
  }
  
  for( vector<CbmKFParticle_simd*>::iterator tr=vPart.begin(); tr!=vPart.end(); ++tr )
  {
    CbmKFParticle_simd Part(**tr);
//cout << "Zstart  " << Part.GetZ();
    Part.Extrapolate(Part.r , Part.GetDStoPoint(Z_min));

    fR[Nvect][0]  = Part.GetX();
    fR[Nvect][1]  = Part.GetY();
    fR[Nvect][2]  = Part.GetZ();
    fP[Nvect][0]  = Part.GetPx();
    fP[Nvect][1]  = Part.GetPy();
    fP[Nvect][2]  = Part.GetPz();
    fdR[Nvect][0] = Part.GetCovariance(0);
    fdR[Nvect][1] = Part.GetCovariance(2);
    fdR[Nvect][2] = Part.GetCovariance(5);
    fdP[Nvect][0] = Part.GetCovariance(9);
    fdP[Nvect][1] = Part.GetCovariance(14);
    fdP[Nvect][2] = Part.GetCovariance(20);
    
    fvec B[3];

      double B1[3], r01[8];
      for(int i = 0; i<8; i++) r01[i] = fR[Nvect][i];
      FairField *MF = CbmKF::Instance()->GetMagneticField();
      MF->GetFieldValue( r01, B1 );
      for(int i = 0; i<3; i++) B[i] = B1[i];
    
    qb[Nvect]     = Part.GetQ()*B[1]*c_light;
    Nvect++;
//cout << "  Zend  " << Part.GetZ()<<endl;
cout << "Bx  " << B[0]<< "By  " << B[1] << "Bz  " << B[2] << endl;
  }

  fvec t[Nvect];
  fvec d_t[Nvect];
  for(Int_t iPart = 0; iPart < Nvect-1; iPart++)
  {
    t[iPart] = 0.;
    d_t[iPart] = 0.;
  }
  fvec weight_t=0.;
  fvec weight_t_temp=0.;
  
  Int_t nCross=0;
  fvec T[Nvect][(int) Nvect*(Nvect-1)/2];
  fvec d_T[Nvect][(int) Nvect*(Nvect-1)/2];

  for(Int_t iPart = 0; iPart < Nvect-1; iPart++)
  for(Int_t jPart = iPart+1; jPart < Nvect; jPart++)
  { 
    fvec Dpy   = fP[iPart][1] - fP[jPart][1];
    fvec d_Dpy = sqrt(fdP[iPart][1]*fdP[iPart][1] + fdP[jPart][1]*fdP[jPart][1]);

    fvec DY   = fR[iPart][1] - fR[jPart][1];
    fvec d_DY = sqrt(fdR[iPart][1]*fdR[iPart][1] + fdR[jPart][1]*fdR[jPart][1]);

    fvec Dpx   = fP[iPart][0] - fP[jPart][0];
    fvec d_Dpx = sqrt(fdP[iPart][0]*fdP[iPart][0] + fdP[jPart][0]*fdP[jPart][0]);

    fvec DX   = fR[iPart][0] - fR[jPart][0];
    fvec d_DX = sqrt(fdR[iPart][0]*fdR[iPart][0] + fdR[jPart][0]*fdR[jPart][0]);

    fvec Dpz   = fP[iPart][2] - fP[jPart][2];
    fvec d_Dpz = sqrt(fdP[iPart][2]*fdP[iPart][2] + fdP[jPart][2]*fdP[jPart][2]);

    fvec DZ   = fR[iPart][2] - fR[jPart][2];
    fvec d_DZ = sqrt(fdR[iPart][2]*fdR[iPart][2] + fdR[jPart][2]*fdR[jPart][2]);

////
    fvec tij[2][3];
    fvec d_tij[2][3];
    fvec wt[2][3];
    
    fvec txy11   = fP[jPart][0]*(fR[jPart][1]-fR[iPart][1]) + fP[jPart][1]*(fR[jPart][0]-fR[jPart][0]);
    fvec txy12   = (fP[jPart][1]*fP[iPart][0] - fP[jPart][0]*fP[iPart][1]);
    fvec d_txy11 = sqrt(fdP[jPart][0]*fdP[jPart][0]*(fR[jPart][1]-fR[iPart][1])*(fR[jPart][1]-fR[iPart][1])
                          +fdP[jPart][1]*fdP[jPart][1]*(fR[jPart][0]-fR[jPart][0])*(fR[jPart][0]-fR[jPart][0])
			  +fP[jPart][0]*fP[jPart][0]*(fdR[jPart][1]*fdR[jPart][1]+fdR[iPart][1]*fdR[iPart][1])
			  +fP[jPart][1]*fP[jPart][1]*(fdR[jPart][0]*fdR[jPart][0]+fdR[iPart][0]*fdR[iPart][0]));
    fvec d_txy12 = sqrt(fdP[jPart][1]*fdP[jPart][1]*fP[iPart][0]*fP[iPart][0]
                          +fP[jPart][1]*fP[jPart][1]*fdP[iPart][0]*fdP[iPart][0]
			  +fdP[jPart][0]*fdP[jPart][0]*fP[iPart][1]*fP[iPart][1]
			  +fP[jPart][0]*fP[jPart][0]*fdP[iPart][1]*fdP[iPart][1]);
    tij[0][0]    = txy11/txy12;
    d_tij[0][0]  = sqrt(d_txy11*d_txy11/(txy12*txy12) + tij[0][0]*tij[0][0]*d_txy12*d_txy12/(txy12*txy12));

    fvec txy21   = fP[iPart][0]*(fR[jPart][1]-fR[iPart][1]) + fP[iPart][1]*(fR[jPart][0]-fR[jPart][0]);
    fvec txy22   = (fP[jPart][1]*fP[iPart][0] - fP[jPart][0]*fP[iPart][1]);
    fvec d_txy21 = sqrt(fdP[iPart][0]*fdP[iPart][0]*(fR[jPart][1]-fR[iPart][1])*(fR[jPart][1]-fR[iPart][1])
                          +fdP[iPart][1]*fdP[iPart][1]*(fR[jPart][0]-fR[jPart][0])*(fR[jPart][0]-fR[jPart][0])
			  +fP[iPart][0]*fP[iPart][0]*(fdR[jPart][1]*fdR[jPart][1]+fdR[iPart][1]*fdR[iPart][1])
			  +fP[iPart][1]*fP[iPart][1]*(fdR[jPart][0]*fdR[jPart][0]+fdR[iPart][0]*fdR[iPart][0]));
    fvec d_txy22 = sqrt(fdP[jPart][1]*fdP[jPart][1]*fP[iPart][0]*fP[iPart][0]
                          +fP[jPart][1]*fP[jPart][1]*fdP[iPart][0]*fdP[iPart][0]
			  +fdP[jPart][0]*fdP[jPart][0]*fP[iPart][1]*fP[iPart][1]
			  +fP[jPart][0]*fP[jPart][0]*fdP[iPart][1]*fdP[iPart][1]);
    tij[1][0]    = txy21/txy22;
    d_tij[1][0]  = sqrt(d_txy21*d_txy21/(txy22*txy22) + tij[1][0]*tij[1][0]*d_txy22*d_txy22/(txy22*txy22));
    
    
    fvec txz11   = fP[jPart][0]*(fR[jPart][2]-fR[iPart][2]) + fP[jPart][2]*(fR[jPart][0]-fR[jPart][0]);
    fvec txz12   = (fP[jPart][2]*fP[iPart][0] - fP[jPart][0]*fP[iPart][2]);
    fvec d_txz11 = sqrt(fdP[jPart][0]*fdP[jPart][0]*(fR[jPart][2]-fR[iPart][2])*(fR[jPart][2]-fR[iPart][2])
                          +fdP[jPart][2]*fdP[jPart][2]*(fR[jPart][0]-fR[jPart][0])*(fR[jPart][0]-fR[jPart][0])
			  +fP[jPart][0]*fP[jPart][0]*(fdR[jPart][2]*fdR[jPart][2]+fdR[iPart][2]*fdR[iPart][2])
			  +fP[jPart][2]*fP[jPart][2]*(fdR[jPart][0]*fdR[jPart][0]+fdR[iPart][0]*fdR[iPart][0]));
    fvec d_txz12 = sqrt(fdP[jPart][2]*fdP[jPart][2]*fP[iPart][0]*fP[iPart][0]
                          +fP[jPart][2]*fP[jPart][2]*fdP[iPart][0]*fdP[iPart][0]
			  +fdP[jPart][0]*fdP[jPart][0]*fP[iPart][2]*fP[iPart][2]
			  +fP[jPart][0]*fP[jPart][0]*fdP[iPart][2]*fdP[iPart][2]);
    tij[0][1]  = txz11/txz12;
    d_tij[0][1] = sqrt(d_txz11*d_txz11/(txz12*txz12) + tij[0][1]*tij[0][1]*d_txz12*d_txz12/(txz12*txz12));

    fvec txz21   = fP[iPart][0]*(fR[jPart][2]-fR[iPart][2]) + fP[iPart][2]*(fR[jPart][0]-fR[jPart][0]);
    fvec txz22   = (fP[jPart][2]*fP[iPart][0] - fP[jPart][0]*fP[iPart][2]);
    fvec d_txz21 = sqrt(fdP[iPart][0]*fdP[iPart][0]*(fR[jPart][2]-fR[iPart][2])*(fR[jPart][2]-fR[iPart][2])
                          +fdP[iPart][2]*fdP[iPart][2]*(fR[jPart][0]-fR[jPart][0])*(fR[jPart][0]-fR[jPart][0])
			  +fP[iPart][0]*fP[iPart][0]*(fdR[jPart][2]*fdR[jPart][2]+fdR[iPart][2]*fdR[iPart][2])
			  +fP[iPart][2]*fP[iPart][2]*(fdR[jPart][0]*fdR[jPart][0]+fdR[iPart][0]*fdR[iPart][0]));
    fvec d_txz22 = sqrt(fdP[jPart][2]*fdP[jPart][2]*fP[iPart][0]*fP[iPart][0]
                          +fP[jPart][2]*fP[jPart][2]*fdP[iPart][0]*fdP[iPart][0]
			  +fdP[jPart][0]*fdP[jPart][0]*fP[iPart][2]*fP[iPart][2]
			  +fP[jPart][0]*fP[jPart][0]*fdP[iPart][2]*fdP[iPart][2]);
    tij[1][1]    = txz21/txz22;
    d_tij[1][1]  = sqrt(d_txz21*d_txz21/(txz22*txz22) + tij[1][1]*tij[1][1]*d_txz22*d_txz22/(txz22*txz22));
    
    fvec tzy11   = fP[jPart][2]*(fR[jPart][1]-fR[iPart][1]) - fP[jPart][1]*(fR[jPart][2]-fR[jPart][2]);
    fvec tzy12   = (fP[jPart][1]*fP[iPart][2] - fP[jPart][2]*fP[iPart][1]);
    fvec d_tzy11 = sqrt(fdP[jPart][2]*fdP[jPart][2]*(fR[jPart][1]-fR[iPart][1])*(fR[jPart][1]-fR[iPart][1])
                          +fdP[jPart][1]*fdP[jPart][1]*(fR[jPart][2]-fR[jPart][2])*(fR[jPart][2]-fR[jPart][2])
			  +fP[jPart][2]*fP[jPart][2]*(fdR[jPart][1]*fdR[jPart][1]+fdR[iPart][1]*fdR[iPart][1])
			  +fP[jPart][1]*fP[jPart][1]*(fdR[jPart][2]*fdR[jPart][2]+fdR[iPart][2]*fdR[iPart][2]));
    fvec d_tzy12 = sqrt(fdP[jPart][1]*fdP[jPart][1]*fP[iPart][2]*fP[iPart][2]
                          +fP[jPart][1]*fP[jPart][1]*fdP[iPart][2]*fdP[iPart][2]
			  +fdP[jPart][2]*fdP[jPart][2]*fP[iPart][1]*fP[iPart][1]
			  +fP[jPart][2]*fP[jPart][2]*fdP[iPart][1]*fdP[iPart][1]);
    tij[0][2]    = tzy11/tzy12;
    d_tij[0][2]  = sqrt(d_tzy11*d_tzy11/(tzy12*tzy12) + tij[0][2]*tij[0][2]*d_tzy12*d_tzy12/(tzy12*tzy12));

    fvec tzy21   = fP[iPart][2]*(fR[jPart][1]-fR[iPart][1]) - fP[iPart][1]*(fR[jPart][2]-fR[jPart][2]);
    fvec tzy22   = (fP[jPart][1]*fP[iPart][2] - fP[jPart][2]*fP[iPart][1]);
    fvec d_tzy21 = sqrt(fdP[iPart][2]*fdP[iPart][2]*(fR[jPart][1]-fR[iPart][1])*(fR[jPart][1]-fR[iPart][1])
                          +fdP[iPart][1]*fdP[iPart][1]*(fR[jPart][2]-fR[jPart][2])*(fR[jPart][2]-fR[jPart][2])
			  +fP[iPart][2]*fP[iPart][2]*(fdR[jPart][1]*fdR[jPart][1]+fdR[iPart][1]*fdR[iPart][1])
			  +fP[iPart][1]*fP[iPart][1]*(fdR[jPart][2]*fdR[jPart][2]+fdR[iPart][2]*fdR[iPart][2]));
    fvec d_tzy22 = sqrt(fdP[jPart][1]*fdP[jPart][1]*fP[iPart][2]*fP[iPart][2]
                          +fP[jPart][1]*fP[jPart][1]*fdP[iPart][2]*fdP[iPart][2]
			  +fdP[jPart][2]*fdP[jPart][2]*fP[iPart][1]*fP[iPart][1]
			  +fP[jPart][2]*fP[jPart][2]*fdP[iPart][1]*fdP[iPart][1]);
    tij[1][2]    = tzy21/tzy22;
    d_tij[1][2]  = sqrt(d_tzy21*d_tzy21/(tzy22*tzy22) + tij[1][2]*tij[1][2]*d_tzy22*d_tzy22/(tzy22*tzy22));

    fvec a = fP[iPart][0]*fP[iPart][0] + fP[iPart][1]*fP[iPart][1] + fP[iPart][2]*fP[iPart][2];
    fvec b = fP[iPart][0]*fP[iPart][0] + fP[iPart][1]*fP[iPart][1] + fP[iPart][2]*fP[iPart][2];

    nCross++;
  }

  for(Int_t iPart = 0; iPart < Nvect; iPart++)
  {
    for(Int_t iCross = 0; iCross < nCross; iCross++)
    {
      weight_t_temp = 1./(d_T[iPart][iCross]*d_T[iPart][iCross]);
      t[iPart]   += T[iPart][iCross]*weight_t_temp;
      d_t[iPart] += d_T[iPart][iCross]*weight_t_temp*d_T[iPart][iCross]*weight_t_temp;
      weight_t += weight_t_temp;
    }
    t[iPart] /= weight_t;
    d_t[iPart] = sqrt(d_t[iPart])/weight_t;

    cout << Form("t[%i] = ",iPart) << t[iPart] << " +- " << d_t[iPart] << endl;
  }

  for(Int_t iPart = 0; iPart < Nvect; iPart++)
  {
    fvec P2 = fP[iPart][0]*fP[iPart][0] + fP[iPart][1]*fP[iPart][1] + fP[iPart][2]*fP[iPart][2];
    
    fvec fX   = fR[iPart][0] - fP[iPart][2]/qb[iPart]*(cos(qb[iPart]*t[iPart]) - 1) + fP[iPart][0]/qb[iPart]*sin(qb[iPart]*t[iPart]);
//    fvec fX   = fR[iPart][0] + fP[iPart][0]*t;
    fvec d_fX = sqrt(fdR[iPart][0]*fdR[iPart][0]
                         +fdP[iPart][2]*fdP[iPart][2]/qb[iPart]*(cos(qb[iPart]*t[iPart]) - 1)/qb[iPart]*(cos(qb[iPart]*t[iPart]) - 1)
                         +fdP[iPart][0]*fdP[iPart][0]/qb[iPart]*sin(qb[iPart]*t[iPart])/qb[iPart]*sin(qb[iPart]*t[iPart])
                         +d_t[iPart]*d_t[iPart]*(fP[iPart][0]*cos(qb[iPart]*t[iPart])+fP[iPart][2]*sin(qb[iPart]*t[iPart]))
                                               *(fP[iPart][0]*cos(qb[iPart]*t[iPart])+fP[iPart][2]*sin(qb[iPart]*t[iPart])));

    fvec fZ   = fR[iPart][2] - fP[iPart][0]/qb[iPart]*(cos(qb[iPart]*t[iPart]) - 1) - fP[iPart][2]/qb[iPart]*sin(qb[iPart]*t[iPart]);
//    fvec fZ   = fR[iPart][2] - fP[iPart][2]*t;
    fvec d_fZ = sqrt(fdR[iPart][2]*fdR[iPart][2]
                         +fdP[iPart][0]*fdP[iPart][0]/qb[iPart]*(cos(qb[iPart]*t[iPart]) - 1)/qb[iPart]*(cos(qb[iPart]*t[iPart]) - 1)
                         +fdP[iPart][2]*fdP[iPart][2]/qb[iPart]*sin(qb[iPart]*t[iPart])/qb[iPart]*sin(qb[iPart]*t[iPart])
                         +d_t[iPart]*d_t[iPart]*(fP[iPart][2]*cos(qb[iPart]*t[iPart])-fP[iPart][0]*sin(qb[iPart]*t[iPart]))
                                               *(fP[iPart][2]*cos(qb[iPart]*t[iPart])-fP[iPart][0]*sin(qb[iPart]*t[iPart])));

    fvec fY   = fR[iPart][1] - fP[iPart][1]*t[iPart];
    fvec d_fY = sqrt(fdR[iPart][1]*fdR[iPart][1] + fdP[iPart][1]*fdP[iPart][1]*t[iPart]*t[iPart] + fP[iPart][1]*fP[iPart][1]*d_t[iPart]*d_t[iPart]);
    
///cout << "x0  " << fR[iPart][0]<< "  y0  " << fR[iPart][1]<< "  z0  " << fR[iPart][2]<<endl;
///cout << "fX  " << fX << "  fY  " << fY << "  fZ  "<<fZ << endl;

    vert_x += fX*fabs(fX/d_fX);
    vert_y += fY*fabs(fY/d_fY);
    vert_z += fZ*fabs(fZ/d_fZ);

    weight_x += fabs(fX/d_fX);
    weight_y += fabs(fY/d_fY);
    weight_z += fabs(fZ/d_fZ);
  }
 //       cout << "X1  "<<fX1 << "  Y1  " << fY1 << "  Z1  " << fZ1<<endl;
 //     cout << "X10 "<<x0[iPart] << "  Y10 " << y0[iPart] << "  Z10 " << z0[iPart]<<endl;
 //     cout << "p1y/qb[iPart] "<< fP[iPart][1]/qb[iPart] << "  p1x/qb[iPart] "<< fP[iPart][0]/qb[iPart] <<endl;
 //     cout << "det3  " << det3 << endl;
//      cout << "dX  "<<fX1-x01 << "  dY  " << fY1-y01 << "  dZ  " << fZ1-z01<<endl;
  vert_x /= weight_x;
  vert_y /= weight_y;
  vert_z /= weight_z;

  XX = vert_x;
  YY = vert_y;
  ZZ = vert_z;
*/
//  cout << "  vx  "<<vert_x<<"  vy  "<<vert_y<<"  vz  "<<vert_z<<endl;
}

#undef cnst


