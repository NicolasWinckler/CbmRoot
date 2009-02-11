#include "CbmSttPoint.h"
#include "CbmSttHit.h"

#include "TVector3.h"

//___________________________________________________________________
//
// CbmSttHit
//
// Class for description of the hit in the STT
//

// ------------------------------------------------------------------
CbmSttHit::CbmSttHit()
{
    // Default constructor
}
// ------------------------------------------------------------------

// ------------------------------------------------------------------
/*
CbmTrdHit::CbmTrdHit(Int_t detID, TVector3& pos, TVector3& dpos, Int_t index,
		     Int_t planeID, Double_t eLossTR, Double_t eLossdEdx,
		     Double_t eLoss)
: FairHit(detID, pos, dpos, index)
{
    // Standard constructor
    fPlaneID = planeID;
    fELossTR = eLossTR;
    fELossdEdx = eLossdEdx;
    fELoss = eLoss;
}
*/
// ------------------------------------------------------------------

// ------------------------------------------------------------------
CbmSttHit::CbmSttHit(Int_t detID, TVector3& pos, TVector3& dpos, 
		     Int_t index, CbmSttPoint *point)
: CbmTrkHit(detID, pos, dpos, 0., index)
{
  // Standard constructor
  fPlaneID = point->GetLayerNo() + (point->GetStationNo() - 1) * 6;
  fTube = fSegment = 0;
  fChamber = point->GetChamberNo();
  fAlong = 0.;
}
// ------------------------------------------------------------------

// ------------------------------------------------------------------
CbmSttHit::~CbmSttHit()
{
    // Destructor
}
// ------------------------------------------------------------------

ClassImp(CbmSttHit)

