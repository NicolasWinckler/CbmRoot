/*
 * CbmPixelHit.cxx
 *
 *  Created on: Mar 17, 2009
 *      Author: andrey
 */

#include "CbmPixelHit.h"

#include "TVector3.h"

#include <iostream>

CbmPixelHit::CbmPixelHit():
	CbmBaseHit(-1, 0., 0., -1),
	fX(0.),
	fY(0.),
	fDx(0.),
	fDy(0.),
	fDxy(0.)
{
	SetType(kPIXELHIT);
}

CbmPixelHit::CbmPixelHit(
		Int_t detectorId,
		Double_t x,
		Double_t y,
		Double_t z,
		Double_t dx,
		Double_t dy,
		Double_t dz,
		Double_t dxy,
		Int_t refId):
	CbmBaseHit(detectorId, z, dz, refId),
	fX(x),
	fY(y),
	fDx(dx),
	fDy(dy),
	fDxy(dxy)
{
	SetType(kPIXELHIT);
}

CbmPixelHit::CbmPixelHit(
		Int_t detectorId,
		const TVector3& pos,
		const TVector3& err,
		Double_t dxy,
		Int_t refId):
	CbmBaseHit(detectorId, pos.Z(), err.Z(), refId),
	fX(pos.X()),
	fY(pos.Y()),
	fDx(err.X()),
	fDy(err.Y()),
	fDxy(dxy)
{
	SetType(kPIXELHIT);
}

CbmPixelHit::~CbmPixelHit()
{
}

void CbmPixelHit::Print() const
{
	std::cout << "CbmPixelHit: detectorId=" << GetDetectorId()
		<< " pos=(" << GetX() << "," << GetY() << "," << GetZ()
		<< ") err=(" << GetDx() << "," << GetDy() << "," << GetDz()
		<< ") dxy=" << GetDxy() << " refId=" << GetRefId() << std::endl;
}

ClassImp(CbmPixelHit);
