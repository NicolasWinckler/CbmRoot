/* *******************************************************************
// (C)opyright 2004
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
//     - read and write access to a file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/11/08 12:37:31 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/tableStringEntry.h"
#include "../include/tableFile.h"


#define max(a, b)  (((a) > (b)) ? (a) : (b)) 


/* Defines the separator which is used in the file to delimit	*
 * a part of the struct which should be written in one line.	*
 * Important is that the newline character is forbidden			*
 */
#define fileDataSeparator '\t'

/*
 * default values for the initialization of each parameter
 */
#define defValName                  "table"
#define defValUsage                 "not specified"
#define defValNumberOfEntries       0
#define defValStructureSeparator    "TABULATOR"
#define defValBlockSeparator        "NEWLINE"
#define defValContent               "Signature => Classification"
#define defValFormat                "[Radix]x[Number]"defValStructureSeparator"[Radix]x[Number]"defValBlockSeparator


/*
 * stringcommands which represent the parameters read from file
 */
#define stringCmdName               "name"
#define stringCmdUsage              "usage"
#define stringCmdNumberOfEntries    "numberOfEntries"
#define stringCmdStructureSeparator "/**/ structureSeparator"
#define stringCmdBlockSeparator     "/**/ blockSeparator"
#define stringCmdContent            "/**/ content"
#define stringCmdFormat             "/**/ format"


/*
 * index of each parameter for the commandID table
 */
#define idCmdName                   0
#define idCmdUsage                  1
#define idCmdNumberOfEntries        2


/****************************************************************
 * Default constructor											*
 * Dependencies:												*
 * - Io::setDataPtr												*
 * - Io::setDataNum												*
 * Errors:														*
 * - memoryAllocationError										*
 ****************************************************************/

tableFile::tableFile() : io() {

	for (unsigned int i = 0; i < numberOfTableFileCmds; i++)
		commandID[i] = 0;

	init();

}

/****************************************************************
 * destructor													*
 ****************************************************************/

tableFile::~tableFile() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void tableFile::init() {

	setHeaderDefValues();

}

/****************************************************************
 * This method returns the number of accepted commands.			*
 ****************************************************************/

int tableFile::getNumberOfCmds() {

	return numberOfTableFileCmds;

}


/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool tableFile::getHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;

/********************************************************/
/* make code changes for a different configuration here */
	if (specifier.compare(stringCmdName) == 0) {
		if (!(commandID[idCmdName])) {
			header.name = value;
			commandID[idCmdName] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdUsage) == 0) {
		if (!(commandID[idCmdUsage])) {
			header.usage = value;
			commandID[idCmdUsage] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdNumberOfEntries) == 0) {
		if (!(commandID[idCmdNumberOfEntries])) {
			header.numberOfEntries = stoul((char*)value.c_str(), 10);
			commandID[idCmdNumberOfEntries] = true;
			specifierFound = true;
		}
	}
/********************************************************/

	return specifierFound;

}

/****************************************************************
 * This method sets the number of the data structures			*
 * to the value that all data is read from file. Normally		*
 * this value should be in the header of the file as one		*
 * command.														*
 ****************************************************************/

void tableFile::readAllData() {

	if (getDataNum() == 0) {

/********************************************************/
/* make code changes for allocation number of data here*/
		tableStringEntry* typeCastedData = new tableStringEntry[header.numberOfEntries];
		setDataNum(header.numberOfEntries);
		setDataPtr(typeCastedData);
/********************************************************/

		if (typeCastedData == NULL)
			throw memoryAllocationError(DATAOBJECTLIB);

	}

}

/****************************************************************
 * This method assigns the data to the corresponding structure.	*
 * The whole data structure to read must be in one single line	*
 * in the file.													*
 ****************************************************************/

bool tableFile::getDataValue(std::string& buffer, unsigned long index) {

	std::string       temp;
	bool              noError = true;
/********************************************************/
/* make code changes for a different data object here	*/
	tableStringEntry* typeCastedData = (tableStringEntry*) getDataPtr();
/********************************************************/

/********************************************************/
/* make code changes for a different data object here	*/
	temp = getDataString(buffer, 0, fileDataSeparator);
	if (temp.empty())
		noError = false;
	else
		typeCastedData[index].setSignature(temp);
	temp = getDataString(buffer, 1, fileDataSeparator);
	if (temp.empty())
		noError = false;
	else
		typeCastedData[index].setClassification(temp);
/********************************************************/
	return noError;

}

/****************************************************************
 * This method writes one data structure to the stream.			*
 * The whole data structure to write must be in one single line	*
 * in the file.													*
 ****************************************************************/

std::string tableFile::setDataValue(unsigned long index) {

	std::string returnValue;
/********************************************************/
/* make code changes for a different data object here	*/
	tableStringEntry* typeCastedData = (tableStringEntry*) getDataPtr();
/********************************************************/

/********************************************************/
/* make code changes for a different data object here	*/
	returnValue  = typeCastedData[index].getSignature();
	returnValue += fileDataSeparator;
	returnValue += typeCastedData[index].getClassification();
/********************************************************/

	return returnValue;

}

/****************************************************************
 * This method is to write the header of the file.				*
 * Dependencies:												*
 * - Fileio::setHeaderValue										*
 ****************************************************************/

void tableFile::writeFileHeader(std::ofstream& fileStream) {

/********************************************************/
/* make code changes for a different configuration here */
	setHeaderValue(fileStream, stringCmdName,               header.name,              "Name of the look up table");
	setHeaderValue(fileStream, stringCmdUsage,              header.usage,             "Usage of the look up table");
	setHeaderValue(fileStream, stringCmdNumberOfEntries,    header.numberOfEntries,   "Number of entries in the look up table");
	setHeaderValue(fileStream, stringCmdStructureSeparator, defValStructureSeparator, "The separator for the members of the data structure");
	setHeaderValue(fileStream, stringCmdBlockSeparator,     defValBlockSeparator,     "The separator for the blocks of the data");
	setHeaderValue(fileStream, stringCmdContent,            defValContent,            "The content of the file");
	setHeaderValue(fileStream, stringCmdFormat,             defValFormat,             "The format for an entry in the file");
/********************************************************/

}

/****************************************************************
 * This method is to set the default values for each header		*
 * parameter.
 ****************************************************************/

void tableFile::setHeaderDefValues() {

/********************************************************/
/* make code changes for a different configuration here */
	header.name            = defValName;
	header.usage           = defValUsage;
	header.numberOfEntries = defValNumberOfEntries;
/********************************************************/

}

/****************************************************************
 * This method returns a reference of the data struct.			*
 ****************************************************************/

tableFileHeader& tableFile::getHeaderReference() {

	return header;

}

/****************************************************************
 * This method sets the data struct	to the existing one.		*
 ****************************************************************/

void tableFile::setHeader(tableFileHeader& structure) {

	header.name                     = structure.name;
	header.usage                    = structure.usage;
	header.numberOfEntries          = structure.numberOfEntries;
	commandID[idCmdName]            = true;
	commandID[idCmdUsage]           = true;
	commandID[idCmdNumberOfEntries] = true;

}

/****************************************************************
 * This method returns an information string about the object.	*
 ****************************************************************/

std::string tableFile::getInfo() {

	std::string message;
	char buffer[longConversionDigits + 1];

/********************************************************/
/* make code changes for a different information here	*/
	message += "Setup for the file access:\n";
	message += "------------------------------------------\n";
	message += "Filename";
	message += "\t: ";
	message += getFileName();
	message += "\n";
	message += "Data-pointer";
	message += "\t: ";
	if (getDataPtr() == NULL)
		message += "FAILED ; ADDRESS = NULL";
	else {
		message += "OK ; ADDRESS = ";
		sprintf(buffer, "%p", getDataPtr());
		message += buffer;
	}
	message += "\n";
	message += "Number of Data";
	message += "\t: ";
	ultos(getDataNum(), buffer, 10, longConversionDigits);
	message += buffer;
	message += "\n";
	message += "\n";
	message += "Setup for the table file:\n";
	message += "------------------------------\n";
	message += "CommandID";
	message += "\t: ";
	for (int i = 0; i < numberOfTableFileCmds; i++) {
		btods(commandID[i], buffer);
		message += buffer;
	}
	message += "\n";
	message += stringCmdName;
	message += "\t: ";
	message += header.name;
	message += "\n";
	message += stringCmdUsage;
	message += "\t: ";
	message += header.usage;
	message += "\n";
	message += stringCmdNumberOfEntries;
	message += "\t: ";
	ultos(header.numberOfEntries, buffer, 10, longConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdStructureSeparator;
	message += "\t: ";
	message += defValStructureSeparator;
	message += "\n";
	message += stringCmdBlockSeparator;
	message += "\t: ";
	message += defValBlockSeparator;
	message += "\n";
	message += stringCmdContent;
	message += "\t: ";
	message += defValContent;
	message += "\n";
	message += stringCmdFormat;
	message += "\t: ";
	message += defValFormat;
	message += "\n";
/********************************************************/

	return message;

}
