#ifndef L1TrackPar_h
#define L1TrackPar_h 1

#include "L1Types.h"

class L1TrackPar{

 public:

  fvec
    x,y,tx,ty,qp,z,
    C00, 
    C10, C11, 
    C20, C21, C22, 
    C30, C31, C32, C33, 
    C40, C41, C42, C43, C44,
    chi2, NDF                ;

} _fvecalignment;


#endif
