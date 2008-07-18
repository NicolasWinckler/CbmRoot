/* *******************************************************************
// (C)opyright 2005
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gläß
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
//     - base class for handling the created borders of one layer
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/12/12 13:04:39 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/histogramCell.h"


/****************************************************************
 * CLASS histogramCell							 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

histogramCell::histogramCell() {

	value = 0;

#ifndef NOANALYSIS

	hits.reset();

#endif

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

histogramCell::histogramCell(const histogramCell& value) {

	this->value = value.value;

#ifndef NOANALYSIS

	this->hits  = value.hits;

#endif

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

histogramCell::~histogramCell() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const histogramCell& histogramCell::operator = (const histogramCell& value) {

	this->value = value.value;

#ifndef NOANALYSIS

	this->hits  = value.hits;

#endif

	return *this;

}

/****************************************************************
 * Method resets the object.									*
 ****************************************************************/

void histogramCell::reset() {

	value.reset();

#ifndef NOANALYSIS

	hits.reset();

#endif

}

/****************************************************************
 * Method returns the size of the used memory for the hits.		*
 ****************************************************************/

size_t histogramCell::getUsedSizeForHitMemory() {

#ifndef NOANALYSIS

	return hits.getUsedSizeOfMemory();

#else

	return 0;

#endif

}

/****************************************************************
 * Method returns the size of the allocated memory for the hits.*
 ****************************************************************/

size_t histogramCell::getAllocatedSizeForHitMemory() {

#ifndef NOANALYSIS

	return hits.getAllocatedSizeOfMemory();

#else

	return 0;

#endif

}
