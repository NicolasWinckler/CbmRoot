/** The CbmKFParticle class
 *
 * @author  S.Gorbunov, I.Kisel
 * @version 1.0
 * @since   24.11.06
 * 
 * Class to fit decayed particle with the Kalman Filter method
 *
 */
#ifndef CBMKFPARTICLE_H
#define CBMKFPARTICLE_H

#include "CbmKFTrackInterface.h"
#include "CbmKFVertexInterface.h"

#include <vector>
using namespace std;

class CbmKFParticle:public TObject {
  
 public:

  Double_t r[8], C[36];
  Int_t NDF;
  Double_t Chi2, Q;
  Bool_t AtProductionVertex;

  CbmKFParticle(){};
  CbmKFParticle( CbmKFTrackInterface* Track, Double_t *z0=0 );

  ~CbmKFParticle(){};

  // Construction

  void Construct( vector<CbmKFTrackInterface*> &vDaughters, 
		  CbmKFVertexInterface *Parent=0, 
		  Double_t Mass=-1, Double_t CutChi2=-1  );

  // Transportation

  void TransportToProductionVertex();  
  void TransportToDecayVertex();

  // Accessors ?
  
  void GetKFTrack( CbmKFTrackInterface* Track);
  void GetKFVertex( CbmKFVertexInterface  &vtx );
  void GetMomentum( Double_t &P, Double_t &Error );
  void GetMass( Double_t &M, Double_t &Error );
  void GetDecayLength( Double_t &L, Double_t &Error );
  void GetLifeTime( Double_t &T, Double_t &Error );

 protected:

  Double_t& Cij( Int_t i, Int_t j ){ 
    return C[( j<=i ) ? i*(i+1)/2+j :j*(j+1)/2+i];
  }

  void MeasureMass( Double_t r0[], Double_t Mass );
  void MeasureProductionVertex( Double_t r0[], CbmKFVertexInterface *Parent);
  void Convert( Double_t r0[], bool ToProduction );
  void Extrapolate( Double_t r0[], double T );
  void ExtrapolateLine( Double_t r0[], double T );
  
  ClassDef( CbmKFParticle, 1 );
};
#endif /* !CBMKFPARTICLE_H */
