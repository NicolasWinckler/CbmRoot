#ifndef CBMLITSTRIPHIT_H_
#define CBMLITSTRIPHIT_H_

#include "CbmLitHit.h"

#include <sstream>

class CbmLitStripHit : public CbmLitHit
{
public:
	CbmLitStripHit():
		fU(0.),
		fDu(0.),
		fPhi(0.),
		fCosPhi(0.),
		fSinPhi(0.)	{
		SetHitType(kLITSTRIPHIT);
	}

	virtual ~CbmLitStripHit() {};

	myf GetU() const { return fU; }
	myf GetDu() const { return fDu; }
	myf GetPhi() const { return fPhi; }
	myf GetCosPhi() const { return fCosPhi; }
	myf GetSinPhi() const { return fSinPhi; }

	void SetU(myf u) { fU = u; }
	void SetDu(myf du) { fDu = du; }
	void SetPhi(myf phi) { fPhi = phi; }
	void SetCosPhi(myf cosPhi) { fCosPhi = cosPhi; }
	void SetSinPhi(myf sinPhi) { fSinPhi = sinPhi; }

	virtual std::string ToString() const {
		std::stringstream ss;
		ss << "StripHit: pos=(" << GetU() << "," << GetZ()
		          << ") err=(" << GetDu() << "," << GetDz() << ") "
		          << " phi=" << GetPhi()
		          << " cosPhi=" << GetCosPhi() << " sinPhi=" << GetSinPhi()
		          << " planeId=" << GetPlaneId()
		          << " refId=" << GetRefId() << " w=" << GetW()
		          << " isOutlier=" << IsOutlier()
		          << " hitType=" << GetType() << std::endl;
		return ss.str();;
	}

protected:
	myf fU;
	myf fDu;
	myf fPhi;
	myf fCosPhi;
	myf fSinPhi;
};

#endif /*CBMLITSTRIPHIT_H_*/
