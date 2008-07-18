/* *******************************************************************
// (C)opyright 2005
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
//     - base class for holding the houghTransformation border from
//       the prelut look-up-table
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-05-18 12:24:16 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/dataObjectError.h"
#include "../include/dataObjectWarningMsg.h"
#include "../include/lutHoughBorder.h"


/****************************************************************
 * CLASS lutHoughBorder							 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

lutHoughBorder::lutHoughBorder() {

	houghCoord.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

lutHoughBorder::lutHoughBorder(const lutHoughBorder& value) {

	this->houghCoord = value.houghCoord;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

lutHoughBorder::~lutHoughBorder() {

	houghCoord.clear();

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const lutHoughBorder& lutHoughBorder::operator = (const lutHoughBorder& value) {

	this->houghCoord = value.houghCoord;

	return *this;

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds no identifiers.										*
 ****************************************************************/

std::string lutHoughBorder::toNotIdentifiedString() {

	std::string         returnValue;
	houghBorderPosition actualHoughCoord;

	returnValue.clear();
	houghCoord.resetActiveObject();
	for (unsigned short i = 0; i < houghCoord.getNumberOfEntries(); i++) {
		
		actualHoughCoord = houghCoord.readActiveObjectAndMakeNextOneActive();

		returnValue += actualHoughCoord.toNotIdentifiedString();
		returnValue += ", ";

	}

	return returnValue;

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds identifiers.										*
 ****************************************************************/

std::string lutHoughBorder::toIdentifiedString() {

	std::string returnValue;

	returnValue  = "(Pos1,Pos2): ";
	returnValue += toNotIdentifiedString();

	return returnValue;

}

/****************************************************************
 * This method clears the object.								*
 ****************************************************************/

void lutHoughBorder::clear() {

	houghCoord.clear();

}

/****************************************************************
 * This method returns the houghBorder command.					*
 ****************************************************************/

houghBorderCommand lutHoughBorder::getHoughBorderCommand(unsigned short maxDim) {

	houghBorderCommand  houghCmd;
	houghBorderPosition actualHoughCoord;
	houghBorderPosition previousHoughCoord;
	unsigned short      cmdCounter;

	if (!houghCoord.isEmpty()) {

		houghCoord.resetActiveObject();
		houghCmd.cmd.clear();

		cmdCounter         = 0;
		previousHoughCoord = houghCoord.readActiveObjectAndMakeNextOneActive();
		houghCmd.startPos  = previousHoughCoord.pos2;

		for (unsigned long i = 1 ; i < houghCoord.getNumberOfEntries(); i++) {

			actualHoughCoord = houghCoord.readActiveObjectAndMakeNextOneActive();

			if (actualHoughCoord.pos1 - previousHoughCoord.pos1 != 1)
				throw borderIdRuleError();

#if (LUTVERSION > 1)

			if (actualHoughCoord.pos2 - previousHoughCoord.pos2 > 1)
				throw borderIdRuleError();

#endif

			if (actualHoughCoord.pos2 - previousHoughCoord.pos2) {
				
				houghCmd.cmd.push(true);
				cmdCounter += actualHoughCoord.pos2 - previousHoughCoord.pos2;
			
			}
			else
				houghCmd.cmd.push(false);

			previousHoughCoord = actualHoughCoord;

		}

		if (houghCmd.startPos + cmdCounter + 1 > maxDim)
			throw borderIdConversionError();

	}

	return houghCmd;

}

/****************************************************************
 * This method sets the houghBorder command.					*
 ****************************************************************/

void lutHoughBorder::setHoughBorderCommand(houghBorderCommand& value, unsigned short maxFirstDim, unsigned short maxSecondDim) {

	houghBorderPosition actualHoughCoord;
	bool                actualCommand;

	houghCoord.clear();

	actualHoughCoord.pos1 = 0;
	actualHoughCoord.pos2 = value.startPos;

	if ((actualHoughCoord.pos1 < maxFirstDim) && (actualHoughCoord.pos2 < maxSecondDim)) {

		houghCoord.push(actualHoughCoord);

	}
	else {

		rangeLutHoughBorderWarningMsg* rangeLutHoughBorder = new rangeLutHoughBorderWarningMsg(maxFirstDim, maxSecondDim, actualHoughCoord.pos1, actualHoughCoord.pos2);
		rangeLutHoughBorder->warningMsg();
		if(rangeLutHoughBorder != NULL) {
			delete rangeLutHoughBorder;
			rangeLutHoughBorder = NULL;
		}

	}

	value.cmd.resetActiveObject();
	for (unsigned long i = 0; i < value.cmd.getNumberOfEntries(); i++) {

		actualCommand = value.cmd.readActiveObjectAndMakeNextOneActive();

		actualHoughCoord.pos1++;
		if (actualCommand)
			actualHoughCoord.pos2++;

		if ((actualHoughCoord.pos1 < maxFirstDim) && (actualHoughCoord.pos2 < maxSecondDim)) {

			houghCoord.push(actualHoughCoord);

		}
		else {

			rangeLutHoughBorderWarningMsg* rangeLutHoughBorder = new rangeLutHoughBorderWarningMsg(maxFirstDim, maxSecondDim, actualHoughCoord.pos1, actualHoughCoord.pos2);
			rangeLutHoughBorder->warningMsg();
			if(rangeLutHoughBorder != NULL) {
				delete rangeLutHoughBorder;
				rangeLutHoughBorder = NULL;
			}

		}

	}

}
