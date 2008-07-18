// -------------------------------------------------------------------------
// -----                    CbmStsTrackMatch source file               -----
// -----                  Created 22/11/05  by V. Friese               -----
// -------------------------------------------------------------------------


#include "CbmStsTrackMatch.h"



// -----   Default constructor   -------------------------------------------
CbmStsTrackMatch::CbmStsTrackMatch() {
  fMCTrackId    = -1;
  fNofTrueHits  =  0;
  fNofWrongHits =  0;
  fNofFakeHits  =  0;
  fNofMCTracks  =  0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsTrackMatch::CbmStsTrackMatch(Int_t mcTrackId, Int_t nTrue,
				   Int_t nWrong, Int_t nFake, 
				   Int_t nTracks) {
  fMCTrackId    = mcTrackId;
  fNofTrueHits  = nTrue;
  fNofWrongHits = nWrong;
  fNofFakeHits  = nFake;
  fNofMCTracks  = nTracks;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsTrackMatch::~CbmStsTrackMatch() {}
// -------------------------------------------------------------------------


ClassImp(CbmStsTrackMatch)
