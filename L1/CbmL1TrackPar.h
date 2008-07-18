
#ifndef CbmL1TrackPar_H
#define CbmL1TrackPar_H

#include "CbmKFTrackInterface.h"

struct CbmL1TrackPar : public CbmKFTrackInterface{
 public:
  double *GetTrack(){ return T; }
  double *GetCovMatrix(){ return C; }
  double &GetRefChi2(){ return chi2; }
  int    &GetRefNDF(){ return NDF; }
  double GetMass(){ return mass; }
  bool   IsElectron(){ return is_electron; }

  double T[6], C[15], chi2 ;
  int NDF ;
  double mass; // mass hypothesis
  bool is_electron;
};

#endif
