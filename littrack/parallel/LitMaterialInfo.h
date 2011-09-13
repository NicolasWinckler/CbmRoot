/**
 * \file LitMaterialInfo.h
 *
 * \brief Material info data class.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 */
#ifndef LITMATERIALINFO_H_
#define LITMATERIALINFO_H_

#include "LitTypes.h"
#include "LitUtils.h"

namespace lit {
namespace parallel {

/**
 * \class LitMaterialInfo
 *
 * \brief Properties of the material.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 */
template<class T>
class LitMaterialInfo
{
public:
   /**
    * \brief Constructor.
    */
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

   /**
    * \brief Destructor.
    */
   virtual ~LitMaterialInfo() {}

   /**
    * \brief Calculate some material properties to speed up reconstruction.
    */
   void CalculateValues() {
      RadThick = Thickness / X0; // Length/X0
      SqrtRadThick = sqrt(RadThick); // std::sqrt(Length/X0)
      LogRadThick = log(RadThick); // std::log(Length/X0)
      ElLoss = exp(RadThick * log(3.) / log (2.)) - exp(-2. * RadThick);
   }

   /**
    * \brief Returns std::string representation of the class.
    * \return Class representation as std::string.
    */
   std::string ToString() const {
      return "LitMaterialinfo: Thickness=" + lit::parallel::ToString<T>(Thickness)
            + ", X0=" + lit::parallel::ToString<T>(X0)
            + ", Rho=" + lit::parallel::ToString<T>(Rho)
            + ", Z=" + lit::parallel::ToString<T>(Z)
            + ", A=" + lit::parallel::ToString<T>(A)
            + ", Zpos=" + lit::parallel::ToString<T>(Zpos)
            + ", I=" + lit::parallel::ToString<T>(I) + "\n";
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitMaterialInfo& mat) {
      strm << mat.ToString();
      return strm;
   }

public:
   T Thickness; // Thickness of the material [cm]
   T X0; // Radiation length [cm]
   T Rho; // Density [g/cm^3]
   T Z; // Atomic number
   T A; // Atomic mass
   T Zpos; // Z position of the material
   T RadThick; // Length/X0
   T SqrtRadThick; // std::sqrt(Length/X0)
   T LogRadThick; // std::log(Length/X0)
   // approximation of the mean excitation energy in GeV
   // I = (Z > 16)? 10 * Z * 1e-9 : 16 * std::pow(Z, 0.9) * 1e-9;
   T I;
   T ElLoss; // (exp(radThick * log(THREE) / log (TWO)) - exp(-TWO * radThick));
} _fvecalignment;

/**
 * \typedef LitMaterialInfo<fvec> LitMaterialInfoVec
 * \brief Vector version of LitMaterialInfo.
 */
typedef LitMaterialInfo<fvec> LitMaterialInfoVec;

/**
 * \typedef LitMaterialInfo<fscal> LitMaterialInfoScal
 * \brief Scalar version of LitMaterialInfo.
 */
typedef LitMaterialInfo<fscal> LitMaterialInfoScal;

} // namespace parallel
} // namespace lit
#endif /* LITMATERIALINFO_H_ */
