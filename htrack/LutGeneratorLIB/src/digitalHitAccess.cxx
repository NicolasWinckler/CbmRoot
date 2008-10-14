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
//     - class for the digital hit look-up-table implemented with a file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-10 13:50:00 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../../DataObjectLIB/include/digitalHitAccessFile.h"
#include "../include/lutGeneratorError.h"
#include "../include/lutGeneratorWarningMsg.h"
#include "../include/digitalHitAccess.h"
#include <fstream>


#define standardUsage "DIGITALHIT"


/****************************************************************
 * This method allocates new memory.							*
 ****************************************************************/
	
void digitalHitAccess::allocateNewMemory(unsigned long number) {

	numberOfEntries += number;

	if (lutMem == NULL)
		lutMem = (digitalHit*)calloc(numberOfEntries, sizeof(digitalHit));
	else
		lutMem = (digitalHit*)realloc(lutMem, numberOfEntries * sizeof(digitalHit));

	if (lutMem == NULL)
		throw memoryAllocationError();

}

/****************************************************************
 * Default Constructor											*
 ****************************************************************/

digitalHitAccess::digitalHitAccess() {

	lutMem = NULL;

	clear();

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

digitalHitAccess::~digitalHitAccess() {

	clear();

}

/****************************************************************
 * Method inits the variables based on the detector stations.	*
 ****************************************************************/

void digitalHitAccess::init() {

	clear();

}

/****************************************************************
 * This method evaluates the value from the prelut table.		*
 ****************************************************************/

#if (ARCHITECTURE != PS3)

void digitalHitAccess::evaluate(trackfinderInputHit* hit, digitalHit* digitalHitPointer) {

	if (hit == NULL)
		throw noHitError();
/**/
	if (digitalHitPointer != NULL)
		digitalHitPointer->setIdentifier(numberOfEntries); //hit->digitize

}

#endif

/****************************************************************
 * This method clears the prelut table.							*
 ****************************************************************/
	
void digitalHitAccess::clear() {

	if (lutMem != NULL) {
		free(lutMem);
		lutMem = NULL;
	}

	numberOfEntries = 0;

}

/****************************************************************
 * method returns the number of entries							*
 ****************************************************************/

unsigned long digitalHitAccess::getNumberOfEntries() {

	return numberOfEntries;

}
unsigned long digitalHitAccess::getNumberOfMembers() {

	return getNumberOfEntries();

}

/****************************************************************
 * This method returns the value from the prelut table.			*
 ****************************************************************/

digitalHit digitalHitAccess::getEntry(unsigned long index) {

	digitalHit returnValue;

	if (index < numberOfEntries) {

		if (lutMem == NULL)
			throw cannotAccessDigitalHitLutError();

		returnValue = lutMem[index];

	}
	else {

		throw tooBigIndexForDigitalHitLutError(index, numberOfEntries);
	
		returnValue = digitalHit();
	
	}

	return returnValue;

}
digitalHit digitalHitAccess::getMember(unsigned long index) {

	return getEntry(index);

}

/****************************************************************
 * This method adds the value at the end of the prelut table.	*
 ****************************************************************/
	
void digitalHitAccess::addEntry(digitalHit& value) {

	allocateNewMemory(1);

	lutMem[numberOfEntries - 1] = value;

}

/****************************************************************
 * This method converts the prelut table into a string.			*
 ****************************************************************/
	
std::string digitalHitAccess::toString() {

	std::string returnValue;

	returnValue.clear();

	if (numberOfEntries == 0) {

		returnValue += "\n";
		returnValue += "No entry";

	}
	else {

		if (lutMem == NULL)
			throw cannotAccessDigitalHitLutError();

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

void digitalHitAccess::read(std::string fileName) {

	digitalHitAccessFile readFile;

	readFile.setFileName(fileName);

	readFile.readJustFileHeader();

	digitalHitAccessFileHeader& fileHeader = readFile.getHeaderReference();

	if (fileHeader.usage != standardUsage) {

		differentDigitalHitLutUsageAsFileDetectedWarningMsg* differentDigitalHitLutUsageAsFileDetected = new differentDigitalHitLutUsageAsFileDetectedWarningMsg();
		differentDigitalHitLutUsageAsFileDetected->warningMsg();
		if(differentDigitalHitLutUsageAsFileDetected != NULL) {
			delete differentDigitalHitLutUsageAsFileDetected;
			differentDigitalHitLutUsageAsFileDetected = NULL;
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

void digitalHitAccess::write(std::string fileName, std::string name) {

	digitalHitAccessFileHeader fileHeader;
	digitalHitAccessFile       writeFile;

	if (numberOfEntries == 0) {

		cannotWriteEmptyDigitalHitLutWarningMsg* cannotWriteEmptyDigitalHitLut = new cannotWriteEmptyDigitalHitLutWarningMsg();
		cannotWriteEmptyDigitalHitLut->warningMsg();
		if(cannotWriteEmptyDigitalHitLut != NULL) {
			delete cannotWriteEmptyDigitalHitLut;
			cannotWriteEmptyDigitalHitLut = NULL;
		}

	}
	else {

		if (lutMem == NULL)
			throw cannotAccessDigitalHitLutError();

		fileHeader.name            = name;
		fileHeader.usage           = standardUsage;
		fileHeader.numberOfEntries = numberOfEntries;

		writeFile.setFileName(fileName);

		writeFile.setDataNum(fileHeader.numberOfEntries);

		writeFile.setDataPtr(lutMem);

		writeFile.setHeader(fileHeader);

		writeFile.writeFile();

	}

}
