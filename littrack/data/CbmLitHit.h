#ifndef CBMLITHIT_H_
#define CBMLITHIT_H_

#include "CbmLitEnums.h"

#include <string>

class CbmLitHit
{
public:
	CbmLitHit();
	virtual ~CbmLitHit();

	int GetPlaneId() const { return fPlaneId; }
	int GetRefId() const { return fRefId; }
	double GetW() const { return fW; }
	bool IsOutlier() const { return fIsOutlier; }
	LitHitType GetType() const { return fHitType; }
	double GetZ() const { return fZ; }
	double GetDz() const { return fDz; }
	LitDetectorId GetDetectorId() const {return fDetectorId;}

	void SetPlaneId(int planeId) { fPlaneId = planeId; }
	void SetRefId(int refId) { fRefId = refId; }
	void SetW(double w) { fW = w; }
	void IsOutlier(bool isOutlier) { fIsOutlier = isOutlier;}
	void SetHitType(LitHitType hitType) { fHitType = hitType; }
	void SetZ(double z) { fZ = z; }
	void SetDz(double dz) { fDz = dz; }
	void SetDetectorId(LitDetectorId detectorId) {fDetectorId = detectorId;}

	virtual std::string ToString() const {;}

private:
	int fPlaneId;
	int fRefId;
	double fW;
	bool fIsOutlier;
	LitHitType fHitType;
	double fZ;
	double fDz;
	LitDetectorId fDetectorId;
};

#endif /*CBMLITHIT_H_*/
