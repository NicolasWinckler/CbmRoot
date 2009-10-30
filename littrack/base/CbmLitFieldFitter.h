/** CbmLitFieldFitter.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** The class performs fit of the magnetic field with the 3rd order polynom.
 ** MINUIT is used for minimization.
 **/

#ifndef CBMLITFIELDFITTER_H_
#define CBMLITFIELDFITTER_H_

#include "CbmLitMyField.h"
#include "CbmLitFieldSlice.h"
#include <vector>

class FairField;

class CbmLitFieldFitter {
public:
	/* Constructor */
	CbmLitFieldFitter();

	/* Destructor */
	virtual ~CbmLitFieldFitter();

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

	void SetXangle(double xangle) {fXangle = xangle;}
	void SetYangle(double yangle) {fYangle = yangle;}
	void SetNofBinsX(double nofBinsX) {fNofBinsX = nofBinsX;}
	void SetNofBinsY(double nofBinsY) {fNofBinsY = nofBinsY;}
	void SetUseEllipseAcc(bool useEllipseAcc) {fUseEllipseAcc = useEllipseAcc;}

private:
	FairField* fField; // Magnetic field

	double fXangle; // acceptance angle for X
	double fYangle; // acceptance angle for Y

	int fNofBinsX; // number of bins for X
	int fNofBinsY; // number of bins for Y

	bool fUseEllipseAcc; // if true than only values inside a certain ellipse will be fitted
};

#endif /* CBMLITFIELDFITTER_H_ */
