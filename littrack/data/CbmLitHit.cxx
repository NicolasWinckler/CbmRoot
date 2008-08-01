#include "CbmLitHit.h"

#include <iostream>

CbmLitHit::CbmLitHit():
	fX(0.),	
	fY(0.), 
	fZ(0.),
	fDx(0.),
	fDy(0.),
	fDz(0.),
	fDxy(0.),
	fPlaneId(0),
	fRefId(0),
	fW(1.0),
	fIsOutlier(false)
{

}

CbmLitHit::~CbmLitHit()
{

}

void CbmLitHit::Print() const
{
	std::cout << "Hit: pos=(" << fX << "," << fY << "," << fZ 
	          << ") err=(" << fDx << "," << fDy << "," << fDz << ") "
	          << " dxy=" << fDxy << " planeId=" << fPlaneId 
	          << " refId=" << fRefId << " w=" << fW 
	          << " isOutlier=" << fIsOutlier << std::endl;
}

ClassImp(CbmLitHit)

