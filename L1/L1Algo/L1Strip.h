#ifndef _L1Strip_h_
#define _L1Strip_h_


struct L1Strip{
  fscal f;
  unsigned short int n; // number of event

  L1Strip():f(0),n(0){}
  L1Strip(fscal _f, unsigned short int _n = 0):f(_f),n(_n){}

  operator fscal() { return f;}
//   operator fvec() { return fscal(f);}
};

typedef unsigned /*short*/ int TStripI; // strip index type

#endif
