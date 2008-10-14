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
//     - derived from prelut
//     - class for the first look-up-table implemented with a file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-10 13:50:01 $
// $Revision: 1.6 $
//
// *******************************************************************/


#include "../../DataObjectLIB/include/prelutAccessFile.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/lutGeneratorError.h"
#include "../include/lutGeneratorWarningMsg.h"
#include "../include/prelutAccess.h"
#include <fstream>


#define standardUsage "PRELUT"


/****************************************************************
 * This method allocates new memory.							*
 ****************************************************************/
	
void prelutAccess::allocateNewMemory(unsigned long number) {

	numberOfEntries += number;

	if (lutMem == NULL)
		lutMem = (prelutHoughBorder*)calloc(numberOfEntries, sizeof(prelutHoughBorder));
	else
		lutMem = (prelutHoughBorder*)realloc(lutMem, numberOfEntries * sizeof(prelutHoughBorder));

	if (lutMem == NULL)
		throw memoryAllocationError();

}

/****************************************************************
 * Default Constructor											*
 ****************************************************************/

prelutAccess::prelutAccess() : prelut() {

	lutMem = NULL;

	clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

prelutAccess::prelutAccess(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry) : prelut(dim3Min, dim3Max, dim3Step, dim3StartEntry, dim3StopEntry) {

	lutMem = NULL;

	clear();

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

prelutAccess::~prelutAccess() {

	clear();

}

/****************************************************************
 * Method inits the variables based on the detector stations.	*
 ****************************************************************/

void prelutAccess::init(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry) {

	clear();
	prelut::init(dim3Min, dim3Max, dim3Step, dim3StartEntry, dim3StopEntry);

}

/****************************************************************
 * This method evaluates the value from the prelut table.		*
 ****************************************************************/

#if (ARCHITECTURE != PS3)

void prelutAccess::evaluate(trackfinderInputHit* hit, prelutHoughBorder* borderPointer) {

	if (hit == NULL)
		throw noHitError();
/**/
	if (borderPointer != NULL)
		*borderPointer = getEntry(0); //hit->digitize
	else
		border         = getEntry(0); //hit->digitize

}

#endif

/****************************************************************
 * This method clears the prelut table.							*
 ****************************************************************/
	
void prelutAccess::clear() {

	if (lutMem != NULL) {
		free(lutMem);
		lutMem = NULL;
	}

	numberOfEntries = 0;

}

/****************************************************************
 * method returns the number of entries							*
 ****************************************************************/

unsigned long prelutAccess::getNumberOfEntries() {

	return numberOfEntries;

}
unsigned long prelutAccess::getNumberOfMembers() {

	return getNumberOfEntries();

}

/****************************************************************
 * This method returns the value from the prelut table.			*
 ****************************************************************/
	
prelutHoughBorder prelutAccess::getEntry(unsigned long index) {

	prelutHoughBorder returnValue;

	if (index < numberOfEntries) {

		if (lutMem == NULL)
			throw cannotAccessPrelutError();

		returnValue = lutMem[index];

	}
	else {

		throw tooBigIndexForPrelutError(index, numberOfEntries);
	
		returnValue = prelutHoughBorder();
	
	}

	return returnValue;

}
prelutHoughBorder prelutAccess::getMember(unsigned long index) {

	return getEntry(index);

}

/****************************************************************
 * This method adds the value at the end of the prelut table.	*
 ****************************************************************/
	
void prelutAccess::addEntry(prelutHoughBorder& value) {

	allocateNewMemory(1);

	lutMem[numberOfEntries - 1] = value;

}

/****************************************************************
 * This method converts the prelut table into a string.			*
 ****************************************************************/
	
std::string prelutAccess::toString() {

	std::string returnValue;

	returnValue.clear();
	returnValue += def.toString();

	if (numberOfEntries == 0) {

		returnValue += "\n";
		returnValue += "No entry";

	}
	else {

		if (lutMem == NULL)
			throw cannotAccessPrelutError();

		for (unsigned long i = 0; i < numberOfEntries; i++) {

			returnValue += "\n";
			returnValue += lutMem[i].toIdentifiedString();

		}

	}

	return returnValue;

}

/****************************************************************
 * method reads a file to get the table							*
 ****************************************************************/

void prelutAccess::read(std::string fileName) {

	unsigned short   countDifferentPrelutDefinitionAsFile;
	prelutAccessFile readFile;
	std::string      definitionString;
	std::string      fileHeaderString;

	countDifferentPrelutDefinitionAsFile = 0;

	readFile.setFileName(fileName);

	readFile.readJustFileHeader();

	prelutAccessFileHeader& fileHeader = readFile.getHeaderReference();

	if (fileHeader.usage != standardUsage) {

		differentPrelutUsageAsFileDetectedWarningMsg* differentPrelutUsageAsFileDetected = new differentPrelutUsageAsFileDetectedWarningMsg();
		differentPrelutUsageAsFileDetected->warningMsg();
		if(differentPrelutUsageAsFileDetected != NULL) {
			delete differentPrelutUsageAsFileDetected;
			differentPrelutUsageAsFileDetected = NULL;
		}

	}

	dtos(def.dim3Min,       &definitionString, doubleConversionDigits);
	dtos(fileHeader.dimMin, &fileHeaderString, doubleConversionDigits);
	if (definitionString != fileHeaderString)
		countDifferentPrelutDefinitionAsFile++;
	dtos(def.dim3Max,       &definitionString, doubleConversionDigits);
	dtos(fileHeader.dimMax, &fileHeaderString, doubleConversionDigits);
	if (definitionString != fileHeaderString)
		countDifferentPrelutDefinitionAsFile++;
	if (def.dim3Step != fileHeader.dimStep)
		countDifferentPrelutDefinitionAsFile++;
	dtos(def.dim3StartEntry,       &definitionString, doubleConversionDigits);
	dtos(fileHeader.dimStartEntry, &fileHeaderString, doubleConversionDigits);
	if (definitionString != fileHeaderString)
		countDifferentPrelutDefinitionAsFile++;
	dtos(def.dim3StopEntry,       &definitionString, doubleConversionDigits);
	dtos(fileHeader.dimStopEntry, &fileHeaderString, doubleConversionDigits);
	if (definitionString != fileHeaderString)
		countDifferentPrelutDefinitionAsFile++;

	if (countDifferentPrelutDefinitionAsFile != 0) {

		differentPrelutDefinitionAsFileDetectedWarningMsg* differentPrelutDefinitionAsFileDetected = new differentPrelutDefinitionAsFileDetectedWarningMsg(countDifferentPrelutDefinitionAsFile);
		differentPrelutDefinitionAsFileDetected->warningMsg();
		if(differentPrelutDefinitionAsFileDetected != NULL) {
			delete differentPrelutDefinitionAsFileDetected;
			differentPrelutDefinitionAsFileDetected = NULL;
		}

	}

	clear();

	allocateNewMemory(fileHeader.numberOfEntries);

	readFile.setDataNum(numberOfEntries);

	readFile.setDataPtr(lutMem);

	readFile.readFile();

}

/****************************************************************
 * method writes a file representing the table					*
 ****************************************************************/

void prelutAccess::write(std::string fileName, std::string name) {

	prelutAccessFileHeader fileHeader;
	prelutAccessFile       writeFile;

	if (numberOfEntries == 0) {

		cannotWriteEmptyPrelutWarningMsg* cannotWriteEmptyPrelut = new cannotWriteEmptyPrelutWarningMsg();
		cannotWriteEmptyPrelut->warningMsg();
		if(cannotWriteEmptyPrelut != NULL) {
			delete cannotWriteEmptyPrelut;
			cannotWriteEmptyPrelut = NULL;
		}

	}
	else {

		if (lutMem == NULL)
			throw cannotAccessPrelutError();

		fileHeader.name            = name;
		fileHeader.usage           = standardUsage;
		fileHeader.numberOfEntries = numberOfEntries;
		fileHeader.dimMin          = def.dim3Min;
		fileHeader.dimMax          = def.dim3Max;
		fileHeader.dimStep         = def.dim3Step;
		fileHeader.dimStartEntry   = def.dim3StartEntry;
		fileHeader.dimStopEntry    = def.dim3StopEntry;

		writeFile.setFileName(fileName);

		writeFile.setDataNum(fileHeader.numberOfEntries);

		writeFile.setDataPtr(lutMem);

		writeFile.setHeader(fileHeader);

		writeFile.writeFile();

	}

}
