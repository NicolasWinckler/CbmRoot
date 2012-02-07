/** CbmTrdHit.cxx
 ** updated by Andrey Lebedev 26/05/2009
 **/
#include "CbmTrdHit.h"

#include "TVector3.h"

CbmTrdHit::CbmTrdHit()
  : CbmPixelHit(),
    fPlaneId(-1),
    fELossTR(-1.),
    fELossdEdx(-1.),
    fELoss(-1.)
{
  SetType(kTRDHIT);
}

CbmTrdHit::CbmTrdHit(
		Int_t detectorId,
		TVector3& pos,
		TVector3& dpos,
		Double_t dxy,
		Int_t refId,
		Int_t planeId,
		Double_t eLossTR,
		Double_t eLossdEdx,
		Double_t eLoss)
  : CbmPixelHit(detectorId, pos, dpos, dxy, refId),
    fPlaneId(planeId),
    fELossTR(eLossTR),
    fELossdEdx(eLossdEdx),
    fELoss(eLoss)
{
  SetType(kTRDHIT);
}

CbmTrdHit::~CbmTrdHit()
{
}

ClassImp(CbmTrdHit);
