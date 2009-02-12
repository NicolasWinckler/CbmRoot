// -------------------------------------------------------------------------
// -----                    CbmTrdTrackMatch source file               -----
// -----                  Created 01/12/05  by V. Friese               -----
// -------------------------------------------------------------------------


#include "CbmTrdTrackMatch.h"



// -----   Default constructor   -------------------------------------------
CbmTrdTrackMatch::CbmTrdTrackMatch() {
  fMCTrackID    = -1;
  fNofTrueHits  =  0;
  fNofWrongHits =  0;
  fNofFakeHits  =  0;
  fNofMCTracks  =  0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmTrdTrackMatch::CbmTrdTrackMatch(Int_t mcTrackID, Int_t nTrue,
				   Int_t nWrong, Int_t nFake, 
				   Int_t nTracks) {
  fMCTrackID    = mcTrackID;
  fNofTrueHits  = nTrue;
  fNofWrongHits = nWrong;
  fNofFakeHits  = nFake;
  fNofMCTracks  = nTracks;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrdTrackMatch::~CbmTrdTrackMatch() {}
// -------------------------------------------------------------------------


ClassImp(CbmTrdTrackMatch)
