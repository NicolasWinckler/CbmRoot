#ifndef L1Types_h
#define L1Types_h 1

#ifdef HAVE_SSE
  #include "vectors/P4_F32vec4.h"
#else
  #include "vectors/PSEUDO_F32vec4.h"
  #error NoSseFound
#endif

//#include "vectors/PSEUDO_F64vec1.h"

#endif
