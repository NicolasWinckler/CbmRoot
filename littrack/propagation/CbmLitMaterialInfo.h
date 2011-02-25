/** CbmLitMaterialInfo.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **
 ** Data class to store information about detector materials.
 **/

#ifndef CBMLITMATERIALINFO_H_
#define CBMLITMATERIALINFO_H_

#include "base/CbmLitFloat.h"

#include <string>
#include <sstream>

class CbmLitMaterialInfo
{
public:
	/* Constructor */
	CbmLitMaterialInfo():
		fLength(0.),
		fRL(0.),
		fRho(0.),
		fZ(0.),
		fA(0.),
		fZpos(0.) {}

	/* Constructor with assignment
	 * @param length Length of the material [cm]
	 * @param rl Radiation length [cm]
	 * @param rho Density [g/cm^3]
	 * @param Z
	 * @param A
	 * @param zpos Z position of the material
	 */
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
		fZpos(zpos){}

	/* Destructor */
	virtual ~CbmLitMaterialInfo() {};

	/*@return Length of the material */
	myf GetLength() const { return fLength;}

	/*@return Radiation length */
	myf GetRL() const { return fRL;}

	/*@return Density */
	myf GetRho() const { return fRho;}

	/*@return */
	myf GetZ() const { return fZ;}

	/*@return  */
	myf GetA() const { return fA;}

	/*@return Z position of the material */
	myf GetZpos() const { return fZpos;}

	/* Sets length of the material */
	void SetLength(myf length) {fLength = length;}

	/* Sets radiation length of the material */
	void SetRL(myf rl) {fRL = rl;}

	/* Sets density */
	void SetRho(myf rho) {fRho = rho;}

	/* Sets Z */
	void SetZ(myf Z) {fZ = Z;}

	/* Sets A */
	void SetA(myf A) {fA = A;}

	/* Sets Z position of the material */
	void SetZpos(myf zpos) {fZpos = zpos;}

	/* @return String representation of the class */
	virtual std::string ToString() const {
		std::stringstream ss;
		ss << "MaterialInfo: length=" << fLength << " rl=" << fRL
			<< " rho=" << fRho << " Z=" << fZ << " A=" << fA << " zpos=" << fZpos
			<< std::endl;
		return ss.str();
	}

private:
	myf fLength; // Length of the material [cm]
	myf fRL; // Radiation length [cm]
	myf fRho; // Density [g/cm^3]
	myf fZ; //
	myf fA; //
	myf fZpos; // Z position of the material
};

#endif /*CBMLITMATERIALINFO_H_*/
