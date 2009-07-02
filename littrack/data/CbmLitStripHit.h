#ifndef CBMLITSTRIPHIT_H_
#define CBMLITSTRIPHIT_H_

#include "CbmLitHit.h"

class CbmLitStripHit : public CbmLitHit
{
public:
	CbmLitStripHit();
	virtual ~CbmLitStripHit();

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

	virtual std::string ToString() const;

protected:
	myf fU;
	myf fDu;
	myf fPhi;
	myf fCosPhi;
	myf fSinPhi;
};

#endif /*CBMLITSTRIPHIT_H_*/
