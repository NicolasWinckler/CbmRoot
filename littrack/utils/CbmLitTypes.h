
#ifndef CBMLITTYPES_H_
#define CBMLITTYPES_H_

#include <vector>

class CbmLitHit;
class CbmLitTrack;

typedef std::vector<CbmLitHit*> HitVector;
typedef std::vector<CbmLitHit*>::iterator HitIterator;
typedef std::vector<CbmLitTrack*>::iterator TrackIterator;
typedef std::vector<CbmLitTrack*> TrackVector;
typedef std::pair<HitIterator, HitIterator> HitIteratorPair;

#endif /*CBMLITTYPES_H_*/
