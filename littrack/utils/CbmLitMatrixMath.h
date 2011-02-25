#ifndef CBMLITMATRIXMATH_H_
#define CBMLITMATRIXMATH_H_

#include "base/CbmLitFloat.h"

#include <vector>

//Matrix operations

/* 5x5 symmetric matrix inverse */
bool InvSym15(
		std::vector<myf>& a);

/* Multiplication of two 5x5 matrices */
bool Mult25(
		const std::vector<myf>& a,
		const std::vector<myf>& b,
		std::vector<myf>& c);

/* Multiplication of 5x5 matrix and 5x1 vector*/
bool Mult25On5(
		const std::vector<myf>& a,
		const std::vector<myf>& b,
		std::vector<myf>& c);

/* Transpose of 5x5 matrix */
bool Transpose25(
		std::vector<myf>& a);

/* Subtraction of two matrices*/
bool Subtract(
		const std::vector<myf>& a,
		const std::vector<myf>& b,
		std::vector<myf>& c);

/* Addition of two matrices*/
bool Add(
		const std::vector<myf>& a,
		const std::vector<myf>& b,
		std::vector<myf>& c);

/* a*b*a^T */
bool Similarity(
		const std::vector<myf>& a,
		const std::vector<myf>& b,
		std::vector<myf>& c);

/* Multiplication of two 5x5 matrices */
bool Mult15On25(
		const std::vector<myf>& a,
		const std::vector<myf>& b,
		std::vector<myf>& c);

/* Multiplication of two 5x5 matrices */
bool Mult25On15(
		const std::vector<myf>& a,
		const std::vector<myf>& b,
		std::vector<myf>& c);

#endif

