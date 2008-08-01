#ifndef CBMLITHIT_H_
#define CBMLITHIT_H_

#include "TObject.h"

class CbmLitHit
{
public:

	CbmLitHit();
	virtual ~CbmLitHit();
	
	Double_t GetX() const { return fX; }
	Double_t GetY() const { return fY; }
	Double_t GetZ() const { return fZ; }
	Double_t GetDx() const { return fDx; }
	Double_t GetDy() const { return fDy; }
	Double_t GetDz() const { return fDz; }
	Double_t GetDxy() const { return fDxy; }
	Int_t GetPlaneId() const { return fPlaneId; }
	Int_t GetRefId() const { return fRefId; }
	Double_t GetW() const { return fW; }
	Bool_t IsOutlier() const { return fIsOutlier; }
	
	void SetX(Double_t x) { fX = x; }
	void SetY(Double_t y) { fY = y; }
	void SetZ(Double_t z) { fZ = z; }
	void SetDx(Double_t dx) { fDx = dx; }
	void SetDy(Double_t dy) { fDy = dy; }
	void SetDz(Double_t dz) { fDz = dz; }
	void SetDxy(Double_t dxy) { fDxy = dxy; }
	void SetPlaneId(Int_t planeId) { fPlaneId = planeId; }
	void SetRefId(Int_t refId) { fRefId = refId; }
	void SetW(Double_t w) { fW = w; }
	void IsOutlier(Bool_t isOutlier) { fIsOutlier = isOutlier;}
	
	virtual void Print() const;
	
protected:
	
	Double_t fX, fY, fZ;
	Double_t fDx, fDy, fDz;
	Double_t fDxy;
	Int_t fPlaneId;
	Int_t fRefId;
	Double_t fW;
	Bool_t fIsOutlier;
	
	ClassDef(CbmLitHit, 1);
};

#endif /*CBMLITHIT_H_*/
