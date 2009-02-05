
#include "CbmMuchTrack.h"

ClassImp(CbmMuchTrack);

// -----   Default constructor   -------------------------------------------
CbmMuchTrack::CbmMuchTrack() {
  fChi2  = 0.;
  fNDF = 0;
  fNHits = 0;
  fNMissedHits = 0;
  fNMissedStations = 0;
  fStsTrackID = -1;
  fFlag = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMuchTrack::~CbmMuchTrack() {
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmMuchTrack::Print() {
}
// -------------------------------------------------------------------------
