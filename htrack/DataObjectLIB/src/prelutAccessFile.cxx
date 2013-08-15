/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. M�nner
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
//     - read and write access to a file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-24 16:36:58 $
// $Revision: 1.8 $
//
// *******************************************************************/


#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/prelutHoughBorder.h"
#include "../include/prelutAccessFile.h"


#define max(a, b)  (((a) > (b)) ? (a) : (b)) 


/* Defines the separator which is used in the file to delimit	*
 * a part of the struct which should be written in one line.	*
 * Important is that the newline character is forbidden			*
 */
#define fileDataSeparator '\t'

/*
 * default values for the initialization of each parameter
 */
#define defValName                  "prelut"
#define defValUsage                 "PRELUT"
#define defValNumberOfEntries       0
#define defValDimMin                0
#define defValDimMax                0
#define defValDimStep               0
#define defValDimStartEntry         0
#define defValDimStopEntry          0
#define defValStructureSeparator    "TABULATOR"
#define defValBlockSeparator        "NEWLINE"
#define defValContent               "Start & Stop"
#define defValFormat                "[Radix]x[Number]"defValStructureSeparator"[Radix]x[Number]"defValBlockSeparator


/*
 * stringcommands which represent the parameters read from file
 */
#define stringCmdName               "name"
#define stringCmdUsage              "usage"
#define stringCmdNumberOfEntries    "numberOfEntries"
#define stringCmdDimMin             "dimMin"
#define stringCmdDimMax             "dimMax"
#define stringCmdDimStep            "dimStep"
#define stringCmdDimStartEntry      "dimStartEntry"
#define stringCmdDimStopEntry       "dimStopEntry"
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
#define idCmdDimMin                 3
#define idCmdDimMax                 4
#define idCmdDimStep                5
#define idCmdDimStartEntry          6
#define idCmdDimStopEntry           7


/****************************************************************
 * This method deletes the local memory if it exists.			*
 ****************************************************************/

void prelutAccessFile::deleteLocalMemory() {

	if (localMemory) {

/********************************************************/
/* make code changes for allocation number of data here*/
		prelutHoughBorder* typeCastedData = (prelutHoughBorder*)getDataPtr();
/********************************************************/

		if (typeCastedData != NULL) {
			delete [] typeCastedData;
			setDataNum(0);
			setDataPtr(NULL);
		}

		localMemory = false;

	}

}

/****************************************************************
 * This method returns the number of accepted commands.			*
 ****************************************************************/

unsigned int prelutAccessFile::getNumberOfCmds() {

	return numberOfPrelutAccessFileCmds;

}


/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool prelutAccessFile::getHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;

/********************************************************/
/* make code changes for a different configuration here */
	if (specifier.compare(stringCmdName) == 0) {
		if (!isHeaderLockSet(idCmdName)) {
			header.name = value;
			setHeaderLock(idCmdName, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdUsage) == 0) {
		if (!isHeaderLockSet(idCmdUsage)) {
			header.usage = value;
			setHeaderLock(idCmdUsage, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdNumberOfEntries) == 0) {
		if (!isHeaderLockSet(idCmdNumberOfEntries)) {
			header.numberOfEntries = stoul((char*)value.c_str(), 10);
			setHeaderLock(idCmdNumberOfEntries, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdDimMin) == 0) {
		if (!isHeaderLockSet(idCmdDimMin)) {
			header.dimMin = stod((char*)value.c_str());
			setHeaderLock(idCmdDimMin, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdDimMax) == 0) {
		if (!isHeaderLockSet(idCmdDimMax)) {
			header.dimMax = stod((char*)value.c_str());
			setHeaderLock(idCmdDimMax, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdDimStep) == 0) {
		if (!isHeaderLockSet(idCmdDimStep)) {
			header.dimStep = stoi((char*)value.c_str(), 10);
			setHeaderLock(idCmdDimStep, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdDimStartEntry) == 0) {
		if (!isHeaderLockSet(idCmdDimStartEntry)) {
			header.dimStartEntry = stod((char*)value.c_str());
			setHeaderLock(idCmdDimStartEntry, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdDimStopEntry) == 0) {
		if (!isHeaderLockSet(idCmdDimStopEntry)) {
			header.dimStopEntry = stod((char*)value.c_str());
			setHeaderLock(idCmdDimStopEntry, true);
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

void prelutAccessFile::readAllData() {

	if ((getDataNum() == 0) && (getDataPtr() == NULL)) {

/********************************************************/
/* make code changes for allocation number of data here*/
		prelutHoughBorder* typeCastedData = new prelutHoughBorder[header.numberOfEntries];
/********************************************************/

		setDataNum(header.numberOfEntries);
		setDataPtr(typeCastedData);

		if (typeCastedData == NULL)
			throw memoryAllocationError(DATAOBJECTLIB);
		else
			localMemory = true;

	}

}

/****************************************************************
 * This method assigns the data to the corresponding structure.	*
 * The whole data structure to read must be in one single line	*
 * in the file.													*
 ****************************************************************/

bool prelutAccessFile::getDataValue(std::string& buffer, unsigned long index) {

	std::string       temp;
	bool              noError = true;
/********************************************************/
/* make code changes for a different data object here	*/
	prelutHoughBorder* typeCastedData = (prelutHoughBorder*) getDataPtr();
/********************************************************/

/********************************************************/
/* make code changes for a different data object here	*/
	temp = getDataString(buffer, 0, fileDataSeparator);
	if (temp.empty())
		noError = false;
	else
		typeCastedData[index].setStart(temp);
	temp = getDataString(buffer, 1, fileDataSeparator);
	if (temp.empty())
		noError = false;
	else
		typeCastedData[index].setStop(temp);
/********************************************************/
	return noError;

}

/****************************************************************
 * This method writes one data structure to the stream.			*
 * The whole data structure to write must be in one single line	*
 * in the file.													*
 ****************************************************************/

std::string prelutAccessFile::setDataValue(unsigned long index) {

	std::string returnValue;
/********************************************************/
/* make code changes for a different data object here	*/
	prelutHoughBorder* typeCastedData = (prelutHoughBorder*) getDataPtr();
/********************************************************/

/********************************************************/
/* make code changes for a different data object here	*/
	returnValue  = typeCastedData[index].getStart();
	returnValue += fileDataSeparator;
	returnValue += typeCastedData[index].getStop();
/********************************************************/

	return returnValue;

}

/****************************************************************
 * This method is to write the header of the file.				*
 * Dependencies:												*
 * - Fileio::setHeaderValue										*
 ****************************************************************/

void prelutAccessFile::writeFileHeader(std::ofstream& fileStream, terminalSequence* statusSequence) {

/********************************************************/
/* make code changes for a different configuration here */
	setHeaderValue(fileStream, stringCmdName,               header.name,              "Name of the look up table", statusSequence);
	setHeaderValue(fileStream, stringCmdUsage,              header.usage,             "Usage of the look up table", statusSequence);
	setHeaderValue(fileStream, stringCmdNumberOfEntries,    header.numberOfEntries,   "Number of entries in the look up table", statusSequence);
	setHeaderValue(fileStream, stringCmdDimMin,             header.dimMin,            "Minimum value of the LUT configuration during creation", statusSequence);
	setHeaderValue(fileStream, stringCmdDimMax,             header.dimMax,            "Maximum value of the LUT configuration during creation", statusSequence);
	setHeaderValue(fileStream, stringCmdDimStep,            header.dimStep,           "Number of steps for the value range of the LUT configuration during creation", statusSequence);
	setHeaderValue(fileStream, stringCmdDimStartEntry,      header.dimStartEntry,     "Start entry value of the LUT configuration during creation", statusSequence);
	setHeaderValue(fileStream, stringCmdDimStopEntry,       header.dimStopEntry,      "Stop entry value of the LUT configuration during creation", statusSequence);
	setHeaderValue(fileStream, stringCmdStructureSeparator, defValStructureSeparator, "The separator for the members of the data structure", statusSequence);
	setHeaderValue(fileStream, stringCmdBlockSeparator,     defValBlockSeparator,     "The separator for the blocks of the data", statusSequence);
	setHeaderValue(fileStream, stringCmdContent,            defValContent,            "The content of the file", statusSequence);
	setHeaderValue(fileStream, stringCmdFormat,             defValFormat,             "The format for an entry in the file", statusSequence);
/********************************************************/

}

/****************************************************************
 * This method is to set the default values for each header		*
 * parameter.
 ****************************************************************/

void prelutAccessFile::setHeaderDefValues() {

/********************************************************/
/* make code changes for a different configuration here */
	header.name            = defValName;
	header.usage           = defValUsage;
	header.numberOfEntries = defValNumberOfEntries;
	header.dimMin          = defValDimMin;
	header.dimMax          = defValDimMax;
	header.dimStep         = defValDimStep;
	header.dimStartEntry   = defValDimStartEntry;
	header.dimStopEntry    = defValDimStopEntry;
/********************************************************/

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

prelutAccessFile::prelutAccessFile() : io(), header(), localMemory(false) {

  //	localMemory = false;

	init();

}

/****************************************************************
 * destructor													*
 ****************************************************************/

prelutAccessFile::~prelutAccessFile() {

	if (localMemory)
		deleteLocalMemory();

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void prelutAccessFile::init() {

	if (localMemory)
		deleteLocalMemory();

	resetHeader();

}

/****************************************************************
 * This method returns a reference of the data struct.			*
 ****************************************************************/

prelutAccessFileHeader& prelutAccessFile::getHeaderReference() {

	return header;

}

/****************************************************************
 * This method sets the data struct	to the existing one.		*
 ****************************************************************/

void prelutAccessFile::setHeader(prelutAccessFileHeader& structure) {

	header.name                     = structure.name;
	header.usage                    = structure.usage;
	header.numberOfEntries          = structure.numberOfEntries;
	header.dimMin                   = structure.dimMin;
	header.dimMax                   = structure.dimMax;
	header.dimStep                  = structure.dimStep;
	header.dimStartEntry            = structure.dimStartEntry;
	header.dimStopEntry             = structure.dimStopEntry;
	setHeaderLock(idCmdName, true);
	setHeaderLock(idCmdUsage, true);
	setHeaderLock(idCmdNumberOfEntries, true);
	setHeaderLock(idCmdDimMin, true);
	setHeaderLock(idCmdDimMax, true);
	setHeaderLock(idCmdDimStep, true);
	setHeaderLock(idCmdDimStartEntry, true);
	setHeaderLock(idCmdDimStopEntry, true);

}

/****************************************************************
 * This method returns an information string about the object.	*
 ****************************************************************/

std::string prelutAccessFile::getInfo() {

	std::string message;
	char        intBuffer[intConversionDigits + 1];
	char        longBuffer[longConversionDigits + 1];
	char        doubleBuffer[doubleConversion + 1];

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
		sprintf(longBuffer, "%p", getDataPtr());
		message += longBuffer;
	}
	message += "\n";
	message += "Usage";
	message += "\t";
	message += header.usage;
	message += "\n";
	message += "Number of Data";
	message += "\t: ";
	ultos(getDataNum(), longBuffer, 10, longConversionDigits);
	message += longBuffer;
	message += "\n";
	message += stringCmdDimMin;
	message += "\t: ";
	dtos(header.dimMin, doubleBuffer, doubleConversionDigits);
	message += doubleBuffer;
	message += "\n";
	message += stringCmdDimMax;
	message += "\t: ";
	dtos(header.dimMax, doubleBuffer, doubleConversionDigits);
	message += doubleBuffer;
	message += "\n";
	message += stringCmdDimStep;
	message += "\t: ";
	itos(header.dimStep, intBuffer, 10, intConversionDigits);
	message += intBuffer;
	message += "\n";
	message += stringCmdDimStartEntry;
	message += "\t: ";
	dtos(header.dimStartEntry, doubleBuffer, doubleConversionDigits);
	message += doubleBuffer;
	message += "\n";
	message += stringCmdDimStopEntry;
	message += "\t: ";
	dtos(header.dimStopEntry, doubleBuffer, doubleConversionDigits);
	message += doubleBuffer;
	message += "\n";
	message += "\n";
	message += "Setup for the pre-look-up-table file:\n";
	message += "------------------------------------------\n";
	message += "CommandID";
	message += "\t: ";
	for (unsigned int i = 0; i < getNumberOfCmds(); i++) {
		btods(isHeaderLockSet(i), longBuffer);
		message += longBuffer;
	}
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
