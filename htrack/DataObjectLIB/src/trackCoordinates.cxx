/* *******************************************************************
// (C)opyright 2005
// 
// Institute of Computer Science V
// Prof. M�nner
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gl��
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
// $Date: 2006/11/17 15:12:35 $
// $Revision: 1.2 $
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
