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
   litfloat GetX() const { return fX; }

   /* Returns Y position */
   litfloat GetY() const { return fY; }

   /* Returns Z position */
   litfloat GetZ() const { return fZ; }

   /* Returns Tx slope */
   litfloat GetTx() const { return fTx; }

   /* Returns Ty slope */
   litfloat GetTy() const { return fTy; }

   /* Returns Q/p */
   litfloat GetQp() const { return fQp; }

   /* Returns direction cosines */
   void GetDirCos(litfloat& nx, litfloat& ny, litfloat& nz) const;

   /* Returns state vector as std::vector */
   std::vector<litfloat> GetStateVector() const;

   /* Returns covariance matrix as std::vector */
   const std::vector<litfloat>& GetCovMatrix() const { return fCovMatrix; }

   /* Returns covariance specified by index */
   litfloat GetCovariance(int index) const { return fCovMatrix[index]; }

   /* Sets X position */
   void SetX(litfloat x) { fX  = x; }

   /* Sets Y position */
   void SetY(litfloat y) { fY  = y; }

   /* Sets Z position */
   void SetZ(litfloat z) { fZ  = z; }

   /* Sets Tx slope */
   void SetTx(litfloat tx) { fTx = tx; }

   /* Sets Ty slope */
   void SetTy(litfloat ty) { fTy = ty; }

   /* Sets Q/p */
   void SetQp(litfloat qp) { fQp = qp; }

   /* Sets state vector */
   void SetStateVector(const std::vector<litfloat>& x);

   /* Sets covariance matrix */
   void SetCovMatrix(const std::vector<litfloat>& C) {
      fCovMatrix.assign(C.begin(), C.end());
   }

   /* Sets covariance at position index with value cov */
   void SetCovariance(int index, litfloat cov) { fCovMatrix[index] = cov; }

   /* Returns std::string representation of the class */
   std::string ToString() const;

private:
   /* X, Y, Z coordinates in [cm] */
   litfloat fX, fY, fZ;
   /* Slopes: tx=dx/dz, ty=dy/dz */
   litfloat fTx, fTy;
   /* Q/p: Q is a charge (+/-1), p is momentum in [GeV/c] */
   litfloat fQp;

   /* Covariance matrix
    * Upper triangle symmetric matrix
     * a[0,0..4], a[1,1..4], a[2,2..4], a[3,3..4], a[4,4] */
   std::vector<litfloat> fCovMatrix;
};

#endif /*CBMLITTRACKPARAM_H_*/
