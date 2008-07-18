/* *******************************************************************
// (C)opyright 2004
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
//     - derived from errorHandling
//     - class for errors occuring while interacting with the luts
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-05-14 15:47:55 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/lutGeneratorError.h"


/****************************************************************
 * CLASS lutGeneratorError						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

lutGeneratorError::lutGeneratorError() : errorHandling(LUTGENERATORLIB) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

lutGeneratorError::~lutGeneratorError() {

}


/****************************************************************
 * CLASS noHitError								 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noHitError::noHitError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noHitError::~noHitError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void noHitError::errorMsg() {

	printMsg("There is no hit to use!!!");

}


/****************************************************************
 * CLASS magneticFieldFactorCannotBeZero		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

magneticFieldFactorCannotBeZero::magneticFieldFactorCannotBeZero() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

magneticFieldFactorCannotBeZero::~magneticFieldFactorCannotBeZero() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void magneticFieldFactorCannotBeZero::errorMsg() {

	printMsg("The factor for the magnetic field cannot be set to zero!!!");

}


/****************************************************************
 * CLASS tooBigIndexForMemoryError				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tooBigIndexForMemoryError::tooBigIndexForMemoryError() : lutGeneratorError() {

	index           = 0;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

tooBigIndexForMemoryError::tooBigIndexForMemoryError(unsigned long index, unsigned long numberOfEntries) : lutGeneratorError() {

	this->index           = index;
	this->numberOfEntries = numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tooBigIndexForMemoryError::~tooBigIndexForMemoryError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void tooBigIndexForMemoryError::errorMsg() {

	std::string temp;
	char buffer[longConversionDigits+1];

	temp  = "Cannot access the look up table memory at position ";
	ultos(index, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += ", because there are just ";
	ultos(numberOfEntries, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " entries!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS cannotAccessLutMemError				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessLutMemError::cannotAccessLutMemError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessLutMemError::~cannotAccessLutMemError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessLutMemError::errorMsg() {

	printMsg("The memory for the look-up-table cannot be accessed!!!");

}


/****************************************************************
 * CLASS cannotGetDefinitionOfUndefinedObjectError 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetDefinitionOfUndefinedObjectError::cannotGetDefinitionOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetDefinitionOfUndefinedObjectError::~cannotGetDefinitionOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotGetDefinitionOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot get the definition of an undefined object!!!");

}


/****************************************************************
 * CLASS cannotGetBorderOfUndefinedObjectError	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetBorderOfUndefinedObjectError::cannotGetBorderOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetBorderOfUndefinedObjectError::~cannotGetBorderOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotGetBorderOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot get the border of an undefined object!!!");

}


/****************************************************************
 * CLASS cannotClearBorderOfUndefinedObjectError 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotClearBorderOfUndefinedObjectError::cannotClearBorderOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotClearBorderOfUndefinedObjectError::~cannotClearBorderOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotClearBorderOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot clear the border of an undefined object!!!");

}


/****************************************************************
 * CLASS cannotGetMagneticFieldOfUndefinedObjectError			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetMagneticFieldOfUndefinedObjectError::cannotGetMagneticFieldOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetMagneticFieldOfUndefinedObjectError::~cannotGetMagneticFieldOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotGetMagneticFieldOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot get the magnetic field of an undefined object!!!");

}


/****************************************************************
 * CLASS cannotGetMagneticFieldFactorOfUndefinedObjectError		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetMagneticFieldFactorOfUndefinedObjectError::cannotGetMagneticFieldFactorOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetMagneticFieldFactorOfUndefinedObjectError::~cannotGetMagneticFieldFactorOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotGetMagneticFieldFactorOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot get the magnetic field factor of an undefined object!!!");

}


/****************************************************************
 * CLASS cannotSetMagneticFieldOfUndefinedObjectError			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotSetMagneticFieldOfUndefinedObjectError::cannotSetMagneticFieldOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotSetMagneticFieldOfUndefinedObjectError::~cannotSetMagneticFieldOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotSetMagneticFieldOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot set the magnetic field of an undefined object!!!");

}


/****************************************************************
 * CLASS cannotSetMagneticFieldFactorOfUndefinedObjectError		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotSetMagneticFieldFactorOfUndefinedObjectError::cannotSetMagneticFieldFactorOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotSetMagneticFieldFactorOfUndefinedObjectError::~cannotSetMagneticFieldFactorOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotSetMagneticFieldFactorOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot set the magnetic field factor of an undefined object!!!");

}


/****************************************************************
 * CLASS cannotAccessCorrectionsOfUndefinedObjectError			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessCorrectionsOfUndefinedObjectError::cannotAccessCorrectionsOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessCorrectionsOfUndefinedObjectError::~cannotAccessCorrectionsOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessCorrectionsOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot access the correction functionality of an undefined object!!!");

}


/****************************************************************
 * CLASS cannotEvaluateBorderOfUndefinedObjectError				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotEvaluateBorderOfUndefinedObjectError::cannotEvaluateBorderOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotEvaluateBorderOfUndefinedObjectError::~cannotEvaluateBorderOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotEvaluateBorderOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot evaluate the border of an undefined object!!!");

}
