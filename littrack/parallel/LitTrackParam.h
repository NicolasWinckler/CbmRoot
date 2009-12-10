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

template<class T>
class LitTrackParam
{
public:
	T X, Y, Z, Tx, Ty, Qp;

    T
    C0, C1, C2,  C3,  C4,
        C5, C6,  C7,  C8,
            C9, C10, C11,
                C12, C13,
                     C14;

	friend std::ostream & operator<<(std::ostream &strm, const LitTrackParam &par){
		strm << "LitTrackParam: " << "X=" << par.X << " Y=" << par.Y
			<< " Z=" << par.Z << " Tx=" << par.Tx << " Ty=" << par.Ty
			<< " Qp=" << par.Qp << std::endl;
		return strm;
	}
} _fvecalignment;

#endif
