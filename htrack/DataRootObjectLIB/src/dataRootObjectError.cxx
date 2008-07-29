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
//     - derived from errorHandling
//     - class for errors occuring with the data object handling
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-10-19 14:33:04 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/dataRootObjectError.h"


/****************************************************************
 * CLASS dataRootObjectError					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

dataRootObjectError::dataRootObjectError() : errorHandling(DATAROOTOBJECTLIB) {

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

dataRootObjectError::~dataRootObjectError() {

}


/****************************************************************
 * CLASS cannotFindStationIdError				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotFindStationIdError::cannotFindStationIdError() : dataRootObjectError() {

	stationId = -1;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotFindStationIdError::cannotFindStationIdError(int id) : dataRootObjectError() {

	stationId = id;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotFindStationIdError::~cannotFindStationIdError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotFindStationIdError::errorMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "A detector station with the id ";
	itos(stationId, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " is not found!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS detectorIsNotDefinedError				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

detectorIsNotDefinedError::detectorIsNotDefinedError() : dataRootObjectError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

detectorIsNotDefinedError::~detectorIsNotDefinedError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void detectorIsNotDefinedError::errorMsg() {

	printMsg("The detector is not defined. No station exists!!!");

}


/****************************************************************
 * CLASS tooBigStationOrderNumberError			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tooBigStationOrderNumberError::tooBigStationOrderNumberError() : dataRootObjectError() {

	orderNumber = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

tooBigStationOrderNumberError::tooBigStationOrderNumberError(unsigned int orderNumber) : dataRootObjectError() {

	this->orderNumber = orderNumber;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tooBigStationOrderNumberError::~tooBigStationOrderNumberError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void tooBigStationOrderNumberError::errorMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "A detector station with the order ";
	itos(orderNumber, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " cannot be not found because the order is too high!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS twoTracksWithSameIdError				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

twoTracksWithSameIdError::twoTracksWithSameIdError() : dataRootObjectError() {

	trackId = -1;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

twoTracksWithSameIdError::twoTracksWithSameIdError(int actualTrackId) : dataRootObjectError() {

	trackId = actualTrackId;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

twoTracksWithSameIdError::~twoTracksWithSameIdError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void twoTracksWithSameIdError::errorMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "There are two tracks with the id ";
	itos(trackId, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " !!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS tooBigHitMemoryIndexError				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tooBigHitMemoryIndexError::tooBigHitMemoryIndexError() : dataRootObjectError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tooBigHitMemoryIndexError::~tooBigHitMemoryIndexError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void tooBigHitMemoryIndexError::errorMsg() {

	printMsg("The index for accessing the hit memory is too big!!!");

}

/****************************************************************
 * CLASS cannotAccessHistogramCellError			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHistogramCellError::cannotAccessHistogramCellError() : dataRootObjectError() {

	index1 = (unsigned short)-1;
	index2 = (unsigned short)-1;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotAccessHistogramCellError::cannotAccessHistogramCellError(unsigned short index1, unsigned short index2) : dataRootObjectError() {

	this->index1 = index1;
	this->index2 = index2;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHistogramCellError::~cannotAccessHistogramCellError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHistogramCellError::errorMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "Cannot access histogram cell at position (";
	itos(index1, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += ", ";
	itos(index2, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += ")!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS cannotAccessHistogramBorderError						*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHistogramBorderError::cannotAccessHistogramBorderError() : dataRootObjectError() {

	index = (unsigned short)-1;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotAccessHistogramBorderError::cannotAccessHistogramBorderError(unsigned short index) : dataRootObjectError() {

	this->index = index;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHistogramBorderError::~cannotAccessHistogramBorderError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHistogramBorderError::errorMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "Cannot access histogram border at position (";
	itos(index, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += ")!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS trackDataMemoryIsNullError								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackDataMemoryIsNullError::trackDataMemoryIsNullError() : dataRootObjectError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackDataMemoryIsNullError::~trackDataMemoryIsNullError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void trackDataMemoryIsNullError::errorMsg() {

	printMsg("The memory allocated for the trackdata is NULL!!!");

}


/****************************************************************
 * CLASS cannotAccessMagneticFieldValueError					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessMagneticFieldValueError::cannotAccessMagneticFieldValueError() : dataRootObjectError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessMagneticFieldValueError::~cannotAccessMagneticFieldValueError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessMagneticFieldValueError::errorMsg() {

	printMsg("The pointer to the magnetic field value is NULL!!!");

}


/****************************************************************
 * CLASS cannotAccessMagneticFieldError							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessMagneticFieldError::cannotAccessMagneticFieldError() : dataRootObjectError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessMagneticFieldError::~cannotAccessMagneticFieldError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessMagneticFieldError::errorMsg() {

	printMsg("The pointer to the magnetic field is NULL!!!");

}


/****************************************************************
 * CLASS cannotFinalizeHitTrackAssignmentBeforeInitializingTracksError*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotFinalizeHitTrackAssignmentBeforeInitializingTracksError::cannotFinalizeHitTrackAssignmentBeforeInitializingTracksError() : dataRootObjectError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotFinalizeHitTrackAssignmentBeforeInitializingTracksError::~cannotFinalizeHitTrackAssignmentBeforeInitializingTracksError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotFinalizeHitTrackAssignmentBeforeInitializingTracksError::errorMsg() {

	printMsg("You cannot finalize the hit to track assignment before you have initialized the tracks!!!");

}


/****************************************************************
 * CLASS cannotAccessTrackParameterError						*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessTrackParameterError::cannotAccessTrackParameterError() : dataRootObjectError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessTrackParameterError::~cannotAccessTrackParameterError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessTrackParameterError::errorMsg() {

	printMsg("The trackParameter object is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessTrackInformationError						*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessTrackInformationError::cannotAccessTrackInformationError() : dataRootObjectError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessTrackInformationError::~cannotAccessTrackInformationError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessTrackInformationError::errorMsg() {

	printMsg("The trackInformation object is not accessible!!!");

}


/****************************************************************
 * CLASS tooBigTrackIndexError					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tooBigTrackIndexError::tooBigTrackIndexError() : dataRootObjectError() {

	index   = 0;
	maximum = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

tooBigTrackIndexError::tooBigTrackIndexError(unsigned int index, unsigned int maximum) : dataRootObjectError() {

	this->index   = index;
	this->maximum = maximum;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tooBigTrackIndexError::~tooBigTrackIndexError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void tooBigTrackIndexError::errorMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "A track with the index ";
	uitos(index, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " is not existing. There are just ";
	uitos(maximum, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " tracks!!!";
	printMsg(temp);

}
