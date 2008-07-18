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
//     - derived from errorHandling
//     - class for errors occuring while doing some analysis operations
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-06-06 14:18:41 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../../MiscLIB/include/projects.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/analysisError.h"


/****************************************************************
 * CLASS analysisError							 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

analysisError::analysisError() : errorHandling(ANALYSISLIB) {

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

analysisError::~analysisError() {

}


/****************************************************************
 * CLASS windowDivisionError							 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

windowDivisionError::windowDivisionError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

windowDivisionError::~windowDivisionError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void windowDivisionError::errorMsg() {

	printMsg("The computing of the window division is wrong!!!");

}


/****************************************************************
 * CLASS windowNotFoundError							 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

windowNotFoundError::windowNotFoundError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

windowNotFoundError::~windowNotFoundError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void windowNotFoundError::errorMsg() {

	printMsg("The window is not found!!!");

}


/****************************************************************
 * CLASS fakePassingGradingRError						 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

fakePassingGradingRError::fakePassingGradingRError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

fakePassingGradingRError::~fakePassingGradingRError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void fakePassingGradingRError::errorMsg() {

	printMsg("A fake passes the gradingR table. You have to change the table so that this situation cannot occur any longer!!!");

}


/****************************************************************
 * CLASS magneticFieldNotFoundError						 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

magneticFieldNotFoundError::magneticFieldNotFoundError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

magneticFieldNotFoundError::~magneticFieldNotFoundError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void magneticFieldNotFoundError::errorMsg() {

	printMsg("The magnetic field is not found!!!");

}


/****************************************************************
 * CLASS cannotAccessMagnetfieldAnalyser				 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessMagnetfieldAnalyser::cannotAccessMagnetfieldAnalyser() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessMagnetfieldAnalyser::~cannotAccessMagnetfieldAnalyser() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessMagnetfieldAnalyser::errorMsg() {

	printMsg("The magnetfield analyser object cannot be accessed!!!");

}


/****************************************************************
 * CLASS cannotAccessDisplayInfoError					 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessDisplayInfoError::cannotAccessDisplayInfoError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessDisplayInfoError::~cannotAccessDisplayInfoError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessDisplayInfoError::errorMsg() {

	printMsg("The display information cannot be accessed!!!");

}


/****************************************************************
 * CLASS cannotAccessFindableTracksMemoryError			 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessFindableTracksMemoryError::cannotAccessFindableTracksMemoryError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessFindableTracksMemoryError::~cannotAccessFindableTracksMemoryError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessFindableTracksMemoryError::errorMsg() {

	printMsg("The memory to store the findableTracks cannot be accessed!!!");

}


/****************************************************************
 * CLASS cannotAccessFirstAnalysisLutError				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessFirstAnalysisLutError::cannotAccessFirstAnalysisLutError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessFirstAnalysisLutError::~cannotAccessFirstAnalysisLutError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessFirstAnalysisLutError::errorMsg() {

	printMsg("The first look-up-table for the magnetfield analysis is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessSecondAnalysisLutError				 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessSecondAnalysisLutError::cannotAccessSecondAnalysisLutError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessSecondAnalysisLutError::~cannotAccessSecondAnalysisLutError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessSecondAnalysisLutError::errorMsg() {

	printMsg("The second look-up-table for the magnetfield analysis is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessAllMagnetfieldFactorDistributionsError		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessAllMagnetfieldFactorDistributionsError::cannotAccessAllMagnetfieldFactorDistributionsError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessAllMagnetfieldFactorDistributionsError::~cannotAccessAllMagnetfieldFactorDistributionsError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessAllMagnetfieldFactorDistributionsError::errorMsg() {

	printMsg("Not all magnetfield factor distributions are accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessMagnetfieldFactorAnalyserError				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessMagnetfieldFactorAnalyserError::cannotAccessMagnetfieldFactorAnalyserError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessMagnetfieldFactorAnalyserError::~cannotAccessMagnetfieldFactorAnalyserError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessMagnetfieldFactorAnalyserError::errorMsg() {

	printMsg("The object for analysing the magnetfield factors is not accessible!!!");

}


/****************************************************************
 * CLASS indexIsOutOfDisplayRangeError							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

indexIsOutOfDisplayRangeError::indexIsOutOfDisplayRangeError() : analysisError() {

	this->index            = 0;
	this->numberOfDisplays = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

indexIsOutOfDisplayRangeError::indexIsOutOfDisplayRangeError(unsigned short index, unsigned short numberOfDisplays) : analysisError() {

	this->index            = index;
	this->numberOfDisplays = numberOfDisplays;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

indexIsOutOfDisplayRangeError::~indexIsOutOfDisplayRangeError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void indexIsOutOfDisplayRangeError::errorMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp =  "The display with the index ";
	ustos(index, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += " is out of range [0, ";
	ustos(numberOfDisplays - 1, buffer, shortConversionDigits);
	temp += buffer;
	temp += "]!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS indexIsOutOfFactorRangeError							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

indexIsOutOfFactorRangeError::indexIsOutOfFactorRangeError() : analysisError() {

	this->index           = 0;
	this->numberOfFactors = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

indexIsOutOfFactorRangeError::indexIsOutOfFactorRangeError(unsigned short index, unsigned short numberOfFactors) : analysisError() {

	this->index           = index;
	this->numberOfFactors = numberOfFactors;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

indexIsOutOfFactorRangeError::~indexIsOutOfFactorRangeError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void indexIsOutOfFactorRangeError::errorMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp =  "The factor with the index ";
	ustos(index, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += " is out of range [0, ";
	ustos(numberOfFactors - 1, buffer, shortConversionDigits);
	temp += buffer;
	temp += "]!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS tooManyActiveStationsError								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tooManyActiveStationsError::tooManyActiveStationsError() : analysisError() {

	this->actualNumberOfStations  = 0;
	this->maximumNumberOfStations = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

tooManyActiveStationsError::tooManyActiveStationsError(unsigned short actualNumberOfStations, unsigned short maximumNumberOfStations) : analysisError() {

	this->actualNumberOfStations  = actualNumberOfStations;
	this->maximumNumberOfStations = maximumNumberOfStations;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tooManyActiveStationsError::~tooManyActiveStationsError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void tooManyActiveStationsError::errorMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp =  "The actual number of detector stations ( ";
	ustos(actualNumberOfStations, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += ") exceeds the range of ";
	ustos(maximumNumberOfStations, buffer, shortConversionDigits);
	temp += buffer;
	temp += "!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS magneticFieldNotFoundForComparissonToFactorsError 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

magneticFieldNotFoundForComparissonToFactorsError::magneticFieldNotFoundForComparissonToFactorsError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

magneticFieldNotFoundForComparissonToFactorsError::~magneticFieldNotFoundForComparissonToFactorsError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void magneticFieldNotFoundForComparissonToFactorsError::errorMsg() {

	printMsg("The magnetic field is not found to do the comparisson to the constant factors!!!");

}


/****************************************************************
 * CLASS cannotAccessTrackCoordinatesError				 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessTrackCoordinatesError::cannotAccessTrackCoordinatesError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessTrackCoordinatesError::~cannotAccessTrackCoordinatesError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessTrackCoordinatesError::errorMsg() {

	printMsg("The track coordinates cannot be accessed!!!");

}
