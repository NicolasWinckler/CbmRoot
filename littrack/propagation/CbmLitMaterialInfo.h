#ifndef CBMLITMATERIALINFO_H_
#define CBMLITMATERIALINFO_H_

#include "CbmLitFloat.h"
#include <string>

class CbmLitMaterialInfo
{
public:
	CbmLitMaterialInfo();
	CbmLitMaterialInfo(
			myf length,
			myf rl,
			myf rho,
			myf Z,
			myf A,
			myf zpos):
		fLength(length),
		fRL(rl),
		fRho(rho),
		fZ(Z),
		fA(A),
		fZpos(zpos){;}
	virtual ~CbmLitMaterialInfo();

	myf GetLength() const { return fLength;}
	myf GetRL() const { return fRL;}
	myf GetRho() const { return fRho;}
	myf GetZ() const { return fZ;}
	myf GetA() const { return fA;}
	myf GetZpos() const { return fZpos;}

	void SetLength(myf length) {fLength = length;}
	void SetRL(myf rl) {fRL = rl;}
	void SetRho(myf rho) {fRho = rho;}
	void SetZ(myf Z) {fZ = Z;}
	void SetA(myf A) {fA = A;}
	void SetZpos(myf zpos) {fZpos = zpos;}

	virtual std::string ToString() const;

private:
	myf fLength; // cm
	myf fRL; // cm
	myf fRho; // g/cm^3
	myf fZ;
	myf fA;
	myf fZpos; // z position
};

#endif /*CBMLITMATERIALINFO_H_*/
