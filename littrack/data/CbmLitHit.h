#ifndef CBMLITHIT_H_
#define CBMLITHIT_H_

#include "CbmLitEnums.h"

#include "TObject.h"

#include <string>

class CbmLitHit
{
public:
	CbmLitHit();
	virtual ~CbmLitHit();

	Int_t GetPlaneId() const { return fPlaneId; }
	Int_t GetRefId() const { return fRefId; }
	Double_t GetW() const { return fW; }
	Bool_t IsOutlier() const { return fIsOutlier; }
	LitHitType GetType() const { return fHitType; }
	Double_t GetZ() const { return fZ; }
	Double_t GetDz() const { return fDz; }
	
	void SetPlaneId(Int_t planeId) { fPlaneId = planeId; }
	void SetRefId(Int_t refId) { fRefId = refId; }
	void SetW(Double_t w) { fW = w; }
	void IsOutlier(Bool_t isOutlier) { fIsOutlier = isOutlier;}
	void SetHitType(LitHitType hitType) { fHitType = hitType; }
	void SetZ(Double_t z) { fZ = z; }
	void SetDz(Double_t dz) { fDz = dz; }
	
	virtual std::string ToString() const {;}
	
private:
	Int_t fPlaneId;
	Int_t fRefId;
	Double_t fW;
	Bool_t fIsOutlier;
	LitHitType fHitType;
	Double_t fZ;
	Double_t fDz;
		
	ClassDef(CbmLitHit, 1);
};

#endif /*CBMLITHIT_H_*/
