/*
 * CbmStripHit.cxx
 *
 *  Created on: Mar 18, 2009
 *      Author: andrey
 */

#include "CbmStripHit.h"

#include "TVector3.h"

#include <iostream>

CbmStripHit::CbmStripHit():
	CbmBaseHit(-1, 0., 0., -1),
	fU(0.),
	fPhi(0.),
	fDu(0.),
	fDphi(0.)
{
	SetType(kSTRIPHIT);
}

CbmStripHit::CbmStripHit(
		Int_t detectorId,
		Double_t u,
		Double_t phi,
		Double_t z,
		Double_t du,
		Double_t dphi,
		Double_t dz,
		Int_t refId):
	CbmBaseHit(detectorId, z, dz, refId),
	fU(u),
	fPhi(phi),
	fDu(du),
	fDphi(dphi)
{
	SetType(kSTRIPHIT);
}

CbmStripHit::CbmStripHit(
		Int_t detectorId,
		const TVector3& pos,
		const TVector3& err,
		Int_t refId):
	CbmBaseHit(detectorId, pos.Z(), err.Z(), refId),
	fU(pos.X()),
	fPhi(pos.Y()),
	fDu(err.X()),
	fDphi(err.Y())
{
	SetType(kSTRIPHIT);
}

CbmStripHit::~CbmStripHit()
{
}

void CbmStripHit::Print() const
{
	std::cout << "CbmStripHit: detectorId=" << GetDetectorId()
		<< " pos=(" << GetU() << "," << GetPhi() << "," << GetZ()
		<< ") err=(" << GetDu() << "," << GetDphi() << "," << GetDz()
		<< ") refId=" << GetRefId() << std::endl;
}

ClassImp(CbmStripHit);
