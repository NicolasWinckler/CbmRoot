#ifndef CBMLITPIXELHIT_H_
#define CBMLITPIXELHIT_H_

#include "CbmLitHit.h"

#include "TObject.h"

#include <string>

class CbmLitPixelHit : public CbmLitHit
{
public:
	CbmLitPixelHit();
	virtual ~CbmLitPixelHit();
	
	Double_t GetX() const { return fX; }
	Double_t GetY() const { return fY; }
	Double_t GetDx() const { return fDx; }
	Double_t GetDy() const { return fDy; }
	Double_t GetDxy() const { return fDxy; }
	
	void SetX(Double_t x) { fX = x; }
	void SetY(Double_t y) { fY = y; }
	void SetDx(Double_t dx) { fDx = dx; }
	void SetDy(Double_t dy) { fDy = dy; }
	void SetDxy(Double_t dxy) { fDxy = dxy; }
	
	virtual std::string ToString() const;
	
private:
	
	Double_t fX, fY;
	Double_t fDx, fDy;
	Double_t fDxy;
	
	ClassDef(CbmLitPixelHit, 1);
};

#endif /*CBMLITPIXELHIT_H_*/
