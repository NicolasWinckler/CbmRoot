/** 
 *
 * @author S.Gorbunov
 * @version 0.0
 * @since 02.12.05
 * 
 */

#ifndef CBMKFTRACK_H
#define CBMKFTRACK_H

#include "CbmKFTrackInterface.h"
#include "CbmKFHit.h"

#include <vector>

class CbmKFHit;
class CbmTrackParam;
class CbmStsTrack;

class CbmKFTrack : public CbmKFTrackInterface{

 private:

  Double_t fT[6], fC[15], fMass, fChi2;
  Bool_t   fIsElectron;
  Int_t fNDF;

 public:  

  std::vector<CbmKFHit*> fHits;

  CbmKFTrack();
  ~CbmKFTrack(){ }

  CbmKFTrack( CbmKFTrackInterface &track ){ SetTrack( track ); }

  CbmKFTrack( CbmTrackParam &track ){    SetTrackParam( track ); }

  CbmKFTrack( CbmStsTrack &track, bool first=1 ){ SetStsTrack( track, first ); }

  void SetTrack( CbmKFTrackInterface &track );
  void SetTrackParam( CbmTrackParam &track );
  void SetStsTrack( CbmStsTrack &track, bool first=1 );

  void GetTrackParam( CbmTrackParam &track );
  void GetStsTrack( CbmStsTrack &track, bool first=1 );

  void SetPID( Int_t pidHypo );

  Double_t *GetTrack()       { return fT; }
  Double_t *GetCovMatrix()   { return fC; }
  Double_t &GetRefChi2()     { return fChi2; }
  Int_t    &GetRefNDF()      { return fNDF; }
  Double_t GetMass()         { return fMass; }
  Bool_t   IsElectron()      { return fIsElectron; }
  Int_t    GetNOfHits()      { return fHits.size(); }
  CbmKFHit *GetHit( Int_t i ){ return fHits[i]; }

  ClassDef(CbmKFTrack, 1)
};

#endif
