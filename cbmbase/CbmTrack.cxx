// -------------------------------------------------------------------------
// -----                        CbmTrack source file                   -----
// -----                  Created 29/11/07  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmTrack.h"

#include "CbmTrkHit.h"

#include <iostream>
#include <map>

using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
CbmTrack::CbmTrack() {
  fPidHypo = 0;
  fFlag    = 0;
  fNDF     = 0;
  fChi2    = 0.;
  fPreviousDetId   = -1;
  fPreviousTrackId = -1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrack::~CbmTrack() {
  fHitMap.clear();
}
// -------------------------------------------------------------------------



// -----   Public method AddHit   ------------------------------------------
void CbmTrack::AddHit(Int_t hitIndex, CbmTrkHit* hit) {
  fHitMap[hit->GetZ()] = hitIndex;
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmTrack::Print() {
  cout << " Number of attached hits : " 
       << fHits.GetSize()  << endl;
  fParamFirst.Print();
  cout << " Chi2: " << fChi2 << ", Quality flag " << fFlag << endl;
}
// -------------------------------------------------------------------------



// -----   Public method SortHits   ----------------------------------------
void CbmTrack::SortHits() {

  Int_t index=0;
  map<Double_t, Int_t>::iterator it;

  fHits.Reset();
  fHits.Set(fHitMap.size());
  index = 0;
  for (it = fHitMap.begin(); it != fHitMap.end(); it++) {
    fHits[index] = it->second;
    index++;
  }

}
// -------------------------------------------------------------------------


ClassImp(CbmTrack)
