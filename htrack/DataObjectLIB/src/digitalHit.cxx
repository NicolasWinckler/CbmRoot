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
//     - base class for holding the digital hit information
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-02-29 11:38:12 $
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

	identifier = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

digitalHit::digitalHit(const digitalHit& value) {

	this->identifier = value.identifier;

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

	this->identifier = value.identifier;

	return *this;

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds no identifiers.										*
 ****************************************************************/

std::string digitalHit::toNotIdentifiedString() {

	std::string returnValue;

	returnValue = getIdentifier();

	return returnValue;

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds identifiers.										*
 ****************************************************************/

std::string digitalHit::toIdentifiedString() {

	std::string returnValue;

	returnValue  = "Identifier: ";
	returnValue += getIdentifier();

	return returnValue;

}

/****************************************************************
 * method returns the start value								*
 ****************************************************************/

std::string digitalHit::getIdentifier() {

	std::string returnValue;
	char        buffer[longConversionDigits+1];

	returnValue  = "000";
	addRadix(RADIX, returnValue);
	ultos(identifier, buffer, RADIX, longConversionDigits);
	returnValue += buffer;

	return returnValue;

}

/****************************************************************
 * method sets the identifier value								*
 ****************************************************************/

void digitalHit::setIdentifier(std::string& value) {

	std::string temp;
	int         radix;

	temp       = value;
	radix      = extractRadix(&temp);
	identifier = stoul(temp, radix);

}

/****************************************************************
 * method sets the identifier value								*
 ****************************************************************/

void digitalHit::setIdentifier(unsigned long value) {

	identifier = value;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double digitalHit::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(identifier);

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

	returnValue  = sizeof(identifier);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
