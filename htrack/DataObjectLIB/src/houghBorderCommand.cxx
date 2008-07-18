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
//     - base class for holding the houghTransformation entry command
//       for the histogram
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/11/06 11:12:13 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/houghBorderCommand.h"


/****************************************************************
 * CLASS houghBorderCommand						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

houghBorderCommand::houghBorderCommand() {

	cmd.clear();
	startPos = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

houghBorderCommand::houghBorderCommand(const houghBorderCommand& value) {

	this->cmd      = value.cmd;
	this->startPos = value.startPos;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

houghBorderCommand::~houghBorderCommand() {

	cmd.clear();

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const houghBorderCommand& houghBorderCommand::operator = (const houghBorderCommand& value) {

	this->cmd      = value.cmd;
	this->startPos = value.startPos;

	return *this;

}

/**
 * method returns the startPos value
 */
	
std::string houghBorderCommand::getStartPos() {

	std::string returnValue;
	char        buffer[shortConversionDigits+1];

	ustos(startPos, buffer, 10, shortConversionDigits);
	returnValue = buffer;

	return returnValue;

}

/**
 * method returns the startPos value
 */
	
std::string houghBorderCommand::getCmd() {

	std::string returnValue;
	char        buffer[1+1];
	bool        actualCmd;

	returnValue.clear();
	cmd.resetActiveObject();
	for (unsigned short i = 0; i < cmd.getNumberOfEntries(); i++) {
		
		actualCmd = cmd.readActiveObjectAndMakeNextOneActive();

		btods(actualCmd, buffer);
		returnValue += buffer;

	}

	return returnValue;

}

/****************************************************************
 * method sets the startPos value								*
 ****************************************************************/
	
void houghBorderCommand::setStartPos(std::string& value) {

	startPos = stous((char*)value.c_str(), 10); 

}

/****************************************************************
 * method sets the startPos value								*
 ****************************************************************/
	
void houghBorderCommand::setCmd(std::string& value) {

	cmd.clear();
	for (std::basic_string<char>::iterator i = value.begin(); i != value.end(); i++)
		cmd.push(dstob(*i));

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds no identifiers.										*
 ****************************************************************/

std::string houghBorderCommand::toNotIdentifiedString() {

	std::string returnValue;

	returnValue  = "(";
	returnValue += getStartPos();
	returnValue += ",";
	returnValue += getCmd();
	returnValue += ")";

	return returnValue;

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds identifiers.										*
 ****************************************************************/

std::string houghBorderCommand::toIdentifiedString() {

	std::string returnValue;

	returnValue  = "StartPos: ";
	returnValue += getStartPos();
	returnValue += ", Cmd: ";
	returnValue += getCmd();

	return returnValue;

}
