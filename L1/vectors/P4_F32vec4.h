#ifndef L1Algo_F32vec4P4_H
#define L1Algo_F32vec4P4_H

#include <iostream>
#include <cmath>
#include "xmmintrin.h"
#include "vec_arithmetic.h"

/**********************************
 *
 *   Vector of four single floats
 *
 **********************************/

//#pragma pack(push,16)/* Must ensure class & union 16-B aligned */

//typedef __m128 VectorFloat __attribute__ ((aligned(16)));

const union
{
  float f;
  int i;
} __f_one = {1.f};

const union
{
    int i[4];
    __m128 m;
} 
  __f32vec4_abs_mask_cheat = {{0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff}},
  __f32vec4_sgn_mask_cheat = {{0x80000000, 0x80000000, 0x80000000, 0x80000000}},
  __f32vec4_zero_cheat     = {{         0,          0,          0,          0}},
  __f32vec4_one_cheat      = {{__f_one.i , __f_one.i , __f_one.i , __f_one.i }},
  __f32vec4_true_cheat     = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}},
  __f32vec4_false_cheat    = {{0x00000000, 0x00000000, 0x00000000, 0x00000000}};

#define _f32vec4_abs_mask (static_cast<F32vec4>(__f32vec4_abs_mask_cheat.m))
#define _f32vec4_sgn_mask (static_cast<F32vec4>(__f32vec4_sgn_mask_cheat.m))
#define _f32vec4_zero     (static_cast<F32vec4>(__f32vec4_zero_cheat.m))
#define _f32vec4_one      (static_cast<F32vec4>(__f32vec4_one_cheat.m))
#define _f32vec4_true     (static_cast<F32vec4>(__f32vec4_true_cheat.m))
#define _f32vec4_false    (static_cast<F32vec4>(__f32vec4_false_cheat.m))

class F32vec4 
{
 public:

  __m128 v;

  float & operator[]( int i ){ return (reinterpret_cast<float*>(&v))[i]; }
  float   operator[]( int i ) const { return (reinterpret_cast<const float*>(&v))[i]; }

  F32vec4( ):v(_mm_set_ps1(0)){}
  F32vec4( const __m128 &a ):v(a) {}
  F32vec4( const float &a ):v(_mm_set_ps1(a)) {}

  F32vec4( const float &f0, const float &f1, const float &f2, const float &f3 ):v(_mm_set_ps(f3,f2,f1,f0)) {}

  /* Conversion function */
  operator  __m128() const { return v; }		/* Convert to __m128 */

  /* Arithmetic Operators */
  friend F32vec4 operator +(const F32vec4 &a, const F32vec4 &b) { return _mm_add_ps(a,b); }
  friend F32vec4 operator -(const F32vec4 &a, const F32vec4 &b) { return _mm_sub_ps(a,b); } 
  friend F32vec4 operator *(const F32vec4 &a, const F32vec4 &b) { return _mm_mul_ps(a,b); } 
  friend F32vec4 operator /(const F32vec4 &a, const F32vec4 &b) { return _mm_div_ps(a,b); }

  /* Functions */
  friend F32vec4 min( const F32vec4 &a, const F32vec4 &b ){ return _mm_min_ps(a, b); }
  friend F32vec4 max( const F32vec4 &a, const F32vec4 &b ){ return _mm_max_ps(a, b); }

  /* Square Root */
  friend F32vec4 sqrt ( const F32vec4 &a ){ return _mm_sqrt_ps (a); }

  /* Reciprocal( inverse) Square Root */
  friend F32vec4 rsqrt( const F32vec4 &a ){ return _mm_rsqrt_ps(a); }

  /* Reciprocal (inversion) */
  friend F32vec4 rcp  ( const F32vec4 &a ){ return _mm_rcp_ps  (a); }
  //friend F32vec4 rcp  ( const F32vec4 &a ){ return 1./a; }

  /* Absolute value */
  friend F32vec4 fabs(const F32vec4 &a){ return _mm_and_ps(a, _f32vec4_abs_mask); }

  /* Sign */
  friend F32vec4 sgn(const F32vec4 &a){ return _mm_or_ps(_mm_and_ps(a, _f32vec4_sgn_mask),_f32vec4_one); }
  friend F32vec4 asgnb(const F32vec4 &a, const F32vec4 &b ){ 
    return _mm_or_ps(_mm_and_ps(b, _f32vec4_sgn_mask),a); 
  }

  /* Logical */
 
  friend F32vec4 operator&( const F32vec4 &a, const F32vec4 &b ){ // mask returned
    return _mm_and_ps(a, b);
  }
  friend F32vec4 operator|( const F32vec4 &a, const F32vec4 &b ){ // mask returned
    return _mm_or_ps(a, b);
  }
  friend F32vec4 operator^( const F32vec4 &a, const F32vec4 &b ){ // mask returned
    return _mm_xor_ps(a, b);
  }
  friend F32vec4 operator!( const F32vec4 &a ){ // mask returned
    return _mm_xor_ps(a, _f32vec4_true);
  }
  // friend F32vec4 operator||( const F32vec4 &a, const F32vec4 &b ){ // mask returned
  //   return _mm_or_ps(a, b);
  // }

  /* Comparison */

  friend F32vec4 operator<( const F32vec4 &a, const F32vec4 &b ){ // mask returned
    return _mm_cmplt_ps(a, b);
  }
  friend F32vec4 operator<=( const F32vec4 &a, const F32vec4 &b ){ // mask returned
    return _mm_cmple_ps(a, b);
  }
  friend F32vec4 operator>( const F32vec4 &a, const F32vec4 &b ){ // mask returned
    return _mm_cmpgt_ps(a, b);
  }
  friend F32vec4 operator>=( const F32vec4 &a, const F32vec4 &b ){ // mask returned
    return _mm_cmpge_ps(a, b);
  }
  

  #define if3(a, b, c)   ((a)&(b)) | ((!(a))&(c))    // analog (a) ? b : c

  #define NotEmpty(a)   bool((a)[0])|bool((a)[1])|bool((a)[2])|bool((a)[3])
  #define    Empty(a) !(bool((a)[0])|bool((a)[1])|bool((a)[2])|bool((a)[3]))
  // bool NotEmpty(const F32vec4 &a) { return a[0]||a[1]||a[2]||a[3]; }
  // bool    Empty(const F32vec4 &a) { return !(a[0]||a[1]||a[2]||a[3]); } // optimize
  friend F32vec4 bool2int( const F32vec4 &a){ // mask returned
    return if3(a,1,0);
  }
  
  /* Non intrinsic functions */

#define _f1(A,F) F32vec4( F(A[0]), F(A[1]), F(A[2]), F(A[3]) ) 

  friend F32vec4 exp( const F32vec4 &a ){ return _f1( a, exp ); } 
  friend F32vec4 log( const F32vec4 &a ){ return _f1( a, log ); } 
  friend F32vec4 sin( const F32vec4 &a ){ return _f1( a, sin ); } 
  friend F32vec4 cos( const F32vec4 &a ){ return _f1( a, cos ); } 

#undef _f1

  /* Define all operators for consistensy */
  
  vec_arithmetic(F32vec4,float);

  friend std::ostream & operator<<(std::ostream &strm, const F32vec4 &a ){
    strm<<"["<<a[0]<<" "<<a[1]<<" "<<a[2]<<" "<<a[3]<<"]";
    return strm;
  }

  friend std::istream & operator>>(std::istream &strm, F32vec4 &a ){
    float tmp;
    strm>>tmp;
    a = tmp;
    return strm;
  }

} __attribute__ ((aligned(16)));


typedef F32vec4 fvec;
typedef float  fscal;
const int fvecLen = 4;
//#define fvec_true  _f32vec4_true
//#define fvec_false _f32vec4_false
#define _fvecalignment  __attribute__ ((aligned(16)))


#include "std_alloc.h"


#endif 
