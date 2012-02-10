/**
 * \file CbmLitTypes.h
 * \author A.Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Typedefs for data structures used in littrack.
 */

#ifndef CBMLITTYPES_H_
#define CBMLITTYPES_H_

#include "data/CbmLitFitNode.h"
#include <vector>
class CbmLitHit;
class CbmLitTrack;

typedef std::vector<CbmLitHit*> HitPtrVector;
typedef std::vector<CbmLitHit*>::iterator HitPtrIterator;
typedef std::pair<HitPtrIterator, HitPtrIterator> HitPtrIteratorPair;
typedef std::vector<CbmLitTrack*>::iterator TrackPtrIterator;
typedef std::vector<CbmLitTrack*> TrackPtrVector;
typedef std::vector<CbmLitFitNode> FitNodeVector;
typedef std::vector<CbmLitFitNode>::iterator FitNodeIterator;

#endif /* CBMLITTYPES_H_ */
