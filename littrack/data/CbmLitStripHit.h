#ifndef CBMLITSTRIPHIT_H_
#define CBMLITSTRIPHIT_H_

#include "CbmLitHit.h"

class CbmLitStripHit : public CbmLitHit
{
public:
	CbmLitStripHit();
	virtual ~CbmLitStripHit();

	double GetU() const { return fU; }
	double GetDu() const { return fDu; }
	double GetPhi() const { return fPhi; }
	double GetCosPhi() const { return fCosPhi; }
	double GetSinPhi() const { return fSinPhi; }

	void SetU(double u) { fU = u; }
	void SetDu(double du) { fDu = du; }
	void SetPhi(double phi) { fPhi = phi; }
	void SetCosPhi(double cosPhi) { fCosPhi = cosPhi; }
	void SetSinPhi(double sinPhi) { fSinPhi = sinPhi; }

	virtual std::string ToString() const;

protected:
	double fU;
	double fDu;
	double fPhi;
	double fCosPhi;
	double fSinPhi;
};

#endif /*CBMLITSTRIPHIT_H_*/
