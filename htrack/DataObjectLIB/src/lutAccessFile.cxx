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
#include "../include/lutHoughBorder.h"
#include "../include/lutAccessFile.h"


#define max(a, b)  (((a) > (b)) ? (a) : (b)) 


/* Defines the separator which is used in the file to delimit	*
 * a part of the struct which should be written in one line.	*
 * Important is that the newline character is forbidden			*
 */
#define fileDataSeparator '\t'

/*
 * default values for the initialization of each parameter
 */
#define defValName                  "lut"
#define defValUsage                 "LUT"
#define defValFormat                SOFTWAREFORMAT
#define defValNumberOfEntries       0
#define defValStructureSeparator    "TABULATOR"
#define defValBlockSeparator        "NEWLINE"
#define defValContentSoftware       "(pos1, pos2)...(pos1, pos2)"
#define defValFormatSoftware        "([Radix]x[Number],[Radix]x[Number])"defValStructureSeparator"..."defValStructureSeparator"([Radix]x[Number],[Radix]x[Number])"defValBlockSeparator
#define defValContentHardware       "startPos & Command"
#define defValFormatHardware        "([Radix]x[Number],[Radix]x[Number])"defValStructureSeparator"[Binary Number]"defValBlockSeparator


/*
 * stringcommands which represent the parameters read from file
 */
#define stringCmdName               "name"
#define stringCmdUsage              "usage"
#define stringCmdFormat             "format"
#define stringCmdNumberOfEntries    "numberOfEntries"
#define stringCmdStructureSeparator "/**/ structureSeparator"
#define stringCmdBlockSeparator     "/**/ blockSeparator"
#define stringCmdContent            "/**/ content"
#define stringCmdFormatString       "/**/ format"


/*
 * index of each parameter for the commandID table
 */
#define idCmdName                   0
#define idCmdFormat                 1
#define idCmdNumberOfEntries        2


/****************************************************************
 * This method deletes the local memory if it exists.			*
 ****************************************************************/

void lutAccessFile::deleteLocalMemory() {

	if (localMemory) {

/********************************************************/
/* make code changes for allocation number of data here*/
		lutHoughBorder* typeCastedData = (lutHoughBorder*)getDataPtr();
/********************************************************/

		if (typeCastedData != NULL) {
			delete typeCastedData;
			setDataNum(0);
			setDataPtr(NULL);
		}

		localMemory = false;

	}

}

/****************************************************************
 * This method returns the number of accepted commands.			*
 ****************************************************************/

int lutAccessFile::getNumberOfCmds() {

	return numberOfLutAccessFileCmds;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool lutAccessFile::getHeaderValue(std::string& specifier, std::string& value) {

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
	else if (specifier.compare(stringCmdFormat) == 0) {
		if (!(commandID[idCmdFormat])) {
			header.format = stous((char*)value.c_str(), 10);
			commandID[idCmdFormat] = true;
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

void lutAccessFile::readAllData() {

	if ((getDataNum() == 0) && (getDataPtr() == NULL)) {

/********************************************************/
/* make code changes for allocation number of data here*/
		lutHoughBorder* typeCastedData = new lutHoughBorder[header.numberOfEntries];
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

bool lutAccessFile::getDataValue(std::string& buffer, unsigned long index) {

	std::string       temp;
	bool              noError = true;
/********************************************************/
/* make code changes for a different data object here	*/
	lutHoughBorder*     typeCastedData = (lutHoughBorder*) getDataPtr();
	houghBorderCommand  actualCommand;
	unsigned long       coordCounter;
	houghBorderPosition actualPosition;
/********************************************************/

/********************************************************/
/* make code changes for a different data object here	*/
	switch(header.format) {
		case HARDWAREFORMAT:
			temp = getDataString(buffer, 0, fileDataSeparator);
			if (temp.empty())
				noError = false;
			else
				actualCommand.setStartPos(temp);
			temp = getDataString(buffer, 1, fileDataSeparator);
			if (temp.empty())
				noError = false;
			else
				actualCommand.setCmd(temp);
			if (noError)
				typeCastedData[index].setHoughBorderCommand(actualCommand, header.dim1Step, header.dim2Step);
			break;
		default:
			typeCastedData[index].houghCoord.clear();
			coordCounter = 0;
			while (noError) {
				temp = getDataString(buffer, coordCounter, fileDataSeparator);
				if (temp.empty())
					noError = false;
				else {
					actualPosition.fromNotIdentifiedString(temp);
					typeCastedData[index].houghCoord.push(actualPosition);
					coordCounter++;
				}
			}
			if (coordCounter > 0)
				noError = true;
			break;
	}
/********************************************************/
	return noError;

}

/****************************************************************
 * This method writes one data structure to the stream.			*
 * The whole data structure to write must be in one single line	*
 * in the file.													*
 ****************************************************************/

std::string lutAccessFile::setDataValue(unsigned long index) {

	std::string         returnValue;
/********************************************************/
/* make code changes for a different data object here	*/
	lutHoughBorder*     typeCastedData = (lutHoughBorder*) getDataPtr();
	houghBorderCommand  actualCommand;
	houghBorderPosition actualPosition;
/********************************************************/

/********************************************************/
/* make code changes for a different data object here	*/
	returnValue.clear();
	switch (header.format) {
		case HARDWAREFORMAT:
			actualCommand = typeCastedData[index].getHoughBorderCommand(header.dim2Step);
			returnValue += actualCommand.getStartPos();
			returnValue += fileDataSeparator;
			returnValue += actualCommand.getCmd();
			break;
		default:
			typeCastedData[index].houghCoord.resetActiveObject();
			for (unsigned long i = 0; i < typeCastedData[index].houghCoord.getNumberOfEntries(); i++) {
				actualPosition   = typeCastedData[index].houghCoord.readActiveObjectAndMakeNextOneActive();
				if (i > 0)
					returnValue += fileDataSeparator;
				returnValue     += actualPosition.toNotIdentifiedString();
			}
			break;
	}
/********************************************************/

	return returnValue;

}

/****************************************************************
 * This method is to write the header of the file.				*
 * Dependencies:												*
 * - Fileio::setHeaderValue										*
 ****************************************************************/

void lutAccessFile::writeFileHeader(std::ofstream& fileStream) {

/********************************************************/
/* make code changes for a different configuration here */
	setHeaderValue(fileStream, stringCmdName,               header.name,              "Name of the look up table");
	setHeaderValue(fileStream, stringCmdUsage,              defValUsage,              "Usage of the look up table");
	setHeaderValue(fileStream, stringCmdFormat,             header.format,             "Format of the look up table");
	setHeaderValue(fileStream, stringCmdNumberOfEntries,    header.numberOfEntries,   "Number of entries in the look up table");
	setHeaderValue(fileStream, stringCmdStructureSeparator, defValStructureSeparator, "The separator for the members of the data structure");
	setHeaderValue(fileStream, stringCmdBlockSeparator,     defValBlockSeparator,     "The separator for the blocks of the data");
	
	switch(header.format){
	
		case HARDWAREFORMAT:
			setHeaderValue(fileStream, stringCmdContent,            defValContentHardware,    "The content of the file");
			setHeaderValue(fileStream, stringCmdFormatString,       defValFormatHardware,     "The format for an entry in the file");
			break;

		default:
			setHeaderValue(fileStream, stringCmdContent,            defValContentSoftware,    "The content of the file");
			setHeaderValue(fileStream, stringCmdFormatString,       defValFormatSoftware,     "The format for an entry in the file");
			break;

	}
/********************************************************/

}

/****************************************************************
 * This method is to set the default values for each header		*
 * parameter.
 ****************************************************************/

void lutAccessFile::setHeaderDefValues() {

/********************************************************/
/* make code changes for a different configuration here */
	header.name            = defValName;
	header.format          = defValFormat;
	header.numberOfEntries = defValNumberOfEntries;
/********************************************************/

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

lutAccessFile::lutAccessFile() : io() {

	for (unsigned int i = 0; i < numberOfLutAccessFileCmds; i++)
		commandID[i] = 0;

	init();

}

/****************************************************************
 * destructor													*
 ****************************************************************/

lutAccessFile::~lutAccessFile() {

	if (localMemory)
		deleteLocalMemory();

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void lutAccessFile::init() {

	setHeaderDefValues();

	if (localMemory)
		deleteLocalMemory();

}

/****************************************************************
 * This method returns a reference of the data struct.			*
 ****************************************************************/

lutAccessFileHeader& lutAccessFile::getHeaderReference() {

	return header;

}

/****************************************************************
 * This method sets the data struct	to the existing one.		*
 ****************************************************************/

void lutAccessFile::setHeader(lutAccessFileHeader& structure) {

	header.name                     = structure.name;
	header.format                   = structure.format;
	header.numberOfEntries          = structure.numberOfEntries;
	commandID[idCmdName]            = true;
	commandID[idCmdFormat]          = true;
	commandID[idCmdNumberOfEntries] = true;

}

/****************************************************************
 * This method returns an information string about the object.	*
 ****************************************************************/

std::string lutAccessFile::getInfo() {

	std::string message;
	char        longBuffer[longConversionDigits + 1];
	char        shortBuffer[shortConversionDigits+1];

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
	message += "Number of Data";
	message += "\t: ";
	ultos(getDataNum(), longBuffer, 10, longConversionDigits);
	message += longBuffer;
	message += "\n";
	message += "\n";
	message += "Setup for the look-up-table file:\n";
	message += "--------------------------------------\n";
	message += "CommandID";
	message += "\t: ";
	for (int i = 0; i < numberOfLutAccessFileCmds; i++) {
		btods(commandID[i], longBuffer);
		message += longBuffer;
	}
	message += "\n";
	message += stringCmdName;
	message += "\t: ";
	message += header.name;
	message += "\n";
	message += stringCmdUsage;
	message += "\t: ";
	message += defValUsage;
	message += "\n";
	message += stringCmdFormat;
	message += "\t: ";
	ustos(header.format, shortBuffer, 10, shortConversionDigits);
	message += shortBuffer;
	message += "\n";
	message += stringCmdNumberOfEntries;
	message += "\t: ";
	ultos(header.numberOfEntries, longBuffer, 10, longConversionDigits);
	message += longBuffer;
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
	switch (header.format) {
		case HARDWAREFORMAT:
			message += defValContentHardware;
			break;
		default:
			message += defValContentSoftware;
			break;
	}
	message += "\n";
	message += stringCmdFormatString;
	message += "\t: ";
	switch (header.format) {
		case HARDWAREFORMAT:
			message += defValFormatHardware;
			break;
		default:
			message += defValFormatSoftware;
			break;
	}
	message += "\n";
/********************************************************/

	return message;

}
