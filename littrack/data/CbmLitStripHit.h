#ifndef CBMLITSTRIPHIT_H_
#define CBMLITSTRIPHIT_H_

#include "CbmLitHit.h"

class CbmLitStripHit : public CbmLitHit
{
public:
	CbmLitStripHit();
	virtual ~CbmLitStripHit();
	
	Double_t GetU() const { return fU; }
	Double_t GetDu() const { return fDu; }
	Double_t GetPhi() const { return fPhi; }
	Double_t GetCosPhi() const { return fCosPhi; }
	Double_t GetSinPhi() const { return fSinPhi; }
	
	void SetU(Double_t u) { fU = u; }
	void SetDu(Double_t du) { fDu = du; }		
	void SetPhi(Double_t phi) { fPhi = phi; }
	void SetCosPhi(Double_t cosPhi) { fCosPhi = cosPhi; }
	void SetSinPhi(Double_t sinPhi) { fSinPhi = sinPhi; }
	
	virtual std::string ToString() const;
	
protected:
	Double_t fU;
	Double_t fDu;
	Double_t fPhi;
	Double_t fCosPhi;
	Double_t fSinPhi;

	ClassDef(CbmLitStripHit, 1);
};

#endif /*CBMLITSTRIPHIT_H_*/
