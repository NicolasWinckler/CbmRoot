/** CbmLitStripHit.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **
 ** Base data class for strip hit.
 **/

#ifndef CBMLITSTRIPHIT_H_
#define CBMLITSTRIPHIT_H_

#include "data/CbmLitHit.h"

#include <sstream>

class CbmLitStripHit : public CbmLitHit
{
public:
	/* Constructor */
	CbmLitStripHit():
		fU(0.),
		fDu(0.),
		fPhi(0.),
		fCosPhi(0.),
		fSinPhi(0.)	{
		SetHitType(kLITSTRIPHIT);
	}

	/* Destructor */
	virtual ~CbmLitStripHit() {};

	/* Returns U measurement of hit */
	myf GetU() const { return fU; }

	/* Returns U measurement error */
	myf GetDu() const { return fDu; }

	/* Returns rotation angle of the strip */
	myf GetPhi() const { return fPhi; }

	/* Returns cosine of the strip rotation angle */
	myf GetCosPhi() const { return fCosPhi; }

	/* Returns sine of the strip rotation angle */
	myf GetSinPhi() const { return fSinPhi; }

	/* Sets U measurement of the hit */
	void SetU(myf u) { fU = u; }

	/* Sets U measurement error */
	void SetDu(myf du) { fDu = du; }

	/* Sets rotation angle of the strip */
	void SetPhi(myf phi) { fPhi = phi; }

	/* Sets cosine of strip rotation angle */
	void SetCosPhi(myf cosPhi) { fCosPhi = cosPhi; }

	/* Sets sine of strip rotation angle */
	void SetSinPhi(myf sinPhi) { fSinPhi = sinPhi; }

	/* Returns string representation of the class */
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
	/* U measurement of the hit in [cm] */
	myf fU;
	/* U measurement error in [cm] */
	myf fDu;
	/* Strip rotation angle in [rad] */
	myf fPhi;
	/* Cosine of the strip rotation angle */
	myf fCosPhi;
	/* Sine of the strip rotation angle */
	myf fSinPhi;
};

#endif /*CBMLITSTRIPHIT_H_*/
