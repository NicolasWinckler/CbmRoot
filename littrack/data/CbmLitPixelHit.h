#ifndef CBMLITPIXELHIT_H_
#define CBMLITPIXELHIT_H_

#include "CbmLitHit.h"

#include <string>

class CbmLitPixelHit : public CbmLitHit
{
public:
	CbmLitPixelHit();
	virtual ~CbmLitPixelHit();

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

	virtual std::string ToString() const;

private:

	myf fX, fY;
	myf fDx, fDy;
	myf fDxy;
};

#endif /*CBMLITPIXELHIT_H_*/
