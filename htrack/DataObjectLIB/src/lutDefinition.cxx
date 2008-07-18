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
//     - base class for holding the definition of the lut
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/11/06 11:12:14 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/lutDefinition.h"


/****************************************************************
 * CLASS lutDefinition							 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

lutDefinition::lutDefinition() {

	dim2Min  = 0;
	dim2Max  = 0;
	dim2Step = 0;
	dim1Min  = 0;
	dim1Max  = 0;
	dim1Step = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

lutDefinition::lutDefinition(const lutDefinition& value) {

	this->dim2Min  = value.dim2Min;
	this->dim2Max  = value.dim2Max;
	this->dim2Step = value.dim2Step;
	this->dim1Min  = value.dim1Min;
	this->dim1Max  = value.dim1Max;
	this->dim1Step = value.dim1Step;
}
lutDefinition::lutDefinition(double dim2Min, double dim2Max, int dim2Step, double dim1Min, double dim1Max, int dim1Step) {

	this->dim2Min  = dim2Min;
	this->dim2Max  = dim2Max;
	this->dim2Step = dim2Step;
	this->dim1Min  = dim1Min;
	this->dim1Max  = dim1Max;
	this->dim1Step = dim1Step;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

lutDefinition::~lutDefinition() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const lutDefinition& lutDefinition::operator = (const lutDefinition& value) {

	this->dim2Min  = value.dim2Min;
	this->dim2Max  = value.dim2Max;
	this->dim2Step = value.dim2Step;
	this->dim1Min  = value.dim1Min;
	this->dim1Max  = value.dim1Max;
	this->dim1Step = value.dim1Step;

	return *this;

}

/****************************************************************
 * operator << (stream)											*
 ****************************************************************/

std::ostream& operator << (std::ostream& os, lutDefinition& anyObject) {

	os << anyObject.toString();

	return os;

}

/****************************************************************
 * This method converts the object into a string representation.*
 ****************************************************************/
	
std::string lutDefinition::toString() {

	std::string returnValue;
	char        intBuffer[intConversionDigits+1];
	char        doubleBuffer[doubleConversion+1];

	returnValue  = "Dim1Min: ";
	dtos(dim1Min, doubleBuffer, doubleConversionDigits);
	returnValue += doubleBuffer;
	returnValue += "\n Dim1Max: ";
	dtos(dim1Max, doubleBuffer, doubleConversionDigits);
	returnValue += doubleBuffer;
	returnValue += "\n Dim1Step: ";
	itos(dim1Step, intBuffer, 10, shortConversionDigits);
	returnValue += intBuffer;
	returnValue += "\n Dim2Min: ";
	dtos(dim2Min, doubleBuffer, doubleConversionDigits);
	returnValue += doubleBuffer;
	returnValue += "\n Dim2Max: ";
	dtos(dim2Max, doubleBuffer, doubleConversionDigits);
	returnValue += doubleBuffer;
	returnValue += "\n Dim2Step: ";
	itos(dim2Step, intBuffer, 10, shortConversionDigits);
	returnValue += intBuffer;

	return returnValue;

}
