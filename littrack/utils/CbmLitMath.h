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



////Matrix operations
//
///* 5x5 matrix inverse */
//bool Inv25(
//		std::vector<myf>& a);
//
///* 5x5 symmetric matrix inverse */
//bool InvSym15(
//		std::vector<myf>& a);
//
///* Multiplication of two 5x5 matrices */
//bool Mult25(
//		const std::vector<myf>& a,
//		const std::vector<myf>& b,
//		std::vector<myf>& c);
//
///* Multiplication of 5x5 matrix and 5x1 vector*/
//bool Mult25On5(
//		const std::vector<myf>& a,
//		const std::vector<myf>& b,
//		std::vector<myf>& c);
//
///* Transpose of 5x5 matrix */
//bool Transpose25(
//		std::vector<myf>& a);
//
///* Subtraction of two matrices*/
//bool Subtract(
//		const std::vector<myf>& a,
//		const std::vector<myf>& b,
//		std::vector<myf>& c);
//
///* Addition of two matrices*/
//bool Add(
//		const std::vector<myf>& a,
//		const std::vector<myf>& b,
//		std::vector<myf>& c);
//
///* a*b*a^T */
//bool Similarity(
//		const std::vector<myf>& a,
//		const std::vector<myf>& b,
//		std::vector<myf>& c);
//
///* Multiplication of two 5x5 matrices */
//bool Mult15On25(
//		const std::vector<myf>& a,
//		const std::vector<myf>& b,
//		std::vector<myf>& c);
//
///* Multiplication of two 5x5 matrices */
//bool Mult25On15(
//		const std::vector<myf>& a,
//		const std::vector<myf>& b,
//		std::vector<myf>& c);

#endif /*CBMLITMATH_H_*/
