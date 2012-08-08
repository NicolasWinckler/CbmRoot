/** CbmMuchPixelHit.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **/
#include "CbmMuchPixelHit.h"

CbmMuchPixelHit::CbmMuchPixelHit()
  : CbmPixelHit(),
    fPlaneId(-1),
    fTime(-1.),
    fDTime(-1.)
{
  SetType(kMUCHPIXELHIT);
}

CbmMuchPixelHit::~CbmMuchPixelHit(){

}

CbmMuchPixelHit::CbmMuchPixelHit(
		Int_t detectorId,
		Double_t x,
		Double_t y,
		Double_t z,
		Double_t dx,
		Double_t dy,
		Double_t dz,
		Double_t dxy,
		Int_t refId,
		Int_t planeId,
		Double_t t,
		Double_t dt)
  : CbmPixelHit(detectorId, x, y, z, dx, dy, dz, dxy, refId),
    fPlaneId(planeId),
    fTime(t),
    fDTime(dt)
{
	SetType(kMUCHPIXELHIT);
}

CbmMuchPixelHit::CbmMuchPixelHit(
		Int_t detectorId,
		const TVector3& pos,
		const TVector3& err,
		Double_t dxy,
		Int_t refId,
		Int_t planeId)
  : CbmPixelHit(detectorId, pos, err, dxy, refId),
    fPlaneId(planeId),
    fTime(-1.),
    fDTime(-1.)
{
  SetType(kMUCHPIXELHIT);
}

CbmMuchPixelHit::CbmMuchPixelHit(
		Int_t detectorId,
		const TVector3& pos,
		const TVector3& err,
		Double_t dxy,
		Int_t refId,
		Int_t planeId,
		Double_t time,
		Double_t dtime)
  : CbmPixelHit(detectorId, pos, err, dxy, refId),
    fPlaneId(planeId),
    fTime(time),
    fDTime(dtime)
{
  SetType(kMUCHPIXELHIT);
}

ClassImp(CbmMuchPixelHit);
