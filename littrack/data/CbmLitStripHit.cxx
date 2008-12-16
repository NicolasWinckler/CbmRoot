#include "CbmLitStripHit.h"

#include <sstream>

CbmLitStripHit::CbmLitStripHit():
	fU(0.),	
	fDu(0.), 
	fPhi(0.),
	fCosPhi(0.),
	fSinPhi(0.)
{
	SetHitType(kLITSTRIPHIT);
}

CbmLitStripHit::~CbmLitStripHit()
{
}

std::string CbmLitStripHit::ToString() const
{
	std::stringstream ss;
	ss << "StripHit: pos=(" << GetU() << "," << GetZ() 
	          << ") err=(" << GetDu() << "," << GetDz() << ") "
	          << " phi=" << GetPhi()
	          << " cosPhi=" << GetCosPhi() << " sinPhi=" << GetSinPhi()
	          << " planeId=" << GetPlaneId() 
	          << " refId=" << GetRefId() << " w=" << GetW() 
	          << " isOutlier=" << IsOutlier() 
	          << " hitType=" << GetType() << std::endl;
	return ss.str();
}
