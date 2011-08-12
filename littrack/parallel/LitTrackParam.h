/**
 * \file LitTrackParam.h
 *
 * \brief Track parameters data class.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 */
#ifndef LITTRACKPARAM_H_
#define LITTRACKPARAM_H_ 1

#include "LitTypes.h"
#include "LitUtils.h"

namespace lit {
namespace parallel {

/**
 * \class LitTrackParam
 *
 * \brief Track parameters data class.
 *
 * Track parameters consist of
 * X and Y position of the track,
 * tx and ty slopes,
 * q/p charge (+1 or -1) over momentum and
 * covariance matrix of this elements.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 */
template<class T>
class LitTrackParam
{
public:
   /**
    * \brief Constructor.
    */
   LitTrackParam():
      X(0.), Y(0.), Z(0.), Tx(0.), Ty(0.), Qp(0.),
      C0(0.), C1(0.), C2(0.), C3(0.), C4(0.), C5(0.),
      C6(0.), C7(0.), C8(0.), C9(0.), C10(0.), C11(0.),
      C12(0.), C13(0.), C14(0.) {}

   /**
    * \brief Destructor.
    */
   virtual ~LitTrackParam() {}

   /**
    * \brief Returns std::string representation of the class.
    * \return Class representation as std::string.
    */
   std::string ToString() const {
      return "LitTrackParam: X=" + lit::parallel::ToString<T>(X)
         + " Y=" + lit::parallel::ToString<T>(Y)
         + " Z=" + lit::parallel::ToString<T>(Z)
         + " Tx=" + lit::parallel::ToString<T>(Tx)
         + " Ty=" + lit::parallel::ToString<T>(Ty)
         + " Qp=" + lit::parallel::ToString<T>(Qp) + "\n"
         + " cov matrix[0-14]: " + lit::parallel::ToString<T>(C0) + " "
         + lit::parallel::ToString<T>(C1) + " " + lit::parallel::ToString<T>(C2) + " "
         + lit::parallel::ToString<T>(C3) + " " + lit::parallel::ToString<T>(C4) + " "
         + lit::parallel::ToString<T>(C5) + " " + lit::parallel::ToString<T>(C6) + " "
         + lit::parallel::ToString<T>(C7) + " " + lit::parallel::ToString<T>(C8) + " "
         + lit::parallel::ToString<T>(C9) + " " + lit::parallel::ToString<T>(C10) + " "
         + lit::parallel::ToString<T>(C11) + " " + lit::parallel::ToString<T>(C12) + " "
         + lit::parallel::ToString<T>(C13) + " " + lit::parallel::ToString<T>(C14) + "\n";
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitTrackParam& par) {
      strm << par.ToString();
      return strm;
   }

public:
   T X; // X position of the track
   T Y; // Y position of the track
   T Z; // Z position of the track
   T Tx; // Slope which equals to dx/dz
   T Ty; // Slope which equals to dy/dz
   T Qp; // Charge (+1 or -1) over momentum

   // Symetric covariance matrix
   T C0,  C1,  C2,  C3,  C4,
     C5,  C6,  C7,  C8,
     C9,  C10, C11,
     C12, C13,
     C14;
} _fvecalignment;

/**
 * \typedef LitTrackParam<fvec> LitTrackParamVec
 * \brief Vector version of \c LitTrackParam.
 */
typedef LitTrackParam<fvec> LitTrackParamVec;

/**
 * \typedef LitTrackParam<fscal> LitTrackParamScal
 * \brief Scalar version of \c LitTrackParam.
 */
typedef LitTrackParam<fscal> LitTrackParamScal;

} // namespace parallel
} // namespace lit
#endif /* LITTRACKPARAM_H_ */
