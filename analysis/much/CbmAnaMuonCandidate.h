#ifndef CBMANAMUONCANDIDATE_H
#define CBMANAMUONCANDIDATE_H

#include "CbmKFTrack.h"
#include "TLorentzVector.h"
#define NPLANES 20

class CbmAnaMuonCandidate : public TObject{
  public:
    CbmAnaMuonCandidate() {};
    CbmAnaMuonCandidate(Int_t iTrack, TLorentzVector pMC);
    virtual ~CbmAnaMuonCandidate(){};
    Int_t* GetMuchPoints() { return muchPoints; };
    Int_t* GetMuchHits()   { return muchHits;   };
    void SetMuchPoint(Int_t planeId, Int_t index) { muchPoints[planeId]=index; }
    void SetMuchHit  (Int_t planeId, Int_t index) { muchHits[planeId]=index;   }
  private:
    CbmKFTrack track;
    Int_t fMCTrackId;
    TLorentzVector fpMC;
    Int_t muchPoints[NPLANES];
    Int_t muchHits[NPLANES];
  ClassDef(CbmAnaMuonCandidate,0);
};

#endif
