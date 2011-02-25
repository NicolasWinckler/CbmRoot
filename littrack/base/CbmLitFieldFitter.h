/** CbmLitFieldFitter.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Class performs fit of the magnetic field using
 ** 0-9 degree polynom. MINUIT is used for minimization.
 ** Also my LSM fit is implemented.
 **/

#ifndef CBMLITFIELDFITTER_H_
#define CBMLITFIELDFITTER_H_

#include <vector>

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

class CbmLitFieldFitter {
public:
	/* Constructor */
	CbmLitFieldFitter(unsigned int polynomDegree);

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

	/* Returns polynom. */
	const CbmLitPolynom* GetPolynom() const {return fPolynom;}

private:
	FairField* fField; // Magnetic field

	double fXangle; // acceptance angle for X
	double fYangle; // acceptance angle for Y

	int fNofBinsX; // number of bins for X
	int fNofBinsY; // number of bins for Y

	bool fUseEllipseAcc; // if true than only values inside a certain ellipse will be fitted

	unsigned int fPolynomDegree; // degree of the polynom to be used for the field fit
	CbmLitPolynom* fPolynom; // polynom to be used in the field fit
};

#endif /* CBMLITFIELDFITTER_H_ */
