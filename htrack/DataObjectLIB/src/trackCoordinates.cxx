/* *******************************************************************
// (C)opyright 2005
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle
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
//     - base class for holding the track coordinates of the transformation
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:32:44 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../include/dataObjectError.h"
#include "../include/trackCoordinates.h"


/****************************************************************
 * CLASS trackCoordinates						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackCoordinates::trackCoordinates() {

	for (unsigned short i = 0; i < DIMENSIONS; i++)
		dim[i] = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackCoordinates::trackCoordinates(const trackCoordinates& value) {

	for (unsigned short i = 0; i < DIMENSIONS; i++)
		this->dim[i] = value.dim[i];

}
trackCoordinates::trackCoordinates(unsigned short dim1, unsigned short dim2, unsigned short dim3) {

	set(dim1, DIM1);
	set(dim2, DIM2);
	set(dim3, DIM3);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackCoordinates::~trackCoordinates() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackCoordinates& trackCoordinates::operator = (const trackCoordinates& value) {

	for (unsigned short i = 0; i < DIMENSIONS; i++)
		this->dim[i] = value.dim[i];

	return *this;

}

/****************************************************************
 * returns the value in the dimension							*
 ****************************************************************/

unsigned short trackCoordinates::get(unsigned short dimension) {

	unsigned short returnValue = 0;

	if (dimension < DIMENSIONS)
		returnValue = dim[dimension];
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

	return returnValue;

}

/****************************************************************
 * sets the value in the dimension								*
 ****************************************************************/

void trackCoordinates::set(unsigned short set, unsigned short dimension) {

	if (dimension < DIMENSIONS)
		dim[dimension] = set;
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double trackCoordinates::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(dim);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double trackCoordinates::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double trackCoordinates::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(dim);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
