/*
 * CbmMuchPixelHit.cxx
 *
 *  Created on: Mar 19, 2009
 *      Author: andrey
 */

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
		const TVector3& pos,
		const TVector3& err,
		Double_t dxy,
		Int_t refId):
	CbmPixelHit(detectorId, pos, err, dxy, refId)
{
	SetType(kMUCHPIXELHIT);
}

CbmMuchPixelHit::CbmMuchPixelHit(
		Int_t detectorId,
		const TVector3& pos,
		const TVector3& err,
		Double_t dxy,
		Int_t refId,
		const Double_t* times,
		Double_t dtime):
	CbmPixelHit(detectorId, pos, err, dxy, refId)
{
	SetTimes(times);
	SetDTime(dtime);
	SetType(kMUCHPIXELHIT);
}

ClassImp(CbmMuchPixelHit);
