#ifndef CBMLITMATH_H_
#define CBMLITMATH_H_

#include "CbmLitTrackParam.h"
class CbmLitHit;
class CbmLitStripHit;
class CbmLitPixelHit;
//class CbmLitTrackParam;

Double_t ChiSq(
		const CbmLitTrackParam* par, 						
		const CbmLitHit* hit);

Double_t ChiSq(
		const CbmLitTrackParam* par, 						
		const CbmLitStripHit* hit);

Double_t ChiSq(
		const CbmLitTrackParam* par, 						
		const CbmLitPixelHit* hit);

Int_t NDF(
		Int_t nofHits);

#endif /*CBMLITMATH_H_*/
