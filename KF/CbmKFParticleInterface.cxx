
#include "CbmKFParticleInterface.h"

#define cnst static const fvec


CbmKFParticleInterface::CbmKFParticleInterface(){
  //KFPart = (CbmKFParticle_simd*) _mm_malloc(sizeof(CbmKFParticle_simd), 16);
  KFPart = new CbmKFParticle_simd();
}

CbmKFParticleInterface::CbmKFParticleInterface(CbmKFTrackInterface* Track[]){
  KFPart = new CbmKFParticle_simd();
  KFPart->Create(Track);
//  KFPart -> SetField();
}

CbmKFParticleInterface::~CbmKFParticleInterface()
{
  //_mm_free(KFPart);
  delete KFPart;
}

void CbmKFParticleInterface::Construct(CbmKFTrackInterface* vDaughters[][fvecLen],int NDaughters,CbmKFVertexInterface *Parent[],float Mass,float CutChi2)
{
  const int ND = NDaughters;

  CbmKFParticle_simd vDaughters_part[ND];

  for( int tr=0; tr<NDaughters; ++tr )
    vDaughters_part[tr].Create(vDaughters[tr]);

  Construct( vDaughters_part, NDaughters, Parent, Mass, CutChi2 );
}

void CbmKFParticleInterface::Construct( CbmKFParticle_simd* vDaughters, int NDaughters, CbmKFVertexInterface *Parent[], float Mass, float CutChi2 )
{
  //fvec *r = KFPart->GetParameters();
  //fvec *C = KFPart->GetCovMatrix();

  if(!(KFPart->fIsVtxGuess))
  {
    fvec Ty[NDaughters],Y[NDaughters],Z[NDaughters];

    for( int tr=0; tr<NDaughters; tr++ )
    {
      Y[tr]  = vDaughters[tr].GetY();
      Z[tr]  = vDaughters[tr].GetZ();
      Ty[tr] = vDaughters[tr].GetPy()/vDaughters[tr].GetPz();
    }

    fvec r0[3];
    r0[0] = 0.;
    r0[1] = 0.;
    r0[2] = (Ty[0]*Z[0]-Ty[1]*Z[1] + Y[1] -Y[0])/(Ty[0]-Ty[1]);

    Extrapolate( &vDaughters[0], vDaughters[0].r , GetDStoPoint(&vDaughters[0], r0) );

    KFPart->fVtxGuess[0] = vDaughters[0].GetX();
    KFPart->fVtxGuess[1] = vDaughters[0].GetY();
    KFPart->fVtxGuess[2] = vDaughters[0].GetZ();

    if(!(KFPart->fIsVtxErrGuess))
    {
      KFPart->fVtxErrGuess[0] = 10.*sqrt(vDaughters[0].C[0] );
      KFPart->fVtxErrGuess[1] = 10.*sqrt(vDaughters[0].C[2] );
      KFPart->fVtxErrGuess[2] = 10.*sqrt(vDaughters[0].C[5] );
    }
 //   KFPart->fVtxErrGuess[0] = 0.01*KFPart->fVtxGuess[0];
 //   KFPart->fVtxErrGuess[1] = 0.01*KFPart->fVtxGuess[1];
 //   KFPart->fVtxErrGuess[2] = 0.01*KFPart->fVtxGuess[2];
  }

  const Int_t MaxIter=3;
  cnst ZERO = 0.0, ONE = 1.;
  cnst TWO = 2.;	
  /**if( CbmKF::Instance()->vTargets.empty() ){
    r[0] = r[1] = r[2] = 0.;
    C[0] = C[2] = C[5] = 1.;
    C[1] = C[3] = C[4] = 0;
  }else{
    CbmKFTube &t = CbmKF::Instance()->vTargets[0];
    r[0] = r[1] = 0.;
    r[2] = t.z;
    C[0] = C[2] = t.R*t.R/9.;
    C[5] = t.dz/6.;
    C[1] = C[3] = C[4] = 0;
  }**/

  KFPart->r[3] = ZERO;
  KFPart->r[4] = ZERO;
  KFPart->r[5] = ZERO;
  KFPart->r[6] = ZERO;
  KFPart->r[7] = ZERO;

  KFPart->r[0] = KFPart->fVtxGuess[0];
  KFPart->r[1] = KFPart->fVtxGuess[1];
  KFPart->r[2] = KFPart->fVtxGuess[2];

  KFPart->C[0] = KFPart->fVtxErrGuess[0] * KFPart->fVtxErrGuess[0];
  KFPart->C[2] = KFPart->fVtxErrGuess[1] * KFPart->fVtxErrGuess[1];
  KFPart->C[5] = KFPart->fVtxErrGuess[2] * KFPart->fVtxErrGuess[2];
  KFPart->C[1] = KFPart->C[3] = KFPart->C[4] = ZERO;

  for ( Int_t iteration =0; iteration<MaxIter;++iteration )
  {
    fvec r0[8], C0[6];

    for( int i=0; i<8; i++) r0[i] = KFPart->r[i];
    for( int i=0; i<6; i++) C0[i] = KFPart->C[i];

    KFPart->r[3] = ZERO;
    KFPart->r[4] = ZERO;
    KFPart->r[5] = ZERO;
    KFPart->r[6] = ZERO;
   
    for(Int_t i=0;i<36;++i) KFPart->C[i]=ZERO;

    KFPart->C[0] = KFPart->fVtxErrGuess[0]*KFPart->fVtxErrGuess[0];
    KFPart->C[2] = KFPart->fVtxErrGuess[1]*KFPart->fVtxErrGuess[1];
    KFPart->C[5] = KFPart->fVtxErrGuess[2]*KFPart->fVtxErrGuess[2];

    KFPart->C[35] = ONE;

    fvec B[3];
    {
      double B1[3], r01[8];
      for(int j = 0; j<fvecLen; j++)
      {
        for(int i = 0; i<8; i++)
          r01[i] = r0[i][j];
        FairField *MF = CbmKF::Instance()->GetMagneticField();
        MF->GetFieldValue( r01, B1 );
        const float c_light =  0.000299792458;
        for(int i = 0; i<3; i++) B[i][j] = B1[i]*c_light;
      }
    }

    KFPart->NDF  =  -3.;
    KFPart->Chi2 =  ZERO;
    KFPart->Q = ZERO;

    for( int tr=0; tr<NDaughters; ++tr )
      {
	CbmKFParticle_simd Daughter = vDaughters[tr] ;
	Extrapolate(&Daughter, Daughter.r , GetDStoPoint(&Daughter, r0));

	fvec *m = Daughter.r;
	fvec *Cd = Daughter.C;

	fvec d[3] = { r0[0]-m[0], r0[1]-m[1], r0[2]-m[2] };
	fvec SigmaS = .1+10.*sqrt( (d[0]*d[0]+d[1]*d[1]+d[2]*d[2])/
				       (m[3]*m[3]+m[4]*m[4]+m[5]*m[5])  );

	fvec h[6];

	h[0] = m[3]*SigmaS;
	h[1] = m[4]*SigmaS;
	h[2] = m[5]*SigmaS; 
	h[3] = ( h[1]*B[2]-h[2]*B[1] )*Daughter.Q;
	h[4] = ( h[2]*B[0]-h[0]*B[2] )*Daughter.Q;
	h[5] = ( h[0]*B[1]-h[1]*B[0] )*Daughter.Q;

	//* Fit of daughter momentum (x,y,z) to r0[0,1,2] vertex
	{
	  fvec zeta[3] = { r0[0]-m[0], r0[1]-m[1], r0[2]-m[2] };

	  fvec Vv[6] = 
	    { Cd[ 0] + h[0]*h[0], 
	      Cd[ 1] + h[1]*h[0], Cd[ 2] + h[1]*h[1],			     
	      Cd[ 3] + h[2]*h[0], Cd[ 4] + h[2]*h[1], Cd[ 5] + h[2]*h[2] };
	  
	  fvec Vvp[9]=
	    { Cd[ 6] + h[0]*h[3], Cd[ 7] + h[1]*h[3], Cd[ 8] + h[2]*h[3],
	      Cd[10] + h[0]*h[4], Cd[11] + h[1]*h[4], Cd[12] + h[2]*h[4],
	      Cd[15] + h[0]*h[5], Cd[16] + h[1]*h[5], Cd[17] + h[2]*h[5] };

/**	  if( CutChi2 >0. ){

	    fvec Si[6]={ Vv[0]+C0[0],
			     Vv[1]+C0[1], Vv[2]+C0[2],
			     Vv[3]+C0[3], Vv[4]+C0[4], Vv[5]+C0[5] };
	    fvec S[6] = { Si[2]*Si[5] - Si[4]*Si[4],
			      Si[3]*Si[4] - Si[1]*Si[5],
			      Si[0]*Si[5] - Si[3]*Si[3],
			      Si[1]*Si[4] - Si[2]*Si[3],
			      Si[1]*Si[3] - Si[0]*Si[4],
			      Si[0]*Si[2] - Si[1]*Si[1] };		 	
	    fvec det = ( Si[0]*S[0] + Si[1]*S[1] + Si[3]*S[3] );
	    fvec chi2 = 
	      ( +(S[0]*zeta[0] + S[1]*zeta[1] + S[3]*zeta[2])*zeta[0]
		+(S[1]*zeta[0] + S[2]*zeta[1] + S[4]*zeta[2])*zeta[1]
		+(S[3]*zeta[0] + S[4]*zeta[1] + S[5]*zeta[2])*zeta[2]);
	    if( chi2 > 2*CutChi2*det ) continue;
	  }
**/
	  fvec S[6] = 
	    { Vv[2]*Vv[5] - Vv[4]*Vv[4],
	      Vv[3]*Vv[4] - Vv[1]*Vv[5], Vv[0]*Vv[5] - Vv[3]*Vv[3],
	      Vv[1]*Vv[4] - Vv[2]*Vv[3], Vv[1]*Vv[3] - Vv[0]*Vv[4], 
	      Vv[0]*Vv[2] - Vv[1]*Vv[1] };		 
	
	  fvec s = ( Vv[0]*S[0] + Vv[1]*S[1] + Vv[3]*S[3] );

          for(int j = 0; j<fvecLen; j++)
	    s[j] = ( s[j] > 1.E-20 )  ?1./s[j] :0;

	  S[0]*=s; S[1]*=s; S[2]*=s; S[3]*=s; S[4]*=s; S[5]*=s;

	  fvec Sz[3] = { (S[0]*zeta[0]+S[1]*zeta[1]+S[3]*zeta[2]),
			     (S[1]*zeta[0]+S[2]*zeta[1]+S[4]*zeta[2]),
			     (S[3]*zeta[0]+S[4]*zeta[1]+S[5]*zeta[2]) };
	  
	  fvec x = m[3] + Vvp[0]*Sz[0] + Vvp[1]*Sz[1] + Vvp[2]*Sz[2];	  
	  fvec y = m[4] + Vvp[3]*Sz[0] + Vvp[4]*Sz[1] + Vvp[5]*Sz[2];	  
	  fvec z = m[5] + Vvp[6]*Sz[0] + Vvp[7]*Sz[1] + Vvp[8]*Sz[2];	  

	  h[0] = x*SigmaS;
	  h[1] = y*SigmaS;
	  h[2] = z*SigmaS; 
	  h[3] = ( h[1]*B[2]-h[2]*B[1] )*Daughter.Q;
	  h[4] = ( h[2]*B[0]-h[0]*B[2] )*Daughter.Q;
	  h[5] = ( h[0]*B[1]-h[1]*B[0] )*Daughter.Q;
	}
	
	fvec V[28];

	V[ 0] = Cd[ 0] + h[0]*h[0];
	V[ 1] = Cd[ 1] + h[1]*h[0];
	V[ 2] = Cd[ 2] + h[1]*h[1];
	V[ 3] = Cd[ 3] + h[2]*h[0];
	V[ 4] = Cd[ 4] + h[2]*h[1];
	V[ 5] = Cd[ 5] + h[2]*h[2];
	
	V[ 6] = Cd[ 6] + h[3]*h[0];
	V[ 7] = Cd[ 7] + h[3]*h[1];
	V[ 8] = Cd[ 8] + h[3]*h[2];
	V[ 9] = Cd[ 9] + h[3]*h[3];

	V[10] = Cd[10] + h[4]*h[0];
	V[11] = Cd[11] + h[4]*h[1];
	V[12] = Cd[12] + h[4]*h[2];
	V[13] = Cd[13] + h[4]*h[3];
	V[14] = Cd[14] + h[4]*h[4];

	V[15] = Cd[15] + h[5]*h[0];
	V[16] = Cd[16] + h[5]*h[1];
	V[17] = Cd[17] + h[5]*h[2];
	V[18] = Cd[18] + h[5]*h[3];
	V[19] = Cd[19] + h[5]*h[4];
	V[20] = Cd[20] + h[5]*h[5];	  
      
	V[21] = Cd[21];
	V[22] = Cd[22];
	V[23] = Cd[23];
	V[24] = Cd[24];
	V[25] = Cd[25];
	V[26] = Cd[26];
	V[27] = Cd[27];

	//* 
	KFPart->NDF += TWO;
	KFPart->Q   += Daughter.Q;

	//*

	fvec S[6];
	{
	  fvec Si[6] = { KFPart->C[0]+V[0], 
			   KFPart->C[1]+V[1], KFPart->C[2]+V[2], 
			   KFPart->C[3]+V[3], KFPart->C[4]+V[4], KFPart->C[5]+V[5] };
	  
	  S[0] = Si[2]*Si[5] - Si[4]*Si[4];
	  S[1] = Si[3]*Si[4] - Si[1]*Si[5];
	  S[2] = Si[0]*Si[5] - Si[3]*Si[3];
	  S[3] = Si[1]*Si[4] - Si[2]*Si[3];
	  S[4] = Si[1]*Si[3] - Si[0]*Si[4];
	  S[5] = Si[0]*Si[2] - Si[1]*Si[1];	 
	
	  fvec s = ( Si[0]*S[0] + Si[1]*S[1] + Si[3]*S[3] );
          for(int j=0; j<fvecLen; j++)
	    s[j] = ( s[j] > 1.E-20 )  ?1./s[j] :0;	  

	  S[0]*=s;
	  S[1]*=s;
	  S[2]*=s;
	  S[3]*=s;
	  S[4]*=s;
	  S[5]*=s;
	}
	
	//* Residual (measured - estimated)

	fvec zeta[3] = { m[0]-KFPart->r[0], m[1]-KFPart->r[1], m[2]-KFPart->r[2] };
	
	//* Add the daughter momentum to the particle momentum

	KFPart->r[ 3] += m[ 3];
	KFPart->r[ 4] += m[ 4];
	KFPart->r[ 5] += m[ 5];
	KFPart->r[ 6] += m[ 6];

	KFPart->C[ 9] += V[ 9];
	KFPart->C[13] += V[13];
	KFPart->C[14] += V[14];
	KFPart->C[18] += V[18];
	KFPart->C[19] += V[19];
	KFPart->C[20] += V[20];
	KFPart->C[24] += V[24];
	KFPart->C[25] += V[25];
	KFPart->C[26] += V[26];
	KFPart->C[27] += V[27];

	//* CHt = CH' - D'
	
	fvec CHt0[7], CHt1[7], CHt2[7];

	CHt0[0]=KFPart->C[ 0] ;      CHt1[0]=KFPart->C[ 1] ;      CHt2[0]=KFPart->C[ 3] ;
	CHt0[1]=KFPart->C[ 1] ;      CHt1[1]=KFPart->C[ 2] ;      CHt2[1]=KFPart->C[ 4] ;
	CHt0[2]=KFPart->C[ 3] ;      CHt1[2]=KFPart->C[ 4] ;      CHt2[2]=KFPart->C[ 5] ;
	CHt0[3]=KFPart->C[ 6]-V[ 6]; CHt1[3]=KFPart->C[ 7]-V[ 7]; CHt2[3]=KFPart->C[ 8]-V[ 8];
	CHt0[4]=KFPart->C[10]-V[10]; CHt1[4]=KFPart->C[11]-V[11]; CHt2[4]=KFPart->C[12]-V[12];
	CHt0[5]=KFPart->C[15]-V[15]; CHt1[5]=KFPart->C[16]-V[16]; CHt2[5]=KFPart->C[17]-V[17];
	CHt0[6]=KFPart->C[21]-V[21]; CHt1[6]=KFPart->C[22]-V[22]; CHt2[6]=KFPart->C[23]-V[23];

	//* Kalman gain K = CH'*S
	
	fvec K0[7], K1[7], K2[7];

	for(Int_t i=0;i<7;++i){
	  K0[i] = CHt0[i]*S[0] + CHt1[i]*S[1] + CHt2[i]*S[3];
	  K1[i] = CHt0[i]*S[1] + CHt1[i]*S[2] + CHt2[i]*S[4];
	  K2[i] = CHt0[i]*S[3] + CHt1[i]*S[4] + CHt2[i]*S[5];
	}

	//* New estimation of the vertex position r += K*zeta

	for(Int_t i=0;i<7;++i) 
	  KFPart->r[i] += K0[i]*zeta[0] + K1[i]*zeta[1] + K2[i]*zeta[2];
      
	//* New covariance matrix C -= K*(CH')'

	for(Int_t i=0, k=0;i<7;++i){
	  for(Int_t j=0;j<=i;++j,++k) 
	    KFPart->C[k] -= K0[i]*CHt0[j] + K1[i]*CHt1[j] + K2[i]*CHt2[j];
	}

	//* Calculate Chi^2 & NDF

	KFPart->Chi2 += (S[0]*zeta[0] + S[1]*zeta[1] + S[3]*zeta[2])*zeta[0]
	  +     (S[1]*zeta[0] + S[2]*zeta[1] + S[4]*zeta[2])*zeta[1]
	  +     (S[3]*zeta[0] + S[4]*zeta[1] + S[5]*zeta[2])*zeta[2];

      } // add tracks 

    if( iteration==0 ){
      r0[3] = KFPart->r[3];
      r0[4] = KFPart->r[4];
      r0[5] = KFPart->r[5];
      r0[6] = KFPart->r[6];
      if( Parent ){
	fvec dx, dy, dz;
        for(int j=0; j<fvecLen; j++)
        {
          dx[j] = Parent[j]->GetRefX()-KFPart->r[0][j];
 	  dy[j] = Parent[j]->GetRefY()-KFPart->r[1][j];
  	  dz[j] = Parent[j]->GetRefZ()-KFPart->r[2][j];
        }
        r0[7] = KFPart->r[7] = sqrt( (dx*dx+dy*dy+dz*dz)
			     /(KFPart->r[3]*KFPart->r[3]+KFPart->r[4]*KFPart->r[4]+KFPart->r[5]*KFPart->r[5]) );
      }
    }

    if( Mass>=0 ) 
    {
      fvec vMass = Mass;
      MeasureMass( KFPart, r0, vMass );
    }
    if( Parent ) MeasureProductionVertex( KFPart, r0, Parent );
    
  }// iterations

   KFPart->AtProductionVertex = 0;
}

void CbmKFParticleInterface::MeasureMass(CbmKFParticle_simd*  Particle,  fvec r0[], fvec Mass )
{
  fvec *r    = Particle->GetParameters();
  fvec *C    = Particle->GetCovMatrix();

  fvec H[8];
  H[0] = H[1] = H[2] = 0.;
  H[3] = -2*r0[3]; 
  H[4] = -2*r0[4]; 
  H[5] = -2*r0[5]; 
  H[6] =  2*r0[6];
  H[7] = 0; 
  fvec m2 = r0[6]*r0[6] - r0[3]*r0[3] - r0[4]*r0[4] - r0[5]*r0[5]; 

  fvec zeta = Mass*Mass - m2;
  for(Int_t i=0;i<8;++i) zeta -= H[i]*(r[i]-r0[i]);
  
  fvec S = 0.;
  fvec CHt[8];
  for (Int_t i=0;i<8;++i ){
    CHt[i] = 0.0;
    for (Int_t j=0;j<8;++j) CHt[i] += Cij(i,j)*H[j];
    S += H[i]*CHt[i];
  }
    
  for (int gg=0; gg<fvecLen; gg++)
  {
    if( S[gg]<1.e-20 ) continue;
    S[gg] = 1./S[gg];
    Particle->Chi2[gg] += zeta[gg]*zeta[gg]*S[gg];
    Particle->NDF[gg]  += 1.;
    for( Int_t i=0, ii=0; i<8; ++i ){
      float Ki = CHt[i][gg]*S[gg];
      r[i][gg] += Ki*zeta[gg];
      for(Int_t j=0;j<=i;++j) C[ii++] -= Ki*CHt[j];    
    }
  }
}


void CbmKFParticleInterface::MeasureProductionVertex( CbmKFParticle_simd*  Particle, fvec r0[], CbmKFVertexInterface **Parent)
{
  fvec *r = Particle->GetParameters();
  fvec *C = Particle->GetCovMatrix();

  fvec m[3], V[6];
  for(int j=0; j<fvecLen; j++)
  {    
    m[0][j] = Parent[j]->GetRefX();
    m[1][j] = Parent[j]->GetRefY();
    m[2][j] = Parent[j]->GetRefZ();
    double *V1 = Parent[j]->GetCovMatrix();
    for(int i=0; i<6; i++) V[i][j]=V1[i];
  }
  r[7] = r0[7];
  Extrapolate(r0, -r0[7]);
  Convert(Particle, r0, 1);
 
  fvec Ai[6];
  Ai[0] = C[4]*C[4] - C[2]*C[5];
  Ai[1] = C[1]*C[5] - C[3]*C[4];
  Ai[3] = C[2]*C[3] - C[1]*C[4];
  fvec det = 1./(C[0]*Ai[0] + C[1]*Ai[1] + C[3]*Ai[3]);
  Ai[0] *= det;
  Ai[1] *= det;
  Ai[3] *= det;
  Ai[2] = ( C[3]*C[3] - C[0]*C[5] )*det;
  Ai[4] = ( C[0]*C[4] - C[1]*C[3] )*det;
  Ai[5] = ( C[1]*C[1] - C[0]*C[2] )*det;

  fvec B[5][3];

  B[0][0] = C[ 6]*Ai[0] + C[ 7]*Ai[1] + C[ 8]*Ai[3];
  B[0][1] = C[ 6]*Ai[1] + C[ 7]*Ai[2] + C[ 8]*Ai[4];
  B[0][2] = C[ 6]*Ai[3] + C[ 7]*Ai[4] + C[ 8]*Ai[5];

  B[1][0] = C[10]*Ai[0] + C[11]*Ai[1] + C[12]*Ai[3];
  B[1][1] = C[10]*Ai[1] + C[11]*Ai[2] + C[12]*Ai[4];
  B[1][2] = C[10]*Ai[3] + C[11]*Ai[4] + C[12]*Ai[5];

  B[2][0] = C[15]*Ai[0] + C[16]*Ai[1] + C[17]*Ai[3];
  B[2][1] = C[15]*Ai[1] + C[16]*Ai[2] + C[17]*Ai[4];
  B[2][2] = C[15]*Ai[3] + C[16]*Ai[4] + C[17]*Ai[5];

  B[3][0] = C[21]*Ai[0] + C[22]*Ai[1] + C[23]*Ai[3];
  B[3][1] = C[21]*Ai[1] + C[22]*Ai[2] + C[23]*Ai[4];
  B[3][2] = C[21]*Ai[3] + C[22]*Ai[4] + C[23]*Ai[5];

  B[4][0] = C[28]*Ai[0] + C[29]*Ai[1] + C[30]*Ai[3];
  B[4][1] = C[28]*Ai[1] + C[29]*Ai[2] + C[30]*Ai[4];
  B[4][2] = C[28]*Ai[3] + C[29]*Ai[4] + C[30]*Ai[5];

  fvec z[3] = { m[0]-r[0], m[1]-r[1], m[2]-r[2] };

  {
    fvec AV[6] = { C[0]-V[0], C[1]-V[1], C[2]-V[2], 
		     C[3]-V[3], C[4]-V[4], C[5]-V[5] };
    fvec AVi[6];
    AVi[0] = AV[4]*AV[4] - AV[2]*AV[5];
    AVi[1] = AV[1]*AV[5] - AV[3]*AV[4];
    AVi[2] = AV[3]*AV[3] - AV[0]*AV[5] ;
    AVi[3] = AV[2]*AV[3] - AV[1]*AV[4];
    AVi[4] = AV[0]*AV[4] - AV[1]*AV[3] ;
    AVi[5] = AV[1]*AV[1] - AV[0]*AV[2] ;

    det = 1./( AV[0]*AVi[0] + AV[1]*AVi[1] + AV[3]*AVi[3] );

    Particle->NDF  += 2.;
    Particle->Chi2 += 
      ( +(AVi[0]*z[0] + AVi[1]*z[1] + AVi[3]*z[2])*z[0]
	+(AVi[1]*z[0] + AVi[2]*z[1] + AVi[4]*z[2])*z[1]
	+(AVi[3]*z[0] + AVi[4]*z[1] + AVi[5]*z[2])*z[2] )*det;
  }
  
  r[0] = m[0];
  r[1] = m[1];
  r[2] = m[2];
  r[3]+= B[0][0]*z[0] + B[0][1]*z[1] + B[0][2]*z[2];
  r[4]+= B[1][0]*z[0] + B[1][1]*z[1] + B[1][2]*z[2];
  r[5]+= B[2][0]*z[0] + B[2][1]*z[1] + B[2][2]*z[2];
  r[6]+= B[3][0]*z[0] + B[3][1]*z[1] + B[3][2]*z[2];
  r[7]+= B[4][0]*z[0] + B[4][1]*z[1] + B[4][2]*z[2];
  
  fvec d0, d1, d2;

  C[0] = V[0];
  C[1] = V[1];
  C[2] = V[2];
  C[3] = V[3];
  C[4] = V[4];
  C[5] = V[5];

  d0= B[0][0]*V[0] + B[0][1]*V[1] + B[0][2]*V[3] - C[ 6];
  d1= B[0][0]*V[1] + B[0][1]*V[2] + B[0][2]*V[4] - C[ 7];
  d2= B[0][0]*V[3] + B[0][1]*V[4] + B[0][2]*V[5] - C[ 8];

  C[ 6]+= d0;
  C[ 7]+= d1;
  C[ 8]+= d2;
  C[ 9]+= d0*B[0][0] + d1*B[0][1] + d2*B[0][2];

  d0= B[1][0]*V[0] + B[1][1]*V[1] + B[1][2]*V[3] - C[10];
  d1= B[1][0]*V[1] + B[1][1]*V[2] + B[1][2]*V[4] - C[11];
  d2= B[1][0]*V[3] + B[1][1]*V[4] + B[1][2]*V[5] - C[12];

  C[10]+= d0;
  C[11]+= d1;
  C[12]+= d2;
  C[13]+= d0*B[0][0] + d1*B[0][1] + d2*B[0][2];
  C[14]+= d0*B[1][0] + d1*B[1][1] + d2*B[1][2];

  d0= B[2][0]*V[0] + B[2][1]*V[1] + B[2][2]*V[3] - C[15];
  d1= B[2][0]*V[1] + B[2][1]*V[2] + B[2][2]*V[4] - C[16];
  d2= B[2][0]*V[3] + B[2][1]*V[4] + B[2][2]*V[5] - C[17];

  C[15]+= d0;
  C[16]+= d1;
  C[17]+= d2;
  C[18]+= d0*B[0][0] + d1*B[0][1] + d2*B[0][2];
  C[19]+= d0*B[1][0] + d1*B[1][1] + d2*B[1][2];
  C[20]+= d0*B[2][0] + d1*B[2][1] + d2*B[2][2];

  d0= B[3][0]*V[0] + B[3][1]*V[1] + B[3][2]*V[3] - C[21];
  d1= B[3][0]*V[1] + B[3][1]*V[2] + B[3][2]*V[4] - C[22];
  d2= B[3][0]*V[3] + B[3][1]*V[4] + B[3][2]*V[5] - C[23];

  C[21]+= d0;
  C[22]+= d1;
  C[23]+= d2;
  C[24]+= d0*B[0][0] + d1*B[0][1] + d2*B[0][2];
  C[25]+= d0*B[1][0] + d1*B[1][1] + d2*B[1][2];
  C[26]+= d0*B[2][0] + d1*B[2][1] + d2*B[2][2];
  C[27]+= d0*B[3][0] + d1*B[3][1] + d2*B[3][2];

  d0= B[4][0]*V[0] + B[4][1]*V[1] + B[4][2]*V[3] - C[28];
  d1= B[4][0]*V[1] + B[4][1]*V[2] + B[4][2]*V[4] - C[29];
  d2= B[4][0]*V[3] + B[4][1]*V[4] + B[4][2]*V[5] - C[30];

  C[28]+= d0;
  C[29]+= d1;
  C[30]+= d2;
  C[31]+= d0*B[0][0] + d1*B[0][1] + d2*B[0][2];
  C[32]+= d0*B[1][0] + d1*B[1][1] + d2*B[1][2];
  C[33]+= d0*B[2][0] + d1*B[2][1] + d2*B[2][2];
  C[34]+= d0*B[3][0] + d1*B[3][1] + d2*B[3][2];
  C[35]+= d0*B[4][0] + d1*B[4][1] + d2*B[4][2];
  
  Extrapolate(Particle, r0, r[7]);
  Convert(Particle, r0, 0);
}


void CbmKFParticleInterface::Convert( CbmKFParticle_simd*  Particle, fvec r0[], bool ToProduction )
{
  fvec *r = Particle->GetParameters();
  fvec *C = Particle->GetCovMatrix();

    fvec B[3];
    {
      double B1[3], r01[8];
      for(int j = 0; j<fvecLen; j++)
      {
        for(int i = 0; i<8; i++)
          r01[i] = r0[i][j];
        FairField *MF = CbmKF::Instance()->GetMagneticField();
        MF->GetFieldValue( r01, B1 );
        const float c_light =  0.000299792458;
        for(int i = 0; i<3; i++) B[i][j] = B1[i]*c_light;
      }
    }

  fvec h[6];
  
  h[0] = r0[3];
  h[1] = r0[4];
  h[2] = r0[5];
  if( ToProduction ){ h[0]=-h[0]; h[1]=-h[1]; h[2]=-h[2]; } 
  h[3] = h[1]*B[2]-h[2]*B[1];
  h[4] = h[2]*B[0]-h[0]*B[2];
  h[5] = h[0]*B[1]-h[1]*B[0];
  
  fvec c;

  c = C[28]+h[0]*C[35];
  C[ 0]+= h[0]*(c+C[28]);
  C[28] = c;

  C[ 1]+= h[1]*C[28] + h[0]*C[29];
  c = C[29]+h[1]*C[35];
  C[ 2]+= h[1]*(c+C[29]);
  C[29] = c;

  C[ 3]+= h[2]*C[28] + h[0]*C[30];
  C[ 4]+= h[2]*C[29] + h[1]*C[30];
  c = C[30]+h[2]*C[35];
  C[ 5]+= h[2]*(c+C[30]);
  C[30] = c;

  C[ 6]+= h[3]*C[28] + h[0]*C[31];
  C[ 7]+= h[3]*C[29] + h[1]*C[31];
  C[ 8]+= h[3]*C[30] + h[2]*C[31];
  c = C[31]+h[3]*C[35];
  C[ 9]+= h[3]*(c+C[31]);
  C[31] = c;
  
  C[10]+= h[4]*C[28] + h[0]*C[32];
  C[11]+= h[4]*C[29] + h[1]*C[32];
  C[12]+= h[4]*C[30] + h[2]*C[32];
  C[13]+= h[4]*C[31] + h[3]*C[32];
  c = C[32]+h[4]*C[35];
  C[14]+= h[4]*(c+C[32]);
  C[32] = c;
  
  C[15]+= h[5]*C[28] + h[0]*C[33];
  C[16]+= h[5]*C[29] + h[1]*C[33];
  C[17]+= h[5]*C[30] + h[2]*C[33];
  C[18]+= h[5]*C[31] + h[3]*C[33];
  C[19]+= h[5]*C[32] + h[4]*C[33];
  c = C[33]+h[5]*C[35];
  C[20]+= h[5]*(c+C[33]);
  C[33] = c;

  C[21]+= h[0]*C[34];
  C[22]+= h[1]*C[34];
  C[23]+= h[2]*C[34];
  C[24]+= h[3]*C[34];
  C[25]+= h[4]*C[34];
  C[26]+= h[5]*C[34];
}

void CbmKFParticleInterface::ExtrapolateLine(CbmKFParticle_simd*  Particle, fvec r0[], fvec dS )
{
  fvec *r = Particle->GetParameters();
  fvec *C = Particle->GetCovMatrix();

  if( r0 && r0!=r ){
    r0[0]+= dS*r0[3];
    r0[1]+= dS*r0[4];
    r0[2]+= dS*r0[5];
  }

  r[0]+= dS*r[3];
  r[1]+= dS*r[4];
  r[2]+= dS*r[5];

  fvec C6  = C[ 6] + dS*C[ 9];
  fvec C11 = C[11] + dS*C[14];
  fvec C17 = C[17] + dS*C[20];
  fvec SC13 = dS*C[13];
  fvec SC18 = dS*C[18];
  fvec SC19 = dS*C[19];

  C[ 0] += dS*( C[ 6] + C6  );
  C[ 2] += dS*( C[11] + C11 );
  C[ 5] += dS*( C[17] + C17 );

  C[ 7] += SC13;
  C[ 8] += SC18;
  C[12] += SC19;

  C[ 1] += dS*( C[10] + C[ 7] );
  C[ 3] += dS*( C[15] + C[ 8] );
  C[ 4] += dS*( C[16] + C[12] );
 
  C[ 6]  = C6;
  C[10] += SC13;
  C[11]  = C11;
  C[15] += SC18;
  C[16] += SC19;
  C[17]  = C17;
  
  C[21] += dS*C[24];
  C[22] += dS*C[25];
  C[23] += dS*C[26];
  C[28] += dS*C[31];
  C[29] += dS*C[32];
  C[30] += dS*C[33];
}

void CbmKFParticleInterface::Extrapolate(CbmKFParticle_simd*  Particle, fvec r0[], fvec dS )
{
  if( Particle->Q[0]==0 ){
    ExtrapolateLine( Particle, r0, dS );
    return;
  }

  fvec *r = Particle->GetParameters();
  fvec *C = Particle->GetCovMatrix();

  const float c_light = 0.000299792458;
  FairField *MF = CbmKF::Instance()->GetMagneticField();

  fvec c = Particle->Q*c_light;

  // construct coefficients 

  fvec 
    px   = r[3],
    py   = r[4],
    pz   = r[5];
      
  fvec sx=0, sy=0, sz=0, syy=0, syz=0, syyy=0, Sx=0, Sy=0, Sz=0, Syy=0, Syz=0, Syyy=0;

  { // get field integrals

    fvec B[3][3];   
    fvec p0[3], p1[3], p2[3];

    // line track approximation

    p0[0] = r[0];
    p0[1] = r[1];
    p0[2] = r[2];
  
    p2[0] = r[0] + px*dS;
    p2[1] = r[1] + py*dS;
    p2[2] = r[2] + pz*dS;
  
    p1[0] = 0.5*(p0[0]+p2[0]);
    p1[1] = 0.5*(p0[1]+p2[1]);
    p1[2] = 0.5*(p0[2]+p2[2]);

    // first order track approximation
    double p01[3], p11[3], p21[3], B1[3][3];
    for(int gg=0; gg<fvecLen; gg++)
    {
      for(int i=0; i<3; i++)
      {
        p01[i] = p0[i][gg];
        p11[i] = p1[i][gg];
        p21[i] = p2[i][gg];
      }

      MF->GetFieldValue( p01, B1[0] );
      MF->GetFieldValue( p11, B1[1] );
      MF->GetFieldValue( p21, B1[2] );

      for(int i=0; i<3; i++)
        for(int j=0; j<3; j++)
          B[i][j][gg] = B1[i][j];
    }

      fvec Sy1 = ( 7*B[0][1] + 6*B[1][1]-B[2][1] )*c*dS*dS/96.;
      fvec Sy2 = (   B[0][1] + 2*B[1][1]         )*c*dS*dS/6.;

      p1[0] -= Sy1*pz;
      p1[2] += Sy1*px;
      p2[0] -= Sy2*pz;
      p2[2] += Sy2*px;

    for(int gg=0; gg<fvecLen; gg++)
    {
      for(int i=0; i<3; i++)
      {
        p01[i] = p0[i][gg];
        p11[i] = p1[i][gg];
        p21[i] = p2[i][gg];
      }

      MF->GetFieldValue( p01, B1[0] );
      MF->GetFieldValue( p11, B1[1] );
      MF->GetFieldValue( p21, B1[2] );

      for(int i=0; i<3; i++)
        for(int j=0; j<3; j++)
          B[i][j][gg] = B1[i][j];
    }
    
    sx = c*( B[0][0] + 4*B[1][0] + B[2][0] )*dS/6.;
    sy = c*( B[0][1] + 4*B[1][1] + B[2][1] )*dS/6.;
    sz = c*( B[0][2] + 4*B[1][2] + B[2][2] )*dS/6.;

    Sx = c*( B[0][0] + 2*B[1][0])*dS*dS/6.;
    Sy = c*( B[0][1] + 2*B[1][1])*dS*dS/6.;
    Sz = c*( B[0][2] + 2*B[1][2])*dS*dS/6.;

    fvec c2[3][3]    =   { {  5, -4, -1},{  44,  80,  -4},{ 11, 44, 5} }; // /=360.    
    fvec C2[3][3]    =   { { 38,  8, -4},{ 148, 208, -20},{  3, 36, 3} }; // /=2520.
    for(Int_t n=0; n<3; n++)
      for(Int_t m=0; m<3; m++) 
	{
	  syz += c2[n][m]*B[n][1]*B[m][2];
	  Syz += C2[n][m]*B[n][1]*B[m][2];
	}
 
    syz  *= c*c*dS*dS/360.;
    Syz  *= c*c*dS*dS*dS/2520.;
    
    syy  = c*( B[0][1] + 4*B[1][1] + B[2][1] )*dS;
    syyy = syy*syy*syy / 1296;
    syy  = syy*syy/72;

    Syy = ( B[0][1]*( 38*B[0][1] + 156*B[1][1]  -   B[2][1] )+
	    B[1][1]*(              208*B[1][1]  +16*B[2][1] )+
	    B[2][1]*(                             3*B[2][1] )  
	    )*dS*dS*dS*c*c/2520.;
    Syyy = 
      (
       B[0][1]*( B[0][1]*( 85*B[0][1] + 526*B[1][1]  - 7*B[2][1] )+
		 B[1][1]*(             1376*B[1][1]  +84*B[2][1] )+
		 B[2][1]*(                            19*B[2][1] )  )+
       B[1][1]*( B[1][1]*(             1376*B[1][1] +256*B[2][1] )+
		 B[2][1]*(                            62*B[2][1] )  )+
       B[2][1]*B[2][1]  *(                             3*B[2][1] )       
       )*dS*dS*dS*dS*c*c*c/90720.;    
 
  }

  fvec J[8][8];
  for( int i=0; i<8; i++ ) for( int j=0; j<8; j++) J[i][j]=0;

  J[0][0]=1; J[0][1]=0; J[0][2]=0; J[0][3]=dS-Syy;  J[0][4]=Sx;  J[0][5]=Syyy-Sy;
  J[1][0]=0; J[1][1]=1; J[1][2]=0; J[1][3]=-Sz;     J[1][4]=dS;  J[1][5]=Sx+Syz;
  J[2][0]=0; J[2][1]=0; J[2][2]=1; J[2][3]=Sy-Syyy; J[2][4]=-Sx; J[2][5]=dS-Syy;
  
  J[3][0]=0; J[3][1]=0; J[3][2]=0; J[3][3]=1-syy;   J[3][4]=sx;  J[3][5]=syyy-sy;
  J[4][0]=0; J[4][1]=0; J[4][2]=0; J[4][3]=-sz;     J[4][4]=1;   J[4][5]=sx+syz;
  J[5][0]=0; J[5][1]=0; J[5][2]=0; J[5][3]=sy-syyy; J[5][4]=-sx; J[5][5]=1-syy;
  J[6][6] = J[7][7] = 1;
  
  r[0]+= J[0][3]*px + J[0][4]*py + J[0][5]*pz;
  r[1]+= J[1][3]*px + J[1][4]*py + J[1][5]*pz;
  r[2]+= J[2][3]*px + J[2][4]*py + J[2][5]*pz;
  r[3] = J[3][3]*px + J[3][4]*py + J[3][5]*pz;
  r[4] = J[4][3]*px + J[4][4]*py + J[4][5]*pz;
  r[5] = J[5][3]*px + J[5][4]*py + J[5][5]*pz;

  if( r0 && r0!=r ){
    px   = r0[3];
    py   = r0[4];
    pz   = r0[5];
    
    r0[0]+= J[0][3]*px + J[0][4]*py + J[0][5]*pz;
    r0[1]+= J[1][3]*px + J[1][4]*py + J[1][5]*pz;
    r0[2]+= J[2][3]*px + J[2][4]*py + J[2][5]*pz;
    r0[3] = J[3][3]*px + J[3][4]*py + J[3][5]*pz;
    r0[4] = J[4][3]*px + J[4][4]*py + J[4][5]*pz;
    r0[5] = J[5][3]*px + J[5][4]*py + J[5][5]*pz;
  }

  multQSQt( 8, J[0], C, C);

}

void CbmKFParticleInterface::multQSQt( int N, const fvec Q[], const fvec S[], fvec S_out[] )
{
 fvec A[N*N];

 {for( Int_t i=0, n=0; i<N; i++ )
   {
     for( Int_t j=0; j<N; j++, ++n )
      {
        A[n] = 0 ;
        for( Int_t k=0; k<N; ++k ) A[n]+= S[IJ(i,k)] * Q[ j*N+k];
      }
   }
 }
    
 {for( Int_t i=0; i<N; i++ )
   {
     for( Int_t j=0; j<=i; j++ )
      {
        Int_t n = IJ(i,j);
        S_out[n] = 0 ;
        for( Int_t k=0; k<N; k++ )  S_out[n] += Q[ i*N+k ] * A[ k*N+j ];
      }
   }
 }
}

void CbmKFParticleInterface::Extrapolate( fvec r0[], fvec dS )
{
  Extrapolate(KFPart, r0, dS);
}

void CbmKFParticleInterface::TransportToProductionVertex()
{
  if( KFPart->AtProductionVertex ) return;
  fvec r0[8];
  for( int i=0; i<8; i++ ) r0[i] = KFPart->r[i];
  Extrapolate(r0, -(KFPart->r[7]));
  Convert(KFPart, r0, 1);    
  KFPart->AtProductionVertex = 1;
}

void CbmKFParticleInterface::TransportToDecayVertex()
{
  if( !KFPart->AtProductionVertex ) return;
  fvec r0[8];
  for( int i=0; i<8; i++ ) r0[i] = KFPart->r[i];
  Extrapolate(r0, KFPart->r[7]);
  Convert(KFPart, r0, 0);
  KFPart->AtProductionVertex = 0;
}

fvec CbmKFParticleInterface::GetDStoPoint( CbmKFParticle_simd*  Particle, const fvec xyz[] ) const
{
  fvec *r = Particle->GetParameters();

  fvec dx = xyz[0] - r[0];
  fvec dy = xyz[1] - r[1];
  fvec dz = xyz[2] - r[2];
  fvec p2 = r[3]*r[3] + r[4]*r[4]+r[5]*r[5];

  //if (Q == 0 && r[2] < xyz[2]) return sqrt((dx*dx+dy*dy+dz*dz)/p2);
  //if (Q == 0 && r[2] >= xyz[2]) return -sqrt((dx*dx+dy*dy+dz*dz)/p2);

  fvec B[3];
  {
    double B1[3], r01[8];
    for(int j = 0; j<fvecLen; j++)
    {
      for(int i = 0; i<8; i++)
        r01[i] = r[i][j];
      FairField *MF = CbmKF::Instance()->GetMagneticField();
      MF->GetFieldValue( r01, B1 );
      const float c_light =  0.000299792458;
      for(int i = 0; i<3; i++) B[i][j] = B1[i]*c_light;
    }
  }
  fvec bq1 = B[0]*Particle->Q;
  fvec bq2 = B[1]*Particle->Q;
  fvec bq3 = B[2]*Particle->Q;
  fvec a = dx*r[3]+dy*r[4]+dz*r[5];
  fvec B2 = B[0]*B[0]+B[1]*B[1]+B[2]*B[2];
  fvec bq = Particle->Q*sqrt(B2);
  fvec pB = r[3]*B[0]+r[4]*B[1]+r[5]*B[2];
  fvec rB = dx*B[0]+dy*B[1]+dz*B[2];
  for(int i=0; i<fvecLen; i++) B2[i] = (fabs(B2[i])<0.0001) ? 0.0001 : B2[i];
  fvec pt2 = p2 - pB*pB/B2;
  a = a - pB*rB/B2;

  fvec dS;

  for(int j=0; j<fvecLen; j++)
  {
    if( pt2[j] < 1.e-4 ) dS[j]=0.;
    if( TMath::Abs(bq[j]) < 1.e-8 ) dS[j] = a[j]/pt2[j];
    else dS[j] =  TMath::ATan2( bq[j]*a[j], pt2[j] + bq1[j]*(dz[j]*r[4][j]-dy[j]*r[5][j]) - bq2[j]*(dz[j]*r[3][j]-dx[j]*r[5][j])+bq3[j]*(dy[j]*r[3][j]-dx[j]*r[4][j]))/bq[j];
  }
  //fvec dSm = rB/pB;

//  cout <<"normalnyj S  " <<  dS << endl;
//  cout <<"moj       S  " << dSm << endl;
//  cout <<"S            " <<  sqrt(dx*dx+dy*dy+dz*dz) << endl;

  return dS;
  //return sqrt(dx*dx+dy*dy+dz*dz);
}

fvec CbmKFParticleInterface::GetDStoPoint( const fvec xyz[] ) const
{
  GetDStoPoint( KFPart, xyz );
}

void CbmKFParticleInterface::GetKFVertex( CbmKFVertex vtx[]  )
{
  float r0[fvecLen], r1[fvecLen], r2[fvecLen];
  float C_temp[6][fvecLen];
  float Chi2_temp[fvecLen];
  int NDF_temp[fvecLen];

  for(int j=0; j<fvecLen; j++)
  {
    r0[j] = KFPart->r[0][j];
    r1[j] = KFPart->r[1][j];
    r2[j] = KFPart->r[2][j];
    for( int i=0; i<6; i++) C_temp[i][j] = KFPart->C[i][j];

    Chi2_temp[j] = KFPart->Chi2[j];
    NDF_temp[j] = (int) KFPart->NDF[j];
  }

  for(int j=0; j<fvecLen; j++)
    vtx[j] = GetKFVertexJ(j);
}

CbmKFVertex CbmKFParticleInterface::GetKFVertexJ(int j)
{
  CbmKFVertex vtx;
  vtx.GetRefX() = KFPart->r[0][j];
  vtx.GetRefY() = KFPart->r[1][j];
  vtx.GetRefZ() = KFPart->r[2][j];
  for( int i=0; i<6; i++) vtx.GetCovMatrix()[i] = KFPart->C[i][j];
  vtx.GetRefChi2() = KFPart->Chi2[j];
  vtx.GetRefNDF()  = (int) KFPart->NDF[j];

  return vtx;
}

void CbmKFParticleInterface::GetKFVertexJ(int j, CbmKFVertex *vtx)
{
  vtx->GetRefX() = KFPart->r[0][j];
  vtx->GetRefY() = KFPart->r[1][j];
  vtx->GetRefZ() = KFPart->r[2][j];
  for( int i=0; i<6; i++) vtx->GetCovMatrix()[i] = KFPart->C[i][j];
  vtx->GetRefChi2() = KFPart->Chi2[j];
  vtx->GetRefNDF()  = (int) KFPart->NDF[j];
}

void CbmKFParticleInterface::GetKFParticle(CbmKFParticle &Part, int iPart)
{
  for(int iP=0; iP<8; iP++)
    Part.GetParameters()[iP] = KFPart->GetParameters()[iP][iPart];
  for(int iC=0; iC<36; iC++)
    Part.GetCovMatrix()[iC] = KFPart->GetCovMatrix()[iC][iPart];

  Part.NDF = static_cast<int>(KFPart->GetNDF()[iPart]);
  Part.Chi2 = KFPart->GetChi2()[iPart];
  Part.Q    = KFPart->GetQ()[iPart];
  Part.AtProductionVertex = KFPart->AtProductionVertex;
}

void CbmKFParticleInterface::SetVtxGuess( fvec &x, fvec &y, fvec &z )
{
  KFPart->SetVtxGuess(x,y,z);
}

void CbmKFParticleInterface::SetVtxErrGuess( fvec &dx, fvec &dy, fvec &dz )
{
  KFPart -> SetVtxErrGuess(dx,dy,dz);
}

void CbmKFParticleInterface::GetMomentum( fvec &P, fvec &Error )
{
  KFPart->GetMomentum( P, Error );
}

void CbmKFParticleInterface::GetMass( fvec &M, fvec &Error )
{
  KFPart->GetMass ( M, Error );
}

#undef cnst
