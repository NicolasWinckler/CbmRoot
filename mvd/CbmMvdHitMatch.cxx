// -------------------------------------------------------------------------
// -----                    CbmMvdHitMatch source file                 -----
// -----                  Created 07/11/06  by V. Friese               -----
// -----            Based on CbmStsMapsHitInfo by M. Deveaux           -----
// -------------------------------------------------------------------------



#include "CbmMvdHitMatch.h"


// -----   Default constructor   -------------------------------------------
CbmMvdHitMatch::CbmMvdHitMatch() {
  Clear();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdHitMatch::CbmMvdHitMatch(Int_t fileNumber, Int_t eventNumber,
				     Int_t trackId, Int_t pointId,
				     Int_t nMerged) {
  fFileNumber  = fileNumber;
  fEventNumber = eventNumber;
  fTrackId     = trackId;
  fPointId     = pointId;
  fNMerged     = nMerged;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvdHitMatch::~CbmMvdHitMatch() { }
// -------------------------------------------------------------------------



// -----   Public method Clear   -------------------------------------------
void CbmMvdHitMatch::Clear() {
  fFileNumber  = -1;
  fEventNumber = -1;
  fTrackId     = -1;
  fPointId     = -1;
  fNMerged     =  0;
}
// -------------------------------------------------------------------------




ClassImp(CbmMvdHitMatch);
