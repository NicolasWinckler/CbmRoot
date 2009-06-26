#ifndef CBMLITMATERIALINFO_H_
#define CBMLITMATERIALINFO_H_

#include <string>

class CbmLitMaterialInfo
{
public:
	CbmLitMaterialInfo();
	CbmLitMaterialInfo(
			double length,
			double rl,
			double rho,
			double Z,
			double A,
			double zpos):
		fLength(length),
		fRL(rl),
		fRho(rho),
		fZ(Z),
		fA(A),
		fZpos(zpos){;}
	virtual ~CbmLitMaterialInfo();

	double GetLength() const { return fLength;}
	double GetRL() const { return fRL;}
	double GetRho() const { return fRho;}
	double GetZ() const { return fZ;}
	double GetA() const { return fA;}
	double GetZpos() const { return fZpos;}

	void SetLength(double length) {fLength = length;}
	void SetRL(double rl) {fRL = rl;}
	void SetRho(double rho) {fRho = rho;}
	void SetZ(double Z) {fZ = Z;}
	void SetA(double A) {fA = A;}
	void SetZpos(double zpos) {fZpos = zpos;}

	virtual std::string ToString() const;

private:
	double fLength; // cm
	double fRL; // cm
	double fRho; // g/cm^3
	double fZ;
	double fA;
	double fZpos; // z position
};

#endif /*CBMLITMATERIALINFO_H_*/
