/** LitMaterialInfo.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Material info data class.
 **/

#ifndef LITMATERIALINFO_H_
#define LITMATERIALINFO_H_

#include "LitTypes.h"
#include "LitUtils.h"

namespace lit {
namespace parallel {

template<class T>
class LitMaterialInfo
{
public:
   LitMaterialInfo():
      Thickness(0.),
      X0(0.),
      Rho(0.),
      Z(0.),
      A(0.),
      Zpos(0.),
      RadThick(0.),
      SqrtRadThick(0.),
      LogRadThick(0.),
      I(0.),
      ElLoss(0.) {}

   virtual ~LitMaterialInfo() {}

   T Thickness; // Thickness of the material [cm]
   T X0; // Radiation length [cm]
   T Rho; // Density [g/cm^3]
   T Z; //
   T A; //
   T Zpos; // Z position of the material
   T RadThick; // Length/X0
   T SqrtRadThick; // std::sqrt(Length/X0)
   T LogRadThick; // std::log(Length/X0)
   // approximation of the mean excitation energy in GeV
   // I = (Z > 16)? 10 * Z * 1e-9 : 16 * std::pow(Z, 0.9) * 1e-9;
   T I;
   T ElLoss; // (exp(radThick * log(THREE) / log (TWO)) - exp(-TWO * radThick));

   friend std::ostream& operator<<(std::ostream& strm, const LitMaterialInfo& mat) {
      strm << "LitMaterialinfo: Thickness=" << mat.Thickness << ", X0=" << mat.X0
           << ", Rho=" << mat.Rho << ", Z=" << mat.Z << ", A=" << mat.A
           << ", Zpos=" << mat.Zpos << ", I=" << mat.I << std::endl;
      return strm;
   }

   std::string ToStringShort() {
      std::string str = ToString<T>(Thickness) + " " + ToString<T>(X0)
            + " " + ToString<T>(Rho) + " " + ToString<T>(Z) + " "
            + ToString<T>(A) + " " + ToString<T>(Zpos)
            + " " + ToString<T>(I) + "\n";
      return str;
   }
} _fvecalignment;

typedef LitMaterialInfo<fvec> LitMaterialInfoVec;
typedef LitMaterialInfo<fscal> LitMaterialInfoScal;

} // namespace parallel
} // namespace lit
#endif /* LITMATERIALINFO_H_ */
