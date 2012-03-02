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
using std::vector;
using std::pair;
class CbmLitHit;
class CbmLitTrack;

typedef vector<CbmLitHit*> HitPtrVector;
typedef vector<CbmLitHit*>::iterator HitPtrIterator;
typedef pair<HitPtrIterator, HitPtrIterator> HitPtrIteratorPair;

//typedef vector<const CbmLitHit*> HitCnstPtrVector;
//typedef vector<const CbmLitHit*>::iterator HitCnstPtrIterator;
//typedef pair<HitCnstPtrIterator, HitCnstPtrIterator> HitCnstPtrIteratorPair;

typedef vector<CbmLitTrack*>::iterator TrackPtrIterator;
typedef vector<CbmLitTrack*> TrackPtrVector;

typedef vector<CbmLitFitNode> FitNodeVector;
typedef vector<CbmLitFitNode>::iterator FitNodeIterator;

#endif /* CBMLITTYPES_H_ */
