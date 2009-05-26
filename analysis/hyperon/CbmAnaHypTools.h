#ifndef CBMANAHYPTOOLS_H
#define CBMANAHYPTOOLS_H

#include "TObject.h"
#include "CbmMCTrack.h"
#include <map>
#include <vector>
class CbmHit;
class CbmStsTrack;
class TClonesArray;
class CbmStsTrack;
class CbmKFTrack;
class CbmVertex;
class TVector3;
class TTree;
class CbmTrackMatch;

using std::vector;
using std::map;

class CbmAnaHypTools
{
public:
  CbmAnaHypTools(){};
  ~CbmAnaHypTools(){};
  void Init();
  Int_t GetTrackId(CbmTrackMatch* match);
  Int_t GetTrackId(CbmHit* hit);
  void CopyTrack(CbmStsTrack* trackIn, CbmStsTrack* trackOut);
  void SetFlags();
  Double_t CalculateDca(CbmStsTrack* tr1, CbmStsTrack* tr2, CbmVertex vtx);
  Double_t CalculateCascadeDca (CbmStsTrack* la, CbmStsTrack* da, CbmVertex vtx);
  Int_t SignalPair(CbmStsTrack* tr1, CbmStsTrack* tr2);
  Int_t SignalCascade(CbmStsTrack* la, CbmStsTrack* da,
                Int_t motherPdg, Int_t sisterPdg);
  Double_t GetB(CbmStsTrack track);
  Int_t GetPidHypo(CbmStsTrack track);
  void SetFlagsOnly();
  Int_t IsL1accepted(Int_t trackId);

private:
  TClonesArray* fListMCTracks;
  TClonesArray* fStsPointArray;
  TClonesArray* fListRCTracks;
  TClonesArray* fmArray;
  TClonesArray* fsArray;
  TClonesArray* fTrackMatch;
  CbmVertex* fPV;
  TVector3 pv;
  TTree* fTree;

};

class Lambda{
 public:
  Lambda():laMC(0),piMC(0),prMC(0),piRC(0),prRC(0),piKF(0),prKF(0){}
  ~Lambda(){}
  CbmMCTrack* laMC;
  CbmMCTrack* piMC;
  CbmMCTrack* prMC;
  CbmStsTrack* piRC;
  CbmStsTrack* prRC;
  CbmKFTrack* piKF;
  CbmKFTrack* prKF;
};

struct HitVector{
    vector<int> vMvd;
    vector<int> vSts;
    vector<int> vStationID;
};

class Da{
 public:
  Da():pdg(0),idMC(-1),idRC(-1),mc(0),rc(0),moId(-1),nPts(0),nHits(0),isL1AC(0){}
  Da(Int_t IDMC, CbmMCTrack* MC):idMC(IDMC),idRC(0),mc(MC),rc(0),nHits(0),isL1AC(0){
    moId=MC->GetMotherId();
    pdg=MC->GetPdgCode();
    nPts=MC->GetNPoints(kMUCH);
  }
  ~Da(){}
  Int_t pdg;
  Int_t idMC;
  Int_t idRC;
  CbmMCTrack* mc;
  CbmStsTrack* rc;
  Int_t moId;
  Int_t nPts;
  Int_t nHits;
  HitVector hits;
  Bool_t isL1AC;
};


#endif
