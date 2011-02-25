#ifndef CBMLITPIXELHIT_H_
#define CBMLITPIXELHIT_H_

#include "data/CbmLitHit.h"

#include <string>
#include <sstream>

class CbmLitPixelHit : public CbmLitHit
{
public:
	CbmLitPixelHit():
		fX(0.),
		fY(0.),
		fDx(0.),
		fDy(0.),
		fDxy(0.) {
		SetHitType(kLITPIXELHIT);
	}

	virtual ~CbmLitPixelHit() {};

	myf GetX() const { return fX; }
	myf GetY() const { return fY; }
	myf GetDx() const { return fDx; }
	myf GetDy() const { return fDy; }
	myf GetDxy() const { return fDxy; }

	void SetX(myf x) { fX = x; }
	void SetY(myf y) { fY = y; }
	void SetDx(myf dx) { fDx = dx; }
	void SetDy(myf dy) { fDy = dy; }
	void SetDxy(myf dxy) { fDxy = dxy; }

	virtual std::string ToString() const {
		std::stringstream ss;
		ss << "PixelHit: pos=(" << GetX() << "," << GetY() << "," << GetZ()
		   << ") err=(" << GetDx() << "," << GetDy() << "," << GetDz() << ") "
		   << " dxy=" << GetDxy() << " planeId=" << GetPlaneId()
		   << " refId=" << GetRefId() << " w=" << GetW()
		   << " isOutlier=" << IsOutlier()
		   << " hitType=" << GetType() << std::endl;
		return ss.str();
	}

private:
	myf fX, fY;
	myf fDx, fDy;
	myf fDxy;
};

#endif /*CBMLITPIXELHIT_H_*/
