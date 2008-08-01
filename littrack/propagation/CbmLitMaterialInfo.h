#ifndef CBMLITMATERIALINFO_H_
#define CBMLITMATERIALINFO_H_

#include "TObject.h"

class CbmLitMaterialInfo
{
public:
	CbmLitMaterialInfo();
	virtual ~CbmLitMaterialInfo();
	
	Double_t GetLength() const { return fLength;}
	Double_t GetRL() const { return fRL;}
	Double_t GetRho() const { return fRho;}
	Double_t GetZ() const { return fZ;}
	Double_t GetA() const { return fA;}
	Double_t GetZpos() const { return fZpos;}
	
	void SetLength(Double_t length) {fLength = length;}
	void SetRL(Double_t rl) {fRL = rl;}
	void SetRho(Double_t rho) {fRho = rho;}
	void SetZ(Double_t Z) {fZ = Z;}
	void SetA(Double_t A) {fA = A;}
	void SetZpos(Double_t zpos) {fZpos = zpos;}
	
	void Print() const;
	
private:
	Double_t fLength; // cm
	Double_t fRL; // cm
	Double_t fRho; // g/cm^3
	Double_t fZ;
	Double_t fA;
	Double_t fZpos; // z position
	
	ClassDef(CbmLitMaterialInfo,1);
};

#endif /*CBMLITMATERIALINFO_H_*/
