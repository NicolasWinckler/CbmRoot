#ifndef CBMLITMATH_H_
#define CBMLITMATH_H_

#include "CbmLitTrackParam.h"
class CbmLitHit;
class CbmLitStripHit;
class CbmLitPixelHit;
class CbmLitTrack;
//class CbmLitTrackParam;

myf ChiSq(
		const CbmLitTrackParam* par,
		const CbmLitHit* hit);

myf ChiSq(
		const CbmLitTrackParam* par,
		const CbmLitStripHit* hit);

myf ChiSq(
		const CbmLitTrackParam* par,
		const CbmLitPixelHit* hit);

int NDF(
		const CbmLitTrack* track);

#endif /*CBMLITMATH_H_*/
