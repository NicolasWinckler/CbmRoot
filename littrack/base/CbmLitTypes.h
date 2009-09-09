#ifndef CBMLITTYPES_H_
#define CBMLITTYPES_H_

#include "data/CbmLitFitNode.h"
//#include "CbmLitHit.h"
//#include "CbmLitTrack.h"
//#include "CbmLitTrackBranch.h"

#include <vector>

class CbmLitHit;
class CbmLitTrack;
//class CbmLitTrackBranch;

typedef std::vector<CbmLitHit*> HitPtrVector;
typedef std::vector<CbmLitHit*>::iterator HitPtrIterator;
typedef std::pair<HitPtrIterator, HitPtrIterator> HitPtrIteratorPair;

typedef std::vector<CbmLitTrack*>::iterator TrackPtrIterator;
typedef std::vector<CbmLitTrack*> TrackPtrVector;
//typedef std::vector<CbmLitTrackBranch*>::iterator TrackBranchPtrIterator;
//typedef std::vector<CbmLitTrackBranch*> TrackBranchPtrVector;

typedef std::vector<CbmLitFitNode> FitNodeVector;
typedef std::vector<CbmLitFitNode>::iterator FitNodeIterator;

#endif /*CBMLITTYPES_H_*/
