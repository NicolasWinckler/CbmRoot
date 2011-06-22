/** LitTrackParam.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Track parameters data class.
 **/

#ifndef LITTRACKPARAM_H_
#define LITTRACKPARAM_H_ 1

#include "LitTypes.h"
#include "LitUtils.h"

namespace lit {
namespace parallel {

template<class T>
class LitTrackParam
{
public:
   LitTrackParam():
      X(0.), Y(0.), Z(0.), Tx(0.), Ty(0.), Qp(0.),
      C0(0.), C1(0.), C2(0.), C3(0.), C4(0.), C5(0.),
      C6(0.), C7(0.), C8(0.), C9(0.), C10(0.), C11(0.),
      C12(0.), C13(0.), C14(0.) {}

   virtual ~LitTrackParam() {}

   /* Returns std::string representation for the class */
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

   /* Operator << for convenient output to std::ostream */
   friend std::ostream& operator<<(std::ostream& strm, const LitTrackParam& par) {
      strm << par.ToString();
      return strm;
   }

public:
   T X, Y, Z, Tx, Ty, Qp;

   T C0,  C1,  C2,  C3,  C4,
     C5,  C6,  C7,  C8,
     C9,  C10, C11,
     C12, C13,
     C14;
} _fvecalignment;

/* Some typedefs for convenience */
typedef LitTrackParam<fvec> LitTrackParamVec;
typedef LitTrackParam<fscal> LitTrackParamScal;

} // namespace parallel
} // namespace lit
#endif /* LITTRACKPARAM_H_ */
