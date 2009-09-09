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

	/* Fits the magnetic field. */
	void Fit();

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

	/* @return Number of field slices. */
	int GetNofFieldSlices() const {return fNofSlices;}

	/* Copies field slices to output vector
	 * @param slices Output field slices.
	 */
	void GetFieldSlices(std::vector<CbmLitFieldSlice>& slices) const {
		slices.assign(fSlices.begin(), fSlices.end());
	}

	/* Returns some information used in the fit.
	 * This is used to check the accuracy of the fit.
	 * @param z Vector with z positions of slices.
	 * @param x Vector with x half-width.
	 * @param y Vector with y half-width.
	 * @param nbinsx Vector with number of bins for X coordinate.
	 * @param nbinsy Vector with number of bins for Y coordinate.
	 */
	void GetInputInfo(
			std::vector<double>& z,
			std::vector<double>& x,
			std::vector<double>& y,
			std::vector<int>& nbinsx,
			std::vector<int>& nbinsy) const {
		z.assign(fZpos.begin(), fZpos.end());
		x.assign(fXpos.begin(), fXpos.end());
		y.assign(fYpos.begin(), fYpos.end());
		nbinsx.assign(fNofBinsX.begin(), fNofBinsX.end());
		nbinsy.assign(fNofBinsY.begin(), fNofBinsY.end());
	}

private:
	FairField* fField; // Magnetic field
	int fNofSlices; // Number of slices
    std::vector<double> fZpos; // Z positions of the field slice
    std::vector<double> fXpos; // X outer radius of the field slice
    std::vector<double> fYpos; // Y outer radius of the field slice
    std::vector<int> fNofBinsX; // Number of bins for X
    std::vector<int> fNofBinsY; // Number of bins for Y

    std::vector<CbmLitFieldSlice> fSlices; // Vector with field slices
};

#endif /* CBMLITFIELDFITTER_H_ */
