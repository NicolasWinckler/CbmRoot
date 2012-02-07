/** CbmMuchStrawHit.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **/
#include "CbmMuchStrawHit.h"

CbmMuchStrawHit::CbmMuchStrawHit()
  : CbmStripHit(),
    fX(0.),
    fY(0.),
    fDouble(),
    fInt(),
    fFlag(0),
    fPlaneId(0),
    fTube(0),
    fSegment(0)   
{
	SetType(kMUCHSTRAWHIT);
}

CbmMuchStrawHit::~CbmMuchStrawHit()
{

}

CbmMuchStrawHit::CbmMuchStrawHit(
		Int_t detectorId,
		Double_t u,
		Double_t phi,
		Double_t z,
		Double_t du,
		Double_t dphi,
		Double_t dz,
		Int_t refId,
		Int_t planeId)
  : CbmStripHit(detectorId, u, phi, z, du, dphi, dz, refId),
    fX(0.),
    fY(0.),
    fDouble(),
    fInt(),
    fFlag(0),
    fPlaneId(planeId),
    fTube(0),
    fSegment(0)   
{
	SetType(kMUCHSTRAWHIT);
}

CbmMuchStrawHit::CbmMuchStrawHit(
		Int_t detectorId,
		const TVector3& pos,
		const TVector3& err,
		Int_t refId)
   : CbmStripHit(detectorId, pos, err, refId),
    fX(0.),
    fY(0.),
    fDouble(),
    fInt(),
    fFlag(0),
    fPlaneId(0),
    fTube(0),
    fSegment(0)   
{
	SetType(kMUCHSTRAWHIT);
}

//CbmMuchStrawHit::CbmMuchStrawHit(
//		Int_t detectorId,
//		const TVector3& pos,
//		const TVector3& err,
//		Int_t refId,
//		const Double_t* times,
//		Double_t dtime):
//	CbmStripHit(detectorId, pos, err, refId)
//{
//	SetTimes(times);
//	SetDTime(dtime);
//	SetType(kMUCHSTRAWHIT);
//}

ClassImp(CbmMuchStrawHit);
