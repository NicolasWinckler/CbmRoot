#include "CbmLitPixelHit.h"

#include <sstream>

CbmLitPixelHit::CbmLitPixelHit():
	fX(0.),
	fY(0.),
	fDx(0.),
	fDy(0.),
	fDxy(0.)
{
	SetHitType(kLITPIXELHIT);
}

CbmLitPixelHit::~CbmLitPixelHit()
{
}

std::string CbmLitPixelHit::ToString() const
{
	std::stringstream ss;
	ss << "PixelHit: pos=(" << GetX() << "," << GetY() << "," << GetZ()
	   << ") err=(" << GetDx() << "," << GetDy() << "," << GetDz() << ") "
	   << " dxy=" << GetDxy() << " planeId=" << GetPlaneId()
	   << " refId=" << GetRefId() << " w=" << GetW()
	   << " isOutlier=" << IsOutlier()
	   << " hitType=" << GetType() << std::endl;
	return ss.str();
}
