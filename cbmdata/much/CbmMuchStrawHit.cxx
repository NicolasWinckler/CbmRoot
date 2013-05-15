/**
 * \file CbmMuchStrawHit.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
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
		Int_t address,
		Double_t u,
		Double_t phi,
		Double_t z,
		Double_t du,
		Double_t dphi,
		Double_t dz,
		Int_t refId,
		Int_t planeId)
  : CbmStripHit(address, u, phi, z, du, dphi, dz, refId),
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
		Int_t address,
		const TVector3& pos,
		const TVector3& err,
		Int_t refId)
   : CbmStripHit(address, pos, err, refId),
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

ClassImp(CbmMuchStrawHit);
