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
//     - base class for errors occuring during HistogramTransform
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/07/17 11:35:22 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../include/histogramTransformationError.h"


/****************************************************************
 * CLASS histogramTransformationError			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

histogramTransformationError::histogramTransformationError() : errorHandling(HISTOGRAMTRANSFORMATIONLIB) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

histogramTransformationError::~histogramTransformationError() {

}


/****************************************************************
 * CLASS cannotAccessElementsToFilterError		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessElementsToFilterError::cannotAccessElementsToFilterError() : histogramTransformationError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessElementsToFilterError::~cannotAccessElementsToFilterError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessElementsToFilterError::errorMsg() {

	printMsg("The elements to filter are not accessible!!!");

}


/****************************************************************
 * CLASS middleElementIsOutOfRangeError			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

middleElementIsOutOfRangeError::middleElementIsOutOfRangeError() : histogramTransformationError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

middleElementIsOutOfRangeError::~middleElementIsOutOfRangeError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void middleElementIsOutOfRangeError::errorMsg() {

	printMsg("The middle element for the new value is out of the filter's range!!!");

}


/****************************************************************
 * CLASS cannotAccessFilterError				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessFilterError::cannotAccessFilterError() : histogramTransformationError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessFilterError::~cannotAccessFilterError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessFilterError::errorMsg() {

	printMsg("The filter is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessFilterMemoryError			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessFilterMemoryError::cannotAccessFilterMemoryError() : histogramTransformationError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessFilterMemoryError::~cannotAccessFilterMemoryError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessFilterMemoryError::errorMsg() {

	printMsg("The filter's memory is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessPeakHistogramError			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessPeakHistogramError::cannotAccessPeakHistogramError() : histogramTransformationError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessPeakHistogramError::~cannotAccessPeakHistogramError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessPeakHistogramError::errorMsg() {

	printMsg("The peak histogram is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessHistogramError			 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHistogramError::cannotAccessHistogramError() : histogramTransformationError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHistogramError::~cannotAccessHistogramError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHistogramError::errorMsg() {

	printMsg("The histogram is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessTracksError			 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessTracksError::cannotAccessTracksError() : histogramTransformationError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessTracksError::~cannotAccessTracksError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessTracksError::errorMsg() {

	printMsg("The tracks are not accessible!!!");

}
