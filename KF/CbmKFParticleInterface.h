#ifndef CbmKFParticleInterface_h
#define CbmKFParticleInterface_h

#include "CbmKFParticle.h"
#include "CbmKFParticle_simd.h"
#include "CbmKFTrackInterface.h"

#include "CbmKF.h"
#include "CbmKFVertexInterface.h"
#include "CbmKFVertex.h"
#include "TMath.h"

class CbmKFParticleInterface
{
 public:

  CbmKFParticleInterface();
  CbmKFParticleInterface(CbmKFTrackInterface* Track[]);

  ~CbmKFParticleInterface();

  void Construct(CbmKFTrackInterface* vDaughters[][fvecLen], int NDaughters, CbmKFVertexInterface *Parent[] = 0, float Mass=-1, float CutChi2=-1);
  void Construct(CbmKFParticle_simd  vDaughters[],          int NDaughters, CbmKFVertexInterface *Parent[] = 0, float Mass=-1, float CutChi2=-1);
  
  void TransportToProductionVertex();
  void TransportToDecayVertex();

  void Extrapolate( CbmKFParticle_simd*  Particle, fvec r0[], fvec T );
  void Extrapolate( fvec r0[], fvec T );
  void ExtrapolateLine( CbmKFParticle_simd*  Particle, fvec r0[], fvec T );

  fvec GetDStoPoint( CbmKFParticle_simd*  Particle, const fvec xyz[] ) const;
  fvec GetDStoPoint( const fvec xyz[] ) const;

  void SetVtxGuess(fvec &x, fvec &y, fvec &z);
  void SetVtxErrGuess(fvec &d_x, fvec &d_y, fvec &d_z);

  //* Simple accessors 
  fvec GetX    ()  const { return KFPart->r[0]; }
  fvec GetY    ()  const { return KFPart->r[1]; }
  fvec GetZ    ()  const { return KFPart->r[2]; }
  fvec GetPx   ()  const { return KFPart->r[3]; }
  fvec GetPy   ()  const { return KFPart->r[4]; }
  fvec GetPz   ()  const { return KFPart->r[5]; }
  fvec GetE    ()  const { return KFPart->r[6]; }
  fvec GetS    ()  const { return KFPart->r[7]; }
  fvec GetQ    ()  const { return KFPart->GetQ();    }
  fvec GetChi2 ()  const { return KFPart->GetChi2(); }
  fvec GetNDF  ()  const { return KFPart->GetNDF();  }
  fvec *GetParameters()  { return KFPart->GetParameters(); }
  fvec *GetCovMatrix()   { return KFPart->GetCovMatrix(); }
  
  fvec GetParameter ( Int_t i )          const { return KFPart->r[i]; }
  fvec GetCovariance( Int_t i )          const { return KFPart->C[i]; }
  fvec GetCovariance( Int_t i, Int_t j ) const { return KFPart->C[IJ(i,j)]; }

  //*
  //*  MODIFIERS
  //*
  
  fvec & rX    () { return KFPart->rX(); }
  fvec & rY    () { return KFPart->rY(); }
  fvec & rZ    () { return KFPart->rZ(); }
  fvec & rPx   () { return KFPart->rPx();}
  fvec & rPy   () { return KFPart->rPy();}
  fvec & rPz   () { return KFPart->rPz();}
  fvec & rE    () { return KFPart->rE(); }
  fvec & rS    () { return KFPart->rS(); }
  fvec & rQ    () { return KFPart->rQ(); }
  fvec & rChi2 () { return KFPart->rChi2();}
  fvec & rNDF  () { return KFPart->rNDF();}

  fvec & rParameter ( Int_t i )        { return KFPart->r[i];}
  fvec & rCovariance( Int_t i )        { return KFPart->C[i];}
  fvec & rCovariance( Int_t i, Int_t j ) { return KFPart->C[IJ(i,j)];}

  void GetMomentum( fvec &P, fvec &Error );
  void GetMass( fvec &M, fvec &Error );

  fvec GetTx    ()  const { return KFPart->GetTx(); }
  fvec GetTy    ()  const { return KFPart->GetTy(); }

  void MeasureMass(CbmKFParticle_simd*  Particle, fvec r0[], fvec Mass );
  void MeasureProductionVertex(CbmKFParticle_simd*  Particle, fvec r0[], CbmKFVertexInterface **Parent);
  void Convert(CbmKFParticle_simd*  Particle, fvec r0[], bool ToProduction );
  void multQSQt( int N, const fvec Q[], const fvec S[], fvec S_out[] );

  void GetKFVertex( CbmKFVertex *vtx);
  void GetKFParticle( CbmKFParticle &Part, int iPart = 0);
  CbmKFVertex GetKFVertexJ(int j);

void GetKFVertexJ(int j, CbmKFVertex *vtx);


  void SetPDG ( fvec pdg ) { KFPart->SetPDG( pdg ); }
  const fvec& GetPDG () const { return KFPart->GetPDG(); }
  
 protected:

  fvec& Cij( Int_t i, Int_t j ){ 
    return KFPart->C[( j<=i ) ? i*(i+1)/2+j :j*(j+1)/2+i];
  }
  static Int_t IJ( Int_t i, Int_t j ){ 
    return ( j<=i ) ? i*(i+1)/2+j :j*(j+1)/2+i;
  }

 public:
  CbmKFParticle_simd *KFPart;
};

#endif /* !CbmKFParticleInterface_h */

