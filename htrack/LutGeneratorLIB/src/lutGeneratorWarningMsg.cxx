/* *******************************************************************
// (C)opyright 2004
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
//     - base class for warnings occuring during lut access
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:36:37 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../../MiscLIB/include/projects.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/lutGeneratorWarningMsg.h"
#include <iostream>


/****************************************************************
 * CLASS lutGeneratorWarningMsg					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

lutGeneratorWarningMsg::lutGeneratorWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

lutGeneratorWarningMsg::~lutGeneratorWarningMsg() {

}

/****************************************************************
 * This method prints a message.				 				*
 ****************************************************************/

void lutGeneratorWarningMsg::printMsg(std::string message) {

#ifndef NOLUTGENERATORWARNINGMESSAGE 

	std::cerr << std::endl;
	
	std::cerr << LUTGENERATORLIB << " WARNING: ";
	
	if (!message.empty())
		std::cerr << message;
	
	std::cerr << std::endl;

#endif

}


/****************************************************************
 * CLASS magneticFieldNotDefinedWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

magneticFieldNotDefinedWarningMsg::magneticFieldNotDefinedWarningMsg() : lutGeneratorWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

magneticFieldNotDefinedWarningMsg::~magneticFieldNotDefinedWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void magneticFieldNotDefinedWarningMsg::warningMsg() {

	printMsg("The magnetic field is not defined. So the value is always set to be 1.0!!!");

}


/****************************************************************
 * CLASS magneticFieldRenewWarningMsg			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

magneticFieldRenewWarningMsg::magneticFieldRenewWarningMsg() : lutGeneratorWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

magneticFieldRenewWarningMsg::~magneticFieldRenewWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void magneticFieldRenewWarningMsg::warningMsg() {

	printMsg("The magnetic field is renewed.");

}


/****************************************************************
 * CLASS differentPrelutDefinitionAsFileDetectedWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

differentPrelutDefinitionAsFileDetectedWarningMsg::differentPrelutDefinitionAsFileDetectedWarningMsg() : lutGeneratorWarningMsg() {

	numberOfDifferences = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

differentPrelutDefinitionAsFileDetectedWarningMsg::differentPrelutDefinitionAsFileDetectedWarningMsg(unsigned short numberOfDifferences) : lutGeneratorWarningMsg() {

	this->numberOfDifferences = numberOfDifferences;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

differentPrelutDefinitionAsFileDetectedWarningMsg::~differentPrelutDefinitionAsFileDetectedWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void differentPrelutDefinitionAsFileDetectedWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp  = "The definition, read from file, has ";
	ustos(numberOfDifferences, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += " differences compared to the prelut definition which is set!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS differentDigitalHitLutUsageAsFileDetectedWarningMsg	*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

differentDigitalHitLutUsageAsFileDetectedWarningMsg::differentDigitalHitLutUsageAsFileDetectedWarningMsg() : lutGeneratorWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

differentDigitalHitLutUsageAsFileDetectedWarningMsg::~differentDigitalHitLutUsageAsFileDetectedWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void differentDigitalHitLutUsageAsFileDetectedWarningMsg::warningMsg() {

	std::string temp;

	temp = "The usage of this digital hit look up table is set different to the standard. Maybe it is not the correct one!!!";

	printMsg(temp);

}


/****************************************************************
 * CLASS differentPrelutUsageAsFileDetectedWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

differentPrelutUsageAsFileDetectedWarningMsg::differentPrelutUsageAsFileDetectedWarningMsg() : lutGeneratorWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

differentPrelutUsageAsFileDetectedWarningMsg::~differentPrelutUsageAsFileDetectedWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void differentPrelutUsageAsFileDetectedWarningMsg::warningMsg() {

	std::string temp;

	temp = "The usage of this pre look up table is set different to the standard. Maybe it is not the correct one!!!";

	printMsg(temp);

}


/****************************************************************
 * CLASS differentLutDefinitionAsFileDetectedWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

differentLutDefinitionAsFileDetectedWarningMsg::differentLutDefinitionAsFileDetectedWarningMsg() : lutGeneratorWarningMsg() {

	numberOfDifferences = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

differentLutDefinitionAsFileDetectedWarningMsg::differentLutDefinitionAsFileDetectedWarningMsg(unsigned short numberOfDifferences) : lutGeneratorWarningMsg() {

	this->numberOfDifferences = numberOfDifferences;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

differentLutDefinitionAsFileDetectedWarningMsg::~differentLutDefinitionAsFileDetectedWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void differentLutDefinitionAsFileDetectedWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp  = "The definition, read from file, has ";
	ustos(numberOfDifferences, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += " differences compared to the lut definition which is set!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS differentLutUsageAsFileDetectedWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

differentLutUsageAsFileDetectedWarningMsg::differentLutUsageAsFileDetectedWarningMsg() : lutGeneratorWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

differentLutUsageAsFileDetectedWarningMsg::~differentLutUsageAsFileDetectedWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void differentLutUsageAsFileDetectedWarningMsg::warningMsg() {

	std::string temp;

	temp = "The usage of this look up table is set different to the standard. Maybe it is not the correct one!!!";

	printMsg(temp);

}


/****************************************************************
 * CLASS cannotWriteEmptyDigitalHitLutWarningMsg 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotWriteEmptyDigitalHitLutWarningMsg::cannotWriteEmptyDigitalHitLutWarningMsg() : lutGeneratorWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotWriteEmptyDigitalHitLutWarningMsg::~cannotWriteEmptyDigitalHitLutWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotWriteEmptyDigitalHitLutWarningMsg::warningMsg() {

	printMsg("You cannot write an empty digital hit look up table into the file!!!");

}


/****************************************************************
 * CLASS cannotWriteEmptyPrelutWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotWriteEmptyPrelutWarningMsg::cannotWriteEmptyPrelutWarningMsg() : lutGeneratorWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotWriteEmptyPrelutWarningMsg::~cannotWriteEmptyPrelutWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotWriteEmptyPrelutWarningMsg::warningMsg() {

	printMsg("You cannot write an empty pre look up table into the file!!!");

}


/****************************************************************
 * CLASS cannotWriteEmptyLutWarningMsg			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotWriteEmptyLutWarningMsg::cannotWriteEmptyLutWarningMsg() : lutGeneratorWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotWriteEmptyLutWarningMsg::~cannotWriteEmptyLutWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotWriteEmptyLutWarningMsg::warningMsg() {

	printMsg("You cannot write an empty look up table into the file!!!");

}
