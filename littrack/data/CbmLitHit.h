#ifndef CBMLITHIT_H_
#define CBMLITHIT_H_

#include "CbmLitFloat.h"
#include "CbmLitEnums.h"

#include <string>

class CbmLitHit
{
public:
	CbmLitHit();
	virtual ~CbmLitHit();

	int GetPlaneId() const { return fPlaneId; }
	int GetRefId() const { return fRefId; }
	myf GetW() const { return fW; }
	bool IsOutlier() const { return fIsOutlier; }
	LitHitType GetType() const { return fHitType; }
	myf GetZ() const { return fZ; }
	myf GetDz() const { return fDz; }
	LitDetectorId GetDetectorId() const {return fDetectorId;}

	void SetPlaneId(int planeId) { fPlaneId = planeId; }
	void SetRefId(int refId) { fRefId = refId; }
	void SetW(myf w) { fW = w; }
	void IsOutlier(bool isOutlier) { fIsOutlier = isOutlier;}
	void SetHitType(LitHitType hitType) { fHitType = hitType; }
	void SetZ(myf z) { fZ = z; }
	void SetDz(myf dz) { fDz = dz; }
	void SetDetectorId(LitDetectorId detectorId) {fDetectorId = detectorId;}

	virtual std::string ToString() const {;}

private:
	int fPlaneId;
	int fRefId;
	myf fW;
	bool fIsOutlier;
	LitHitType fHitType;
	myf fZ;
	myf fDz;
	LitDetectorId fDetectorId;
};

#endif /*CBMLITHIT_H_*/
