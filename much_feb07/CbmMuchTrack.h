#ifndef CbmMUCHTRACK_H
#define CbmMUCHTRACK_H 1


#include <map>
#include "TObject.h"
#include "TArrayI.h"
#include "TLorentzVector.h"
#include "FairTrackParam.h"

class CbmMuchTrack : public TObject
{

 public:

  /** Default constructor **/
  CbmMuchTrack();


  /** Destructor **/
  virtual ~CbmMuchTrack();

  void Print();

  /** Adds the hit index to the index array **/

  void AddHitIndex(Int_t hitID){ fHits[fNHits++] = hitID; }

  /** Accessors  **/

  Int_t GetStsTrackID() const { return fStsTrackID;}

  Int_t GetNHits() const { return  fNHits;}
  Int_t GetNMissedHits() const { return  fNMissedHits;}
  Int_t GetNMissedStations() const { return  fNMissedStations;}
  Int_t GetHitIndex(Int_t iHit)   const { return fHits[iHit]; }
  Double_t GetChi2()   const { return fChi2; } // much chi^2
  Int_t GetNDF() const { return fNDF; }// much NDF
  FairTrackParam *GetMuchTrack(){ return &fMuchTrack; }
  Int_t GetFlag() const {return fFlag;};


  void SetNMissedHits( Int_t n ){ fNMissedHits = n; }
  void SetNMissedStations( Int_t n ){  fNMissedStations = n; }
  void SetChi2( Double_t chi2 ) { fChi2  = chi2; }
  void SetNDF( Int_t ndf ){ fNDF = ndf; }
  void SetStsTrackID(Int_t it) { fStsTrackID = it; }
  void SetMuchTrack( FairTrackParam *track ){ fMuchTrack = *track; }
  void SetFlag(Int_t flag) { fFlag = flag;};

 private:

  Int_t fStsTrackID;
  Int_t fNHits;
  Int_t fNMissedHits;
  Int_t fNMissedStations;
  Int_t fHits[30];
  FairTrackParam fMuchTrack;
  Double_t fChi2;
  Int_t fNDF;
  Int_t fFlag;

  ClassDef(CbmMuchTrack,2);

};

#endif
