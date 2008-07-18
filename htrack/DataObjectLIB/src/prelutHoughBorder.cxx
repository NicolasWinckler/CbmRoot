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
//     - base class for holding the houghTransformation border from
//       the prelut look-up-table
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/11/06 11:12:14 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/prelutHoughBorder.h"


/****************************************************************
 * CLASS prelutHoughBorder						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

prelutHoughBorder::prelutHoughBorder() {

	start = 0;
	stop  = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

prelutHoughBorder::prelutHoughBorder(const prelutHoughBorder& value) {

	this->start = value.start;
	this->stop  = value.stop;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

prelutHoughBorder::~prelutHoughBorder() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const prelutHoughBorder& prelutHoughBorder::operator = (const prelutHoughBorder& value) {

	this->start = value.start;
	this->stop  = value.stop;

	return *this;

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds no identifiers.										*
 ****************************************************************/

std::string prelutHoughBorder::toNotIdentifiedString() {

	std::string returnValue;

	returnValue  = "(";
	returnValue += getStart();
	returnValue += ",";
	returnValue += getStop();
	returnValue += ")";

	return returnValue;

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds identifiers.										*
 ****************************************************************/

std::string prelutHoughBorder::toIdentifiedString() {

	std::string returnValue;

	returnValue  = "Start: ";
	returnValue += getStart();
	returnValue += ", Stop: ";
	returnValue += getStop();

	return returnValue;

}

/****************************************************************
 * method returns the start value								*
 ****************************************************************/

std::string prelutHoughBorder::getStart() {

	std::string returnValue;
	char        buffer[shortConversionDigits+1];

	ustos(start, buffer, 10, shortConversionDigits);
	returnValue = buffer;

	return returnValue;

}

/****************************************************************
 * method returns the stop value								*
 ****************************************************************/

std::string prelutHoughBorder::getStop() {

	std::string returnValue;
	char        buffer[shortConversionDigits+1];

	ustos(stop, buffer, 10, shortConversionDigits);
	returnValue = buffer;

	return returnValue;

}

/****************************************************************
 * method sets the start value									*
 ****************************************************************/

void prelutHoughBorder::setStart(std::string& value) {

	start = stous(value, 10);

}

/****************************************************************
 * method sets the stop value									*
 ****************************************************************/

void prelutHoughBorder::setStop(std::string& value) {

	stop = stous(value, 10);

}
