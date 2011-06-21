/** CbmLitFieldFitter.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Class performs approximation of magnetic field using
 ** 0-9 degree polynom. MINUIT is used for minimization.
 ** Also my LSM fit is implemented.
 **/

#ifndef CBMLITFIELDFITTER_H_
#define CBMLITFIELDFITTER_H_

#include <vector>
#include "../../parallel/LitField.h"
class FairField;

/* Class represents polynom. */
class CbmLitPolynom
{
public:
   /* Returns calculated value. */
   virtual double Calculate(
      double x,
      double y,
      double c[]) const = 0;

   /* Returns number of coefficients. */
   virtual unsigned int GetNofCoefficients() const = 0;
};

class CbmLitFieldFitter
{
public:
   /* Constructor
    * @param polynomDegree Polynom degree to be used for field approximation */
   CbmLitFieldFitter(unsigned int polynomDegree);

   /* Destructor */
   virtual ~CbmLitFieldFitter();

   /* Fits (X, Y) slice of the magnetic field at Z position
    *@param Z Z position of the slice.
    *@param slice Output field slice */
   template <class T>
   void FitSlice(
      float Z,
      lit::parallel::LitFieldSlice<T>& slice);

   /* FitSlice implementation using fscal data type */
   void FitSliceScal(
      float Z,
      lit::parallel::LitFieldSlice<fscal>& slice);

   /* FitSlice implementation using fvec data type */
   void FitSliceVec(
      float Z,
      lit::parallel::LitFieldSlice<fvec>& slice);

   /* Sets acceptance angle for X. */
   void SetXangle(double xangle) {fXangle = xangle;}

   /* Sets acceptance angle for Y. */
   void SetYangle(double yangle) {fYangle = yangle;}

   /* Sets number of bins for X. */
   void SetNofBinsX(int nofBinsX) {fNofBinsX = nofBinsX;}

   /* Sets number of bins for Y. */
   void SetNofBinsY(int nofBinsY) {fNofBinsY = nofBinsY;}

   /* Sets Ellipse shape for acceptance. */
   void SetUseEllipseAcc(bool useEllipseAcc) {fUseEllipseAcc = useEllipseAcc;}

private:

   /* Fits the (X, Y) slice of the magnetic field at Z position.
    * @param Z Z position of the slice.
    * @param parBx Output vector with polynom coefficients for Bx.
    * @param parBy Output vector with polynom coefficients for By.
    * @param parBz Output vector with polynom coefficients for Bz.
    */
   void FitSlice(
      double Z,
      std::vector<double>& parBx,
      std::vector<double>& parBy,
      std::vector<double>& parBz);

   /* Fits the (X, Y) slice of the magnetic field.
    * @param x Vector with X coordinates.
    * @param y Vector with y coordinates.
    * @param z Vector with field values.
    * @param par Output vector with polynom coefficients.
    */
   void FitSlice(
      const std::vector<double>& x,
      const std::vector<double>& y,
      const std::vector<double>& z,
      std::vector<double>& par);

   /* Fits the (X, Y) slice of the magnetic field at Z position.
    * @param Z Z position of the slice.
    * @param parBx Output vector with polynom coefficients for Bx.
    * @param parBy Output vector with polynom coefficients for By.
    * @param parBz Output vector with polynom coefficients for Bz.
    */
   void FitSliceMy(
      double Z,
      std::vector<double>& parBx,
      std::vector<double>& parBy,
      std::vector<double>& parBz);

   /* Returns polynom. */
   const CbmLitPolynom* GetPolynom() const {return fPolynom;}

private:
   FairField* fField; // Magnetic field

   double fXangle; // Acceptance angle for X [grad]
   double fYangle; // Acceptance angle for Y [grad]

   int fNofBinsX; // Number of bins for X [cm]
   int fNofBinsY; // Number of bins for Y [cm]

   bool fUseEllipseAcc; // If true than only values inside an ellipse will be fitted

   unsigned int fPolynomDegree; // Degree of the polynom to be used for the field approximation
   CbmLitPolynom* fPolynom; // Polynom to be used for field approximation
};

#endif /* CBMLITFIELDFITTER_H_ */
