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
//     - derived from errorHandling
//     - class for errors occuring while reading the events
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/07/17 11:35:32 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/inputError.h"
#include <string>


/****************************************************************
 * CLASS inputError								 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

inputError::inputError() : errorHandling(INPUTLIB) {

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

inputError::~inputError() {

}


/****************************************************************
 * CLASS eventNotFoundError										*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

eventNotFoundError::eventNotFoundError() : inputError() {

	eventID = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

eventNotFoundError::eventNotFoundError(int event) : inputError() {

	eventID = event;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

eventNotFoundError::~eventNotFoundError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void eventNotFoundError::errorMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp = "ERROR: The eventID: ";
	itos(eventID, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " is not found!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS zeroTracksOrHitsError					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

zeroTracksOrHitsError::zeroTracksOrHitsError() : inputError() {

	numberOfTracks = 0;
	numberOfHits   = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

zeroTracksOrHitsError::zeroTracksOrHitsError(int tracks, int hits) : inputError() {

	numberOfTracks = tracks;
	numberOfHits   = hits;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

zeroTracksOrHitsError::~zeroTracksOrHitsError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void zeroTracksOrHitsError::errorMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "Too many tracks (";
	itos(numberOfTracks, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += ") or hits (";
	itos(numberOfHits, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += ") in the file!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS detectorNodeNotFoundError				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

detectorNodeNotFoundError::detectorNodeNotFoundError() : inputError() {

	nodeName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

detectorNodeNotFoundError::detectorNodeNotFoundError(std::string actualNodeName) : inputError() {

	nodeName = actualNodeName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

detectorNodeNotFoundError::~detectorNodeNotFoundError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void detectorNodeNotFoundError::errorMsg() {

	std::string temp;

	temp = "The detector node '";
	temp += nodeName;
	temp += "', is not found, neither in the vacuum nor out of the vacuum!!!\n";
	printMsg(temp);

}


/****************************************************************
 * CLASS noTrackInFileError										*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noTrackInFileError::noTrackInFileError() : inputError() {

	branchName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

noTrackInFileError::noTrackInFileError(std::string actualBranchName) : inputError() {

	branchName = actualBranchName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noTrackInFileError::~noTrackInFileError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void noTrackInFileError::errorMsg() {

	std::string temp;

	temp = "The branch '";
	temp += branchName;
	temp += "', consisting of the track data, is not found in the tree!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS noPointInFileError										*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noPointInFileError::noPointInFileError() : inputError() {

	branchNamePoint.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

noPointInFileError::noPointInFileError(std::string actualBranchNamePoint) : inputError() {

	branchNamePoint = actualBranchNamePoint;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noPointInFileError::~noPointInFileError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void noPointInFileError::errorMsg() {

	std::string temp;

	temp += "The branch '";
	temp += branchNamePoint;
	temp += "', consisting of the point information, is found in the tree!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS detectorMatrixNotFoundError			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

detectorMatrixNotFoundError::detectorMatrixNotFoundError() : inputError() {

	nodeName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

detectorMatrixNotFoundError::detectorMatrixNotFoundError(std::string actualNodeName) : inputError() {

	nodeName = actualNodeName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

detectorMatrixNotFoundError::~detectorMatrixNotFoundError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void detectorMatrixNotFoundError::errorMsg() {

	std::string temp;

	temp = "The matrix of the detector node '";
	temp += nodeName;
	temp += "', is not found!!!\n";
	printMsg(temp);

}


/****************************************************************
 * CLASS detectorTranslationNotFoundError		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

detectorTranslationNotFoundError::detectorTranslationNotFoundError() : inputError() {

	nodeName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

detectorTranslationNotFoundError::detectorTranslationNotFoundError(std::string actualNodeName) : inputError() {

	nodeName = actualNodeName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

detectorTranslationNotFoundError::~detectorTranslationNotFoundError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void detectorTranslationNotFoundError::errorMsg() {

	std::string temp;

	temp = "The translation of the detector node '";
	temp += nodeName;
	temp += "', is not found!!!\n";
	printMsg(temp);

}
