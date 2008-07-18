/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. M‰nner
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gl‰ﬂ
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
//     - base class for warnings occuring during file access
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/07/17 11:35:00 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../../MiscLIB/include/projects.h"
#include "../include/fileioWarningMsg.h"
#include <iostream>


/****************************************************************
 * CLASS fileioWarningMsg						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

fileioWarningMsg::fileioWarningMsg() {

}

/****************************************************************
 * Dstructor													*
 ****************************************************************/

fileioWarningMsg::~fileioWarningMsg() {

}

/****************************************************************
 * This method prints a message.				 				*
 ****************************************************************/

void fileioWarningMsg::printMsg(std::string message) {

#ifndef NOFILEIOWARNINGMESSAGE 

	std::cerr << std::endl;
	
	std::cerr << FILEIOLIB << " WARNING: ";
	
	if (!message.empty())
		std::cerr << message;
	
	std::cerr << std::endl;

#endif

}


/****************************************************************
 * CLASS missingCmdValWarningMsg				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

missingCmdValWarningMsg::missingCmdValWarningMsg() : fileioWarningMsg() {

}

/****************************************************************
 * Dstructor													*
 ****************************************************************/

missingCmdValWarningMsg::~missingCmdValWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void missingCmdValWarningMsg::warningMsg() {

	printMsg("Missing specifier value in the file detected!!!");

}


/****************************************************************
 * CLASS missingCmdSpecWarningMsg				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

missingCmdSpecWarningMsg::missingCmdSpecWarningMsg() : fileioWarningMsg() {

}

/****************************************************************
 * Dstructor													*
 ****************************************************************/

missingCmdSpecWarningMsg::~missingCmdSpecWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void missingCmdSpecWarningMsg::warningMsg() {

	printMsg("Missing command specifier in the file detected!!!");

}


/****************************************************************
 * CLASS unknownCmdSpecValPairWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

unknownCmdSpecValPairWarningMsg::unknownCmdSpecValPairWarningMsg() : fileioWarningMsg() {

}

/****************************************************************
 * Dstructor													*
 ****************************************************************/

unknownCmdSpecValPairWarningMsg::~unknownCmdSpecValPairWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void unknownCmdSpecValPairWarningMsg::warningMsg() {

	printMsg("The value cannot be assigned to the given specifier!!!");

}


/****************************************************************
 * CLASS cannotOpenFileWarning					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotOpenFileWarningMsg::cannotOpenFileWarningMsg() : fileioWarningMsg() {

}

/****************************************************************
 * Dstructor													*
 ****************************************************************/

cannotOpenFileWarningMsg::~cannotOpenFileWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotOpenFileWarningMsg::warningMsg() {

	printMsg("The file cannot be opened!!!");

}


/****************************************************************
 * CLASS writeDefaultFileWarningMsg				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

writeDefaultFileWarningMsg::writeDefaultFileWarningMsg() : fileioWarningMsg() {

	fileName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

writeDefaultFileWarningMsg::writeDefaultFileWarningMsg(std::string name) {

	fileName = name;

}

/****************************************************************
 * Dstructor													*
 ****************************************************************/

writeDefaultFileWarningMsg::~writeDefaultFileWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void writeDefaultFileWarningMsg::warningMsg() {

	std::string temp;

	temp  = "The default configuration file '";
	temp += fileName;
	temp += "' will be written!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS dataCommentWarningMsg					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

dataCommentWarningMsg::dataCommentWarningMsg() : fileioWarningMsg() {

}

/****************************************************************
 * Dstructor													*
 ****************************************************************/

dataCommentWarningMsg::~dataCommentWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void dataCommentWarningMsg::warningMsg() {

	printMsg("A comment line is detected in the data area of the file!!!");

}


/****************************************************************
 * CLASS tooMuchDataWarningMsg					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tooMuchDataWarningMsg::tooMuchDataWarningMsg() : fileioWarningMsg() {

}

/****************************************************************
 * Dstructor													*
 ****************************************************************/

tooMuchDataWarningMsg::~tooMuchDataWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void tooMuchDataWarningMsg::warningMsg() {

	printMsg("Too much data is detected in the file!!!");

}
