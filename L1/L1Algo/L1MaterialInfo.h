#ifndef L1MaterialInfo_h
#define L1MaterialInfo_h

#include "L1Types.h"

class L1MaterialInfo{
 public:
   L1MaterialInfo(): thick(0), RL(0),  RadThick(0), logRadThick(0){};
   
  fvec thick, RL,  RadThick, logRadThick;
} _fvecalignment;


#endif
