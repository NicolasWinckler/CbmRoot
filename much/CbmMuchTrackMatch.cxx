// -------------------------------------------------------------------------
// -----                    CbmMuchTrackMatch source file               -----
// -----                  Created 10/10/07  by A.Lebedev                -----
// -------------------------------------------------------------------------
#include "CbmMuchTrackMatch.h"

CbmMuchTrackMatch::CbmMuchTrackMatch():
  fMCTrackId(-1),
  fNofTrueHits(0),
  fNofWrongHits(0),
  fNofFakeHits(0),
  fNofMCTracks(0)
{
}

CbmMuchTrackMatch::CbmMuchTrackMatch(
		Int_t mcTrackId, 
		Int_t nTrue,  
		Int_t nWrong, 
		Int_t nFake,
		Int_t nTracks):
  fMCTrackId(mcTrackId),
  fNofTrueHits(nTrue),
  fNofWrongHits(nWrong),
  fNofFakeHits(nFake),
  fNofMCTracks(nTracks)
{
}

CbmMuchTrackMatch::~CbmMuchTrackMatch() 
{
}

ClassImp(CbmMuchTrackMatch)
