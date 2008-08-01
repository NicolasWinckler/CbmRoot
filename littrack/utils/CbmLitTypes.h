#ifndef CBMLITTYPES_H_
#define CBMLITTYPES_H_

#include "CbmLitFitNode.h"

#include <vector>

class CbmLitHit;
class CbmLitTrack;


typedef std::vector<CbmLitHit*> HitVector;
typedef std::vector<CbmLitHit*>::iterator HitIterator;
typedef std::vector<CbmLitTrack*>::iterator TrackIterator;
typedef std::vector<CbmLitTrack*> TrackVector;
typedef std::pair<HitIterator, HitIterator> HitIteratorPair;
typedef std::vector<CbmLitFitNode> FitNodeVector;
typedef std::vector<CbmLitFitNode>::iterator FitNodeIterator;

#endif /*CBMLITTYPES_H_*/
