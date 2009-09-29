#ifndef LITTYPES_H_
#define LITTYPES_H_

#ifdef HAVE_SSE
  #include "../L1/L1Algo/vectors/P4_F32vec4.h"
#else
  #include "../L1/L1Algo/vectors/PSEUDO_F32vec4.h"
#endif

//#include "vectors/PSEUDO_F64vec1.h"

class LitScalPixelHit;
class LitTrack;
#include <vector>
typedef std::vector<LitScalPixelHit*> ScalPixelHitVector;
typedef std::vector<LitScalPixelHit*>::iterator ScalPixelHitIterator;
typedef std::vector<LitTrack*> TrackVector;
typedef std::vector<LitTrack*>::iterator TrackIterator;

#endif /* LITTYPES_H_ */
