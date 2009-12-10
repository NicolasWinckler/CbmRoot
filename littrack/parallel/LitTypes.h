#ifndef LITTYPES_H_
#define LITTYPES_H_

#ifdef HAVE_SSE
  #include "../L1/L1Algo/vectors/P4_F32vec4.h"
#else
  #include "../L1/L1Algo/vectors/PSEUDO_F32vec1.h"
#endif

//#include "vectors/PSEUDO_F64vec1.h"

#endif /* LITTYPES_H_ */
