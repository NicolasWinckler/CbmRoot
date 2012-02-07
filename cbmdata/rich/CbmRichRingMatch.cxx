/******************************************************************************
*  $Id: CbmRichRingMatch.cxx,v 1.2 2006/02/22 09:16:15 hoehne Exp $
*
*  Class      : CbmRichMatchRings
*  Description: Impementation
*  Author     : Supriya Das (following CbmStsTrackMatch)
*  E-mail     : S.Das@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingMatch.cxx,v $
*  Revision 1.2  2006/02/22 09:16:15  hoehne
*  store also number of MC hits per ring (Simeon Lebedev)
*
*  Revision 1.1  2006/01/23 11:42:05  hoehne
*  initial version: data class describing the marching of reconstructed to MC rings
*
*
*******************************************************************************/

#include "CbmRichRingMatch.h"



// -----   Default constructor   -------------------------------------------
CbmRichRingMatch::CbmRichRingMatch() 
  : TObject(),
    fMCTrackID(-1),
    fNofTrueHits(0),
    fNofWrongHits(0),
    fNofFakeHits(0),
    fNofMCHits(0),
    fNofRings(0)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmRichRingMatch::CbmRichRingMatch(Int_t mcTrackID, Int_t nTrue,
				   Int_t nWrong, Int_t nFake,
				   Int_t nMCHits, Int_t nRings) 
  : TObject(),
    fMCTrackID(mcTrackID),
    fNofTrueHits(nTrue),
    fNofWrongHits(nWrong),
    fNofFakeHits(nFake),
    fNofMCHits(nMCHits),
    fNofRings(nRings)
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmRichRingMatch::~CbmRichRingMatch() {}
// -------------------------------------------------------------------------


ClassImp(CbmRichRingMatch)
