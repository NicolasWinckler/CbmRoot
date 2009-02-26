// -------------------------------------------------------------------------
// -----                      CbmStsTrack source file                  -----
// -----                  Created 11/07/05  by M. Kalisky              -----
// -------------------------------------------------------------------------
#include "CbmTrdTrack.h"

#include "CbmTrdHit.h"

#include <iostream>
#include <map>
using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
CbmTrdTrack::CbmTrdTrack() {
  fPidHypo = 0;
  fFlag    = 0;
  fChi2    = 0.;
  fNDF     = 0;
  fELoss   = 0.;
  fPidWkn   = -1.;
  fPidLikeEL   = -1.;
  fPidLikePI   = -1.;
  fPidLikeKA   = -1.;
  fPidLikePR   = -1.;
  fPidLikeMU   = -1.;
  fPidANN      = -1.;
  fStsTrackIndex = -1;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmTrdTrack::~CbmTrdTrack() {
    fTrdHitMap.clear();
}
// -------------------------------------------------------------------------

// -----   Public method AddMapsHit   --------------------------------------
void CbmTrdTrack::AddHit(Int_t hitID, CbmTrdHit* hit) {
  fTrdHitMap[hit->GetZ()] = hitID;
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void CbmTrdTrack::Print() {
    cout << " Number of attached TRD hits : "<< fTrdHits.GetSize()  << endl;
    cout << " Parameters at first point: " << endl;
    //fParamFirst.Print();
    //cout << " Chi2: " << fChi2 << ", Quality flag " << fFlag << endl;
}
// -------------------------------------------------------------------------

// -----   Public method SortHits   ----------------------------------------
void CbmTrdTrack::SortHits() {

  Int_t index=0;
  map<Double_t, Int_t>::iterator it;

  fTrdHits.Reset();
  fTrdHits.Set(fTrdHitMap.size());
  index = 0;
  for (it = fTrdHitMap.begin(); it != fTrdHitMap.end(); it++) {
    fTrdHits[index] = it->second;
    index++;
  }

}
// -------------------------------------------------------------------------

ClassImp(CbmTrdTrack)


