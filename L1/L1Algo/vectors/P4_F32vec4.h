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
} __f_one = {(float)1.};

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

#define _f32vec4_abs_mask ((F32vec4)__f32vec4_abs_mask_cheat.m)
#define _f32vec4_sgn_mask ((F32vec4)__f32vec4_sgn_mask_cheat.m)
#define _f32vec4_zero     ((F32vec4)__f32vec4_zero_cheat.m)
#define _f32vec4_one      ((F32vec4)__f32vec4_one_cheat.m)
#define _f32vec4_true     ((F32vec4)__f32vec4_true_cheat.m)
#define _f32vec4_false    ((F32vec4)__f32vec4_false_cheat.m)

class F32vec4 
{
 public:

  __m128 v;

  float & operator[]( int i ){ return ((float*)&v)[i]; }
  float   operator[]( int i ) const { return ((float*)&v)[i]; }

  F32vec4( ){}
  F32vec4( const __m128 &a ) { v = a; }
  F32vec4( const float &a )  { v = _mm_set_ps1(a); }

  F32vec4( const float &f0, const float &f1, const float &f2, const float &f3 ){ 
    v = _mm_set_ps(f3,f2,f1,f0); 
  }

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
  friend F32vec4 operator||( const F32vec4 &a, const F32vec4 &b ){ // mask returned
    return _mm_or_ps(a, b);
  }

  /* Comparison */

  friend F32vec4 operator<( const F32vec4 &a, const F32vec4 &b ){ // mask returned
    return _mm_cmplt_ps(a, b);
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
    strm<<a[0]<<" "<<a[1]<<" "<<a[2]<<" "<<a[3];
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



  // ---------------------- Allocator for using STL ------------------------

#include <vector>
#include <limits>

             namespace nsL1
{

// #define DEBUG_nsL1
  
  template <class T>
      class SimdAlloc {
        public:
        // type definitions
          typedef T        value_type;
          typedef T*       pointer;
          typedef const T* const_pointer;
          typedef T&       reference;
          typedef const T& const_reference;
          typedef std::size_t    size_type;
          typedef std::ptrdiff_t difference_type;

        // rebind allocator to type U
          template <class U>
              struct rebind {
            typedef SimdAlloc<U> other;
              };

          // return address of values
              pointer address (reference value) const {
                return &value;
              }
              const_pointer address (const_reference value) const {
                return &value;
              }

        /* constructors and destructor
              * - nothing to do because the allocator has no state
        */
              SimdAlloc() throw() {
              }
              SimdAlloc(const SimdAlloc&) throw() {
              }
              template <class U>
                  SimdAlloc (const SimdAlloc<U>&) throw() {
                  }
                  ~SimdAlloc() throw() {
                  }

              // return maximum number of elements that can be allocated
                  size_type max_size () const throw() {
                    return std::numeric_limits<std::size_t>::max() / sizeof(T);
                  }

              // allocate but don't initialize num elements of type T
                  pointer allocate (size_type num, const void* = 0) {
//               print message and allocate memory with global new
#ifdef DEBUG_nsL1
                    std::cerr << "Allocator: allocate " << num << " element(s)"
                        << " of size " << sizeof(T) << std::endl;
#endif // DEBUG_nsL1
                    pointer ret = reinterpret_cast<pointer>( /*T::*/operator new(num*sizeof(T)) );
#ifdef DEBUG_nsL1
                    std::cerr << " allocated at: " << (void*)ret << std::endl;
#endif // DEBUG_nsL1
                    return ret;
                  }

                // initialize elements of allocated storage p with value value
                  void construct (pointer p, const T& value) {
                 // initialize memory with placement new
#ifdef DEBUG_nsL1
                    std::cerr << "Allocator: construct " << p /*<< " " << value*/ << std::endl;
#endif // DEBUG_nsL1
                    new(p) T(value);
//                     p = reinterpret_cast<pointer>( operator new(sizeof(T), p) );
//                     *p = value;
#ifdef DEBUG_nsL1
                    std::cerr << "done." << std::endl;
#endif // DEBUG_nsL1
                  }

                // destroy elements of initialized storage p
                  void destroy (pointer p) {
                 // destroy objects by calling their destructor
#ifdef DEBUG_nsL1
                    std::cerr << "Allocator: destroy " << p << std::endl;
#endif // DEBUG_nsL1
                    p->~T();
#ifdef DEBUG_nsL1
                    std::cerr << "done." << std::endl;
#endif // DEBUG_nsL1
                  }

               // deallocate storage p of deleted elements
                  void deallocate (pointer p, size_type num) {
                  // print message and deallocate memory with global delete
#ifdef DEBUG_nsL1
                    std::cerr << "Allocator: deallocate " << num << " element(s)"
                        << " of size " << sizeof(T)
                        << " at: " << (void*)p << std::endl;
#endif // DEBUG_nsL1
                    /*T::*/operator delete((void*)p, num*sizeof(T));
#ifdef DEBUG_nsL1
                           std::cerr << "done." << std::endl;
#endif // DEBUG_nsL1
                  }


                  void *operator new(size_t size, void *ptr) { return ::operator new(size, ptr);}
                  void *operator new[](size_t size, void *ptr) { return ::operator new(size, ptr);}
                  void *operator new(size_t size) { return _mm_malloc(size, 16); }
                  void *operator new[](size_t size) { return _mm_malloc(size, 16); }
                  void operator delete(void *ptr, size_t) { _mm_free(ptr); }
                  void operator delete[](void *ptr, size_t) { _mm_free(ptr); }
      }; // SimdAlloc

      // return that all specializations of this allocator are interchangeable
      template <class T1, class T2>
          bool operator== (const SimdAlloc<T1>&, const SimdAlloc<T2>&) throw()
          {
            return true;
          };
      template <class T1, class T2>
          bool operator!= (const SimdAlloc<T1>&, const SimdAlloc<T2>&) throw()
          {
            return false;
          };

      template<typename T>
      struct vector
      {
        typedef std::vector<T> TStd;
//         typedef std::vector<T > TSimd;
        typedef std::vector<T, SimdAlloc<T> > TSimd;
      };

      typedef nsL1::vector<fvec>::TSimd vector_fvec;
}; // namespace nsL1


#endif 
