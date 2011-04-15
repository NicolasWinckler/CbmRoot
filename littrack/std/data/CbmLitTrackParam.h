/** CbmLitTrackParam.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **
 ** Base data class for track parameters.
 **/

#ifndef CBMLITTRACKPARAM_H_
#define CBMLITTRACKPARAM_H_

#include "base/CbmLitFloat.h"

#include <vector>
#include <string>

class CbmLitTrackParam
{
public:
   /* Constructor */
   CbmLitTrackParam();

   /* Destructor */
   virtual ~CbmLitTrackParam();

   /* Returns X position */
   myf GetX() const { return fX; }

   /* Returns Y position */
   myf GetY() const { return fY; }

   /* Returns Z position */
   myf GetZ() const { return fZ; }

   /* Returns Tx slope */
   myf GetTx() const { return fTx; }

   /* Returns Ty slope */
   myf GetTy() const { return fTy; }

   /* Returns Q/p */
   myf GetQp() const { return fQp; }

   /* Returns direction cosines */
   void GetDirCos(myf& nx, myf& ny, myf& nz) const;

   /* Returns state vector as std::vector */
   std::vector<myf> GetStateVector() const;

   /* Returns covariance matrix as std::vector */
   const std::vector<myf>& GetCovMatrix() const { return fCovMatrix; }

   /* Returns covariance specified by index */
   myf GetCovariance(int index) const { return fCovMatrix[index]; }

   /* Sets X position */
   void SetX(myf x) { fX  = x; }

   /* Sets Y position */
   void SetY(myf y) { fY  = y; }

   /* Sets Z position */
   void SetZ(myf z) { fZ  = z; }

   /* Sets Tx slope */
   void SetTx(myf tx) { fTx = tx; }

   /* Sets Ty slope */
   void SetTy(myf ty) { fTy = ty; }

   /* Sets Q/p */
   void SetQp(myf qp) { fQp = qp; }

   /* Sets state vector */
   void SetStateVector(const std::vector<myf>& x);

   /* Sets covariance matrix */
   void SetCovMatrix(const std::vector<myf>& C) {
      fCovMatrix.assign(C.begin(), C.end());
   }

   /* Sets covariance at position index with value cov */
   void SetCovariance(int index, myf cov) { fCovMatrix[index] = cov; }

   /* Returns std::string representation of the class */
   std::string ToString() const;

private:
   /* X, Y, Z coordinates in [cm] */
   myf fX, fY, fZ;
   /* Slopes: tx=dx/dz, ty=dy/dz */
   myf fTx, fTy;
   /* Q/p: Q is a charge (+/-1), p is momentum in [GeV/c] */
   myf fQp;

   /* Covariance matrix
    * Upper triangle symmetric matrix
     * a[0,0..4], a[1,1..4], a[2,2..4], a[3,3..4], a[4,4] */
   std::vector<myf> fCovMatrix;
};

#endif /*CBMLITTRACKPARAM_H_*/
