/*
 * CbmBaseHit.cxx
 *
 *  Created on: Mar 17, 2009
 *      Author: andrey
 */

#include "CbmBaseHit.h"

CbmBaseHit::CbmBaseHit():
	fType(kHIT),
	fDetectorId(-1),
	fZ(0.),
	fDz(0.),
	fRefId(-1)
{
}

CbmBaseHit::CbmBaseHit(
		Int_t detectorId,
		Double_t z,
		Double_t dz,
		Int_t refId):
	fType(kHIT),
	fDetectorId(detectorId),
	fZ(z),
	fDz(dz),
	fRefId(refId)
{
}

CbmBaseHit::~CbmBaseHit()
{
}

ClassImp(CbmBaseHit);
