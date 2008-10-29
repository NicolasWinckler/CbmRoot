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
//     - base class for holding the digital hit information
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-24 16:39:21 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/digitalHit.h"


#define RADIX 10


/****************************************************************
 * CLASS digitalHit								 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

digitalHit::digitalHit() {

	data = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

digitalHit::digitalHit(const digitalHit& value) {

	this->data = value.data;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

digitalHit::~digitalHit() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const digitalHit& digitalHit::operator = (const digitalHit& value) {

	this->data = value.data;

	return *this;

}

/****************************************************************
 * method returns the maximuum number of digital hits			*
 * which can occur												*
 ****************************************************************/

#if (ARCHITECTURE != PS3)

unsigned long digitalHit::getMaxNumberOfDigitalHitData() {

	unsigned long returnValue;

#if (ARCHITECTURE == CBMROOT)

/**/

#else

	returnValue = 0;

#endif

	return returnValue;

}

#endif

/****************************************************************
 * method returns the digital hit data							*
 ****************************************************************/

unsigned long digitalHit::getData() {

	return data;

}

/****************************************************************
 * Method returns an analog hit based on the digital data.		*
 * CAUTION: The return object contains just coordinates.		*
 * Other parts of such an object are not set.					*
 ****************************************************************/

#if (ARCHITECTURE != PS3)

trackfinderInputHit digitalHit::getHit() {

	trackfinderInputHit returnValue;

#if (ARCHITECTURE == CBMROOT)

/**/

#else

	returnValue.initDefault();

#endif

	return returnValue;

}

#endif

/****************************************************************
 * method sets the digital hit data								*
 ****************************************************************/

void digitalHit::setData(unsigned long value) {

	data = value;

}

/****************************************************************
 * method sets the digital data based on an analog hit			*
 ****************************************************************/

#if (ARCHITECTURE != PS3)

void digitalHit::setHit(trackfinderInputHit* hit) {

	if (hit == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

#if (ARCHITECTURE == CBMROOT)

/**/

#else

	data = (unsigned long)hit->getHitOrder();

#endif

}

#endif

/****************************************************************
 * method sets the digital data based on a string representation*
 ****************************************************************/

void digitalHit::setNotIdentifiedString(std::string& value) {

	std::string temp;
	int         radix;

	temp = value;
	extractRadix(&radix, &temp);
	data = stoul(temp, radix);

}

/****************************************************************
 * This method converts the object into a string representation	*
 * without adding an identifier.								*
 ****************************************************************/

std::string digitalHit::toNotIdentifiedString() {

	std::string returnValue;
	char        buffer[longConversionDigits+1];

	returnValue  = "000";
	addRadix(RADIX, returnValue);
	ultos(data, buffer, RADIX, longConversionDigits);
	returnValue += buffer;

	return returnValue;

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds an identifier.										*
 ****************************************************************/

std::string digitalHit::toIdentifiedString() {

	std::string returnValue;

	returnValue  = "Identifier: ";
	returnValue += toNotIdentifiedString();

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double digitalHit::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(data);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double digitalHit::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double digitalHit::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(data);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
