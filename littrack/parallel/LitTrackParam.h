#ifndef LITTRACKPARAM_H_
#define LITTRACKPARAM_H_ 1

#include "LitTypes.h"

class LitTrackParam
{
public:
	fvec X, Y, Z, Tx, Ty, Qp;

    fvec
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


class LitScalTrackParam
{
public:
	fscal X, Y, Z, Tx, Ty, Qp;

    fscal
    C0, C1, C2,  C3,  C4,
        C5, C6,  C7,  C8,
            C9, C10, C11,
                C12, C13,
                     C14;

	friend std::ostream & operator<<(std::ostream &strm, const LitScalTrackParam &par){
		strm << "LitTrackParam: " << "X=" << par.X << " Y=" << par.Y
			<< " Z=" << par.Z << " Tx=" << par.Tx << " Ty=" << par.Ty
			<< " Qp=" << par.Qp << std::endl;
		return strm;
	}
};

#endif
