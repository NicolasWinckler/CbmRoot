#ifndef CbmKFParticleInterface_h
#define CbmKFParticleInterface_h

#include "CbmKFParticle.h"
#include "CbmKFParticle_simd.h"
#include "CbmKFTrackInterface.h"

#include "CbmKFVertexInterface.h"
#include "CbmKFVertex.h"

class L1FieldRegion;
class CbmL1Track;
class CbmStsTrack;

class CbmKFParticleInterface
{
 public:

  CbmKFParticleInterface();
  CbmKFParticleInterface(CbmKFTrackInterface* Track[]);
  ~CbmKFParticleInterface();

  void SetField(const L1FieldRegion &field, bool isOneEntry=0, const int iVec=0) { KFPart->SetField(field, isOneEntry, iVec);}

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

  // functions for particle finding

  /// Find particles with 2-body decay channel from input tracks vRTracks with primary vertex PrimVtx:
  /// 1. K0s->pi+ pi-
  /// 2. Lambda->p pi-
  /// All particles are put into the Particles array. 3 cuts for each particle are required.
  /// First index in the cuts array sets a particle number (see table above), second index - a cut number:
  /// cut[0][0] - chi to a primary vertex of a track (sqare root from a normalized on a total error of 
  ///             the track and the vertex distance between the track and the primary vertex), only
  ///             element cut[0][0] is used to select tracks, all other elements cut[*][0] are not used;
  /// cut[*][1] - chi2/ndf of the reconstructed particle;
  /// cut[*][2] - z coordinate of the reconstructed particle.
  template<class T> 
    static void Find2PDecayT(vector<T>& vRTracks, vector<CbmKFParticle>& Particles, CbmKFVertex& PrimVtx,
                             const float cuts[2][3]);

  static void Find2PDecay(vector<CbmL1Track>& vRTracks, vector<CbmKFParticle>& Particles, CbmKFVertex& PrimVtx,
                          const float cuts[2][3] = DefaultCuts);
  static void Find2PDecay(vector<CbmStsTrack>& vRTracks, vector<CbmKFParticle>& Particles, CbmKFVertex& PrimVtx,
                          const float cuts[2][3] = DefaultCuts);

  template<class T> 
    void ConstructPVT(vector<T>& vRTracks);

  void ConstructPV(vector<CbmL1Track>& vRTracks);
  void ConstructPV(vector<CbmStsTrack>& vRTracks);

 protected:

  fvec& Cij( Int_t i, Int_t j ){ 
    return KFPart->C[( j<=i ) ? i*(i+1)/2+j :j*(j+1)/2+i];
  }
 public:
  static Int_t IJ( Int_t i, Int_t j ){ 
    return ( j<=i ) ? i*(i+1)/2+j :j*(j+1)/2+i;
  }

 public:
  CbmKFParticle_simd *KFPart;

 private:

  static const float DefaultCuts[2][3];

  CbmKFParticleInterface(const CbmKFParticleInterface&);
  void operator=(const CbmKFParticleInterface&);
};

#endif /* !CbmKFParticleInterface_h */

