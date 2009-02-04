#ifndef CBMLITTYPES_H_
#define CBMLITTYPES_H_

#include "CbmLitFitNode.h"
//#include "CbmLitTrackLight.h"

#include <vector>

class CbmLitHit;
class CbmLitTrack;
class CbmLitTrackBranch;
//class CbmLitPixelHit;

typedef std::vector<CbmLitHit*> HitPtrVector;
typedef std::vector<CbmLitHit*>::iterator HitPtrIterator;
typedef std::pair<HitPtrIterator, HitPtrIterator> HitPtrIteratorPair;

//typedef std::vector<CbmLitPixelHit*> PixelHitPtrVector;
//typedef std::vector<CbmLitPixelHit*>::iterator PixelHitPtrIterator;
//typedef std::pair<PixelHitPtrIterator, PixelHitPtrIterator> PixelHitPtrIteratorPair;

//typedef std::vector<CbmLitTrackLight>::iterator TrackLightIterator;
//typedef std::vector<CbmLitTrackLight> TrackLightVector;

typedef std::vector<CbmLitTrack*>::iterator TrackPtrIterator;
typedef std::vector<CbmLitTrack*> TrackPtrVector;

typedef std::vector<CbmLitTrackBranch*>::iterator TrackBranchPtrIterator;
typedef std::vector<CbmLitTrackBranch*> TrackBranchPtrVector;

typedef std::vector<CbmLitFitNode> FitNodeVector;
typedef std::vector<CbmLitFitNode>::iterator FitNodeIterator;

#endif /*CBMLITTYPES_H_*/
