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
//     - derived from lut
//     - class for the second look-up-table implemented with a file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-10 13:50:01 $
// $Revision: 1.6 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/lutGeneratorError.h"
#include "../include/lutGeneratorWarningMsg.h"
#include "../include/lutAccess.h"
#include <fstream>


#define standardUsage "LUT"


/****************************************************************
 * This method allocates new memory.							*
 ****************************************************************/
	
void lutAccess::allocateNewMemory(unsigned long number) {

	numberOfEntries += number;

	if (lutMem == NULL)
		lutMem = (lutHoughBorder*)calloc(numberOfEntries, sizeof(lutHoughBorder));
	else {

		lutMem = (lutHoughBorder*)realloc(lutMem, numberOfEntries * sizeof(lutHoughBorder));
		if (number > 0)
			memset(&lutMem[numberOfEntries - number], 0, number * sizeof(lutHoughBorder));

	}

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
 * This method returns the magnetic field to use.				*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

#if (ARCHITECTURE != PS3)

trackfinderInputMagneticField* lutAccess::getMagneticField() {

	return NULL;

}

#endif

/****************************************************************
 * This method returns the magnetic field factor to use instead	*
 * of the magneticField.										*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

double lutAccess::getMagneticFieldFactor() {

	return 0;

}

/****************************************************************
 * This method sets the magnetic field to use.					*
 * Caution: This function here has no useful					*
 * implementation. It is just here because in					*
 * another subclass of lut, this function can					*
 * be used and is very important. This function					*
 * is virtual in the base class.								*
 ****************************************************************/

#if (ARCHITECTURE != PS3)

void lutAccess::setMagneticField(trackfinderInputMagneticField* magneticField) {

}

#endif

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

#if (ARCHITECTURE != PS3)

void lutAccess::evaluate(trackfinderInputHit* hit, lutHoughBorder* borderPointer) {

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
 * method returns the number of entries							*
 ****************************************************************/

unsigned long lutAccess::getNumberOfEntries() {

	return numberOfEntries;

}
unsigned long lutAccess::getNumberOfMembers() {

	return getNumberOfEntries();

}

/****************************************************************
 * This method returns the value from the lut table.			*
 ****************************************************************/
	
lutHoughBorder lutAccess::getEntry(unsigned long index) {

	lutHoughBorder returnValue;

	if (index < numberOfEntries) {

		if (lutMem == NULL)
			throw cannotAccessLutError();

		returnValue = lutMem[index];

	}
	else {

		throw tooBigIndexForLutError(index, numberOfEntries);
	
		returnValue = lutHoughBorder();
	
	}

	return returnValue;

}
lutHoughBorder lutAccess::getMember(unsigned long index) {

	return getEntry(index);

}

/****************************************************************
 * This method adds the value at the end of the prelut table.	*
 ****************************************************************/
	
void lutAccess::addEntry(lutHoughBorder& value) {

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
			throw cannotAccessLutError();
		
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
	std::string     definitionString;
	std::string     fileHeaderString;

	countDifferentLutDefinitionAsFile = 0;

	readFile.setFileName(fileName);

	readFile.readJustFileHeader();

	lutAccessFileHeader& fileHeader = readFile.getHeaderReference();

	if (fileHeader.usage != standardUsage) {

		differentLutUsageAsFileDetectedWarningMsg* differentLutUsageAsFileDetected = new differentLutUsageAsFileDetectedWarningMsg();
		differentLutUsageAsFileDetected->warningMsg();
		if(differentLutUsageAsFileDetected != NULL) {
			delete differentLutUsageAsFileDetected;
			differentLutUsageAsFileDetected = NULL;
		}

	}

	dtos(def.dim1Min,        &definitionString, doubleConversionDigits);
	dtos(fileHeader.dim1Min, &fileHeaderString, doubleConversionDigits);
	if (definitionString != fileHeaderString)
		countDifferentLutDefinitionAsFile++;
	dtos(def.dim1Max,        &definitionString, doubleConversionDigits);
	dtos(fileHeader.dim1Max, &fileHeaderString, doubleConversionDigits);
	if (definitionString != fileHeaderString)
		countDifferentLutDefinitionAsFile++;
	if (def.dim1Step != fileHeader.dim1Step)
		countDifferentLutDefinitionAsFile++;
	dtos(def.dim2Min,        &definitionString, doubleConversionDigits);
	dtos(fileHeader.dim2Min, &fileHeaderString, doubleConversionDigits);
	if (definitionString != fileHeaderString)
		countDifferentLutDefinitionAsFile++;
	dtos(def.dim2Max,        &definitionString, doubleConversionDigits);
	dtos(fileHeader.dim2Max, &fileHeaderString, doubleConversionDigits);
	if (definitionString != fileHeaderString)
		countDifferentLutDefinitionAsFile++;
	dtos(def.dim2Step,        &definitionString, doubleConversionDigits);
	dtos(fileHeader.dim2Step, &fileHeaderString, doubleConversionDigits);
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
			throw cannotAccessLutError();

		fileHeader.name            = name;
		fileHeader.usage           = standardUsage;
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
