#ifndef LITMATERIALINFO_H_
#define LITMATERIALINFO_H_

#include "LitTypes.h"

class LitMaterialInfo
{
public:
	fvec Thickness; // Thickness of the material [cm]
	fvec X0; // Radiation length [cm]
	fvec Rho; // Density [g/cm^3]
	fvec Z; //
	fvec A; //
	fvec Zpos; // Z position of the material
//	fvec RadThick; // Length/X0
//	fvec SqrtRadThick; // std::sqrt(Length/X0)
//	fvec LogRadThick; // std::log(Length/X0)
	// approximation of the mean excitation energy in GeV
	// I = (Z > 16)? 10 * Z * 1e-9 : 16 * std::pow(Z, 0.9) * 1e-9;
	fvec I;

	friend std::ostream & operator<<(std::ostream &strm, const LitMaterialInfo &mat){
		strm << "LitMaterialinfo: Thickness=" << mat.Thickness << ", X0=" << mat.X0
			<< ", Rho=" << mat.Rho << ", Z=" << mat.Z << ", A=" << mat.A
			<< ", Zpos=" << mat.Zpos << ", I=" << mat.I << std::endl;
		return strm;
	}
} _fvecalignment;

#endif /* LITMATERIALINFO_H_ */
