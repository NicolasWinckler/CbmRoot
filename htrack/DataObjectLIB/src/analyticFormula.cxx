/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. M‰nner
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gl‰ﬂ
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   class:
//     - class for computing the analytic formula
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-06-19 14:32:01 $
// $Revision: 1.10 $
//
// *******************************************************************/


#include "../include/dataObjectError.h"
#include "../include/analyticFormula.h"
#include <math.h>
#include <stdio.h>


#define sqr(a)  ((a) * (a)) 

#define vl 0.299792458


/****************************************************************
 * Default constructor											*
 ****************************************************************/

analyticFormula::analyticFormula() {

}

/****************************************************************
 * destructor													*
 ****************************************************************/

analyticFormula::~analyticFormula() {

}

/****************************************************************
 * method returns the absolute value of the momentum.			*
 ****************************************************************/

double analyticFormula::evaluateAbsP(trackMomentum& momentum) {

	double returnValue;

	returnValue  = sqr(momentum.get(PX));
	returnValue += sqr(momentum.get(PY));
	returnValue += sqr(momentum.get(PZ));

	returnValue  = sqrt(returnValue);

	return returnValue;

}

/****************************************************************
 * method returns the absolute value of the transversal			*
 * momentum.													*
 ****************************************************************/

double analyticFormula::evaluateAbsPt(trackMomentum& momentum) {

	double returnValue;

	returnValue  = sqr(momentum.get(PX));
	returnValue += sqr(momentum.get(PY));

	returnValue = sqrt(returnValue);

	return returnValue;

}

/****************************************************************
 * method returns the momentum components in each direction.	*
 ****************************************************************/

void analyticFormula::evaluateP(trackCoordinates& coordinates, histogramSpace& space, trackMomentum* momentum) {

	trackParameter parameter;

	parameter.set(space.getAnalogFromCell(coordinates.get(DIM1), DIM1), DIM1);
	parameter.set(space.getAnalogFromCell(coordinates.get(DIM2), DIM2), DIM2);
	parameter.set(space.getAnalogFromCell(coordinates.get(DIM3), DIM3), DIM3);

	evaluateP(parameter, space, momentum);

}
void analyticFormula::evaluateP(trackCoordinates& coordinates, histogramSpace& space, trackFrameworkMomentum* momentum) {

	trackParameter parameter;

	parameter.set(space.getAnalogFromCell(coordinates.get(DIM1), DIM1), DIM1);
	parameter.set(space.getAnalogFromCell(coordinates.get(DIM2), DIM2), DIM2);
	parameter.set(space.getAnalogFromCell(coordinates.get(DIM3), DIM3), DIM3);

	evaluateP(parameter, space, momentum);

}
void analyticFormula::evaluateP(trackParameter& parameter, histogramSpace& space, trackMomentum* momentum) {

	double q_p_xz;
	double value;

	if (momentum != NULL) {

		q_p_xz = parameter.get(HRADIUS);

		/*
		 * If q_p_xz is zero, there would be a track which has a momentum in z
		 * direction which is so big that the quantization in the Hough space
		 * is not big enough. So this track is found in the Hough space at a
		 * coordinate representing an infinite momentum in z direction.
		 * Ordinary this infinite coordinate in the Hough space just exists, if
		 * the quantization is chosen in the way that there is a cell
		 * representing zero. If one adds just one to the chosen quatization,
		 * there are two cells with 0+epsilon and 0-epsilon instead of an
		 * infinite cell. So q_p_xz can never be zero. But a solution for this
		 * problem is to give such a track a momentum which is not infinite
		 * but at least twice times higher than the highest momentumg which
		 * lies in this cell when transforming.
		 */
		if (q_p_xz == 0)
			q_p_xz = space.getIncr(HRADIUS) / 2;
		
		if (q_p_xz < 0)
			q_p_xz = - q_p_xz;

		value = 1.0 / (q_p_xz * sqrt(sqr(tan(parameter.get(HTHETA))) + 1));
		momentum->set(value, MRADIUS);

		value = tan(parameter.get(HGAMMA)) * momentum->get(MRADIUS);
		momentum->set(value, MGAMMA);

		value = tan(parameter.get(HTHETA)) * momentum->get(MRADIUS);
		momentum->set(value, MTHETA);

	}
	else
		throw resultPointerIsNotAccessibleError();

}
void analyticFormula::evaluateP(trackParameter& parameter, histogramSpace& space, trackFrameworkMomentum* momentum) {

	double q_p_xz;
	double value;

	if (momentum != NULL) {

		q_p_xz = parameter.get(HRADIUS);
		
		/*
		 * If q_p_xz is zero, there would be a track which has a momentum in z
		 * direction which is so big that the quantization in the Hough space
		 * is not big enough. So this track is found in the Hough space at a
		 * coordinate representing an infinite momentum in z direction.
		 * Ordinary this infinite coordinate in the Hough space just exists, if
		 * the quantization is chosen in the way that there is a cell
		 * representing zero. If one adds just one to the chosen quatization,
		 * there are two cells with 0+epsilon and 0-epsilon instead of an
		 * infinite cell. So q_p_xz can never be zero. But a solution for this
		 * problem is to give such a track a momentum which is not infinite
		 * but at least twice times higher than the highest momentumg which
		 * lies in this cell when transforming.
		 */
		if (q_p_xz == 0)
			q_p_xz = space.getIncr(HRADIUS) / 2;
		
		if (q_p_xz < 0)
			q_p_xz = - q_p_xz;

		value = tan(parameter.get(HTHETA));
		momentum->setTx(value);

		value = tan(parameter.get(HGAMMA));
		momentum->setTy(value);

		value = sqrt(sqr(tan(parameter.get(HTHETA))) + sqr(tan(parameter.get(HGAMMA))) + 1) / (q_p_xz * (sqrt(sqr(tan(parameter.get(HTHETA))) + 1)));
		momentum->setQp(value);

	}
	else
		throw resultPointerIsNotAccessibleError();

}

/****************************************************************
 * method returns the computed values for the hough				*
 * cell based on the given momentum								*
 ****************************************************************/

void analyticFormula::evaluateHough(trackMomentum& momentum, double charge, histogramSpace& space, trackCoordinates* coordinates) {

	trackParameter parameter;

	if (coordinates != NULL) {

		evaluateHough(momentum, charge, &parameter);

		coordinates->set(space.getCellFromAnalog(parameter.get(DIM1), DIM1), DIM1);
		coordinates->set(space.getCellFromAnalog(parameter.get(DIM2), DIM2), DIM2);
		coordinates->set(space.getCellFromAnalog(parameter.get(DIM3), DIM3), DIM3);

	}
	else
		throw resultPointerIsNotAccessibleError();

}
void analyticFormula::evaluateHough(trackFrameworkMomentum& momentum, histogramSpace& space, trackCoordinates* coordinates) {

	trackParameter parameter;

	if (coordinates != NULL) {

		evaluateHough(momentum, &parameter);

		coordinates->set(space.getCellFromAnalog(parameter.get(DIM1), DIM1), DIM1);
		coordinates->set(space.getCellFromAnalog(parameter.get(DIM2), DIM2), DIM2);
		coordinates->set(space.getCellFromAnalog(parameter.get(DIM3), DIM3), DIM3);

	}
	else
		throw resultPointerIsNotAccessibleError();

}
void analyticFormula::evaluateHough(trackMomentum& momentum, double charge, trackParameter* parameter) {

	double value;

	if (parameter != NULL) {

		value = atan(momentum.get(MTHETA) / momentum.get(MRADIUS));
		parameter->set(value, HTHETA);

		value = atan(momentum.get(MGAMMA) / momentum.get(MRADIUS));
		parameter->set(value, HGAMMA);

		value = (- charge) / (momentum.get(MRADIUS) * sqrt(sqr(momentum.get(MTHETA) / momentum.get(MRADIUS)) + 1));
		parameter->set(value, HRADIUS);

	}
	else
		throw resultPointerIsNotAccessibleError();

}
void analyticFormula::evaluateHough(trackFrameworkMomentum& momentum, trackParameter* parameter) {

	double value;

	if (parameter != NULL) {

		value = atan(momentum.getTx());
		parameter->set(value, HTHETA);

		value = atan(momentum.getTy());
		parameter->set(value, HGAMMA);

		value = sqrt(1 + (sqr(momentum.getTy()) / (sqr(momentum.getTx()) + 1))) / momentum.getQp();
		parameter->set(value, HRADIUS);

	}
	else
		throw resultPointerIsNotAccessibleError();

}

/****************************************************************
 * This formula implements the computation neccessary for the	*
 * prelut														*
 ****************************************************************/

double analyticFormula::evaluatePrelut(double yPos, double zPos, double gamma) {

	double sinGamma;
	double cosGamma;
	double m;

	sinGamma = sin(gamma);
	cosGamma = cos(gamma);

	m        =  ((1000 * 2) * (zPos * sinGamma - yPos * cosGamma))
			 /	sqr(yPos * sinGamma + zPos * cosGamma);

	return m;

}

/****************************************************************
 * This formula implements the computation neccessary for the	*
 * lut															*
 ****************************************************************/

double analyticFormula::evaluateLut(double xPos, double zPos, double theta, double bField) {

	double sinTheta;
	double cosTheta;
	double q_p_xz;

	sinTheta = sin(theta);
	cosTheta = cos(theta);

	q_p_xz   = ((10000 * vl * 2) * (zPos * sinTheta - xPos * cosTheta))
		     / (bField * sqr(xPos * sinTheta + zPos * cosTheta));

	return q_p_xz;

}

/****************************************************************
 * This formula implements the computation of the original		*
 * formula in the coordinate space.								*
 ****************************************************************/

double analyticFormula::evaluateFormula(double numberOfElementaryCharges, double momentumX, double momentumZ, double zPos, double bField) {

	double sinTheta;
	double cosTheta;
	double bQ_p_xz;
	double constant;
	double p_2;
	double q;
	double x;

	sinTheta = sin(atan(momentumX / momentumZ));
	cosTheta = cos(atan(momentumX / momentumZ));
	bQ_p_xz  = bField * (numberOfElementaryCharges / sqrt(sqr(momentumX) + sqr(momentumZ)));
	constant = 10000 * vl * 2;
	p_2      = (cosTheta / sinTheta) * (zPos - (constant / (2 * bQ_p_xz * sinTheta)));
	q        = (zPos / sinTheta) * (((zPos * sqr(cosTheta)) / sinTheta) + (constant / bQ_p_xz));

	if (p_2 > 0)
		x    = - p_2 + sqrt(sqr(p_2) - q);
	else
		x    = - p_2 - sqrt(sqr(p_2) - q);

	return x;

}
