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
//     - base class for errors occuring during trackfinding
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/07/17 11:36:26 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../include/trackfinderError.h"


/****************************************************************
 * CLASS trackfinderError						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackfinderError::trackfinderError() : errorHandling(HISTOGRAMTRANSFORMATIONLIB) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackfinderError::~trackfinderError() {

}


/****************************************************************
 * CLASS cannotAccessHoughMemManagerError		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHoughMemManagerError::cannotAccessHoughMemManagerError() : trackfinderError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHoughMemManagerError::~cannotAccessHoughMemManagerError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHoughMemManagerError::errorMsg() {

	printMsg("The houghMemManager is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessHistogramMemManagerError	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHistogramMemManagerError::cannotAccessHistogramMemManagerError() : trackfinderError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHistogramMemManagerError::~cannotAccessHistogramMemManagerError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHistogramMemManagerError::errorMsg() {

	printMsg("The histogramMemManager is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessHoughTransformError		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHoughTransformError::cannotAccessHoughTransformError() : trackfinderError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHoughTransformError::~cannotAccessHoughTransformError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHoughTransformError::errorMsg() {

	printMsg("The houghTransform is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessHistogramTransformError	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHistogramTransformError::cannotAccessHistogramTransformError() : trackfinderError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHistogramTransformError::~cannotAccessHistogramTransformError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHistogramTransformError::errorMsg() {

	printMsg("The histogramTransform is not accessible!!!");

}
