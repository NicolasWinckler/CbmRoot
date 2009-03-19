/*
 * CbmMuchStrawHit.cxx
 *
 *  Created on: Mar 19, 2009
 *      Author: andrey
 */

#include "CbmMuchStrawHit.h"

CbmMuchStrawHit::CbmMuchStrawHit()
{
}

CbmMuchStrawHit::~CbmMuchStrawHit()
{
	SetType(kMUCHSTRAWHIT);
}

CbmMuchStrawHit::CbmMuchStrawHit(
		Int_t detectorId,
		const TVector3& pos,
		const TVector3& err,
		Int_t refId):
	CbmStripHit(detectorId, pos, err, refId)
{
	SetType(kMUCHSTRAWHIT);
}

CbmMuchStrawHit::CbmMuchStrawHit(
		Int_t detectorId,
		const TVector3& pos,
		const TVector3& err,
		Int_t refId,
		const Double_t* times,
		Double_t dtime):
	CbmStripHit(detectorId, pos, err, refId)
{
	SetTimes(times);
	SetDTime(dtime);
	SetType(kMUCHSTRAWHIT);
}

ClassImp(CbmMuchStrawHit);
