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
//     - derived from lut
//     - class for the second look-up-table implemented with a file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/11/07 12:48:06 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/lutGeneratorError.h"
#include "../include/lutGeneratorWarningMsg.h"
#include "../include/lutAccess.h"
#include <fstream>


/****************************************************************
 * This method allocates new memory.							*
 ****************************************************************/
	
void lutAccess::allocateNewMemory(unsigned long number) {

	numberOfEntries += number;

	if (lutMem == NULL)
		lutMem = (lutHoughBorder*)calloc(numberOfEntries, sizeof(lutHoughBorder));
	else
		lutMem = (lutHoughBorder*)realloc(lutMem, numberOfEntries * sizeof(lutHoughBorder));

	if (lutMem == NULL)
		throw memoryAllocationError();

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

lutAccess::lutAccess() : lut() {

	lutMem = NULL;

	clear();

}

/****************************************************************
 * constructor													*
 ****************************************************************/

lutAccess::lutAccess(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step) : lut(dim1Min, dim1Max, dim1Step, dim2Min, dim2Max, dim2Step) {

	lutMem = NULL;

	clear();

}

/****************************************************************
 * destructor													*
 ****************************************************************/

lutAccess::~lutAccess() {

	clear();

}

/****************************************************************
 * Method inits the variables based on the detector stations.	*
 ****************************************************************/

void lutAccess::init(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step) {

	clear();
	lut::init(dim1Min, dim1Max, dim1Step, dim2Min, dim2Max, dim2Step);

}

/****************************************************************
 * This method sets the magnetic field to use.					*
 * Caution: This function here has no useful					*
 * implementation. It is just here because in					*
 * another subclass of lut, this function can					*
 * be used and is very important. This function					*
 * is virtual in the base class.								*
 ****************************************************************/

void lutAccess::setMagneticField(trackfinderInputMagneticField* magneticField) {

}

/****************************************************************
 * This method sets the magnetic field factor to use instead of	*
 * the magneticField.											*
 * Caution: This function here has no useful					*
 * implementation. It is just here because in					*
 * another subclass of lut, this function can					*
 * be used and is very important. This function					*
 * is virtual in the base class.								*
 ****************************************************************/

void lutAccess::setMagneticFieldFactor(double magneticFieldFactor) {

}

/****************************************************************
 * This method returns the number of corrections.				*
 * Caution: This function here has no useful					*
 * implementation. It is just here because in					*
 * another subclass of lut, this function can					*
 * be used and is very important. This function					*
 * is virtual in the base class.								*
 ****************************************************************/

unsigned long lutAccess::getNumberOfCorrections() {

	return 0;

}

/****************************************************************
 * This method returns the number of coord corrections.			*
 * Caution: This function here has no useful					*
 * implementation. It is just here because in					*
 * another subclass of lut, this function can					*
 * be used and is very important. This function					*
 * is virtual in the base class.								*
 ****************************************************************/

unsigned long lutAccess::getNumberOfCoordCorrections() {

	return 0;

}

/****************************************************************
 * This method resets both correction counters.					*
 * Caution: This function here has no useful					*
 * implementation. It is just here because in					*
 * another subclass of lut, this function can					*
 * be used and is very important. This function					*
 * is virtual in the base class.								*
 ****************************************************************/

void lutAccess::resetCorrectionCounter() {

}

/****************************************************************
 * This method evaluates the value from the prelut table.		*
 ****************************************************************/

void lutAccess::evaluate(trackfinderInputHit* hit, lutHoughBorder* borderPointer) {

	if (hit == NULL)
		throw noHitError();
/**/
	if (borderPointer != NULL)
		*borderPointer = getEntry(0); //hit->digitize
	else
		border         = getEntry(0); //hit->digitize

}

/****************************************************************
 * This method clears the lut table.							*
 ****************************************************************/
	
void lutAccess::clear() {

	if (lutMem != NULL) {
		free(lutMem);
		lutMem = NULL;
	}

	numberOfEntries = 0;

}

/****************************************************************
 * This method returns the value from the lut table.			*
 ****************************************************************/
	
lutHoughBorder lutAccess::getEntry(unsigned long index) {

	lutHoughBorder returnValue;

	if (index < numberOfEntries) {

		if (lutMem == NULL)
			throw cannotAccessLutMemError();

		returnValue = lutMem[index];

	}
	else {

		throw tooBigIndexForMemoryError(index, numberOfEntries);
	
		returnValue = lutHoughBorder();
	
	}

	return returnValue;

}

/****************************************************************
 * This method adds the value at the end of the prelut table.	*
 ****************************************************************/
	
void lutAccess::addEntry(lutHoughBorder value) {

	allocateNewMemory(1);

	lutMem[numberOfEntries - 1] = value;

}

/****************************************************************
 * This method converts the prelut table into a string.			*
 ****************************************************************/
	
std::string lutAccess::toString() {

	std::string returnValue;

	returnValue.clear();
	returnValue += def.toString();
	
	if (numberOfEntries == 0) {

		returnValue += "\n";
		returnValue += "No entry";

	}
	else {

		if (lutMem == NULL)
			throw cannotAccessLutMemError();
		
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

void lutAccess::read(std::string fileName) {

	unsigned short  countDifferentLutDefinitionAsFile;
	lutAccessFile   readFile;

	countDifferentLutDefinitionAsFile = 0;

	readFile.setFileName(fileName);

	readFile.readJustFileHeader();

	lutAccessFileHeader& fileHeader = readFile.getHeaderReference();

	if (def.dim1Min != fileHeader.dim1Min)
		countDifferentLutDefinitionAsFile++;
	if (def.dim1Max != fileHeader.dim1Max)
		countDifferentLutDefinitionAsFile++;
	if (def.dim1Step != fileHeader.dim1Step)
		countDifferentLutDefinitionAsFile++;
	if (def.dim2Min != fileHeader.dim2Min)
		countDifferentLutDefinitionAsFile++;
	if (def.dim2Max != fileHeader.dim2Max)
		countDifferentLutDefinitionAsFile++;
	if (def.dim2Step != fileHeader.dim2Step)
		countDifferentLutDefinitionAsFile++;

	if (countDifferentLutDefinitionAsFile != 0) {

		differentLutDefinitionAsFileDetectedWarningMsg* differentLutDefinitionAsFileDetected = new differentLutDefinitionAsFileDetectedWarningMsg(countDifferentLutDefinitionAsFile);
		differentLutDefinitionAsFileDetected->warningMsg();
		if(differentLutDefinitionAsFileDetected != NULL) {
			delete differentLutDefinitionAsFileDetected;
			differentLutDefinitionAsFileDetected = NULL;
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

void lutAccess::write(std::string fileName, std::string name, unsigned short format) {

	lutAccessFileHeader fileHeader;
	lutAccessFile       writeFile;

	if (numberOfEntries == 0) {

		cannotWriteEmptyLutWarningMsg* cannotWriteEmptyLut = new cannotWriteEmptyLutWarningMsg();
		cannotWriteEmptyLut->warningMsg();
		if(cannotWriteEmptyLut != NULL) {
			delete cannotWriteEmptyLut;
			cannotWriteEmptyLut = NULL;
		}

	}
	else {

		if (lutMem == NULL)
			throw cannotAccessLutMemError();

		fileHeader.name            = name;
		fileHeader.format          = format;
		fileHeader.numberOfEntries = numberOfEntries;
		fileHeader.dim1Min         = def.dim1Min;
		fileHeader.dim1Max         = def.dim1Max;
		fileHeader.dim1Step        = def.dim1Step;
		fileHeader.dim2Min         = def.dim2Min;
		fileHeader.dim2Max         = def.dim2Max;
		fileHeader.dim2Step        = def.dim2Step;

		writeFile.setFileName(fileName);

		writeFile.setDataNum(fileHeader.numberOfEntries);

		writeFile.setDataPtr(lutMem);

		writeFile.setHeader(fileHeader);

		writeFile.writeFile();

	}

}
