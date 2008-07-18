
#include "CbmSttTrack.h"

ClassImp(CbmSttTrack);

// -----   Default constructor   -------------------------------------------
CbmSttTrack::CbmSttTrack() {
  fChi2  = 0.;
  fNDF = 0;
  fNHits = 0;
  fNMissedHits = 0;
  fNMissedStations = 0;
  fStsTrackID = -1;
  fFlag = 1;
  fScatAng = 0.;
  //fChi2Seed = 0.;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmSttTrack::~CbmSttTrack() {
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmSttTrack::Print() {
}
// -------------------------------------------------------------------------
