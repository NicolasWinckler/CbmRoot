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
//     - base class for holding the track information of the transformation
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-10-19 14:33:09 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../include/trackDigitalInformation.h"


/****************************************************************
 * CLASS trackDigitalInformation				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackDigitalInformation::trackDigitalInformation() {

	position = trackCoordinates(0, 0, 0);
	value    = bitArray(0);

#ifndef NOANALYSIS

	hits     = hitArray();

#endif

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackDigitalInformation::trackDigitalInformation(const trackDigitalInformation& value) {

	this->position = value.position;
	this->value    = value.value;

#ifndef NOANALYSIS

	this->hits     = value.hits;

#endif

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackDigitalInformation::~trackDigitalInformation() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackDigitalInformation& trackDigitalInformation::operator = (const trackDigitalInformation& value) {

	this->position = value.position;
	this->value    = value.value;

#ifndef NOANALYSIS

	this->hits     = value.hits;

#endif

	return *this;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double trackDigitalInformation::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = position.getReservedSizeOfData(0);
	returnValue += value.getReservedSizeOfData(0);

#ifndef NOANALYSIS

	returnValue += hits.getReservedSizeOfData(0);

#endif

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double trackDigitalInformation::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = position.getAllocatedSizeOfData(0);
	returnValue += value.getAllocatedSizeOfData(0);

#ifndef NOANALYSIS

	returnValue += hits.getAllocatedSizeOfData(0);

#endif

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double trackDigitalInformation::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = position.getUsedSizeOfData(0);
	returnValue += value.getUsedSizeOfData(0);

#ifndef NOANALYSIS

	returnValue += hits.getUsedSizeOfData(0);

#endif

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
