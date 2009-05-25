/** CbmMuchPixelHit.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **/
#include "CbmMuchPixelHit.h"

CbmMuchPixelHit::CbmMuchPixelHit()
{

}

CbmMuchPixelHit::~CbmMuchPixelHit()
{
	SetType(kMUCHPIXELHIT);
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
		Int_t refId):
	CbmPixelHit(detectorId, x, y, z, dx, dy, dz, dxy, refId)
{
	SetType(kMUCHPIXELHIT);
}

CbmMuchPixelHit::CbmMuchPixelHit(
		Int_t detectorId,
		const TVector3& pos,
		const TVector3& err,
		Double_t dxy,
		Int_t refId):
	CbmPixelHit(detectorId, pos, err, dxy, refId)
{
	SetType(kMUCHPIXELHIT);
}

//CbmMuchPixelHit::CbmMuchPixelHit(
//		Int_t detectorId,
//		const TVector3& pos,
//		const TVector3& err,
//		Double_t dxy,
//		Int_t refId,
//		const Double_t* times,
//		Double_t dtime):
//	CbmPixelHit(detectorId, pos, err, dxy, refId)
//{
//	SetTimes(times);
//	SetDTime(dtime);
//	SetType(kMUCHPIXELHIT);
//}

ClassImp(CbmMuchPixelHit);
