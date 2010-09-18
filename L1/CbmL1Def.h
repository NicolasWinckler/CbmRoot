#ifndef CbmL1Def_h
#define CbmL1Def_h 1

#ifdef HAVE_SSE
  #include "vectors/P4_F32vec4.h"
#else
  #include "vectors/PSEUDO_F32vec4.h"
  #error NoSseFound
#endif // HAVE_SSE

//#include "vectors/PSEUDO_F64vec1.h"

#if 1 && defined( __GNUC__ ) && __GNUC__ - 0 >= 3   // for speed up conditions
#define ISLIKELY(  x )    __builtin_expect( !!( x ),1 )
#define ISUNLIKELY(  x )  __builtin_expect( !!( x ),0 )
#else
#define ISLIKELY(  x )   (  x )
#define ISUNLIKELY(  x )  (  x )
#endif

#ifdef NDEBUG
#define ASSERT(v, msg)
#else
#define ASSERT(v, msg) \
if (v) {} else { \
  std::cerr << __FILE__ << ":" << __LINE__ << " assertion failed: " \
            << #v << " = " << (v) << "\n" << msg << std::endl; \
  abort(); \
}
#endif



#endif // CbmL1Def_h
