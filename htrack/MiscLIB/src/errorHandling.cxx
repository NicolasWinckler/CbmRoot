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
//     - base class for errors
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-06-06 14:21:08 $
// $Revision: 1.6 $
//
// *******************************************************************/


#include "../include/errorHandling.h"
#include <iostream>


/****************************************************************
 * CLASS errorHandling							 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

errorHandling::errorHandling() {

	errorSource.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

errorHandling::errorHandling(std::string errorSource) {

	this->errorSource = errorSource;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

errorHandling::~errorHandling() {

}

/****************************************************************
 * This method prints a message with a leading source specifier.*
 ****************************************************************/

void errorHandling::printMsg(std::string message) {

#ifndef NOERRORMESSAGE 
	
	std::cerr << std::endl;

	if (!errorSource.empty())
		std::cerr << errorSource << " ";
	
	std::cerr << "ERROR: ";
	
	if (!message.empty())
		std::cerr << message;
	
	std::cerr << std::endl;

#endif

}

/****************************************************************
 * CLASS cannotOpenFileError							 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotOpenFileError::cannotOpenFileError() : errorHandling() {

	this->fileName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotOpenFileError::cannotOpenFileError(std::string errorSource, std::string fileName) : errorHandling(errorSource) {

	this->fileName = fileName;

}
cannotOpenFileError::cannotOpenFileError(std::string errorSource, const char* fileName) : errorHandling(errorSource) {

	this->fileName = fileName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotOpenFileError::~cannotOpenFileError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotOpenFileError::errorMsg() {

	std::string temp;

	if (fileName.empty())
		temp = "The file cannot be opened!!!";
	else {

		temp  = "The file '";
		temp += fileName;
		temp += "' cannot be opened!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS cannotCloseFileError							 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotCloseFileError::cannotCloseFileError() : errorHandling() {

	this->fileName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotCloseFileError::cannotCloseFileError(std::string errorSource, std::string fileName) : errorHandling(errorSource) {

	this->fileName = fileName;

}
cannotCloseFileError::cannotCloseFileError(std::string errorSource, const char* fileName) : errorHandling(errorSource) {

	this->fileName = fileName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotCloseFileError::~cannotCloseFileError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotCloseFileError::errorMsg() {

	std::string temp;

	if (fileName.empty())
		temp = "The file cannot be closed!!!";
	else {

		temp  = "The file '";
		temp += fileName;
		temp += "' cannot be closed!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS cannotCreateFileError							 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotCreateFileError::cannotCreateFileError() : errorHandling() {

	this->fileName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotCreateFileError::cannotCreateFileError(std::string errorSource, std::string fileName) : errorHandling(errorSource) {

	this->fileName = fileName;

}
cannotCreateFileError::cannotCreateFileError(std::string errorSource, const char* fileName) : errorHandling(errorSource) {

	this->fileName = fileName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotCreateFileError::~cannotCreateFileError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotCreateFileError::errorMsg() {

	std::string temp;

	if (fileName.empty())
		temp = "The file cannot be created!!!";
	else {

		temp  = "The file '";
		temp += fileName;
		temp += "' cannot be created!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS unexpectedEofFoundError						 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

unexpectedEofFoundError::unexpectedEofFoundError() : errorHandling() {

	this->fileName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

unexpectedEofFoundError::unexpectedEofFoundError(std::string errorSource, std::string fileName) : errorHandling(errorSource) {

	this->fileName = fileName;

}
unexpectedEofFoundError::unexpectedEofFoundError(std::string errorSource, const char* fileName) : errorHandling(errorSource) {

	this->fileName = fileName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

unexpectedEofFoundError::~unexpectedEofFoundError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void unexpectedEofFoundError::errorMsg() {

	std::string temp;

	if (fileName.empty())
		temp = "The EOF marker is found but not expected!!!";
	else {

		temp  = "The EOF marker in the file '";
		temp += fileName;
		temp += "' is found but not expected!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS noFilenameSpecifiedError						 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noFilenameSpecifiedError::noFilenameSpecifiedError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

noFilenameSpecifiedError::noFilenameSpecifiedError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noFilenameSpecifiedError::~noFilenameSpecifiedError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void noFilenameSpecifiedError::errorMsg() {

	printMsg("No filename is specified!!!");

}


/****************************************************************
 * CLASS memoryAllocationError							 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

memoryAllocationError::memoryAllocationError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

memoryAllocationError::memoryAllocationError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

memoryAllocationError::~memoryAllocationError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void memoryAllocationError::errorMsg() {

	printMsg("Some memory allocation fails!!!");

}


/****************************************************************
 * CLASS cannotAccessHitsOrTracksError							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHitsOrTracksError::cannotAccessHitsOrTracksError() : errorHandling() {

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHitsOrTracksError::cannotAccessHitsOrTracksError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHitsOrTracksError::~cannotAccessHitsOrTracksError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHitsOrTracksError::errorMsg() {

	printMsg("The hits or the tracks are not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessEventDataError								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessEventDataError::cannotAccessEventDataError() : errorHandling() {

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessEventDataError::cannotAccessEventDataError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessEventDataError::~cannotAccessEventDataError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessEventDataError::errorMsg() {

	printMsg("The sourcedata of the event is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessHistogramDataError							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHistogramDataError::cannotAccessHistogramDataError() : errorHandling() {

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHistogramDataError::cannotAccessHistogramDataError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHistogramDataError::~cannotAccessHistogramDataError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHistogramDataError::errorMsg() {

	printMsg("The sourcedata of the histogram is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessTrackDataError								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessTrackDataError::cannotAccessTrackDataError() : errorHandling() {

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessTrackDataError::cannotAccessTrackDataError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessTrackDataError::~cannotAccessTrackDataError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessTrackDataError::errorMsg() {

	printMsg("The data of the evaluated tracks is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessRootManagerError							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessRootManagerError::cannotAccessRootManagerError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotAccessRootManagerError::cannotAccessRootManagerError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessRootManagerError::~cannotAccessRootManagerError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessRootManagerError::errorMsg() {

	printMsg("The root-file-manager is not accessible!!!");

}


/****************************************************************
 * CLASS noDetectorError										*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noDetectorError::noDetectorError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

noDetectorError::noDetectorError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noDetectorError::~noDetectorError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void noDetectorError::errorMsg() {

	printMsg("There is no detector, which can be accessed!!!");

}


/****************************************************************
 * CLASS outOfRangeError										*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

outOfRangeError::outOfRangeError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

outOfRangeError::outOfRangeError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

outOfRangeError::~outOfRangeError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void outOfRangeError::errorMsg() {

	printMsg("The index is out of range!!!");

}


/****************************************************************
 * CLASS zeroDivisionError										*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

zeroDivisionError::zeroDivisionError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

zeroDivisionError::zeroDivisionError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

zeroDivisionError::~zeroDivisionError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void zeroDivisionError::errorMsg() {

	printMsg("The division through zero is not possible!!!");

}


/****************************************************************
 * CLASS zeroModuloError										*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

zeroModuloError::zeroModuloError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

zeroModuloError::zeroModuloError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

zeroModuloError::~zeroModuloError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void zeroModuloError::errorMsg() {

	printMsg("The modulo with zero is not possible!!!");

}


/****************************************************************
 * CLASS cannotAccessStationError								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessStationError::cannotAccessStationError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotAccessStationError::cannotAccessStationError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessStationError::~cannotAccessStationError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessStationError::errorMsg() {

	printMsg("The station of this hit cannot be accessed!!!");

}


/****************************************************************
 * CLASS cannotReadEmptyMemoryError				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotReadEmptyMemoryError::cannotReadEmptyMemoryError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotReadEmptyMemoryError::cannotReadEmptyMemoryError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotReadEmptyMemoryError::~cannotReadEmptyMemoryError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotReadEmptyMemoryError::errorMsg() {

	printMsg("You cannot read something out of an empty memory!!!");

}


/****************************************************************
 * CLASS cannotAccessHistogramSpaceError		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHistogramSpaceError::cannotAccessHistogramSpaceError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotAccessHistogramSpaceError::cannotAccessHistogramSpaceError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHistogramSpaceError::~cannotAccessHistogramSpaceError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHistogramSpaceError::errorMsg() {

	printMsg("The histogram space object is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessMagneticFieldObjectError	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessMagneticFieldObjectError::cannotAccessMagneticFieldObjectError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotAccessMagneticFieldObjectError::cannotAccessMagneticFieldObjectError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessMagneticFieldObjectError::~cannotAccessMagneticFieldObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessMagneticFieldObjectError::errorMsg() {

	printMsg("The magnetic field object is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessTablesError								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessTablesError::cannotAccessTablesError() : errorHandling() {

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessTablesError::cannotAccessTablesError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessTablesError::~cannotAccessTablesError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessTablesError::errorMsg() {

	printMsg("The tabels for the ratings are not accessible!!!");

}


/****************************************************************
 * CLASS functionIsDeprecatedError								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

functionIsDeprecatedError::functionIsDeprecatedError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

functionIsDeprecatedError::functionIsDeprecatedError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

functionIsDeprecatedError::~functionIsDeprecatedError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void functionIsDeprecatedError::errorMsg() {

	printMsg("The function throwing this error is deprecated and cannot be used due to some object problems!!!");

}


/****************************************************************
 * CLASS cannotAccessLutError						 			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessLutError::cannotAccessLutError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotAccessLutError::cannotAccessLutError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessLutError::~cannotAccessLutError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessLutError::errorMsg() {

	printMsg("The look-up-tables for the Hough transform are not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessDistributionMemoryError		 			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessDistributionMemoryError::cannotAccessDistributionMemoryError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotAccessDistributionMemoryError::cannotAccessDistributionMemoryError(std::string errorSource) : errorHandling(errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessDistributionMemoryError::~cannotAccessDistributionMemoryError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessDistributionMemoryError::errorMsg() {

	printMsg("The memory to store the distribution is not accessible!!!");

}
