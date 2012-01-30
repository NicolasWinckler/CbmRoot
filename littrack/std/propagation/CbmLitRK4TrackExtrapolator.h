/** CbmLitRK4TrackExtrapolator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 ** The class performs track extrapolation in the magnetic field by solving
 ** the equation of motion of the charged  particle in the magnetic field
 ** with the 4th order Runge-Kutta method. The transport matrix is calculated
 ** by integrating the derivatives together with "zero trajectory"
 ** with the 4th order Runge-Kutta method.
 **/

#ifndef CBMLITRK4TRACKEXTRAPOLATOR_H_
#define CBMLITRK4TRACKEXTRAPOLATOR_H_

#include "interface/CbmLitTrackExtrapolator.h"

#include "data/CbmLitTrackParam.h"

#include <vector>

class CbmLitField;

class CbmLitRK4TrackExtrapolator: public CbmLitTrackExtrapolator
{
public:
   /* Constructor with field
    *@param field Magnetic field.
    */
   CbmLitRK4TrackExtrapolator(
      CbmLitField* field);

   /* Destructor */
   virtual ~CbmLitRK4TrackExtrapolator();

   /* Inherited from CbmLitTrackExtrapolation */
   virtual LitStatus Extrapolate(
      const CbmLitTrackParam* parIn,
      CbmLitTrackParam* parOut,
      myf zOut,
      std::vector<myf>* F);

   /* Inherited from CbmLitTrackExtrapolation */
   virtual LitStatus Extrapolate(
      CbmLitTrackParam* par,
      myf zOut,
      std::vector<myf>* F);

protected:
   /* Calculates output track parameters and derivatives with the 4th order Runge-Kutta method.
    * @param xIn Input track parameters vector (x, y, tx, ty, q/p).
    * @param zIn Input Z position [cm].
    * @param xOut Output track parameters vector (x, y, tx, ty, q/p)
    * @param zOut Input Z position to extrapolate to [cm].
    * @param derivs Vector with integrated derivatives.
    */
   void RK4Order(
      const std::vector<myf>& xIn,
      myf zIn,
      std::vector<myf>& xOut,
      myf zOut,
      std::vector<myf>& derivs) const;

   /* TODO: add comment */
   myf CalcOut(
      myf in,
      const myf k[4]) const;

   /* Transport the covariance matrix, using formula cOut = F*cIn*Ft.
    * @param cIn Input covariance matrix.
    * @param F Input transport matrix.
    * @param cOut Output covariance matrix.
    */
   void TransportC(
      const std::vector<myf>& cIn,
      const std::vector<myf>& F,
      std::vector<myf>& cOut) const;

private:
   CbmLitField* fField; // magnetic field
};

#endif //CBMLITRK4TRACKEXTRAPOLATOR_H_

