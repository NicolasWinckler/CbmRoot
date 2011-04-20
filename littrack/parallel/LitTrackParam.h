/** LitTrackParam.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Track parameters data class for fast parallel version of the Littrack tracking.
 **/

#ifndef LITTRACKPARAM_H_
#define LITTRACKPARAM_H_ 1

#include "LitTypes.h"

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

   T X, Y, Z, Tx, Ty, Qp;

   T
   C0, C1, C2,  C3,  C4,
     C5, C6,  C7,  C8,
     C9, C10, C11,
     C12, C13,
     C14;

   friend std::ostream& operator<<(std::ostream& strm, const LitTrackParam& par) {
      strm << "LitTrackParam: " << "X=" << par.X << " Y=" << par.Y
           << " Z=" << par.Z << " Tx=" << par.Tx << " Ty=" << par.Ty
           << " Qp=" << par.Qp << std::endl;
      strm << " cov matrix[0-14]: " << par.C0 << " " << par.C1 << " " << par.C2 << " "
           << par.C3 << " " << par.C4 << " " << par.C5 << " " << par.C6 << " " << par.C7 << " "
           << par.C8 << " " << par.C9 << " " << par.C10 << " " << par.C11 << " " << par.C12 << " "
           << par.C13 << " " << par.C14 << std::endl;
      return strm;
   }
} _fvecalignment;

typedef LitTrackParam<fvec> LitTrackParamVec;
typedef LitTrackParam<fscal> LitTrackParamScal;

} // namespace parallel
} // namespace lit
#endif /* LITTRACKPARAM_H_ */
