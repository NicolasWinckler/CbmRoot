#include "CbmTrdHit.h"

#include "TVector3.h"

//___________________________________________________________________
//
// CbmTrdHit
//
// Class for description of the hit in the TRD system
//


// ------------------------------------------------------------------
CbmTrdHit::CbmTrdHit()
{
    // Default constructor
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmTrdHit::CbmTrdHit(Int_t detID, TVector3& pos, TVector3& dpos, 
		     Int_t index, Int_t planeID, Double_t eLossTR, 
		     Double_t eLossdEdx, Double_t eLoss)
  : CbmHit(detID, pos, dpos, 0., index)
{
    // Standard constructor
    fPlaneID = planeID;
    fELossTR = eLossTR;
    fELossdEdx = eLossdEdx;
    fELoss = eLoss;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmTrdHit::~CbmTrdHit()
{
    // Destructor
}
// ------------------------------------------------------------------


ClassImp(CbmTrdHit)

