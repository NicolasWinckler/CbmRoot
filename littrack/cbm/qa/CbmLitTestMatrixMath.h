#ifndef CBMLITTESTMATRIXMATH_H_
#define CBMLITTESTMATRIXMATH_H_

#include "base/CbmLitFloat.h"

#include <string>
#include <vector>


class CbmLitTestMatrixMath
{
public:
   CbmLitTestMatrixMath();
   virtual ~CbmLitTestMatrixMath();

   void RunTest();

private:
   void TestInvSym15(
      int testId);

   void TestMult15On5(
      int testId);

   void Convert15To25(
      const myf* a15,
      myf* a25);

   void Convert25To15(
      const myf* a25,
      myf* a15);

   std::string VectorToString(
      const std::vector<myf>& a);

   std::string ArrayToString(
      const myf* a, int n);

   myf fEpsilon;
};

#endif /* CBMLITTESTMATRIXMATH_H_ */
