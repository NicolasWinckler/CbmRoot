#ifndef CbmKFStsHit_H
#define CbmKFStsHit_H 1 

#include "CbmKFHit.h"
#include "CbmKFPixelMeasurement.h"
#include "CbmKFMaterial.h"

class CbmStsHit;
class CbmMvdHit;

class CbmKFStsHit : public CbmKFHit { 

 public:

  CbmKFStsHit(){}
  ~CbmKFStsHit(){}

  CbmKFPixelMeasurement FitPoint;
  CbmKFTube *tube;

  void Create( CbmStsHit *h );
  void Create( CbmMvdHit *h );

  Int_t Filter( CbmKFTrackInterface &track, Bool_t downstream, Double_t &QP0 );

  static void FilterPDAF( CbmKFTrackInterface &track, 
			  std::vector<CbmKFStsHit*> &vpHits,
			  Bool_t downstream, Double_t *QP0,
			  double gateX, double gateY, int &best_hit_idx );

  ClassDef(CbmKFStsHit, 1);
};

#endif
