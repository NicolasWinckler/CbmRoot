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
//     - implements methods to analyze the magnetfield
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/07/17 11:27:02 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../include/magnetfieldAnalysisDef.h"
#include "../include/magnetfieldZAnalysis.h"


/****************************************************************
 * returns the corresponding string, which is drawn on that axi,*
 * where the impulse is shown.									*
 ****************************************************************/

const char* magnetfieldZAnalysis::getTitleOfXAxi() {
	
	return ZXAXITITLE;

}

/**
 * returns the corresponding modifier for the first constant
 * dimension.
 */

const char* magnetfieldZAnalysis::getDim1Modifier() {

	return DIMXMODIFIER;

}

/**
 * returns the corresponding modifier for the second constant
 * dimension.
 */

const char* magnetfieldZAnalysis::getDim2Modifier() {

	return DIMYMODIFIER;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* magnetfieldZAnalysis::getNameOfBxDisplay() {

	return zBxDisplayName;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* magnetfieldZAnalysis::getTitleOfBxDisplay() {

	return zBxDisplayTitle;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* magnetfieldZAnalysis::getNameOfByDisplay() {

	return zByDisplayName;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* magnetfieldZAnalysis::getTitleOfByDisplay() {

	return zByDisplayTitle;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* magnetfieldZAnalysis::getNameOfBzDisplay() {

	return zBzDisplayName;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* magnetfieldZAnalysis::getTitleOfBzDisplay() {

	return zBzDisplayTitle;

}

/****************************************************************
 * returns true, if the saving should be in subdirectories.		*
 ****************************************************************/

bool magnetfieldZAnalysis::getSavingInSubDirectory() {

	return ZSAVINGINSUBDIRECOTRY;

}

/****************************************************************
 * returns the corresponding string for the saving subdirectory.*
 ****************************************************************/

const char* magnetfieldZAnalysis::getNameOfSavingSubDirectory() {

	return ZSAVINGSUBDIRECTORY;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

magnetfieldZAnalysis::magnetfieldZAnalysis() : magnetfieldDirectionAnalysis() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

magnetfieldZAnalysis::~magnetfieldZAnalysis() {

}
