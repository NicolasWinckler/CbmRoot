#ifndef CBMLITPIXELHIT_H_
#define CBMLITPIXELHIT_H_

#include "CbmLitHit.h"

#include <string>

class CbmLitPixelHit : public CbmLitHit
{
public:
	CbmLitPixelHit();
	virtual ~CbmLitPixelHit();

	double GetX() const { return fX; }
	double GetY() const { return fY; }
	double GetDx() const { return fDx; }
	double GetDy() const { return fDy; }
	double GetDxy() const { return fDxy; }

	void SetX(double x) { fX = x; }
	void SetY(double y) { fY = y; }
	void SetDx(double dx) { fDx = dx; }
	void SetDy(double dy) { fDy = dy; }
	void SetDxy(double dxy) { fDxy = dxy; }

	virtual std::string ToString() const;

private:

	double fX, fY;
	double fDx, fDy;
	double fDxy;
};

#endif /*CBMLITPIXELHIT_H_*/
