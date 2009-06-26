#include "CbmLitHit.h"

CbmLitHit::CbmLitHit():
	fPlaneId(0),
	fRefId(0),
	fW(1.0),
	fIsOutlier(false),
	fHitType(kLITPIXELHIT),
	fZ(0.),
	fDz(0.)
{
}

CbmLitHit::~CbmLitHit()
{
}
