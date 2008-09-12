#ifndef CBMLITMATH_H_
#define CBMLITMATH_H_

#include "CbmLitTrackParam.h"
class CbmLitHit;
//class CbmLitTrackParam;

Double_t ChiSq(
		const CbmLitTrackParam* par, 						
		const CbmLitHit* hit);

Int_t NDF(
		Int_t nofHits);

#endif /*CBMLITMATH_H_*/
